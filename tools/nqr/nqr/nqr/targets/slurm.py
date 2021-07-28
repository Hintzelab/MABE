import sys, os, glob, subprocess
from typing import Tuple, List

from .. import utils
from . import targets
from ..generic import iterate_commands_and_outputs, run_command
from .targets import get_settings

slurm_normal_template = """#!/bin/bash -login
{sbatch}
#SBATCH --output=slurm.out.log
#SBATCH --error=slurm.err.log
#SBATCH --job-name={jobname}
shopt -s expand_aliases
ulimit -s 8192

cd ${{SLURM_SUBMIT_DIR}}

{commands}

touch .finished

#sacct -j ${{SLURM_JOB_ID}} # could print job status info
exit $ret
"""

slurm_indefinite_template = """#!/bin/bash -login
{sbatch}
#SBATCH --output=slurm.out.log
#SBATCH --error=slurm.err.log
#SBATCH --job-name={jobname}
shopt -s expand_aliases
ulimit -s 8192

cd ${{SLURM_SUBMIT_DIR}}
export SLURM_JOBSCRIPT="job.sb" # used for resubmission
######################## start dmtcp_coordinator #######################
fname=port.$SLURM_JOBID # store port number
dmtcp_coordinator --daemon --exit-on-last -p 0 --port-file $fname $@ 1>/dev/null 2>&1   # start coordinater
h=`hostname` # get coordinator's host name
p=`cat $fname` # get coordinator's port number
export DMTCP_COORD_HOST=$h # save coordinators host info in an environment variable
export DMTCP_COORD_PORT=$p # save coordinators port info in an environment variable
export DMTCP_CHECKPOINT_DIR="./" # save ckpt files into unique locations
####################### BODY of the JOB ######################
# prepare work environment of the job
# if first time launch, use "dmtcp_launch" otherwise use "dmtcp_restart"
export CKPT_WAIT_SEC=$(( 4 * 60 * 60 - 10 * 60 )) # when to ckpt, in seconds (just under 4 hrs)
# Launch or restart the execution
if [ ! -f ${{DMTCP_CHECKPOINT_DIR}}/ckpt_*.dmtcp ] # if no ckpt file exists, it is first time run, use dmtcp_launch
then
  # first time run, use dmtcp_launch to start the job and run on background
  dmtcp_launch -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT --rm --ckpt-open-files bash commands.sh &
  #wait for an inverval of checkpoint seconds to start checkpointing
  sleep $CKPT_WAIT_SEC
  # start checkpointing
  dmtcp_command -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT --ckpt-open-files --bcheckpoint
  # kill the running job after checkpointing
  dmtcp_command -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT --quit
  # resubmit the job
  sbatch $SLURM_JOBSCRIPT
else            # it is a restart run
  # clean up artifacts (resulting files that could be in the middle of being written to)
  # clean up any generated mabe files that have been checkpointed
  bash cleanup.sh
  # restart job with checkpoint files ckpt_*.dmtcp and run in background
  dmtcp_restart -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT ckpt_*.dmtcp &
  # wait for a checkpoint interval to start checkpointing
  sleep $CKPT_WAIT_SEC
  # if program is still running, do the checkpoint and resubmit
  if dmtcp_command -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT -s 1>/dev/null 2>&1
  #if dmtcp_command -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT -s 1>/dev/null
  then  
    # clean up old ckpt files before start checkpointing
    rm -r ckpt_*.dmtcp
    # checkpointing the job
    dmtcp_command -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT --ckpt-open-files -bc
    # kill the running program and quit
    dmtcp_command -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT --quit
    # resubmit this script to slurm
    sbatch $SLURM_JOBSCRIPT
  else
    echo "job finished"
  fi
fi
# show the job status info
#scontrol show job $SLURM_JOB_ID

"""

def _get_sbatch_options_str() -> str:
    """constructs '#SBATCH --key=value' strings, returns one big block:str"""
    global targets
    return '\n'.join([f"#SBATCH {string.replace('_','-')}" for string in targets.get_settings('slurm')])

_printed_help = False
def print_help():
    global _printed_help
    # avoid looping into module helps
    if _printed_help:
        sys.exit(0)
    _printed_help = True
    help_output = """
    Help - SLURM Target ==========================

    (no flags)  Normal submission
    -i          Indefinite mode (4hr chunks w/ checkpointing until done)
    -lN         When -i, limit indefinite mode to N 4hr chunks

    """
    print(help_output)

def _determine_indefinite_mode() -> bool:
    indefinite = any([e.startswith('-i') for e in sys.argv])
    return indefinite

def _determine_indefinite_repeats() -> Tuple[bool, int]:
    limited = any([e.startswith('-l') for e in sys.argv])
    repeats = None
    if limited:
        for arg in sys.argv:
            if arg.startswith('-l'):
                if len(arg) > 2:
                    try:
                        repeats = int(arg[2:])
                        if repeats <= 0:
                            raise ValueError()
                    except ValueError:
                        print("\nError: -lN (N should be positive int)")
                        sys.exit(1)
                break
    return limited, repeats

def _create_commands_and_cleanup_scripts(parent:str, commands_pairs:List[Tuple[str,List[str]]]):
    # create helper lists
    zcount = len(str(len(commands_pairs))) # for zfill
    finished_ids = list(range(len(commands_pairs)))
    finished_names = [f".{str(n).zfill(zcount)}.finished" for n in range(len(commands_pairs))]
    # create file content strings
    commands_script = """
##################################
# automatically created by PPNQR #
##################################

# This is the set of commands called by
# the checkpointing system. This bash script
# is actually what is getting checkpointed.

"""
    cleanup_script = """
##################################
# automatically created by PPNQR #
##################################

# This utility is called by the job/checkpointing script
# Cleanup is required if restarting a checkpoint
# and the job is not yet done writing its output files
# we must manually remove them so the checkpointing script
# can restore the version of the files it is expecting
# as small byte-differences can happen between checkpointing
# and job-killing and invalidate those left behind.
# Also, I think existing files make the reloading break anyway.

"""
    for i in finished_ids:
        command,output = commands_pairs[i]
        commands_script += f"""
{command}
if [ $? -ne 0 ]; then exit; fi
touch {finished_names[i]}

"""
        if output:
            removals_list = list()
            for file_glob in output:
                removals_list.append(f"""  rm -rf {file_glob}""")
            removals = '\n'.join(removals_list)
            cleanup_script += f"""
if [ ! -f "{finished_names[i]}" ]; then
{removals}
fi

"""
    # create files
    open(os.path.join(parent,'commands.sh'),'wt').write(commands_script)
    open(os.path.join(parent,'cleanup.sh'),'wt').write(cleanup_script)
    # clear any existing .finished semaphores
    finished_files = glob.glob(os.path.join(parent,".*.finished"))
    for each_file in finished_files:
        os.remove(each_file)

def _create_normal_job_file(run:dict, commands_pairs:List[Tuple[str,List[str]]]):
    command_code = ""
    for command,_ in commands_pairs:
        command_code += f"""
{command}
ret=$?
if [ $? -ne 0 ]; then exit; fi
"""
    content = slurm_normal_template.format(
        sbatch=_get_sbatch_options_str(),
        jobname=run['condition'],
        commands=command_code,
        )
    open(os.path.join(run['dir'],'job.sb'),'wt').write(content)

def _create_indefinite_job_file(run:dict):
    content = slurm_indefinite_template.format(
        sbatch=_get_sbatch_options_str(),
        jobname=run['condition'],
        )
    open(os.path.join(run['dir'],'job.sb'),'wt').write(content)

def _has_sbatch_has_dmtcp() -> Tuple[bool,bool]:
    from shutil import which
    has_sbatch = bool(which('sbatch'))
    has_dmtcp = bool(which('dmtcp_launch'))
    return (has_sbatch, has_dmtcp)

def launch(run):
    help_flag = any([e in sys.argv for e in ('-h','--help')])
    if help_flag:
        print_help()
        return # allow downstream to print help
    dry_run = '--run' not in sys.argv
    indefinite_mode = _determine_indefinite_mode()
    finite,finite_repeats = _determine_indefinite_repeats()
    has_sbatch,has_dmtcp = _has_sbatch_has_dmtcp()
    if dry_run:
        diagnostics = f"""
SLURM Target
  sbatch: {'Yes' if has_sbatch else 'No (no SLURM system found)'}
   dmtcp: {'Yes (indefinite mode available)' if has_dmtcp else 'No (indefinite mode not available)'}
run mode: {'indefinite' if indefinite_mode else 'normal'}{' (limited to '+str(finite_repeats)+'x 4hr repeats)' if indefinite_mode and finite else ''}
"""
        print(diagnostics)
        return
    # create directories and copy files
    utils.make_directories(run)
    utils.copy_requirements(run)
    commands_pairs = list(iterate_commands_and_outputs(run))
    # create necessary job files
    if indefinite_mode:
        if not has_dmtcp:
            print("Error: no dmtcp available for indefinite checkpointing. Try normal mode.")
            if not has_sbatch:
                print("Error: not a SLURM system. Use a valid target for launching jobs.")
            sys.exit(1)
        # override time to 4 hrs
        settings.time='03:59:00'
        # create commands.sh, cleanup.sh to be called by dmtcp checkpointing
        _create_commands_and_cleanup_scripts(run['dir'], commands_pairs)
        _create_indefinite_job_file(run)
    else: # normal mode
        if not has_sbatch:
            print("Error: not a SLURM system. Use a valid target for launching jobs.")
            sys.exit(1)
        _create_normal_job_file(run, commands_pairs)
    # submit the job using slurm
    subprocess.Popen('sbatch job.sb', shell=True, cwd=run['dir'])

# register this target 
from . import targets
current_module = sys.modules[__name__]
targets.addtarget('slurm',current_module)
