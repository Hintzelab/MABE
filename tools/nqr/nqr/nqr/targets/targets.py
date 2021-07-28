import sys
from types import ModuleType
from typing import List, Tuple, Any, Dict, Optional
from collections import OrderedDict, Callable
from ..generic import NS, _flatten_namespace

_targets = OrderedDict()
def addtarget(name:str,module:ModuleType):
    """add an nqr target and give it a CLI flag"""
    global _targets
    def validate_target_name(name:str) -> bool:
        import re
        isvalid = bool(re.match(r'^[a-z]+[a-z0-9_]*$',name.lower()))
        if not isvalid:
            raise ValueError(f"""
            Error: target name of '{name}' contains invalid characters
                   Valid: start with char, have only [a-zA-Z0-9_]
            """)
            sys.exit(1)
    validate_target_name(name)
    _targets[name] = module
    # create settings namespace within the module
    module.settings = NS()

def targets() -> List[Tuple[str,ModuleType]]:
    global _targets
    return list(_targets.items())

def _validate_targets_exist():
    global _targets
    # ensure there are any targets loaded (local, slurm, s3, etc.)
    if not _targets:
        msg = """

Error: targets.launch(...) But no targets exist.
       Please load specific targets and they will
       automatically register themselves.

       ex: from nqr.targets import targets, local, slurm
                                    ^       ^      ^
        """
        print(msg)
        sys.exit(1)
    # ensure target has launch(run) defined
    for name,target in _targets.items():
        if not (hasattr(target,'launch') and callable(target.launch)):
            raise ValueError(f"""
            Error: target '{name}' has no launch(run) defined
            """)

def _get_raw_settings(name:str) -> Dict[str,Any]:
    # finds the namespace _targets[name] (ex: local, slurm, s3...)
    # and flattens the parameters namespace of the ns.settings
    global _targets
    ns = _targets[name].settings.__dict__
    params = _flatten_namespace(ns)
    return params

def get_settings(target_name:str) -> List[str]:
    settings = _get_raw_settings(target_name)
    settings_transform = _get_settings_transform(target_name)
    return [settings_transform(setting_name,value) for setting_name,value in settings.items()]

def _default_settings_transform(name:str, value:Any) -> str:
    return f"--{name}={str(value)}"

def _get_settings_transform(name:str) -> Optional[Callable]:
    global _targets
    if hasattr(_targets[name], 'settings_transform') and callable(_targets[name].settings_transform):
        return _targets[name].settings_transform
    return _default_settings_transform

def print_help():
    help_output = f"""
    Help - All Targets ===========================

    (no flags)  Dry-Run. 
"""
    help_output += f"    --{'all'.ljust(12)}Launch jobs on all targets. Found: {', '.join([target_name for target_name,_ in targets()])}\n"
    for target_name,_ in targets():
        help_output += f"    --{target_name.ljust(12)}Launch jobs on the {target_name} target.\n"
    print(help_output)
    print()

def launch(run):
    help_flag = any([e in sys.argv for e in ('-h','--help')])
    _validate_targets_exist()
    if not any([f"--{target_name}" in sys.argv for target_name,_ in targets()]) and not help_flag and len(sys.argv) > 1:
        print("Error: No targets specified")
        print(f"Valid targets are: {', '.join([target_name for target_name,_ in targets()])}")
        sys.exit(1)
    for target_name,target in targets():
        if "--run" in sys.argv and "--all" in sys.argv:
            target.launch(run)
        elif "--run" in sys.argv and f"--{target_name}" in sys.argv:
            target.launch(run)
        elif "--run" not in sys.argv or help_flag: # dry-run
            target.launch(run)
    if help_flag:
        print_help()
        #return do not return, to allow all targets to print help
