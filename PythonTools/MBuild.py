import argparse
import os
from subprocess import call

parser = argparse.ArgumentParser()

parser.add_argument('-buildFile', type=str, metavar='FILE', default = 'buildOptions.txt',  help=' name of file with build options - default : buildOptions.txt', required=False)
parser.add_argument('-windows', action='store_true', default = False, help='add this flag if you are on a windows computer', required=False)
parser.add_argument('-noCleanup', action='store_true', default = False, help='add this flag if you want to keep the .o files', required=False)
parser.add_argument('-noCompile', action='store_true', default = False, help='create modules.h and makefile, but do not compile', required=False)
parser.add_argument('-pg', action='store_true', default = False, help='compile with -pg option (for gprof)', required=False)

args = parser.parse_args()

if args.windows:
	product = 'MABE.exe'
else:
	product = 'MABE'

compiler='c++'
compFlags='-Wno-c++98-compat -w -Wall -std=c++11 -O3'
if (args.pg):
	compFlags =  compFlags + ' -pg'
	
# load all lines from buildFile into lines, ignore blank lines
file = open(args.buildFile, 'r')
pathToMABE=os.path.dirname(args.buildFile)
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

outFile = open("modules.h", 'w')

outFile.write('//  MABE is a product of The Hintza Lab @ MSU\n')
outFile.write('//     for general research information:\n')
outFile.write('//         http://hintzelab.msu.edu/\n')
outFile.write('//     for MABE documentation:\n')
outFile.write('//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki\n')
outFile.write('//\n')
outFile.write('//  Copyright (c) 2015 Michigan State University. All rights reserved.\n')
outFile.write('//     to view the full license, visit:\n')
outFile.write('//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license\n\n')
outFile.write('//  This file was auto-generated with MBuilder.py\n\n')

# #includes

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

# makeWorld

outFile.write('\n\n//create a world\n')
outFile.write('shared_ptr<AbstractWorld> makeWorld(shared_ptr<ParametersTable> PT = Parameters::root){\n')
outFile.write('  shared_ptr<AbstractWorld> newWorld;\n')
outFile.write('  bool found = false;\n')
outFile.write('  string worldType = (PT == nullptr) ? AbstractWorld::worldTypePL->lookup() : PT->lookupString("WORLD-worldType");\n')
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

# makeOptimizer

outFile.write('\n\n//create an optimizer\n')
outFile.write('shared_ptr<AbstractOptimizer> makeOptimizer(shared_ptr<ParametersTable> PT = Parameters::root){\n')
outFile.write('  shared_ptr<AbstractOptimizer> newOptimizer;\n')
outFile.write('  bool found = false;\n')
outFile.write('  string optimizerType = (PT == nullptr) ? AbstractOptimizer::Optimizer_MethodStrPL->lookup() : PT->lookupString("OPTIMIZER-optimizer");\n')
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

# makeArchivist

outFile.write('\n\n//create an archivist\n')
outFile.write('shared_ptr<DefaultArchivist> makeArchivist(vector<string> aveFileColumns, string maxValueName, shared_ptr<ParametersTable> PT = Parameters::root){\n')
outFile.write('  shared_ptr<DefaultArchivist> newArchivist;\n')
outFile.write('  bool found = false;\n')
outFile.write('  string archivistType = (PT == nullptr) ? DefaultArchivist::Arch_outputMethodStrPL->lookup() : PT->lookupString("ARCHIVIST-outputMethod");\n')
for option in options["Archivist"]:
	outFile.write('  if (archivistType == "'+option+'") {\n')
	outFile.write('    newArchivist = make_shared<'+option+'Archivist>(aveFileColumns, maxValueName, PT);\n')
	outFile.write('    found = true;\n')	
	outFile.write('    }\n')
outFile.write('  if (!found){\n')
outFile.write('    cout << "  ERROR! could not find ARCHIVIST-outputMethod \\"" << archivistType << "\\".\\n  Exiting." << endl;\n')
outFile.write('    exit(1);\n')
outFile.write('    }\n')
outFile.write('  return newArchivist;\n')
outFile.write('}\n')

# makeTemplateGenome

outFile.write('\n\n//create a template genome\n')
outFile.write('shared_ptr<AbstractGenome> makeTemplateGenome(shared_ptr<ParametersTable> PT = nullptr){\n')
outFile.write('  shared_ptr<AbstractGenome> newGenome;\n')
outFile.write('  bool found = false;\n')
outFile.write('  string genomeType = (PT == nullptr) ? AbstractGenome::genomeTypeStrPL->lookup() : PT->lookupString("GENOME-genomeType");\n')
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

# makeTemplateBrain

outFile.write('\n\n//create a template brain\n')
outFile.write('shared_ptr<AbstractBrain> makeTemplateBrain(shared_ptr<AbstractWorld> world, shared_ptr<ParametersTable> PT = nullptr){\n')
outFile.write('  shared_ptr<AbstractBrain> newBrain;\n')
outFile.write('  bool found = false;\n')
outFile.write('  string brainType = (PT == nullptr) ? AbstractBrain::brainTypeStrPL->lookup() : PT->lookupString("BRAIN-brainType");\n')
#outFile.write('  int hiddenNodes = (PT == nullptr) ? AbstractBrain::hiddenNodesPL->lookup() : PT->lookupInt("BRAIN-hiddenNodes");\n')
for option in options["Brain"]:
	outFile.write('  if (brainType == "'+option+'") {\n')
#	outFile.write('    newBrain = '+option+'Brain_brainFactory(world->requiredInputs(), world->requiredOutputs(), hiddenNodes,PT);\n')
	outFile.write('    newBrain = '+option+'Brain_brainFactory(world->requiredInputs(), world->requiredOutputs(), PT);\n')
	outFile.write('    found = true;\n')	
	outFile.write('    }\n')
outFile.write('  if (found == false){\n')
outFile.write('    cout << "  ERROR! could not find BRAIN-brainType \\"" << brainType << "\\".\\n  Exiting." << endl;\n')
outFile.write('    exit(1);\n')
outFile.write('    }\n')
outFile.write('  return newBrain;\n')
outFile.write('}\n')

# configure Defaults and Documentation

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


# Create the make file

if not os.path.exists('objectFiles'):
	os.makedirs('objectFiles')

alwaysSources=['main.cpp','Global.cpp','Group/Group.cpp','Organism/Organism.cpp','Utilities/Data.cpp','Utilities/Parameters.cpp','World/AbstractWorld.cpp','Genome/AbstractGenome.cpp','Brain/AbstractBrain.cpp','Optimizer/AbstractOptimizer.cpp','Archivist/DefaultArchivist.cpp']

options['Archivist'].remove('Default')

moduleSources = []
for o in options:
	for t in options[o]:
		moduleSources.append(pathToMABE+'/'+o+'/'+t+o+'/'+t+o+'.cpp')
		dirs = [d for d in os.listdir(pathToMABE+'/'+o+'/'+t+o+'/') if os.path.isdir(os.path.join(pathToMABE+'/'+o+'/'+t+o+'/', d))]
		for d in dirs:
                    contents = [c for c in os.listdir(pathToMABE+'/'+o+'/'+t+o+'/'+d+'/') if '.cpp' in c and c.startswith('.')==False] ## include cpp files and ignore hidden files
                    for content in contents:
                        moduleSources.append(pathToMABE+'/'+o+'/'+t+o+'/'+d+'/'+content)

alwaysSources = [pathToMABE+'/'+e for e in alwaysSources]
sources = alwaysSources + moduleSources
objects = []


for s in sources:
    objects.append('objectFiles/'+os.path.realpath(s)[1:].split('.')[0].replace('/','_') + '.o')

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

if not (args.noCompile):
	call("make")
if not (args.noCleanup or args.noCompile):
	call(["make","cleanup"])
	call(["rm","makefile"])
