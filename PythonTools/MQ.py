# QM.py takes a list of variables and conditions an generates the calls to
# run MABE on all of the combinations of conditions.
#
# variables is a dictonary with abriviated names and a list of values to iterate over
# varNames is a dictonary associating the actual names in MABE with the abriviated names
# varList is a list of abriviated names, which determines the order of the names

# An Exceptions list can be defined to list combinations which you do not wish to run.
# i.e. if "A" = [0,1] and "B" = [0,1], but you do not wish to run A = 0 and B = 0;
# then define: Exceptions = [["A", 0, "B", 0],[...],...]
# You may define any number of exeptions. If a condition meets all of the rules in an
# exception, then that condition will be excuded from all runs.

# Once all combinations have been generated, MQ proceedes to launch each condition lastRep-firstRep+1
# times. That is condition_1_101, condition_1_102, ... using the rep number as the random seed.

# STRING VALUES vs. NUMERIC VALUES
# you may use string or numeric values for any numeric variable values. If you use numeric values, be
# aware that the formating may not be what you expect (i.e. removal of leading or trailing 0s) so you
# may find that string values are preferable.

from subprocess import call
import glob

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('-noRun', action='store_true', default = False, help='if set, will only print run commands, but not run them - default : false(will run qsub files)', required=False)
parser.add_argument('-runLocal', action='store_true', default = False, help='if set, will run jobs localy - default : false(will run qsub files)', required=False)
parser.add_argument('-runHPCC', action='store_true', default = False, help='if set, will run jobs with qsub on HPCC - default : false(will run qsub files)', required=False)
parser.add_argument('-file', type=str, metavar='FILE_NAME', default = 'MQ_definitions.txt', help='file which defines conditions - default: MQ_definitions.txt', nargs=1, required=False)
args = parser.parse_args()


variables = {}
varNames = {}
varList = []
exceptions = []
executable = "./MABE"

with open(args.file) as openfileobject:
	for line in openfileobject:
			
		line = line.split()
		if (len(line) > 0):
			if line[0] == "REPS":
				firstRep = int(line[2])
				lastRep = int(line[3])
			if line[0] == "JOBNAME":
				displayName = line[2]
				if displayName == "NONE":
					displayName = ""
			if line[0] == "VAR":
				varList.append(line[2])
				variables[line[2]] = line[4].split(",")
				varNames[line[2]] = line[3]
			if line[0] == "EXCEPT":
				exceptions.append(line[2].replace('=',',').split(','))
			if line[0] == "EXECUTABLE":
				executable = line[2]

ex_names = []
for ex in exceptions:
		ex_index = 0
		while ex_index < len(exceptions):
			if ex[ex_index] not in ex_names:
				ex_names.append(ex[ex_index])
			ex_index  += 2;


for ex_name in ex_names:
	found_ex_name = False
	for v in varList:
		if v == ex_name:
			found_ex_name = True
	if not found_ex_name:
		print('exception rules contain variable with name: "' + ex_name + '". But this variable is not defined! Exiting!')
		exit()
	
from subprocess import call
import glob

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('-noRun', action='store_true', default = False, help='if set, will only print run commands, but not run them - default : false(will run jobs assuming a run option has been chosen)', required=False)
parser.add_argument('-runLocal', action='store_true', default = False, help='if set, this run option will run jobs localy - default : false(will run qsub files)', required=False)
parser.add_argument('-runHPCCLJ', action='store_true', default = False, help='if set, this run option will run jobs with qsub on HPCC using Long Job - default : false(will run qsub files)', required=False)
parser.add_argument('-file', type=str, metavar='FILE_NAME', default = 'MQ_definitions.txt', help='file which defines conditions - default: MQ_definitions.txt', nargs=1, required=False)
args = parser.parse_args()

def makeQsubFile():
	outFile = open("MABE.qsub", 'w')
	outFile.write('#!/bin/bash -login\n'+
		'#PBS -l nodes=1:ppn=1,walltime=03:50:00,mem=2gb\n'+
		'#PBS -j oe\n'+
		'#PBS -m ae\n'+
		'\n'+
		'shopt -s expand_aliases\n'+
		'ehco working dir = $PBS_O_WORKDIR\n'+
		'cd $PBS_O_WORKDIR\n'+
		'module load powertools\n'+
		'module load GNU/4.8.3\n'+
		'\n'+
		'\n'+
		'# 4 hours * 60 minutes * 6 seconds - 60 seconds * 20 minutes\n'+
		'# export BLCR_WAIT_SEC=$(( 4 * 60 * 60 ) - ( 60 * 20 ))\n'+
		'export BLCR_WAIT_SEC=$(( 30 * 60 ))\n'+
		'export PBS_JOBSCRIPT="$0"\n'+
		'\n'+
		'if [ ! -f checkfile.blcr ]\n'+
		'then\n'+
		'  WORK=/mnt/scratch/${USER}/${PBS_JOBID}\n'+
		'  mkdir -p ${WORK}\n'+
		'\n'+
		'  # copy executable to work directory (scratch)\n'+
		'  cp -r ' + executable + ' $WORK/\n'+
		'\n'+
		'  # copy settings files to work directory (scratch)\n'+
		'  cp *.cfg $WORK/\n'+
		'\n'+
		'  cd $WORK\n'+
		'\n'+
		'  # create output directory in work directory (scratch)\n'+
		'  mkdir -p ${cond}/${rep}\n'+
		'\n'+
		'  # create local condition directory\n'+
		'  mkdir -p ${PBS_O_WORKDIR}/${cond}\n'+
		'\n'+
		'  # create local link to rep directory in work directory\n'+
		'  ln -s $WORK/${cond}/${rep} ${PBS_O_WORKDIR}/${cond}/${rep}\n'+
		'\n'+
		'fi\n'+
		'\n'+
		'longjob ./MABE -f ${PBS_O_WORKDIR}/*.cfg -p GLOBAL-outputDirectory ${PBS_O_WORKDIR}/${cond}/${rep}/ GLOBAL-randomSeed ${rep} ${params}\n'+
		'ret=$?\n'+
		'\n'+
		'qstat -f ${PBS_JOBID}\n'+
		'\n'+
		'exit $ret\n')
	outFile.close()

print("\nSetting up your jobs...\n")
for v in varList:
	print(v+" ("+varNames[v]+") = " + str(variables[v]))

print("")

reps = range(firstRep,lastRep+1)

lengthsList = []
indexList = []
for key in varList:
	lengthsList.append(len(variables[key]))
	indexList.append(0)

combinations = []
conditions = []

done = False

# iterate over all combinations using nested counter (indexList)
while not done:
	varString = ""
	condString = ""
	keyCount = 0
	# for every key, look up varName and value for that key and add to:
	# varstring - the parameters to be passed to MABE
	# condString - the name of the output directory and job name
	for key in varList:
		varString += " " + varNames[key] + " " + str(variables[key][indexList[keyCount]])
		condString += "_" + key + "_" + str(variables[key][indexList[keyCount]]) + "_"
		keyCount += 1
	
	cond_is_ex = False
	if len(exceptions) > 0:
		for rule in exceptions:
			ruleIndex = 0
			rule_is_ex = True
			while ruleIndex < len(rule):
				keyCount = 0
				for key in varList:
					if (rule[ruleIndex] == key) and (str(rule[ruleIndex+1]) != str(variables[key][indexList[keyCount]])):
						rule_is_ex = False
					keyCount += 1
				ruleIndex += 2
			if rule_is_ex is True:
				cond_is_ex = True
			
	# if this condition is not an exception append this conditions values to:
	# combinations - a list with parameters combinations
	# conditions - a list of directory name/job identifiers
	if not cond_is_ex:
		combinations.append(varString)
		conditions.append(condString)
	else :
		print("excluding: " + condString)
		
	checkIndex = len(indexList)-1
	
	# This block of code moves the index of the last element up by one. If it reaches max
	# then it moves the next to last elements index up by one. If that also reaches max,
	# then the effect cascades. If the first elements index is reaches max then done is
	# set to true and processing of conditions is halted.
	stillChecking = True
	while stillChecking:
		indexList[checkIndex] += 1
		if indexList[checkIndex] == lengthsList[checkIndex]:
			indexList[checkIndex] = 0
			if checkIndex == 0:
				done = True
				stillChecking = False
			else:
				checkIndex -= 1
		else:
			stillChecking = False

if not (args.runLocal or args.runHPCCLJ):
	print("")
	print("If you wish to run, use a run option (runLocal or runHPCCLJ)")
	print("")

# This loop cycles though all of the combinations and constructs to required calls to
# run MABE.

if (args.runHPCCLJ):
	makeQsubFile()

for i in range(len(combinations)):
	for rep in reps:
		if (args.runLocal):
			files = glob.glob("*.cfg")
			print("runing:")
			print("  " + executable + " -f *.cfg -p GLOBAL-outputDirectory " + conditions[i][1:-1] + "/" + str(rep) + "/ " + "GLOBAL-randomSeed " + str(rep) + " " + combinations[i][1:])
			if not args.noRun:
				call(["mkdir","-p",conditions[i][1:-1] + "/" + str(rep)])
				params = combinations[i][1:].split()
				call([executable, "-f"] + files + ["-p", "GLOBAL-outputDirectory" , conditions[i][1:-1] + "/" + str(rep) + "/" , "GLOBAL-randomSeed" , str(rep)] + params)
		if (args.runHPCCLJ):
			if (displayName == ""):
				realDisplayName = "C" + str(i) + "_" + str(rep) + "__" + conditions[i][1:-1]
			else:
				realDisplayName = displayName + "_C" + str(i) + "_" + str(rep) + "__" + conditions[i][1:-1]
			print("runing:")
			print("  " + realDisplayName + " :")
			print("  qsub ./MABE.qsub -N " + realDisplayName + " -v cond=" + conditions[i][1:-1] + " rep=" + str(rep) + ' params="' + combinations[i][1:] + '"')
			if not args.noRun:
				call(["qsub", "MABE.qsub", "-N", realDisplayName, "-v", "cond=" + conditions[i][1:-1] + ",rep=" + str(rep) + ",params=" + combinations[i][1:]])

if args.noRun:
	print("")
	print("  You are using -noRun, so no jobs have been started!")

