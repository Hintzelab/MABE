import os, sys
from types import SimpleNamespace
from typing import List, Optional
from parsimonious.grammar import Grammar
from nqr.generic import (
        logger,
        logi,
        logw,
        loge,
        setreps,
        addcommand as generic_addcommand,
        requires,
        complete_templated_command,
        NS,
        Runs as GenericRuns,
        set_user_module,
        _diff_namespace,
        iterate_commands_and_outputs,
        run_command,
        set_arg_transform,
        save_namespace_to_namespace_file,
        snapshot_vanilla_namespace,
        snapshot_namespace,
        )

def find_mabe_exe_at_path(path:str) -> str:
    """ensures mabe exists and is executable. returns path"""
    from os.path import (exists, join)
    windows_mabe = join(path,'mabe.exe')
    nix_mabe = join(path,'mabe')
    if exists(windows_mabe) and os.access(windows_mabe, os.X_OK):
        logi(f"found windows mabe at: '{windows_mabe}'")
        return windows_mabe
    elif exists(nix_mabe) and os.access(nix_mabe, os.X_OK):
        logi(f"found windows mabe at: '{nix_mabe}'")
        return nix_mabe

def get_mabe_help(path:str) -> str:
    """given path to mabe exe, returns --help output"""
    if path is None:
        print(f"Error: no mabe executable found in script location")
        sys.exit(1)
    command = f"{path} -h"
    return run_command(command).strip().split('\n')

def get_mabe_exe_settings(path:str) -> str:
    """given path to mabe exe, returns settings output"""
    command = f"{path} -ss"
    return run_command(command).strip().split('\n')

def use_help_to_remove_mabe_header(mabehelp:List[str],mabesettings:List[str]):
    """removes common first lines of A from B"""
    # usually linesA will be the help information
    if len(mabesettings) < len(mabehelp):
        return
    i = 0
    for i in range(len(mabehelp)):
        if mabehelp[i] != mabesettings[i]:
            break
    del mabesettings[:i]
    logi(f"got {len(mabesettings)} lines of settings after cleaning")

def get_and_remove_next_config_section_from_lines(lines:List[str]) -> Optional[List[str]]:
    result = None
    if len(lines) < 2:
        return result
    # start on second line, skipping current section header (#SETTINGS_FILE)
    # then we'll look for the next one, or the end of the file
    i = 1
    for i in range(i,len(lines)):
        if lines[i].startswith('#SETTINGS_FILE'):
            break
    result = lines[:i+1]
    del lines[:i+1]
    return result

def get_config_sections(mabesettings:List[str]) -> List[List[str]]:
    sections = list()
    while True:
        section = get_and_remove_next_config_section_from_lines(mabesettings)
        if not section:
            break
        sections.append(section)
    return sections

def parse_config_lines(lines:List[str], name:Optional[str]=None) -> SimpleNamespace:
    grammar = Grammar(
        r"""
        section    = opt_header? (module / definition / comment / endl)*
        opt_header = '#SETTINGS_FILE ' filename
        filename   = ~r"[^\n]+"
        module     = '% ' module_name
        module_name = ~r"[a-zA-Z_]+"
        definition = sp? name sp? '=' sp? value? comment? endl?
        comment    = sp? '#' vartype? anychar*
        vartype    = '(' ('bool' / 'string' / 'double' / 'int') ')'
        name       = ~r"[a-zA-Z_]+"
        value      = ~r"[^#\n]+"
        sp         = ' '*
        endl       = sp? '\n'
        anychar    = ~r"[^\n]"
        """
    )
    output = grammar.parse('\n'.join(lines))
    filename, params = tree_to_filename_and_dict(output)
    # remove temporary state entries
    if '_current_module' in params:
        del params['_current_module']
    if '_filename' in params:
        del params['_filename']
    return filename, params

def tree_to_filename_and_dict(tree, node=None):
    """convert parsed grammar of mabe config into (filename, params)"""
    if node is None:
        node = dict()
    for child in tree.children:
        name = child.expr_name
        if name == 'filename':
            node['_filename'] = child.text
        elif name == 'module':
            module_name = child.children[1].text
            node[module_name] = dict()
            node['_current_module'] = module_name
        elif name == 'definition':
            _, name, _, _, _, value, comment, _ = child
            vartype = comment.children[0].children[2].text[1:-1]
            definitions = node.get("definition", list())
            node.get(node['_current_module'], dict())[name.text.strip()] = {'name':name.text.strip(),'value':value.text.strip(),'comment':comment.text.strip(),'type':vartype}
        if child.children:
            tree_to_filename_and_dict(child, node)
    filename = node.get('_filename', None)
    return filename, node

def _read(fileish:str):
    """reads mabe settings file or file(s) as a glob, returning namespace"""
    import glob
    # read files as sections (1 each)
    # then transform as usual
    sections = list()
    for config in glob.glob(fileish):
        logi(f"reading config file '{config}'")
        if os.path.isfile(config):
            try:
                section = open(config,'r+t').read().split('\n')
            except:
                print(f"Error: couldn't open file '{config}'")
                sys.exit(1)
            sections.append(section)
    # perform normal namespace conversion
    ns = convert_sections_to_namespace(sections)
    return ns

def convert_sections_to_namespace(sections:List[List[str]]) -> NS:
    mabe = NS()
    for section in sections:
        filename, params = parse_config_lines(lines=section)
        for module, definitions in params.items():
            ns = dict()
            for name, definition in definitions.items():
                if definition['type'] == 'string':
                    if definition['value'] == 'NONE':
                        ns[definition['name']] = None
                    else:
                        ns[definition['name']] = definition['value']
                elif definition['type'] == 'int':
                    ns[definition['name']] = int(definition['value'])
                elif definition['type'] == 'double':
                    ns[definition['name']] = float(definition['value'])
                elif definition['type'] == 'bool':
                    ns[definition['name']] = bool(definition['value'])
            new_namespace = NS(**ns)
            mabe.__dict__.update({module:new_namespace})
    return mabe

def read(fileish:str):
    # by the time this is called,
    # the mabe module namespaces are in globals()
    ns = _read(fileish)
    for name,namespace in ns.__dict__.items():
        if name not in globals():
            print(f"Error: variable '{name}' in config files not in current mabe build,")
            print(f"       please update outdated config files or mabe executable.")
        globals()[name] = namespace

def print_help():
    help_output = """
    Help - Mabe ==================================

    -t        Run in test mode (2 reps, 2 updates)

    """
    print(help_output)

#################
# MABE-SPECIFIC #
#################

if os.getenv("DEBUG",False):
    logging.basicConfig()
    logger.root.setLevel(logging.DEBUG)
scriptdir = os.getcwd()
mabepath = find_mabe_exe_at_path(scriptdir)
mabehelp = get_mabe_help(mabepath)
mabesettings = get_mabe_exe_settings(mabepath)
use_help_to_remove_mabe_header(mabehelp, mabesettings)
sections = get_config_sections(mabesettings)
# create mabe namespace for all parameters
mabe = convert_sections_to_namespace(sections)
# create namespace.py file (the 'how' is possibly module-specific)
save_namespace_to_namespace_file(mabe)
del mabe

###########################
# MABE-SPECIFIC OVERRIDES #
###########################

def addcommand(exe:str,template:str="{exe}",output:List[str]=[]):
    test_flag = '-t' in sys.argv
    if test_flag:
        # replace updates with 2 if present
        import re
        template = re.sub(r'GLOBAL-updates\s*[0-9]+', 'GLOBAL-updates 2', template)
    generic_addcommand(exe,template,output)

def Runs(*args,**kwargs):
    global GLOBAL
    test_flag = '-t' in sys.argv
    if test_flag:
        setreps(2)
        GLOBAL.updates = 2
    help_flag = any([e in sys.argv for e in ('-h','--help')])
    if help_flag:
        run = next(GenericRuns(*args,**kwargs)) # force generic module to process its help
        print_help() # now print our help
        yield run # let other downstream modules process their help
    for run in GenericRuns(*args,**kwargs):
        yield run

def arg_transform(name:str, value:str) -> str:
    name = name.replace(".","-")
    return f"{name} {value}"
set_arg_transform(arg_transform)

###########################
# REQUIRED BY ALL MODULES #
###########################

current_module = sys.modules[__name__]
set_user_module(current_module)
snapshot_vanilla_namespace(current_module)
# then import said file to current namespace
sys.path.append(scriptdir)
from namespace import *
# save snapshot of globals for later masking
# after user adds stuff to the namespace
snapshot_namespace(current_module)
