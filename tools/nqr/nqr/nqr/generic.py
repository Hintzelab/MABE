from types import SimpleNamespace, ModuleType
import sys, subprocess, logging
import copy
import re
from collections import OrderedDict
from typing import overload, List, Optional, Any
from shlex import quote
import os

def include(path:str):
    """facilitates include modules from custom system paths"""
    if not os.path.isdir(path):
        raise FileNotFoundError(f"No such directory {path}")
    sys.path.append(path)

#groups are ordered dict of dicts
# {"01": {
#        "mabe.BRAIN_CGP.buildMode": ['a','b','c'],
#        ..<more vars>..},
#  ..<more groups>..,
# }

_groups = OrderedDict()

logger = logging.getLogger("nqr")
def logi(msg:str):
    """logger.info"""
    logger.info(msg)
def logw(msg:str):
    """logger.warning"""
    logger.warning(msg)
def loge(msg:str):
    """logger.error"""
    logger.error(msg)


class NS(SimpleNamespace):
    def __init__(self, **kwargs):
        SimpleNamespace.__init__(self, **kwargs)
    def __enter__(self):
        return self
    def __exit__(self, *args):
        pass

# in lieu of deepcopying modules, collapse dicts with NS objects to dicts
def _transform_ns_or_dict_to_dict(pre):
    if type(pre) is not dict:
        pre = pre.__dict__
    post = dict()
    for k,v in pre.items():
        if isinstance(v,ModuleType):
            # do not both with modules
            continue
        if isinstance(v,(dict,NS)):
            post[k] = _transform_ns_or_dict_to_dict(v)
        else:
            post[k] = v
    return post

class Group(OrderedDict):
    unique_counter = 0
    def __init__(self, name=None):
        self.name = name
        # flag for if we need to generate a name later
        self.unnamed = name is None
        self.id = Group.unique_counter
        if name is None:
            self.name = str(self.id)
        Group.unique_counter += 1
        OrderedDict.__init__(self)
        current_module = sys.modules[__name__]
        current_module._groups[self.name] = self
    def __enter__(self):
        global _nqr_group_context
        _nqr_group_context = self.name
    def __exit__(self, *args):
        global _nqr_group_context
        _nqr_group_context = self.name
        del _nqr_group_context
    def add(self, *varyings):
        current_module = sys.modules[__name__]
        group = current_module._groups[self.name]
        for varying in varyings:
            group.update(varying)


def _get_first_arg_code():
    import traceback,os,sys
    for file,line,_,_ in traceback.extract_stack()[:-2]:
        if os.path.exists(file):
            line -= 1
            with open(file,"r") as f:
                rl = f.readlines()
                tblines = rl[line:min(line+9,len(rl))]
                return ''.join(tblines)

class _Varying():
    def __init__(self,values,name,labels):
        # the list of values for this variable
        # from which to create groups
        self.values = values
        # if not None, a short name
        # to create nicer group dir names
        self.name = name
        # if not None, friendly labels
        # for each of the values
        # (must match length of values list)
        self.labels = labels

# USER_IDENTIFIER_PATTERN captures the below
# Vary(generic.GLOBAL.seed, [101,102])
#              ^^^^^^^^^^^
USER_IDENTIFIER_PATTERN = re.compile(r'\(\s*\n*\S*?\.(.*?)[\s,]+.*$', re.MULTILINE)
def Vary(variable, values:List[Any], *, name:str=None, labels:Optional[List[str]]=None) -> Optional[_Varying]:
    # can be used inside a "Group with" block
    # in which case it relies on the with block setting
    # the '_nqr_group_context' var
    # can optionally set a short name for
    # nicer directory name creation
    # can optionally set labels (list must be same size as values)
    # for nicer directory name creation
    import traceback
    first_arg_as_user_typed_it = _get_first_arg_code()
    varname = USER_IDENTIFIER_PATTERN.search(first_arg_as_user_typed_it)\
            .groups()[0].split(',')[0]
    name = varname if name is None else name
    varying = {varname: _Varying(values, name, labels)}
    # if not used in 'with' context then just return new varying
    if '_nqr_group_context' not in globals():
        return varying
    # otherwise, store the varying using the 'with' context
    current_module = sys.modules[__name__]
    groupname = _nqr_group_context
    current_module._groups[groupname].update(varying)

def _update_reps_list():
    """ uses _reps to fill out the _reps_list and _reps_list_str lists"""
    global _reps_list, _reps_list_str
    _reps_list = list(_reps)
    znum = len(str(max(_reps)))
    _reps_list_str = [str(i).zfill(znum) for i in _reps]

_reps = range(1)
_reps_list = list()
_reps_list_str = list()
_update_reps_list()

# overloads are for static analysis support
@overload
def setreps(newrep:int):
    pass

@overload
def setreps(newrep:slice):
    pass

@overload
def setreps(newrep:range):
    pass

def setreps(newrep):
    global _reps, _reps_list, _reps_list_str
    if type(newrep) is int:
        _reps = range(newrep)
    elif isinstance(newrep,(slice,range)):
        if newrep.step != 1:
            raise ValueError("Error: reps() slices or ranges with steps!=1 are not permitted")
        _reps = range(newrep.start,newrep.stop)
    else:
        raise ValueError("Error: reps() only takes 1 argument type int or slice")
    _update_reps_list()
REPS="reps"
CONDITIONS="conditions"

def _get_iteration_indices_for_group(group):
    global _reps
    import itertools as it
    counts = list()
    for variable,varying in group.items():
        counts.append(list(range(len(varying.values))))
    iterations = it.product(*counts)
    return list(iterations)

def count_conditions() -> int:
    global _groups
    # count how many iterations are in each group
    # by making index lists for each iterable value
    # then calling itertools.product and counting the result
    group_iterations = list()
    groups = list(_groups.values())
    for group in groups:
        iterations = _get_iteration_indices_for_group(group)
        group_iterations.append(len(iterations))
    return(sum(group_iterations))

def count_reps():
    global _reps
    return len(list(_reps))

def validate_varying_labels():
    # Error if any:
    #  * labels contain invalid filename characters
    #  * if no labels defined, values as str contain invalid filename characters
    global _groups
    def stripIllegalDirnameChars(rawString):
        for eachChar in list(':[](),\'"\\!@#$%^&*=+` <>?{}'):
            rawString = rawString.replace(eachChar,'')
        return rawString
    VALID_CHARS = r'[a-zA-Z0-9\.=\-+_@\']+$'
    VALID_FILENAME_PATTERN = re.compile(VALID_CHARS)
    for group_name,group in _groups.items():
        for variable,varying in group.items():
            if varying.labels is not None:
                for label in varying.labels:
                    if not VALID_FILENAME_PATTERN.match(str(label)):
                        raise ValueError(f"""Error: invalid filename chars in label
       group    '{group.name}'
       variable '{variable}'
       label    '{label}'

       valid chars follow the regex pattern
       {VALID_CHARS}""")
            else:
                # auto-convert values to valid dir chars
                labels = list()
                for value in varying.values:
                    labels.append(stripIllegalDirnameChars(str(value)))
                varying.labels = labels

user_arg_transform = None

def _default_arg_transform(name:str,value:str) -> str:
    return f"--{name}={value}"

def set_arg_transform(new_xform):
    global user_arg_transform
    user_arg_transform = new_xform

def _prepare_condition_str(group,group_str,rep_i,iteration_i,iteration,groups_zcount,conditions_zcount):
    global user_module
    # rep
    rep_str = _reps_list_str[rep_i] 
    name = f"{group_str}_C{str(iteration_i).zfill(conditions_zcount)}"
    for condition_i,value_i in enumerate(iteration):
        variable,varying = list(group.items())[condition_i]
        if varying.labels is not None:
            name += f"""_{varying.name}={varying.labels[value_i]}"""
        else:
            # quote strings
            if type(varying.values[value_i]) is str:
                name += f"""_{varying.name}={varying.values[value_i]}"""
            else:
                name += f"_{varying.name}={varying.values[value_i]}"
        # set var
        if type(varying.values[value_i]) is str:
            exec(f"""user_module.{variable} = \"{varying.values[value_i]}\"""")
        else:
            exec(f"user_module.{variable} = {varying.values[value_i]}")
    return name

def _iterate_all_runs(arg_transform, priority=REPS):
    # helper for Runs()
    import itertools as it
    global _groups, _reps, _reps_list_str, requirements
    if _reps is None:
        raise ValueError("""Error: no reps specified.
       use namespace.reps(int)""")
    groups_zcount = len(str(len(_groups)))
    conditions_zcount = len(str(count_conditions()))
    # reps-first iteration
    if priority == REPS:
        for rep_i,rep in enumerate(_reps_list_str):
            for group_name,group in _groups.items():
                run = dict()
                iterations = _get_iteration_indices_for_group(group)
                run['rep'] = rep
                if group.unnamed:
                    run['group'] = "G"+group.name.zfill(groups_zcount)
                else:
                    run['group'] = group.name
                # construct condition filename string
                for iteration_i,iteration in enumerate(iterations):
                    condition_str = _prepare_condition_str(group,run['group'],rep_i,iteration_i,iteration,groups_zcount,conditions_zcount)
                    run['condition'] = condition_str
                    run['dir'] = os.path.join('results',run['condition'],run['rep'])
                    # read current parameters after condition preparation
                    # (which sets parameters)
                    parameters = _get_parameters()
                    xformed = [arg_transform(name,value) for name,value in parameters.items()]
                    run['parameters'] = xformed
                    run['requirements'] = requirements
                    run['last_run'] = '0'
                    yield copy.deepcopy(run)
    else: # priority == CONDITIONS
        for group_name,group in _groups.items():
            run = dict()
            iterations = _get_iteration_indices_for_group(group)
            if group.unnamed:
                run['group'] = "G"+group.name.zfill(groups_zcount)
            else:
                run['group'] = group.name
            # construct condition filename string
            for iteration_i,iteration in enumerate(iterations):
                for rep_i,rep in enumerate(_reps_list_str):
                    run['rep'] = rep
                    condition_str = _prepare_condition_str(group,run['group'],rep_i,iteration_i,iteration,groups_zcount,conditions_zcount)
                    # condition_str begins with '_'
                    run['condition'] = condition_str
                    run['dir'] = os.path.join('results',run['condition'],run['rep'])
                    # read current parameters after condition preparation
                    # (which sets parameters)
                    parameters = _get_parameters()
                    xformed = [arg_transform(name,value) for name,value in parameters.items()]
                    run['parameters'] = xformed
                    run['requirements'] = requirements
                    run['last_run'] = '0'
                    yield copy.deepcopy(run)

_printed_help = False
def print_help():
    global _printed_help
    # avoid looping into module helps
    if _printed_help:
        sys.exit(0)
    _printed_help = True
    # ensure help overrides all other flags
    sys.argv = [sys.argv[0], '-h', '--help']
    help_output = """
NQR: the Enqueuer for Pleasantly Parallel Computation
    
    Usage: python nqr_script.py [options]
    
    Should be run from within target directory.
    
    Help - General ===============================
    
    (no flags)     Dry-run, report what WOULD happen
    -h,--help      This help message
    --run          Actually perform actions, not dry-run
    """
    print(help_output)

def Runs(priority=REPS, arg_transform=None):
    import itertools as it
    global _groups, _reps, _reps_list_str, user_commands
    dry_run = '--run' not in sys.argv
    help_flag = any([e in sys.argv for e in ('-h','--help')])
    if arg_transform is None:
        arg_transform = user_arg_transform
    # ensure we can make valid filenames
    # or error and tell user what to do
    validate_varying_labels()
    #TODO add rep-first or group-first
    groups_iterations = list()
    all_runs = list(_iterate_all_runs(arg_transform=arg_transform, priority=priority))
    if help_flag:
        global _printed_help
        if _printed_help:
            # end generator
            return
        print_help()
        yield all_runs[0] # allow downstream to print
    if dry_run:
        global dry_run_already_called
        print("Dry-Run Report")
        print()
        print("Jobs")
        print(f"{len(all_runs)} total submission(s) from")
        print(f"{count_conditions()} condition(s)")
        print(f"{count_reps()} replicate(S) each condition")
        print()
        print("Directories")
        reps_range_str = "{"+f"{_reps_list_str[0]}..{_reps_list_str[-1]}"+"}"
        directories = sorted(list(set(os.path.join('results',run['condition'],reps_range_str) for run in all_runs)))
        for directory in directories:
            print(directory)
        if 'dry_run_already_called' in locals():
            del dry_run_already_called
            return
        else:
            dry_run_already_called = True
            yield all_runs[0]
    if len(user_commands) == 0:
        raise ValueError("""Error: no commands set
       use namespace.addcommand(...)""")

    if not dry_run:
        last_run_i = len(all_runs)-1
        znum = len(str(len(all_runs)))
        for run_i,run in enumerate(all_runs):
            run['id'] = str(run_i).zfill(znum)
            if run_i == last_run_i:
                run['last_run'] = '1'
            yield run

user_commands = list()
def addcommand(exe:str, template:str="{exe}", output:List[str]=list()):
    """
    exe: Command to execute
    template: Templated execution string include exe (exe, id, rep, dir, condition, parameters)
    output: List of files or file globs for expected generated files from this command
            Required by some targets (such as SLURM indefinite mode)
    """
    from platform import system
    def iscommand(cmd) -> bool:
        from shutil import which
        import subprocess
        if not system()[:3] in ('Win','CYG'):
            return not bool(subprocess.run(f"type {cmd}", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).returncode)
        else: # windows
            return bool(which(cmd))
    global user_commands
    # removing safety, because users might be renaming files using 'requires()' syntax
    #is_file = os.path.isfile(exe)
    is_command = all([iscommand(cmd.strip().split()[0]) for cmd in exe.split('&&')])
    #if not is_file and not is_command:
    #    raise FileNotFoundError(1, f"Error: addcommand() file(s) or command(s) not found", exe)
    if not is_command:
        # assume user forgot to add './' in front of executable
        from platform import system
        if not system()[:3] in ('Win','CYG'):
            exe = './'+exe
        else: # windows
            if exe.lower()[-4] != ".exe":
                exe += ".exe"
    if '{exe}' not in template:
        raise ValueError("template argument {exe} not found in template so nothing will run")
    user_commands.append((exe,template,output))

def iterate_commands_and_outputs(run:dict):
    """generator for template-completed commands"""
    global user_commands
    for command,template,output in user_commands:
        template = template.replace("{exe}",command)
        yield (complete_templated_command(template,run),output)

def complete_templated_command(command:str,run:dict) -> str:
    for k,v in run.items():
        if isinstance(v,list):
            if k == 'requirements':
                continue
            command = command.replace("{"+k+"}", ' '.join(v))
        else:
            command = command.replace("{"+k+"}", v)
    return command

class Requirement():
    def __init__(self,path,destination=None):
        # make all paths relative
        cwd = os.getcwd()
        path = os.path.relpath(path,cwd)
        if destination is not None:
            destination = os.path.relpath(destination,cwd)
        # check make sure the file/dir exists
        if not os.path.isfile(path) and not os.path.isdir(path):
            # check if it's a windows exe extension the user didn't put
            if not os.path.isfile(path+'.exe'):
                raise FileNotFoundError(1, f"Error: requires() dir or file not found", path)
            else:
                path += '.exe'
        self.path = path
        if destination is not None:
            self.destination = destination
            self.replicate_path = False
        else:
            # in path replication mode
            # but we do not support upward directory traversal
            # for obvious replication reasons
            if ".." in path:
                raise FileNotFoundError(1, f"""Error: can't replicate an upward traversal in depenedency
       Explicity specify destination path.
       ex: namespace.requires('../src.txt', 'src.txt')
       ex: namespace.requires('../src.txt', dst='src.txt')""", path)
            self.replicate_path = True
        self.isdir = not os.path.isfile(path)
        self.parent = os.path.split(path)[0]
        # if already dir, then let's say parent is itself
        if self.isdir:
            self.parent = path
    def __repr__(self):
        return repr(self.__dict__)
requirements = list()
def requires(src:str, dst:Optional[str]=None):
    # we don't allow upward traversal
    requirement = Requirement(src,dst)
    requirements.append(requirement)

def run_command(command:str) -> str:
    import subprocess
    result = subprocess.run(command, universal_newlines=True, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    #if result.returncode:
    #    raise OSError(1, f"Error: could not run '{command}'", "")
    return result.stderr.strip() + result.stdout.strip()

def diff_namespace_dicts(originalNS, newNS, background=None):
    # assume ns2 is globals
    # homogenize ns1 to a dict if NS
    # background is the globals() to ignore (only given on depth-0 invocation)
    if type(originalNS) is not dict:
        originalNS = _transform_ns_or_dict_to_dict(originalNS)
    if type(newNS) is not dict:
        newNS = _transform_ns_or_dict_to_dict(newNS)
    if background and type(background) is not dict:
        background = _transform_ns_or_dict_to_dict(background)
    ns1 = originalNS
    ns2 = newNS
    diffs = {}
    # generate sets of names from each namespace
    ns1toplvl = {name for name,val in ns1.items()}
    ns2toplvl = {name for name,val in ns2.items()}
    # get all same and new names between the two namespaces
    if background is not None:
        bglvl = {name for name,val in background.items()}
        # ignore these things
        for dirty in ('_original_namespace','__warningregistry__','PACKAGE_PARENT','SCRIPT_DIR'):
            bglvl.add(dirty)
        toplvl = ((ns2toplvl-bglvl) & ns1toplvl) | ((ns2toplvl-bglvl)-ns1toplvl)
    else:
        toplvl = (ns2toplvl & ns1toplvl) | (ns2toplvl - ns1toplvl)
    for name in toplvl:
        if name in ns1:
            if type(ns1[name]) is dict:
                new_entries = diff_namespace_dicts(ns1[name], ns2[name])
                diffs[name] = new_entries
            else:
                original_value = ns1[name]
                new_value = ns2[name]
                if new_value != original_value:
                    diffs[name] = new_value
        # traverse user-added new namespace
        else:
            if type(ns2[name]) is dict:
                new_entries = diff_namespace_dicts(dict(), ns2[name])
                diffs[name] = new_entries
            else:
                diffs[name] = ns2[name]
    # return non-empty entries
    return {name:value for name,value in diffs.items() if ((type(value) is not dict) or (type(value) is dict and len(value)))}

def _namespace_to_str(ns,prefix=""):
    out = ""
    ns = ns.__dict__
    items = {name for name,val in ns.items()}
    for name in items:
        if type(ns[name]) is NS:
            out += f"\n{prefix}{name} = nqr.NS()"
            out += _namespace_to_str(ns[name], prefix=f"{prefix}{name}.")
        elif type(ns[name]) is str:
            out += f"\n{prefix}{name} = \"{ns[name]}\""
        else:
            out += f"\n{prefix}{name} = {ns[name]}"
    return out+"\n"

def save_namespace_to_namespace_file(ns):
    # saves a namespace to our namespace file format
    header = """
#######################################
# FILE GENERATED AUTOMATICALLY BY nqr #
#######################################

# use this hack for testing and development
import os, sys
#PACKAGE_PARENT = '..'
#SCRIPT_DIR = os.path.dirname(os.path.realpath(os.path.join(os.getcwd(), os.path.expanduser(__file__))))
#sys.path.append(os.path.normpath(os.path.join(SCRIPT_DIR, PACKAGE_PARENT)))
import nqr
#TODO replace above with final release import

"""
    contents = header + _namespace_to_str(ns)
    try:
        file = open("namespace.py","w+t")
        file.write(contents)
    except:
        loge("Error: could not write to 'namespace.py' file")
        sys.exit(1)
    finally:
        file.close()

def _diff_namespace(new_namespace):
    global _original_namespace, _vanilla_namespace
    # globals is a dictionary already
    diffs = diff_namespace_dicts(_original_namespace, new_namespace, _vanilla_namespace)
    return diffs

def _flatten_namespace(hdict,prefix=""):
    # flattens the nested dictionary params
    # returned by _diff_namespace()
    fdict = dict()
    for k,v in hdict.items():
        if type(v) is dict:
            fdict.update(_flatten_namespace(v,prefix=f"{prefix}{k}."))
        else:
            if type(v) is str:
                fdict[f"{prefix}{k}"] = quote(v)
            else:
                fdict[f"{prefix}{k}"] = v
    return fdict

def _get_parameters():
    # this diffs namespaces and returns everything the user
    # loaded or set
    global user_module
    diffs = _diff_namespace(user_module)
    params = _flatten_namespace(diffs)
    return params

def set_user_module(ns):
    global user_module
    user_module = ns

# captures the namespace state
# before any parameters are loaded or initialized
# (captures all the regular 'python-stuff')
def snapshot_vanilla_namespace(ns):
    global _vanilla_namespace
    dns = _transform_ns_or_dict_to_dict(ns)
    _vanilla_namespace = copy.deepcopy(dns)

# save a snapshot of the globals namespace
# so we know what cruft to get rid of when 
# inspecting for user-added variables
def snapshot_namespace(ns):
    global _original_namespace, user_module
    dns = _transform_ns_or_dict_to_dict(ns)
    _original_namespace = copy.deepcopy(dns)

set_arg_transform(_default_arg_transform)

_vanilla_namespace = None
_original_namespace = None
# get current module
# snapshot both vanilla and current
# this seems redundant, but in generic
# mode there is no namespace.py to load
# to snapshot.
current_module = sys.modules[__name__]
user_module = sys.modules[__name__]
snapshot_vanilla_namespace(current_module)
snapshot_namespace(current_module)
