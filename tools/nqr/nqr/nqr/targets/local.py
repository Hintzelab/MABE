import sys
from ..generic import iterate_commands_and_outputs, run_command
from .. import utils
from .targets import get_settings

import subprocess
import time

import threading
from signal import signal, getsignal, SIGINT

from typing import List
from types import ModuleType

CTRL_C = False
original_sigint_handler = getsignal(SIGINT)
serial_proc = None # process handle in serial mode
def handler(signal_received, frame):
    global CTRL_C, serial_proc
    # Handle any cleanup here
    CTRL_C = True
    if serial_proc is not None:
        serial_proc.terminate()

import time, os
class LeashedProcess(threading.Thread):
    def __init__(self,commands:List[str],*args,working_dir,**kwargs):
        if not isinstance(commands,list):
            raise TypeError("LeashedProcess only takes lists of commands")
        if len(commands) == 0:
            raise ValueError("LeashedProcess given 0 commands")
        self._num_lastlines = kwargs.setdefault('lines',0)
        self._delay_lastlines = kwargs.setdefault('delay',0.75)
        del kwargs['lines']
        del kwargs['delay']
        self._commands = commands
        self._working_dir = working_dir
        threading.Thread.__init__(self,*args,**kwargs)
        self._killed = threading.Event()
        self._logfile = open(os.path.join(working_dir,'output.log'),'wt+')
        self._lastlines = ""
        self.start()
        self._proc = None
    def start_process_with_command(self,command):
        self._proc = subprocess.Popen(command, shell=True, stdout=self._logfile, stderr=self._logfile, universal_newlines=True, cwd=self._working_dir)
    def start(self):
        threading.Thread.start(self)
    def run(self):
        last_read = 0
        while not self._killed.is_set() and self._commands:
            self._lastlines = ""
            command = self._commands[0]
            self.start_process_with_command(command)
            while not self._killed.is_set() and self._proc.poll() is None:
                # check if need to update copy of
                # lastlines from process output
                if self._num_lastlines:
                    self._logfile.seek(0,os.SEEK_END) # seek to end of file to not mess up writing
                    filesize = self._logfile.tell()
                    self._logfile.seek(max(0,filesize-1024),os.SEEK_SET)
                    data = self._logfile.read(1024)
                    self._logfile.seek(0,os.SEEK_END) # seek to end of file to not mess up writing
                    if len(data):
                        pos_of_newline = len(data)-1
                        # ignore last actual line as it may be partial
                        pos_of_newline = data.rfind("\n",0,max(pos_of_newline-1,0))
                        end = max(pos_of_newline,0)
                        for i in range(self._num_lastlines):
                            pos_of_newline = data.rfind("\n",0,max(pos_of_newline-1,0))
                        pos = min(max(pos_of_newline+1,0),len(data))
                        self._lastlines = data[pos:end]
                    last_read += len(data)
                # update time for CTRL_C or _last_lines updating
                time.sleep(min(0.5,self._delay_lastlines))
            try:
                self._proc.terminate()
            except ProcessLookupError:
                pass
            self._proc = None
            self._commands.pop(0)
        self._logfile.close()
    def lastlines(self):
        return self._lastlines.strip()
    def isrunning(self):
        return bool(self._commands)
    def wait(self,*args,**kwargs):
        return threading.Thread.wait(self,*args,**kwargs)
    def terminate(self):
        self._killed.set()
        if self._proc:
            try:
                self._proc.terminate()
            except ProcessLookupError:
                pass

class HeapList(list):
    def __init__(self,maxsize=0):
        self.maxsize = maxsize
        self.occupancy = 0
        self.isempty = True
        self.isfull = False
        list.__init__(self)
    def push(self,element):
        if self.occupancy == self.maxsize:
            raise ValueError(f"HeapList occupancy {self.occupancy} breached")
        self.isempty = False
        self.occupancy += 1
        if self.occupancy == self.maxsize:
            self.isfull = True
        list.append(self, element)
    def pop(self,index=-1):
        if self.occupancy == 0:
            raise ValueError(f"HeapList already empty")
        elif self.occupancy == 1:
            self.isempty = True
        self.isfull = False
        self.occupancy -= 1
        return list.pop(self,index)
    def append(self, element):
        raise NotImplementedError("Do not call append() on HeapList. Use push()")
    def empty(self):
        return self.isempty
    def full(self):
        return self.isfull

Q = None # HeapList (running jobs)
preQ = list() # (pre-running jobs)

def _determine_parallelism():
    parallel = any([e.startswith('-j') for e in sys.argv])
    jobs = 1 #TODO
    if parallel:
        for arg in sys.argv:
            if arg.startswith('-j'):
                if len(arg) > 2:
                    try:
                        jobs = int(arg[2:])
                        if jobs <= 0:
                            raise ValueError()
                    except ValueError:
                        print("\nError: -jN (N should be positive integer)")
                        sys.exit(1)
                break
    return parallel, jobs

def _determine_update_delay():
    delayed = any([e.startswith('-d') for e in sys.argv])
    delay = 1
    if delayed:
        for arg in sys.argv:
            if arg.startswith('-d'):
                if len(arg) > 2:
                    try:
                        delay = float(arg[2:])
                        if delay <= 0:
                            raise ValueError()
                    except ValueError:
                        print("\nError: -dS (S should be positive int or float)")
                        sys.exit(1)
                break
    return delayed, delay

def _determine_update_num_lines():
    has_numlines = any([e.startswith('-l') for e in sys.argv])
    num_lines = 1
    if has_numlines:
        for arg in sys.argv:
            if arg.startswith('-l'):
                if len(arg) > 2:
                    try:
                        num_lines = int(arg[2:])
                        if num_lines <= 0:
                            raise ValueError()
                    except ValueError:
                        print("\nError: -lN (N should be positive integer)")
                        sys.exit(1)
                break
    return num_lines


def display_queue_jobs_last_lines():
    # currently unused functionality
    # maybe someday we upgrade this
    # whole bit to use ncurses
    print("-----------------")
    for job in Q:
        print(job.lastline())

def display_queue_status(jobs_finished,jobs_total):
    total_len = len(str(jobs_total))
    done_str = str(jobs_finished).ljust(total_len)
    total_str = str(jobs_total)
    update_str = f"finished {done_str} / {total_str}"
    print(f"\r{update_str}  ",end='')

def tidy_up_queue() -> int:
    jobs_removed = 0
    for job_i in range(len(Q)-1,-1,-1):
        if not Q[job_i].isrunning():
            jobs_removed += 1
            Q.pop(job_i)
    return jobs_removed

def empty_queue_and_prequeue():
    for job in Q:
        job.terminate()
    preQ.clear()

def print_help():
    help_output = """
    Help - Local Target ==========================

    (no flags)  Run jobs in serial (like -j1)
    -jN         Run jobs in parallel up to N
    -dS         Only show latest line(s) of output every S:float seconds
                (default 1, for empty -d)
    -lN         When -d, show N latest lines of output (default 1)

    """
    print(help_output)

def launch(run):
    signal(SIGINT, handler)
    global CTRL_C, original_sigint_handler, Q, serial_proc
    parallel,jobs = _determine_parallelism()
    # frequency of display last line from process output
    delayed, delay = _determine_update_delay()
    num_lines = _determine_update_num_lines() #default 1
    help_flag = any([e in sys.argv for e in ('-h','--help')])
    if help_flag:
        print_help()
        signal(SIGINT, original_sigint_handler)
        return # allow downstream to print help
    dry_run = '--run' not in sys.argv
    if dry_run:
        diagnostics = f"""
Local Target
 parallelism: {'serial' if not parallel else str(jobs)+'x'}
output delay: {'no' if not delayed else str(num_lines)+' latest lines every '+str(delay)+' seconds'}
"""
        print(diagnostics)
        return
    if parallel and Q is None:
        Q = HeapList(maxsize=jobs)
    if parallel and jobs > 1:
        preQ.append(run)
        if run['last_run'] == '1':
            print(f"\nRunning {jobs} jobs at a time")
            jobs_total = len(preQ)
            jobs_finished = 0
            while not Q.empty() or preQ:
                if preQ:
                    # wait for a slot in the queue
                    while Q.full():
                        #display_queue_jobs_last_lines()
                        time.sleep(0.5)
                        jobs_removed = tidy_up_queue()
                        jobs_finished += jobs_removed
                        display_queue_status(jobs_finished, jobs_total)
                        if CTRL_C:
                            empty_queue_and_prequeue()
                    if not CTRL_C:
                        run = preQ.pop()
                        utils.make_directories(run)
                        utils.copy_requirements(run)
                        command_list = [command for command,output in list(iterate_commands_and_outputs(run))]
                        Q.push(LeashedProcess(command_list,working_dir=run['dir']))
                # display information about
                # running processes
                if Q.full() or len(preQ) == 0:
                    #display_queue_jobs_last_lines()
                    time.sleep(0.5)
                jobs_removed = tidy_up_queue()
                jobs_finished += jobs_removed
                display_queue_status(jobs_finished, jobs_total)
            print() # clean up display
    else: # serial
        print(f"\nRunning",run['condition'],flush=True)
        utils.make_directories(run)
        utils.copy_requirements(run)
        help_flag = any([e in sys.argv for e in ('-h','--help')])
        if delayed:
            print(f"(showing last {num_lines} line(s) every {delay} seconds)",flush=True)
            command_list = [command for command,output in list(iterate_commands_and_outputs(run))]
            serial_proc = LeashedProcess(command_list,working_dir=run['dir'],delay=delay,lines=num_lines)
            while not CTRL_C and serial_proc.isrunning():
                print(serial_proc.lastlines())
                time.sleep(delay)
            serial_proc.terminate()
        else:
            for command,output in iterate_commands_and_outputs(run):
                serial_proc = subprocess.Popen(command, shell=True, universal_newlines=True, cwd=run['dir'])
                serial_proc.wait()
                serial_proc.terminate()
                if serial_proc.returncode != 0:
                    CTRL_C = True
                    break
    signal(SIGINT, original_sigint_handler)
    if CTRL_C:
        sys.exit(1)

# register this target 
from . import targets
current_module = sys.modules[__name__]
targets.addtarget('local',current_module)