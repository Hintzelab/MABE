import argparse
import os
import posixpath
import sys
import platform ## system identification
import uuid ## unique guid generator for vs project files
from subprocess import call
if platform.system() == 'Windows':
    import winreg

parser = argparse.ArgumentParser()

SUPPORTED_PROJECT_FILES='make,vs,xcode,devcpp'

parser.add_argument('-b','--buildOptions', type=str, metavar='FILE', default = 'buildOptions.txt',  help=' name of file with build options - default : buildOptions.txt', required=False)
parser.add_argument('-c','--cleanup', action='store_true', default = False, help='add this flag if you want build files (including make) removed after building', required=False)
parser.add_argument('-nc','--noCompile', action='store_true', default = False, help='create modules.h and makefile, but do not compile', required=False)
parser.add_argument('-g','--generate', type=str, default = 'none', help='does not compile but generates a project files of type: '+SUPPORTED_PROJECT_FILES, required=False)
parser.add_argument('-pg','--gprof', action='store_true', default = False, help='compile with -pg option (for gprof)', required=False)
parser.add_argument('-p','--parallel', default = 1, help='how many threads do you want to use when you compile?  i.e. make -j6', required=False)

args = parser.parse_args()

if platform.system() == 'Windows':
    product = 'MABE.exe'
else:
    product = 'MABE'

compiler='c++'
compFlags='-Wno-c++98-compat -w -Wall -std=c++11 -O3'
if (args.gprof):
    compFlags =  compFlags + ' -pg'

args.generate=args.generate.lower()
if args.generate not in SUPPORTED_PROJECT_FILES.split(','):
    print()
    print("option '--generate' or '-g' was given an unsupported project file type. Valid types are: "+SUPPORTED_PROJECT_FILES)
    print()
    sys.exit()
if args.generate!='none': # don't compile if generate option present
    args.noCompile = True
if args.generate=='none': # make is default generate option
    args.generate='make'

if posixpath.exists(args.buildOptions) is False:
    print()
    print('buildOptions file with name "'+args.buildOptions+'" does not exist. Please provide a diffrent filename.')
    print()
    sys.exit();
    
# load all lines from buildOptions into lines, ignore blank lines
file = open(args.buildOptions, 'r')
pathToMABE=posixpath.dirname(args.buildOptions)
if pathToMABE == "":
    pathToMABE = "./"
lines = [line.rstrip('\n').split() for line in file if line.rstrip('\n').split() not in [[],['EOF']] ]
file.close()

options = {'World':[],'Genome':[],'Brain':[],'Optimizer':[],'Archivist':[],}
currentOption = ""

for line in lines:
    if len(line) != 2:
        exit()
    if line[0] == '%':
        currentOption = line[1]
    if line[0] == '*':
        options[currentOption].append(line[1])
        options[currentOption][0], options[currentOption][len(options[currentOption])-1] = options[currentOption][len(options[currentOption])-1], options[currentOption][0]
    if line[0] == '+':
        options[currentOption].append(line[1])


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
outFile.write('shared_ptr<AbstractWorld> makeWorld(shared_ptr<ParametersTable> PT){\n')
outFile.write('  shared_ptr<AbstractWorld> newWorld;\n')
outFile.write('  bool found = false;\n')
outFile.write('  string worldType = AbstractWorld::worldTypePL->get(PT);\n')
for option in options["World"]:
    outFile.write('  if (worldType == "'+option+'") {\n')
    outFile.write('    newWorld = make_shared<'+option+'World>(PT);\n')
    outFile.write('    found = true;\n')    
    outFile.write('    }\n')
outFile.write('  if (!found){\n')
outFile.write('    cout << "  ERROR! could not find WORLD-worldType \\"" << worldType << "\\".\\n  Exiting." << endl;\n')
outFile.write('    exit(1);\n')
outFile.write('    }\n')
outFile.write('  return newWorld;\n')
outFile.write('}\n')

# modules.h:makeOptimizer

outFile.write('\n\n//create an optimizer\n')
outFile.write('shared_ptr<AbstractOptimizer> makeOptimizer(shared_ptr<ParametersTable> PT){\n')
outFile.write('  shared_ptr<AbstractOptimizer> newOptimizer;\n')
outFile.write('  bool found = false;\n')
outFile.write('  string optimizerType = AbstractOptimizer::Optimizer_MethodStrPL->get(PT);\n')
for option in options["Optimizer"]:
    outFile.write('  if (optimizerType == "'+option+'") {\n')
    outFile.write('    newOptimizer = make_shared<'+option+'Optimizer>(PT);\n')
    outFile.write('    found = true;\n')    
    outFile.write('    }\n')
outFile.write('  if (!found){\n')
outFile.write('    cout << "  ERROR! could not find OPTIMIZER-optimizer \\"" << optimizerType << "\\".\\n  Exiting." << endl;\n')
outFile.write('    exit(1);\n')
outFile.write('    }\n')
outFile.write('  return newOptimizer;\n')
outFile.write('}\n')

# modules.h:makeArchivist

outFile.write('\n\n//create an archivist\n')
outFile.write('shared_ptr<DefaultArchivist> makeArchivist(vector<string> popFileColumns, shared_ptr<Abstract_MTree> _maxFormula, shared_ptr<ParametersTable> PT, string groupPrefix = ""){\n')
outFile.write('  shared_ptr<DefaultArchivist> newArchivist;\n')
outFile.write('  bool found = false;\n')
outFile.write('  string archivistType = DefaultArchivist::Arch_outputMethodStrPL->get(PT);\n')
for option in options["Archivist"]:
    outFile.write('  if (archivistType == "'+option+'") {\n')
    outFile.write('    newArchivist = make_shared<'+option+'Archivist>(popFileColumns, _maxFormula, PT, groupPrefix);\n')
    outFile.write('    found = true;\n')    
    outFile.write('    }\n')
outFile.write('  if (!found){\n')
outFile.write('    cout << "  ERROR! could not find ARCHIVIST-outputMethod \\"" << archivistType << "\\".\\n  Exiting." << endl;\n')
outFile.write('    exit(1);\n')
outFile.write('    }\n')
outFile.write('  return newArchivist;\n')
outFile.write('}\n')

# modules.h:makeTemplateGenome

outFile.write('\n\n//create a template genome\n')
outFile.write('shared_ptr<AbstractGenome> makeTemplateGenome(shared_ptr<ParametersTable> PT){\n')
outFile.write('  shared_ptr<AbstractGenome> newGenome;\n')
outFile.write('  bool found = false;\n')
outFile.write('  string genomeType = AbstractGenome::genomeTypeStrPL->get(PT);\n')
for option in options["Genome"]:
    outFile.write('  if (genomeType == "'+option+'") {\n')
    outFile.write('    newGenome = '+option+'Genome_genomeFactory(PT);\n')
    outFile.write('    found = true;\n')    
    outFile.write('    }\n')
outFile.write('  if (found == false){\n')
outFile.write('    cout << "  ERROR! could not find GENOME-genomeType \\"" << genomeType << "\\".\\n  Exiting." << endl;\n')
outFile.write('    exit(1);\n')
outFile.write('    }\n')
outFile.write('  return newGenome;\n')
outFile.write('}\n')

# modules.h:makeTemplateBrain

outFile.write('\n\n//create a template brain\n')
outFile.write('shared_ptr<AbstractBrain> makeTemplateBrain(int inputs, int outputs, shared_ptr<ParametersTable> PT){\n')
outFile.write('  shared_ptr<AbstractBrain> newBrain;\n')
outFile.write('  bool found = false;\n')
outFile.write('  string brainType = AbstractBrain::brainTypeStrPL->get(PT);\n')
for option in options["Brain"]:
    outFile.write('  if (brainType == "'+option+'") {\n')
    outFile.write('    newBrain = '+option+'Brain_brainFactory(inputs, outputs, PT);\n')
    outFile.write('    found = true;\n')    
    outFile.write('    }\n')
outFile.write('  if (found == false){\n')
outFile.write('    cout << "  ERROR! could not find BRAIN-brainType \\"" << brainType << "\\".\\n  Exiting." << endl;\n')
outFile.write('    exit(1);\n')
outFile.write('    }\n')
outFile.write('  return newBrain;\n')
outFile.write('}\n')

# modules.h:configure Defaults and Documentation

outFile.write('\n\n//configure Defaults and Documentation\n')
outFile.write('void configureDefaultsAndDocumentation(){\n')


outFile.write('  Parameters::root->setParameter("BRAIN-brainType", (string)"' + options["Brain"][0] + '");\n')
optionsList = ''
for t in options["Brain"]:
    optionsList += t + ', '
optionsList = optionsList[:-2]
outFile.write('  Parameters::root->setDocumentation("BRAIN-brainType", "brain to be used, [' + optionsList + ']");\n\n')

outFile.write('  Parameters::root->setParameter("GENOME-genomeType", (string)"' + options["Genome"][0] + '");\n')
optionsList = ''
for t in options["Genome"]:
    optionsList += t + ', '
optionsList = optionsList[:-2]
outFile.write('  Parameters::root->setDocumentation("GENOME-genomeType", "genome to be used, [' + optionsList + ']");\n\n')

outFile.write('  Parameters::root->setParameter("ARCHIVIST-outputMethod", (string)"' + options["Archivist"][0] + '");\n')
optionsList = ''
for t in options["Archivist"]:
    optionsList += t + ', '
optionsList = optionsList[:-2]
outFile.write('  Parameters::root->setDocumentation("ARCHIVIST-outputMethod", "output method, [' + optionsList + ']");\n\n')

outFile.write('  Parameters::root->setParameter("OPTIMIZER-optimizer", (string)"' + options["Optimizer"][0] + '");\n')
optionsList = ''
for t in options["Optimizer"]:
    optionsList += t + ', '
optionsList = optionsList[:-2]
outFile.write('  Parameters::root->setDocumentation("OPTIMIZER-optimizer", "optimizer to be used, [' + optionsList + ']");\n\n')

outFile.write('  Parameters::root->setParameter("WORLD-worldType", (string)"' + options["World"][0] + '");\n')
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
alwaysSources=['main.cpp','Global.cpp','Group/Group.cpp','Organism/Organism.cpp','Utilities/Data.cpp','Utilities/Parameters.cpp','World/AbstractWorld.cpp','Genome/AbstractGenome.cpp','Brain/AbstractBrain.cpp','Optimizer/AbstractOptimizer.cpp','Archivist/DefaultArchivist.cpp']
moduleSources = []
objects = []
sources = None
#alwaysSources = [e.replace('/',sep) for e in alwaysSources]

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
    f_filename=0
    f_compilecpp=1
    f_folder=2
    f_compile=3
    f_link=4
    f_priority=5
    f_overridebuildcmd=6
    f_buildcmd=7
    f_numfields=8
    def newUnit():
        return ['','1','','1','1','1000','0','']
    units=[]
    for eachsource in alwaysSources:
        unit=newUnit()
        unit[f_filename]=eachsource.replace('/','\\')
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
            filename=category+'\\'+module+category+'\\'+module+category+'.cpp'
            foldername=category+'/'+module+category
            unit=newUnit()
            unit[f_filename]=filename
            unit[f_folder]=foldername
            units.append(unit)
            filename=category+'\\'+module+category+'\\'+module+category+'.h'
            foldername=category+'/'+module+category
            unit=newUnit()
            unit[f_filename]=filename
            unit[f_folder]=foldername
            units.append(unit)
            dirs = [d for d in os.listdir(category+'/'+module+category+'/') if os.path.isdir(os.path.join(pathToMABE,category,module+category,d))]
            for eachdir in dirs:
                contents = [c for c in os.listdir(category+'/'+module+category+'/'+eachdir+'/') if ('.cpp' in c) or ('.h' in c) and c.startswith('.')==False] ## include cpp/h/hpp files and ignore hidden files
                for content in contents:
                    filename=category+'/'+module+category+'/'+eachdir+'/'+content
                    filename=filename.replace('/','\\')
                    foldername=category+'/'+module+category+'/'+eachdir
                    unit=newUnit()
                    unit[f_filename]=filename
                    unit[f_folder]=foldername
                    units.append(unit)
    folders=[]
    sortedunits=sorted(units, key=lambda x: x[f_folder])
    for eachunit in sortedunits:
        folders.append(eachunit[f_folder])
    folders=sorted(list(filter(bool,list(set(folders))))) #removes dups & empties, then sorts
    folders=','.join(folders)

    outString = ''
    outString += """
[Project]
FileName=MABE.dev
Name=MABE
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
    for i,eachUnit in enumerate(sortedunits):
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
    f_filename=0
    f_compilecpp=1
    f_folder=2
    f_compile=3
    f_link=4
    f_priority=5
    f_overridebuildcmd=6
    f_buildcmd=7
    f_numfields=8
    def newUnit():
        return ['','1','','1','1','1000','0','']
    units=[]
    for eachsource in alwaysSources:
        unit=newUnit()
        unit[f_filename]=eachsource.replace('/','\\')
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
            filename=category+'\\'+module+category+'\\'+module+category+'.cpp'
            foldername=category+'/'+module+category
            unit=newUnit()
            unit[f_filename]=filename
            unit[f_folder]=foldername
            units.append(unit)
            filename=category+'\\'+module+category+'\\'+module+category+'.h'
            foldername=category+'/'+module+category
            unit=newUnit()
            unit[f_filename]=filename
            unit[f_folder]=foldername
            units.append(unit)
            dirs = [d for d in os.listdir(category+'/'+module+category+'/') if os.path.isdir(os.path.join(pathToMABE,category,module+category,d))]
            for eachdir in dirs:
                contents = [c for c in os.listdir(category+'/'+module+category+'/'+eachdir+'/') if ('.cpp' in c) or ('.h' in c) and c.startswith('.')==False] ## include cpp/h/hpp files and ignore hidden files
                for content in contents:
                    filename=category+'/'+module+category+'/'+eachdir+'/'+content
                    filename=filename.replace('/','\\')
                    foldername=category+'/'+module+category+'/'+eachdir
                    unit=newUnit()
                    unit[f_filename]=filename
                    unit[f_folder]=foldername
                    units.append(unit)
    unit=newUnit()
    unit[f_filename]='modules.h'
    units.append(unit)
    folders=[]
    sortedunits=sorted(units, key=lambda x: x[f_folder])
    for eachunit in sortedunits:
        folders.append(eachunit[f_folder])
    folders=sorted(list(filter(bool,list(set(folders))))) #removes dups & empties, then sorts
    folders=','.join(folders)

    outString = ''
    SDKversion = "10.0.16299.0" ## assume win 10...
    if platform.system() == 'Windows':
        version=platform.platform()
        firstdash=version.find('-',len('windows'))
        seconddash=version.find('-',firstdash+1)
        versionNumber = int(version[firstdash+1:seconddash])
        if 6 < versionNumber < 10:
            hkey = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows")
            SDKversion = winreg.QueryValue(hkey, "CurrentVersion")
        elif versionNumber == 10:
            hkey = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots")
            SDKversion = winreg.EnumKey(hkey, 0)
        else:
            print("Warning: unknown windows version. You will need to retarget the project.")
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
    <PlatformToolset>v141</PlatformToolset>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>v141</PlatformToolset>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>v141</PlatformToolset>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>v141</PlatformToolset>
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
""".format(str(uuid.uuid4()),SDKversion)
    outString += "  <ItemGroup>\n" ## start cpp list
    for eachunit in sortedunits:
        if eachunit[f_filename].endswith('.cpp'):
            outString += '    <ClCompile Include="{0}" />\n'.format(eachunit[f_filename])
    outString += "  </ItemGroup>\n"
    outString += "  <ItemGroup>\n" ## start header list
    for eachunit in sortedunits:
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

if not (args.noCompile):
    call(["make","-j"+str(args.parallel)])
if (args.cleanup):
    call(["make","cleanup"])
    call(["rm","makefile"])
    call(["rmdir","objectFiles"])
