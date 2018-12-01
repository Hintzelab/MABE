import argparse
import os
import posixpath
import sys
import platform ## system identification
import re ## regular expressions
import copy ## deepcopy
import uuid ## unique guid generator for vs project files
import collections ## defaultdict
from utils import pyreq
from subprocess import call
import subprocess

if platform.system() == 'Windows':
    pyreq.require("winreg") ## quits if had to attempt install. So user must run script again.
    import winreg ## can now safely import

# Need to make sure winreg import works before trying to import the buildlibs
from mbuildlib.modulewriter import write_modules_h
from mbuildlib.projectwriter import make_visual_studio_project, make_x_code_project, make_make_project, make_cmake_project, make_codeblocks_project, make_dev_cpp_project

parser = argparse.ArgumentParser()

SUPPORTED_PROJECT_FILES='mk,make, vs,visual_studio, xc,x_code, dc,dev_cpp, cb,code_blocks, cm,cmake'

parser.add_argument('-b','--buildOptions', metavar='FILE', default = 'buildOptions.txt',  help=' name of file with build options - default : buildOptions.txt')
parser.add_argument('-c','--cleanup', action='store_true', default = False, help='add this flag if you want build files (including make) removed after building')
parser.add_argument('-nc','--noCompile', action='store_true', default = False, help='create modules.h and makefile, but do not compile')
parser.add_argument('-g','--generate', help='does not compile but generates project files (2 letter abbreviations are the same as expanded form)', choices=[e.strip() for e in SUPPORTED_PROJECT_FILES.split(',')])
parser.add_argument('-pg','--gprof', action='store_true', default = False, help='compile with -pg option (for gprof)')
parser.add_argument('-p','--parallel', default = 1, help='how many threads do you want to use when you compile?  i.e. make -j6')
parser.add_argument('-i','--init', action = 'store_true', default = False, help='refresh or initialize the buildOptions.txt')
parser.add_argument('-C','--compiler', default = 'c++', help='select c++ compiler to build mabe - default : c++')

args = parser.parse_args()

if platform.system() == 'Windows':
    product = 'mabe.exe'
else:
    product = 'mabe'

def touch(fname, mode=0o666, dir_fd=None, **kwargs): ## from https://stackoverflow.com/a/1160227
    flags = os.O_CREAT | os.O_APPEND
    with os.fdopen(os.open(fname, flags=flags, mode=mode, dir_fd=dir_fd)) as f:
        os.utime(f.fileno() if os.utime in os.supports_fd else fname,
            dir_fd=None if os.supports_fd else dir_fd, **kwargs)

compiler = args.compiler
compFlags='-Wno-c++98-compat -w -Wall -std=c++14 -O3 -lpthread -pthread'

if (args.gprof):
    compFlags =  compFlags + ' -pg'

options = {'World':[],'Genome':[],'Brain':[],'Optimizer':[],'Archivist':[],}
currentOption = ""

ptrnBuildOptions = re.compile(r'([+\-*%])\s*(\w+)') # ex: gets ["%","WORLD"] from " %  WORLD", gets ["*","BerryWorld"] from " * BerryWorld"
if args.init:
    ## fill options dict with the subdirs names {'World':['Berry','Xor']}
    for eachTopDir in options.keys():
        if os.path.isdir(eachTopDir):
            for eachSubDir in os.listdir(eachTopDir):
                if os.path.isdir(os.path.join(eachTopDir,eachSubDir)):
                    options[eachTopDir].append(eachSubDir[0:-len(eachTopDir)])
                elif eachSubDir.startswith("Default"):
                    if "Default" not in options[eachTopDir]:
                        options[eachTopDir].append("Default")
    open("buildOptions.txt",'a').close() ## create buildOptions.txt if it doesn't exist
    ## read current buildOptions.txt into a single string
    f = open("buildOptions.txt",'r')
    bopts = f.read().splitlines()
    f.close()
    ## create a data structure to store buildOptions sections, and include directives [+-*] and names ['Berry','Xor']
    oldopts = dict()
    for eachKey in options.keys():
        oldopts[eachKey] = ([],[]) ## Structure: {'World':(['+','-'],['Berry','Xor']),...}
    ## go through the old buildOptions, store into the new structure, and remove names from the discovered (options var) if already here
    ## and also don't add to new structure if not in discovered (options var)
    OPS,NAMES=0,1
    currentSection=''
    for line in bopts:
        line = line.strip()
        if len(line) == 0: continue
        for a,b in ptrnBuildOptions.findall(line):
            if a == '%': currentSection = b
            else:
                if b in options[currentSection]: ## if also in filesystem discovered (options var)
                    oldopts[currentSection][OPS].append(a) ## add to new structure
                    oldopts[currentSection][NAMES].append(b)
                    options[currentSection].remove(b) ## remove from filesystem discovered
    ## for all remaining items (meaning were discovered but not yet in the new structure) add them to new structure
    for currentSection,names in options.items():
        for name in names:
            oldopts[currentSection][OPS].append('+')
            oldopts[currentSection][NAMES].append(name)
        if '*' not in oldopts[currentSection][OPS]:
            oldopts[currentSection][OPS][-1] = '*'
    ## write new buildOptions.txt using the new structure
    with open("buildOptions.txt",'w') as newbopts:
        for currentSection in oldopts.keys():
            line = "% {section}\n".format(section=currentSection)
            newbopts.write(line)
            print(line,end='')
            for op,name in zip(oldopts[currentSection][OPS],oldopts[currentSection][NAMES]):
                line = "  {op} {name}\n".format(op=op,name=name)
                newbopts.write(line)
                print(line,end='')
            newbopts.write("\n")
            print()
    print("buildOptions.txt created")
    print()
    sys.exit()

if args.generate:
    args.noCompile = True
else:
    args.generate = 'make'

if posixpath.exists(args.buildOptions) is False:
    print()
    print('buildOptions file with name "'+args.buildOptions+'" does not exist. Please provide a diffrent filename, or use -i to initialize one.')
    print()
    sys.exit()
    
# load all lines from buildOptions into lines, ignore blank lines
file = open(args.buildOptions, 'r')
pathToMABE=posixpath.dirname(args.buildOptions)
if pathToMABE == "":
    pathToMABE = "./"
lines = [line.rstrip('\n').split() for line in file if line.rstrip('\n').split() not in [['EOF']] ]
file.close()


unrecognizedLinesFound=False
for linenum,line in enumerate(lines):
    if len(line)==0:
        continue
    linenum+=1 ## sane line numbering starting at 1
    if len(line) != 2:
        print("Line "  + linenum + " is incorrectly formatted. The line's contents are: ")
        print(line)
        exit()
    if line[0] == '%': ## set current category
        currentOption = line[1]
    elif line[0] == '*': ## include and make default the module
        options[currentOption].append(line[1])
        options[currentOption][0], options[currentOption][len(options[currentOption])-1] = options[currentOption][len(options[currentOption])-1], options[currentOption][0]
    elif line[0] == '+': ## include added modules
        options[currentOption].append(line[1])
    elif line[0] == '-': ## ignore negated modules
        continue
    else: ## report errors on anything else
        print("unrecognized line in buildoptions (line {0}): {1}".format(linenum,' '.join(line)))
        unrecognizedLinesFound=True
if unrecognizedLinesFound:
    print("Errors found in biuldOptions.txt")
    quit()


if not('Default' in options['Archivist']):
    options['Archivist'].append('Default');
        
print("Building MABE with:")
print("")
for option in options:
    print(option)
    for o in options[option]:
        print("  " + o)
    print("")

write_modules_h(pathToMABE, options)

# Create a project file of type in SUPPORTED_PROJECT_FILES
options['Archivist'].remove('Default')
alwaysSources=['main.cpp','Global.cpp','Group/Group.cpp','Organism/Organism.cpp','Utilities/Data.cpp','Utilities/Parameters.cpp','Utilities/Loader.cpp','Utilities/Filesystem.cpp','Utilities/CSV.cpp','World/AbstractWorld.cpp','Genome/AbstractGenome.cpp','Brain/AbstractBrain.cpp','Optimizer/AbstractOptimizer.cpp','Archivist/DefaultArchivist.cpp','Analyze/neurocorrelates.cpp']
moduleSources = []
objects = []
sources = None
#alwaysSources = [e.replace('/',sep) for e in alwaysSources]

def getSourceFilesByBuildOptions(sep='/'):
    def newUnit():
        #return ['','1','','1','1','1000','0','','','']
        return {
            'filename': '',
            'compilecpp': '1',
            'folder': '',
            'compile': '1',
            'link': '1',
            'priority': '1000',
            'overridebuildcmd': '0',
            'buildcmd': '',
            'uuid': '',
            'filerefuuid': ''
        }
    units=[]
    for eachsource in alwaysSources:
        unit=newUnit()
        unit['filename']=eachsource.replace('/',sep)
        lastSlash=eachsource.rfind('/')
        if lastSlash!=-1:
            unit['folder']=eachsource[0:lastSlash]
        units.append(unit)
        if os.path.isfile(eachsource.replace('cpp','h')): ## add possible header
            unitH=newUnit()
            unitH['filename']=eachsource.replace('cpp','h')
            unitH['folder'] = unit['folder']
            units.append(unitH)
    for category in options: ## buildOptions dictionary
        for module in options[category]:
            filename=category+sep+module+category+sep+module+category+'.cpp'
            foldername=category+'/'+module+category
            unit=newUnit()
            unit['filename']=filename
            unit['folder']=foldername
            units.append(unit)
            filename=category+sep+module+category+sep+module+category+'.h'
            foldername=category+'/'+module+category
            unit=newUnit()
            unit['filename']=filename
            unit['folder']=foldername
            units.append(unit)
            dirs = [d for d in os.listdir(category+'/'+module+category+'/') if os.path.isdir(os.path.join(pathToMABE,category,module+category,d))]
            for eachdir in dirs:
                contents = [c for c in os.listdir(category+'/'+module+category+'/'+eachdir+'/') if (('.cpp' in c) or ('.h' in c)) and c.startswith('.')==False] ## include cpp/h/hpp files and ignore hidden files
                for content in contents:
                    filename=category+'/'+module+category+'/'+eachdir+'/'+content
                    filename=filename.replace('/',sep)
                    foldername=category+'/'+module+category+'/'+eachdir
                    unit=newUnit()
                    unit['filename']=filename
                    unit['folder']=foldername
                    units.append(unit)
    unit=newUnit()
    unit['filename']='modules.h'
    units.append(unit)
    sortedunits=sorted(units, key=lambda x: x['folder'])
    return sortedunits

## create git version integration
## Create an empty file if git is not available
## Otherwise capture commit hash
gitExists = True ## only false is a) git not installed OR b) .git dir not present (zip download from GitHub)
if not subprocess.run("git --version",shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE).stdout.startswith(b"git version"): gitExists = False
if not os.path.isdir(".git"): gitExists = False
with open(os.path.join("Utilities","gitversion.h"),'w') as file:
    if gitExists:
        commitHash = str(subprocess.run("git rev-parse HEAD",shell=True,stdout=subprocess.PIPE).stdout.decode("utf-8").strip())
        file.write('const char *gitversion = "{gitversion}";\n'.format(gitversion=commitHash))
    else:
        file.write('const char *gitversion = "";\n')
if gitExists: touch("main.cpp") ## IDE-independent signal to recompile main.o (only do so if we can use git to get a version number though)

# Create a make file if requested (default)
if args.generate == 'make' or args.generate == 'mk': ## GENERATE make:
    make_make_project(options, moduleSources, pathToMABE, alwaysSources, objects, product, compiler, compFlags)
elif  args.generate == 'dev_cpp' or args.generate == 'dc': ## GENERATE devcpp
    make_dev_cpp_project(getSourceFilesByBuildOptions(sep='\\'))
elif  args.generate == 'visual_studio' or args.generate == 'vs': ## GENERATE vs
    make_visual_studio_project(getSourceFilesByBuildOptions(sep='\\'))
elif args.generate == 'x_code' or args.generate == 'xc':
    make_x_code_project(getSourceFilesByBuildOptions(sep='/'))
elif  args.generate == 'code_blocks' or args.generate == 'cb':
    make_codeblocks_project(getSourceFilesByBuildOptions(sep='/'))
elif args.generate == 'cmake' or args.generate == 'cm':
    make_cmake_project(getSourceFilesByBuildOptions(sep='/'))

if not (args.noCompile):
    call(["make","-j"+str(args.parallel)])
if (args.cleanup):
    call(["make","cleanup"])
    call(["rm","makefile"])
    call(["rmdir","objectFiles"])
