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

parser = argparse.ArgumentParser()

SUPPORTED_PROJECT_FILES='make,vs,xcode,devcpp,cb'
SUPPORTED_PROJECT_NAMES='Make, Visual Studio, XCode, [orwell] Dev-C++, CodeBlocks'

parser.add_argument('-b','--buildOptions', type=str, metavar='FILE', default = 'buildOptions.txt',  help=' name of file with build options - default : buildOptions.txt', required=False)
parser.add_argument('-c','--cleanup', action='store_true', default = False, help='add this flag if you want build files (including make) removed after building', required=False)
parser.add_argument('-nc','--noCompile', action='store_true', default = False, help='create modules.h and makefile, but do not compile', required=False)
parser.add_argument('-g','--generate', type=str, default = 'none', help='does not compile but generates a project files of type: '+SUPPORTED_PROJECT_FILES+' ('+SUPPORTED_PROJECT_NAMES+')', required=False)
parser.add_argument('-pg','--gprof', action='store_true', default = False, help='compile with -pg option (for gprof)', required=False)
parser.add_argument('-p','--parallel', default = 1, help='how many threads do you want to use when you compile?  i.e. make -j6', required=False)
parser.add_argument('-i','--init', action = 'store_true', default = False, help='refresh or initialize the buildOptions.txt', required=False)

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

compiler='c++'
compFlags='-Wno-c++98-compat -w -Wall -std=c++11 -O3 -lpthread -pthread'
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

args.generate=args.generate.lower()
if args.generate=='none': # make is default generate option
    args.generate='make'
elif args.generate not in SUPPORTED_PROJECT_FILES.split(','):
    print()
    print("option '--generate' or '-g' was given an unsupported project file type. Valid types are: "+SUPPORTED_PROJECT_FILES)
    print()
    sys.exit()
else: # don't compile if generate option present
    args.noCompile = True

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

# Create modules.h
outFile = open(posixpath.join(pathToMABE,"modules.h"), 'w')
outFile.write('//  MABE is a product of The Hintza Lab @ MSU\n')
outFile.write('//     for general research information:\n')
outFile.write('//         http://hintzelab.msu.edu/\n')
outFile.write('//     for MABE documentation:\n')
outFile.write('//         github.com/Hintzelab/MABE/wiki\n')
outFile.write('//\n')
outFile.write('//  Copyright (c) 2015 Michigan State University. All rights reserved.\n')
outFile.write('//     to view the full license, visit:\n')
outFile.write('//          github.com/Hintzelab/MABE/wiki\n\n')
outFile.write('//  This file was auto-generated with MBuilder.py\n\n')

# modules.h:includes

outFile.write('#ifndef __AutoBuild__Modules__\n')
outFile.write('#define __AutoBuild__Modules__\n')
for option in options["World"]:
    outFile.write('#include "World/'+option+'World/'+option+'World.h"\n')
for option in options["Genome"]:
    outFile.write('#include "Genome/'+option+'Genome/'+option+'Genome.h"\n')
for option in options["Brain"]:
    outFile.write('#include "Brain/'+option+'Brain/'+option+'Brain.h"\n')
for option in options["Optimizer"]:
    outFile.write('#include "Optimizer/'+option+'Optimizer/'+option+'Optimizer.h"\n')
outFile.write('\n#include "Archivist/DefaultArchivist.h"\n')
for option in options["Archivist"]:
    if option != "Default":
        outFile.write('#include "Archivist/'+option+'Archivist/'+option+'Archivist.h"\n')

# modules.h:makeWorld

outFile.write('\n\n//create a world\n')
outFile.write('std::shared_ptr<AbstractWorld> makeWorld(std::shared_ptr<ParametersTable> PT){\n')
outFile.write('  std::shared_ptr<AbstractWorld> newWorld;\n')
outFile.write('  bool found = false;\n')
outFile.write('  std::string worldType = AbstractWorld::worldTypePL->get(PT);\n')
for option in options["World"]:
    outFile.write('  if (worldType == "'+option+'") {\n')
    outFile.write('    newWorld = std::make_shared<'+option+'World>(PT);\n')
    outFile.write('    found = true;\n')    
    outFile.write('    }\n')
outFile.write('  if (!found){\n')
outFile.write('    std::cout << "  ERROR! could not find WORLD-worldType \\"" << worldType << "\\".\\n  Exiting." << std::endl;\n')
outFile.write('    exit(1);\n')
outFile.write('    }\n')
outFile.write('  return newWorld;\n')
outFile.write('}\n')

# modules.h:makeOptimizer

outFile.write('\n\n//create an optimizer\n')
outFile.write('std::shared_ptr<AbstractOptimizer> makeOptimizer(std::shared_ptr<ParametersTable> PT){\n')
outFile.write('  std::shared_ptr<AbstractOptimizer> newOptimizer;\n')
outFile.write('  bool found = false;\n')
outFile.write('  std::string optimizerType = AbstractOptimizer::Optimizer_MethodStrPL->get(PT);\n')
for option in options["Optimizer"]:
    outFile.write('  if (optimizerType == "'+option+'") {\n')
    outFile.write('    newOptimizer = std::make_shared<'+option+'Optimizer>(PT);\n')
    outFile.write('    found = true;\n')    
    outFile.write('    }\n')
outFile.write('  if (!found){\n')
outFile.write('    std::cout << "  ERROR! could not find OPTIMIZER-optimizer \\"" << optimizerType << "\\".\\n  Exiting." << std::endl;\n')
outFile.write('    exit(1);\n')
outFile.write('    }\n')
outFile.write('  return newOptimizer;\n')
outFile.write('}\n')

# modules.h:makeArchivist

outFile.write('\n\n//create an archivist\n')
outFile.write('std::shared_ptr<DefaultArchivist> makeArchivist(std::vector<std::string> popFileColumns, std::shared_ptr<Abstract_MTree> _maxFormula, std::shared_ptr<ParametersTable> PT, std::string groupPrefix = ""){\n')
outFile.write('  std::shared_ptr<DefaultArchivist> newArchivist;\n')
outFile.write('  bool found = false;\n')
outFile.write('  std::string archivistType = DefaultArchivist::Arch_outputMethodStrPL->get(PT);\n')
for option in options["Archivist"]:
    outFile.write('  if (archivistType == "'+option+'") {\n')
    outFile.write('    newArchivist = std::make_shared<'+option+'Archivist>(popFileColumns, _maxFormula, PT, groupPrefix);\n')
    outFile.write('    found = true;\n')    
    outFile.write('    }\n')
outFile.write('  if (!found){\n')
outFile.write('    std::cout << "  ERROR! could not find ARCHIVIST-outputMethod \\"" << archivistType << "\\".\\n  Exiting." << std::endl;\n')
outFile.write('    exit(1);\n')
outFile.write('    }\n')
outFile.write('  return newArchivist;\n')
outFile.write('}\n')

# modules.h:makeTemplateGenome

outFile.write('\n\n//create a template genome\n')
outFile.write('std::shared_ptr<AbstractGenome> makeTemplateGenome(std::shared_ptr<ParametersTable> PT){\n')
outFile.write('  std::shared_ptr<AbstractGenome> newGenome;\n')
outFile.write('  bool found = false;\n')
outFile.write('  std::string genomeType = AbstractGenome::genomeTypeStrPL->get(PT);\n')
for option in options["Genome"]:
    outFile.write('  if (genomeType == "'+option+'") {\n')
    outFile.write('    newGenome = '+option+'Genome_genomeFactory(PT);\n')
    outFile.write('    found = true;\n')    
    outFile.write('    }\n')
outFile.write('  if (found == false){\n')
outFile.write('    std::cout << "  ERROR! could not find GENOME-genomeType \\"" << genomeType << "\\".\\n  Exiting." << std::endl;\n')
outFile.write('    exit(1);\n')
outFile.write('    }\n')
outFile.write('  return newGenome;\n')
outFile.write('}\n')

# modules.h:makeTemplateBrain

outFile.write('\n\n//create a template brain\n')
outFile.write('std::shared_ptr<AbstractBrain> makeTemplateBrain(int inputs, int outputs, std::shared_ptr<ParametersTable> PT){\n')
outFile.write('  std::shared_ptr<AbstractBrain> newBrain;\n')
outFile.write('  bool found = false;\n')
outFile.write('  std::string brainType = AbstractBrain::brainTypeStrPL->get(PT);\n')
for option in options["Brain"]:
    outFile.write('  if (brainType == "'+option+'") {\n')
    outFile.write('    newBrain = '+option+'Brain_brainFactory(inputs, outputs, PT);\n')
    outFile.write('    found = true;\n')    
    outFile.write('    }\n')
outFile.write('  if (found == false){\n')
outFile.write('    std::cout << "  ERROR! could not find BRAIN-brainType \\"" << brainType << "\\".\\n  Exiting." << std::endl;\n')
outFile.write('    exit(1);\n')
outFile.write('    }\n')
outFile.write('  return newBrain;\n')
outFile.write('}\n')

# modules.h:configure Defaults and Documentation

outFile.write('\n\n//configure Defaults and Documentation\n')
outFile.write('void configureDefaultsAndDocumentation(){\n')


outFile.write('  Parameters::root->setParameter("BRAIN-brainType", (std::string)"' + options["Brain"][0] + '");\n')
optionsList = ''
for t in options["Brain"]:
    optionsList += t + ', '
optionsList = optionsList[:-2]
outFile.write('  Parameters::root->setDocumentation("BRAIN-brainType", "brain to be used, [' + optionsList + ']");\n\n')

outFile.write('  Parameters::root->setParameter("GENOME-genomeType", (std::string)"' + options["Genome"][0] + '");\n')
optionsList = ''
for t in options["Genome"]:
    optionsList += t + ', '
optionsList = optionsList[:-2]
outFile.write('  Parameters::root->setDocumentation("GENOME-genomeType", "genome to be used, [' + optionsList + ']");\n\n')

outFile.write('  Parameters::root->setParameter("ARCHIVIST-outputMethod", (std::string)"' + options["Archivist"][0] + '");\n')
optionsList = ''
for t in options["Archivist"]:
    optionsList += t + ', '
optionsList = optionsList[:-2]
outFile.write('  Parameters::root->setDocumentation("ARCHIVIST-outputMethod", "output method, [' + optionsList + ']");\n\n')

outFile.write('  Parameters::root->setParameter("OPTIMIZER-optimizer", (std::string)"' + options["Optimizer"][0] + '");\n')
optionsList = ''
for t in options["Optimizer"]:
    optionsList += t + ', '
optionsList = optionsList[:-2]
outFile.write('  Parameters::root->setDocumentation("OPTIMIZER-optimizer", "optimizer to be used, [' + optionsList + ']");\n\n')

outFile.write('  Parameters::root->setParameter("WORLD-worldType", (std::string)"' + options["World"][0] + '");\n')
optionsList = ''
for t in options["World"]:
    optionsList += t + ', '
optionsList = optionsList[:-2]
outFile.write('  Parameters::root->setDocumentation("WORLD-worldType","world to be used, [' + optionsList + ']");\n')
outFile.write('}\n')

outFile.write('\n\n#endif /* __AutoBuild__Modules__ */\n')

outFile.close()


# Create a project file of type in SUPPORTED_PROJECT_FILES
options['Archivist'].remove('Default')
alwaysSources=['main.cpp','Global.cpp','Group/Group.cpp','Organism/Organism.cpp','Utilities/Data.cpp','Utilities/Parameters.cpp','Utilities/Loader.cpp','World/AbstractWorld.cpp','Genome/AbstractGenome.cpp','Brain/AbstractBrain.cpp','Optimizer/AbstractOptimizer.cpp','Archivist/DefaultArchivist.cpp','Utilities/zupply.cpp']
moduleSources = []
objects = []
sources = None
#alwaysSources = [e.replace('/',sep) for e in alwaysSources]

## fields for source units (one unit represents one source file cpp,h,etc)
f_filename=0
f_compilecpp=1
f_folder=2
f_compile=3
f_link=4
f_priority=5
f_overridebuildcmd=6
f_buildcmd=7
f_uuid=8
f_filerefuuid=9
def getSourceFilesByBuildOptions(sep='/'):
    def newUnit():
        return ['','1','','1','1','1000','0','','','']
    units=[]
    for eachsource in alwaysSources:
        unit=newUnit()
        unit[f_filename]=eachsource.replace('/',sep)
        lastSlash=eachsource.rfind('/')
        if lastSlash!=-1:
            unit[f_folder]=eachsource[0:lastSlash]
        units.append(unit)
        if os.path.isfile(eachsource.replace('cpp','h')): ## add possible header
            unitH=newUnit()
            unitH[f_filename]=eachsource.replace('cpp','h')
            unitH[f_folder] = unit[f_folder]
            units.append(unitH)
    for category in options: ## buildOptions dictionary
        for module in options[category]:
            filename=category+sep+module+category+sep+module+category+'.cpp'
            foldername=category+'/'+module+category
            unit=newUnit()
            unit[f_filename]=filename
            unit[f_folder]=foldername
            units.append(unit)
            filename=category+sep+module+category+sep+module+category+'.h'
            foldername=category+'/'+module+category
            unit=newUnit()
            unit[f_filename]=filename
            unit[f_folder]=foldername
            units.append(unit)
            dirs = [d for d in os.listdir(category+'/'+module+category+'/') if os.path.isdir(os.path.join(pathToMABE,category,module+category,d))]
            for eachdir in dirs:
                contents = [c for c in os.listdir(category+'/'+module+category+'/'+eachdir+'/') if (('.cpp' in c) or ('.h' in c)) and c.startswith('.')==False] ## include cpp/h/hpp files and ignore hidden files
                for content in contents:
                    filename=category+'/'+module+category+'/'+eachdir+'/'+content
                    filename=filename.replace('/',sep)
                    foldername=category+'/'+module+category+'/'+eachdir
                    unit=newUnit()
                    unit[f_filename]=filename
                    unit[f_folder]=foldername
                    units.append(unit)
    unit=newUnit()
    unit[f_filename]='modules.h'
    units.append(unit)
    sortedunits=sorted(units, key=lambda x: x[f_folder])
    return sortedunits

## create git version integration
## Create an empty file if git is not available
## Otherwise capture commit hash
gitExists = subprocess.run("git --version",shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE).stdout.startswith(b"git version")
with open(os.path.join("Utilities","gitversion.h"),'w') as file:
    if gitExists:
        commitHash = str(subprocess.run("git rev-parse HEAD",shell=True,stdout=subprocess.PIPE).stdout.decode("utf-8").strip())
        file.write('const char *gitversion = "{gitversion}";\n'.format(gitversion=commitHash))
    else:
        file.write('const char *gitversion = "";\n')
touch("main.cpp") ## IDE-independent signal to recompile main.o

# Create a make file if requested (default)
if args.generate == 'make': ## GENERATE make
    if not posixpath.exists('objectFiles'):
        os.makedirs('objectFiles')

    for o in options: ## buildOptions dictionary
        for t in options[o]: ## specific modules in a category
            moduleSources.append(pathToMABE+o+'/'+t+o+'/'+t+o+'.cpp')
            dirs = [d for d in os.listdir(pathToMABE+o+'/'+t+o+'/') if posixpath.isdir(posixpath.join(pathToMABE+'/'+o+'/'+t+o+'/', d))]
            for d in dirs:
                contents = [c for c in os.listdir(pathToMABE+o+'/'+t+o+'/'+d+'/') if '.cpp' in c and c.startswith('.')==False] ## include cpp files and ignore hidden files
                for content in contents:
                    moduleSources.append(pathToMABE+o+'/'+t+o+'/'+d+'/'+content)

    alwaysSources = [pathToMABE+e for e in alwaysSources]
    sources = alwaysSources + moduleSources


    for s in sources:
        objects.append('objectFiles/'+posixpath.relpath(s).split('.')[0].replace('/','_') + '.o')

    outFile = open("makefile", 'w')

    outFile.write('all: '+product+'\n\n')

    outFile.write(product+':')
    for o in objects:
        outFile.write(' '+o)
    outFile.write('\n')
    outFile.write('\t'+compiler+' '+compFlags)
    for o in objects:
        outFile.write(' '+o)
    outFile.write(' -o '+product+'\n\n')

    for i in range(len(sources)):
        if (sources[i][-8:] == 'main.cpp'):
            outFile.write(objects[i]+': '+sources[i]+'\n')
        else:
            outFile.write(objects[i]+': '+sources[i]+' '+sources[i].replace('.cpp','.h')+'\n')
        outFile.write('\t'+compiler+' '+compFlags+' -c '+sources[i]+' -o '+objects[i]+'\n\n')
        
    outFile.write('clean:\n')
    outFile.write('\trm -r objectFiles/* '+product+'\n\n')

    outFile.write('cleanup:\n')
    outFile.write('\trm -r objectFiles/*\n')

    outFile.close()
elif args.generate == 'devcpp': ## GENERATE devcpp
    units=getSourceFilesByBuildOptions(sep='\\')
    folders=[]
    for eachunit in units:
        folders.append(eachunit[f_folder])
    folders=sorted(list(filter(bool,list(set(folders))))) #removes dups & empties, then sorts
    folders=','.join(folders)

    outString = ''
    outString += """[Project]
FileName=MABE.dev
Name=mabe
Type=1
Ver=2
ObjFiles=
Includes=
Libs=
PrivateResource=
ResourceIncludes=
MakeIncludes=
Compiler=
CppCompiler=
Linker=
IsCpp=1
Icon=
ExeOutput=
ObjectOutput=
LogOutput=
LogOutputEnabled=0
OverrideOutput=0
OverrideOutputName=
HostApplication=
UseCustomMakefile=0
CustomMakefile=
CommandLine=
Folders={0}
IncludeVersionInfo=0
SupportXPThemes=0
CompilerSet=0
CompilerSettings=00000000g0000000000000000
UnitCount={1}

[VersionInfo]
Major=1
Minor=0
Release=0
Build=0
LanguageID=1033
CharsetID=1252
CompanyName=
FileVersion=
FileDescription=Developed using the Dev-C++ IDE
InternalName=
LegalCopyright=
LegalTrademarks=
OriginalFilename=
ProductName=
ProductVersion=
AutoIncBuildNr=0
SyncProduct=1


""".format(folders,str(len(units)))
    for i,eachUnit in enumerate(units):
        outString += """[Unit{0}]
FileName={1}
CompileCpp={2}
Folder={3}
Compile={4}
Link={5}
Priority={6}
OverrideBuildCmd={7}
BuildCmd={8}

""".format(i+1,
        eachUnit[f_filename],
        eachUnit[f_compilecpp],
        eachUnit[f_folder],
        eachUnit[f_compile],
        eachUnit[f_link],
        eachUnit[f_priority],
        eachUnit[f_overridebuildcmd],
        eachUnit[f_buildcmd])

    with open('MABE.dev','w') as outfile:
        outfile.write(outString)
elif args.generate == 'vs': ## GENERATE vs
    units=getSourceFilesByBuildOptions(sep='\\') ## build source files list, using proper windows path separator

    outString = ''
    SDKversion = "10.0.16299.0" ## assume win 10...
    platformToolset = "v"
    if platform.system() == 'Windows':
        try:
            hkey = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows")
            #SDKversion = winreg.EnumValue(hkey, 0)[1]
            SDKversion = str(winreg.QueryValueEx(hkey, "CurrentVersion")[0])
            firstPeriod = SDKversion.find('.')
            secondPeriod = SDKversion.find('.',firstPeriod+1)
            SDKversion = SDKversion[0:secondPeriod]
        except:
            try:
                hkey = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots")
                SDKversion = winreg.EnumKey(hkey, 0)
            except:
                print("Warning: Unknown windows version. You will need to retarget the generated project manually in Visual Studio.")
                print("Warning: Could not find Windows SDK version information on this machine.")
                print("Warning: Please contact the authors so they can identify your setup and add it to identification.")
        hkey = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\debug\\x64")
        platformToolset += str(winreg.QueryValueEx(hkey,"Major")[0]) + str(winreg.QueryValueEx(hkey,"Minor")[0])[0]

    outString += """<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|Win32">
      <Configuration>Debug</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|Win32">
      <Configuration>Release</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <VCProjectVersion>15.0</VCProjectVersion>
    <ProjectGuid>{{{0}}}</ProjectGuid>
    <Keyword>Win32Proj</Keyword>
    <WindowsTargetPlatformVersion>{1}</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>{2}</PlatformToolset>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>{2}</PlatformToolset>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>{2}</PlatformToolset>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>{2}</PlatformToolset>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings">
  </ImportGroup>
  <ImportGroup Label="Shared">
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <PropertyGroup Label="UserMacros" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <LinkIncremental>true</LinkIncremental>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <LinkIncremental>true</LinkIncremental>
  </PropertyGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <ClCompile>
      <PreprocessorDefinitions>WIN32;_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>
      <WarningLevel>Level3</WarningLevel>
      <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>
      <Optimization>Disabled</Optimization>
    </ClCompile>
    <Link>
      <TargetMachine>MachineX86</TargetMachine>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <SubSystem>Console</SubSystem>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <ClCompile>
      <PreprocessorDefinitions>WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>
      <WarningLevel>Level3</WarningLevel>
      <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>
    </ClCompile>
    <Link>
      <TargetMachine>MachineX86</TargetMachine>
      <GenerateDebugInformation>true</GenerateDebugInformation>
      <SubSystem>Console</SubSystem>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
    </Link>
  </ItemDefinitionGroup>
""".format(str(uuid.uuid4()),SDKversion,platformToolset)
    outString += "  <ItemGroup>\n" ## start cpp list
    for eachunit in units:
        if eachunit[f_filename].endswith('.cpp'):
            outString += '    <ClCompile Include="{0}" />\n'.format(eachunit[f_filename])
    outString += "  </ItemGroup>\n"
    outString += "  <ItemGroup>\n" ## start header list
    for eachunit in units:
        if eachunit[f_filename].endswith('.h'):
            outString += '    <ClInclude Include="{0}" />\n'.format(eachunit[f_filename])
    outString += "  </ItemGroup>"
    outString += """
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ImportGroup Label="ExtensionTargets">
  </ImportGroup>
</Project>
"""
    with open('MABE.vcxproj','w') as outfile:
        outfile.write(outString)
elif args.generate == 'xcode':
    def newXcodeUUID():
        return ''.join(str(uuid.uuid4()).upper().split('-')[1:])
    units=getSourceFilesByBuildOptions(sep='/')
    for unit in units: ## create XUUIDs for each file
        unit[f_uuid]=newXcodeUUID()
        unit[f_filerefuuid]=newXcodeUUID()
    folders=[]
    for eachunit in units:
        folders.append(eachunit[f_folder])
    folders=sorted(list(filter(bool,list(set(folders))))) #removes dups & empties, then sorts
    folderuuids={}
    for folder in folders:
        folderuuids[folder] = newXcodeUUID()
    folderFiles=collections.defaultdict(list)
    for unit in units:
        foldername=unit[f_folder]
        folderFiles[foldername].append( (unit[f_filename], True, unit[f_filerefuuid]) ) ## true for files, false for folders
    for folderName in folders:
        if '/' in folderName:
            parentName=folderName[0:folderName.rfind('/')]
            folderFiles[parentName].append( (folderName, False, folderuuids[folderName]) )
        else:
            folderFiles[''].append( (folderName, False, folderuuids[folderName]) ) ## mabe-root is ''
    ## create UUIDs
    rootObjUUID=newXcodeUUID()
    productUUID=newXcodeUUID()
    copyPhaseUUID=newXcodeUUID()
    frameworksPhaseUUID=newXcodeUUID()
    sourcesPhaseUUID=newXcodeUUID()
    buildConfigurationListTargetUUID=newXcodeUUID()
    buildConfigurationListProjectUUID=newXcodeUUID()
    mainGroupUUID=newXcodeUUID()
    mainGroupProductUUID=newXcodeUUID()
    mainGroupProductsUUID=newXcodeUUID()
    nativeTargetUUID=newXcodeUUID()
    configurationDebugUUID=newXcodeUUID()
    configurationReleaseUUID=newXcodeUUID()
    buildConfigurationDebugUUID=newXcodeUUID()
    buildConfigurationReleaseUUID=newXcodeUUID()
    outString = '''// !$*UTF9*$!
{{
	archiveVersion = 1;
	classes = {{
	}};
	objectVersion = 48;
	objects = {{

/* Begin PBXBuildFile section */
'''.format()
    for unit in units:
        if unit[f_filename].endswith('.cpp'):
            outString += "		{0} /* {1} in Sources */ = {{isa = PBXBuildFile; fileRef = {2} /* {1} */; }};\n".format( unit[f_uuid], unit[f_filename], unit[f_filerefuuid] )
    outString += '''/* End PBXBuildFile section */

/* Begin PBXCopyFilesBuildPhase section */
		{0} /* CopyFiles */ = {{
			isa = PBXCopyFilesBuildPhase;
			buildActionMask = 2147483647;
			dstPath = /usr/share/man/man1/;
			dstSubfolderSpec = 0;
			files = (
			);
			runOnlyForDeploymentPostprocessing = 1;
		}};
/* End PBXCopyFilesBuildPhase section */

/* Begin PBXFileReference section */
'''.format(copyPhaseUUID)
    outString += '		{0} /* MABE */ = {{isa = PBXFileReference; explicitFileType = "compiled.mach-o.executable"; includeInIndex = 0; path = MABE; sourceTree = BUILT_PRODUCTS_DIR; }};\n'.format(productUUID)
    for unit in units:
        if 'main.cpp' == unit[f_filename]: ## 'main.cpp' file
            outString += '		{0} /* {1} */ = {{isa = PBXFileReference; lastKnownFileType = sourcecode.cpp.cpp; path = {1}; sourceTree = "<group>"; }};\n'.format(unit[f_filerefuuid], unit[f_filename][unit[f_filename].rfind('/')+1:])
        elif unit[f_filename].endswith('.h'): ## '.h' file
            outString += '		{0} /* {1} */ = {{isa = PBXFileReference; lastKnownFileType = sourcecode.c.h; path = {1}; sourceTree = "<group>"; }};\n'.format(unit[f_filerefuuid], unit[f_filename][unit[f_filename].rfind('/')+1:])
        else: ## '.cpp' file
            outString += '		{0} /* {1} */ = {{isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = sourcecode.cpp.cpp; path = {1}; sourceTree = "<group>"; }};\n'.format(unit[f_filerefuuid], unit[f_filename][unit[f_filename].rfind('/')+1:])
    outString += '''/* End PBXFileReference section */

/* Begin PBXFrameworksBuildPhase section */
		{0} /* Frameworks */ = {{
			isa = PBXFrameworksBuildPhase;
			buildActionMask = 2147483647;
			files = (
			);
			runOnlyForDeploymentPostprocessing = 0;
		}};
/* End PBXFrameworksBuildPhase section */

/* Begin PBXGroup section */
		{1} = {{
			isa = PBXGroup;
			children = (
				{2} /* MABE */,
				{3} /* Products */,
			);
			sourceTree = "<group>";
		}};
		{3} /* Products */ = {{
			isa = PBXGroup;
			children = (
				{4} /* MABE */,
			);
			name = Products;
			sourceTree = "<group>";
		}};
		{2} /* MABE */ = {{
			isa = PBXGroup;
			children = (
'''.format(frameworksPhaseUUID, mainGroupUUID, mainGroupProductUUID, mainGroupProductsUUID, productUUID)
    #for unit in units:
    #    outString += '				{0} /* {1} */,\n'.format(unit[f_filerefuuid], unit[f_filename])
    for item in folderFiles['']: ## loop through MABE-root items
        outString += '				{0} /* {1} */,\n'.format(item[2],item[0])
    outString += '''			);
			path = ./;
			sourceTree = "<group>";
		}};
'''.format()
    for folder,items in folderFiles.items():
        if folder:
            outString += '''		{0} /* {1} */ = {{
			isa = PBXGroup;
			children = (
'''.format(folderuuids[folder],folder[folder.rfind('/')+1:])
            for item in items:
                outString += '			{0} /* {1} */,\n'.format(item[2],item[0][item[0].rfind('/')+1:])
            outString += '''			);
			path = {0};
			sourceTree = "<group>";
		}};
'''.format(folder[folder.rfind('/')+1:])
    outString += '''
/* End PBXGroup section */

/* Begin PBXNativeTarget section */
'''.format()
    outString += '''		{0} /* MABE */ = {{
			isa = PBXNativeTarget;
			buildConfigurationList = {1} /* Build configuration list for PBXNativeTarget "MABE" */;
			buildPhases = (
				{2} /* Sources */,
				{3} /* Frameworks */,
				{4} /* CopyFiles */,
			);
			buildRules = (
			);
			dependencies = (
			);
			name = MABE;
			productName = mabe;
			productReference = {5} /* MABE */;
			productType = "com.apple.product-type.tool";
                }};
/* End PBXNativeTarget section */

/* Begin PBXProject section */
'''.format(nativeTargetUUID, buildConfigurationListTargetUUID, sourcesPhaseUUID, frameworksPhaseUUID, copyPhaseUUID, productUUID)
    outString += '''		{0} /* Project object */ = {{
			isa = PBXProject;
			attributes = {{
				LastUpgradeCheck = 9910;
				ORGANIZATIONNAME = MabeUser;
				TargetAttributes = {{
					{1} = {{
						CreatedOnToolsVersion = 9.1;
						ProvisioningStyle = Automatic;
					}};
				}};
			}};
'''.format(rootObjUUID, nativeTargetUUID)
    outString += '''			buildConfigurationList = {0} /* Build configuration list for PBXProject "MABE" */;
			compatibilityVersion = "Xcode 8.0";
			developmentRegion = en;
			hasScannedForEncodings = 0;
			knownRegions = (
				en,
			);
'''.format(buildConfigurationListProjectUUID)
    outString += '''			mainGroup = {0};
			productRefGroup = {1} /* Products */;
			projectDirPath = "";
			projectRoot = "";
			targets = (
				{2} /* MABE */,
			);
		}};
/* End PBXProject section */

/* Begin PBXSourcesBuildPhase section */
'''.format(mainGroupUUID, mainGroupProductsUUID, nativeTargetUUID)
    outString += '''		{0} /* Sources */ = {{
			isa = PBXSourcesBuildPhase;
			buildActionMask = 2147483647;
			files = (
'''.format(sourcesPhaseUUID)
    for unit in units:
        if unit[f_filename].endswith('.cpp'):
            outString += '				{0} /* {1} in Sources */,\n'.format(unit[f_uuid], unit[f_filename])
    outString += '''			);
			runOnlyForDeploymentPostprocessing = 0;
		}};
/* End PBXSourcesBuildPhase section */

/* Begin XCBuildConfiguration section */
		{0} /* Debug */ = {{'''.format(configurationDebugUUID)
    outString += '''
			isa = XCBuildConfiguration;
			buildSettings = {{
				ALWAYS_SEARCH_USER_PATHS = NO;
				CLANG_ANALYZER_NONNULL = YES;
				CLANG_ANALYZER_NUMBER_OBJECT_CONVERSION = YES_AGGRESSIVE;
				CLANG_CXX_LANGUAGE_STANDARD = "gnu++14";
				CLANG_CXX_LIBRARY = "libc++";
				CLANG_ENABLE_MODULES = YES;
				CLANG_ENABLE_OBJC_ARC = YES;
				CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = YES;
				CLANG_WARN_BOOL_CONVERSION = YES;
				CLANG_WARN_COMMA = YES;
				CLANG_WARN_CONSTANT_CONVERSION = YES;
				CLANG_WARN_DIRECT_OBJC_ISA_USAGE = YES_ERROR;
				CLANG_WARN_DOCUMENTATION_COMMENTS = YES;
				CLANG_WARN_EMPTY_BODY = YES;
				CLANG_WARN_ENUM_CONVERSION = YES;
				CLANG_WARN_INFINITE_RECURSION = YES;
				CLANG_WARN_INT_CONVERSION = YES;
				CLANG_WARN_NON_LITERAL_NULL_CONVERSION = YES;
				CLANG_WARN_OBJC_LITERAL_CONVERSION = YES;
				CLANG_WARN_OBJC_ROOT_CLASS = YES_ERROR;
				CLANG_WARN_RANGE_LOOP_ANALYSIS = YES;
				CLANG_WARN_STRICT_PROTOTYPES = YES;
				CLANG_WARN_SUSPICIOUS_MOVE = YES;
				CLANG_WARN_UNGUARDED_AVAILABILITY = YES_AGGRESSIVE;
				CLANG_WARN_UNREACHABLE_CODE = YES;
				CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
				CODE_SIGN_IDENTITY = "-";
				COPY_PHASE_STRIP = NO;
				DEBUG_INFORMATION_FORMAT = dwarf;
				ENABLE_STRICT_OBJC_MSGSEND = YES;
				ENABLE_TESTABILITY = YES;
				GCC_C_LANGUAGE_STANDARD = gnu11;
				GCC_DYNAMIC_NO_PIC = NO;
				GCC_NO_COMMON_BLOCKS = YES;
				GCC_OPTIMIZATION_LEVEL = 0;
				GCC_PREPROCESSOR_DEFINITIONS = (
					"DEBUG=0",
					"$(inherited)",
				);
				GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
				GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;
				GCC_WARN_UNDECLARED_SELECTOR = YES;
				GCC_WARN_UNINITIALIZED_AUTOS = YES_AGGRESSIVE;
				GCC_WARN_UNUSED_FUNCTION = YES;
				GCC_WARN_UNUSED_VARIABLE = YES;
				MACOSX_DEPLOYMENT_TARGET = 10.13;
				MTL_ENABLE_DEBUG_INFO = YES;
				ONLY_ACTIVE_ARCH = YES;
				SDKROOT = macosx;
			}};
			name = Debug;
		}};
'''.format()
    outString += '		{0} /* Release */ = {{'.format(configurationReleaseUUID)
    outString += '''
			isa = XCBuildConfiguration;
			buildSettings = {{
				ALWAYS_SEARCH_USER_PATHS = NO;
				CLANG_ANALYZER_NONNULL = YES;
				CLANG_ANALYZER_NUMBER_OBJECT_CONVERSION = YES_AGGRESSIVE;
				CLANG_CXX_LANGUAGE_STANDARD = "gnu++14";
				CLANG_CXX_LIBRARY = "libc++";
				CLANG_ENABLE_MODULES = YES;
				CLANG_ENABLE_OBJC_ARC = YES;
				CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = YES;
				CLANG_WARN_BOOL_CONVERSION = YES;
				CLANG_WARN_COMMA = YES;
				CLANG_WARN_CONSTANT_CONVERSION = YES;
				CLANG_WARN_DIRECT_OBJC_ISA_USAGE = YES_ERROR;
				CLANG_WARN_DOCUMENTATION_COMMENTS = YES;
				CLANG_WARN_EMPTY_BODY = YES;
				CLANG_WARN_ENUM_CONVERSION = YES;
				CLANG_WARN_INFINITE_RECURSION = YES;
				CLANG_WARN_INT_CONVERSION = YES;
				CLANG_WARN_NON_LITERAL_NULL_CONVERSION = YES;
				CLANG_WARN_OBJC_LITERAL_CONVERSION = YES;
				CLANG_WARN_OBJC_ROOT_CLASS = YES_ERROR;
				CLANG_WARN_RANGE_LOOP_ANALYSIS = YES;
				CLANG_WARN_STRICT_PROTOTYPES = YES;
				CLANG_WARN_SUSPICIOUS_MOVE = YES;
				CLANG_WARN_UNGUARDED_AVAILABILITY = YES_AGGRESSIVE;
				CLANG_WARN_UNREACHABLE_CODE = YES;
				CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
				CODE_SIGN_IDENTITY = "-";
				COPY_PHASE_STRIP = NO;
				DEBUG_INFORMATION_FORMAT = "dwarf-with-dsym";
				ENABLE_NS_ASSERTIONS = NO;
				ENABLE_STRICT_OBJC_MSGSEND = YES;
				GCC_C_LANGUAGE_STANDARD = gnu11;
				GCC_NO_COMMON_BLOCKS = YES;
				GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
				GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;
				GCC_WARN_UNDECLARED_SELECTOR = YES;
				GCC_WARN_UNINITIALIZED_AUTOS = YES_AGGRESSIVE;
				GCC_WARN_UNUSED_FUNCTION = YES;
				GCC_WARN_UNUSED_VARIABLE = YES;
				MACOSX_DEPLOYMENT_TARGET = 10.13;
				MTL_ENABLE_DEBUG_INFO = NO;
				SDKROOT = macosx;
			}};
			name = Release;
		}};'''.format()
    outString += '''
		{0} /* Debug */ = {{
			isa = XCBuildConfiguration;
			buildSettings = {{
				CODE_SIGN_STYLE = Automatic;
				PRODUCT_NAME = "$(TARGET_NAME)";
			}};
			name = Debug;
		}};
		{1} /* Release */ = {{
			isa = XCBuildConfiguration;
			buildSettings = {{
				CODE_SIGN_STYLE = Automatic;
				PRODUCT_NAME = "$(TARGET_NAME)";
			}};
			name = Release;
		}};
/* End XCBuildConfiguration section */

/* Begin XCConfigurationList section */'''.format(buildConfigurationDebugUUID, buildConfigurationReleaseUUID)
    outString += '''
		{0} /* Build configuration list for PBXProject "doggie" */ = {{
			isa = XCConfigurationList;
			buildConfigurations = (
				{1} /* Debug */,
				{2} /* Release */,
			);
			defaultConfigurationIsVisible = 0;
			defaultConfigurationName = Release;
		}};
		{3} /* Build configuration list for PBXNativeTarget "doggie" */ = {{
			isa = XCConfigurationList;
			buildConfigurations = (
				{4} /* Debug */,
				{5} /* Release */,
			);
			defaultConfigurationIsVisible = 0;
			defaultConfigurationName = Release;
		}};
/* End XCConfigurationList section */
	}};
	rootObject = {6} /* Project object */;
}}
'''.format(buildConfigurationListProjectUUID,
        configurationDebugUUID,
        configurationReleaseUUID,
        buildConfigurationListTargetUUID,
        buildConfigurationDebugUUID,
        buildConfigurationReleaseUUID,
        rootObjUUID)
    if not os.path.isdir('MABE.xcodeproj'):
        os.mkdir('MABE.xcodeproj')
    with open('MABE.xcodeproj/project.pbxproj','w') as outfile:
        outfile.write(outString)
if args.generate == 'cb':
    targets='''
			<Option target="Release x64" />
			<Option target="Debug Win32" />
			<Option target="Release Win32" />
			<Option target="Debug x64" />'''
    units=getSourceFilesByBuildOptions(sep='/')
    outString = '''<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<CodeBlocks_project_file>
	<FileVersion major="1" minor="6" />
	<Project>
		<Option title="MABE" />
		<Option pch_mode="2" />
		<Option compiler="gcc" />
		<Build>
			<Target title="Release x64">
				<Option output="mabe" prefix_auto="1" extension_auto="1" />
				<Option type="0" />
				<Option compiler="gcc" />
			</Target>
			<Target title="Debug x64">
				<Option output="mabe" prefix_auto="1" extension_auto="1" />
				<Option type="0" />
				<Option compiler="gcc" />
			</Target>
		</Build>'''
    for unit in units:
        outString += '''
		<Unit filename="{0}">{1}
		</Unit>'''.format(unit[f_filename], targets)
    outString += '''
		<Extensions>
			<code_completion />
			<envvars />
			<debugger />
		</Extensions>
	</Project>
</CodeBlocks_project_file>
'''
    with open('MABE.cbp','w') as outfile:
        outfile.write(outString)
    print("In order for MABE to build properly in Code::Blocks the following flags need to be added to the 'Other Linker Options' section under Settings > Compiler ... > Linker Settings \n '-lpthread' \n '-pthread' ")

if not (args.noCompile):
    call(["make","-j"+str(args.parallel)])
if (args.cleanup):
    call(["make","cleanup"])
    call(["rm","makefile"])
    call(["rmdir","objectFiles"])
