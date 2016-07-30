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
import os
import sys
import pwd
import shutil
import datetime

def makeQsubFile(realDisplayName, conditionDirectoryName, rep, qsubFileName, cfg_files, workDir, conditions):
	outFile = open(qsubFileName, 'w')
	outFile.write('#!/bin/bash -login\n')
	for p in HPCC_parameters:
		outFile.write(p+'\n')
	outFile.write('#PBS -o '+realDisplayName+'.out\n')
	outFile.write('#PBS -N '+realDisplayName+'\n')

	outFile.write('\n'+
		'shopt -s expand_aliases\n'+
		'module load powertools\n'+
		'module load GNU/4.8.3\n'+
		'\n'+
		'cd ' + workDir +
		'\n')
		
	includeFileString = "";
	if (len(cfg_files) > 0):
		includeFileString += "-f "
		for fileName in cfg_files:
			includeFileString += fileName + ' '
			
	if HPCC_LONGJOB:
		outFile.write('# 4 hours * 60 minutes * 6 seconds - 60 seconds * 20 minutes\n'+
			'export BLCR_WAIT_SEC=$(( 4 * 60 * 60 - 60 * 20 ))\n'+
			#'export BLCR_WAIT_SEC=$( 30 * 60 )\n'+
			'export PBS_JOBSCRIPT="$0"\n'+
			'\n'+
			'longjob ./MABE ' + includeFileString + '-p GLOBAL-outputDirectory ' + conditionDirectoryName + '/' + str(rep) + '/ GLOBAL-randomSeed ' + str(rep) + ' ' + conditions + '\n')
	else:
		outFile.write('./MABE ' + includeFileString + '-p GLOBAL-outputDirectory ' + conditionDirectoryName + '/' + str(rep) + '/ GLOBAL-randomSeed ' + str(rep) + ' ' + conditions + '\n')
	outFile.write('ret=$?\n\n'+
		'qstat -f ${PBS_JOBID}\n'+
		'\n'+
		'exit $ret\n')
	outFile.close()
	
	
parser = argparse.ArgumentParser()
parser.add_argument('-noRun', action='store_true', default = False, help='if set, will do everything (i.e. create files and directories) but launch jobs, allows you to do a dry run - default : false(will run)', required=False)
parser.add_argument('-runLocal', action='store_true', default = False, help='if set, will run jobs localy - default : false(no action)', required=False)
parser.add_argument('-runHPCC', action='store_true', default = False, help='if set, will run jobs with qsub on HPCC - default : false(no action)', required=False)
parser.add_argument('-file', type=str, metavar='FILE_NAME', default = 'MQ_conditions.txt', help='file which defines conditions - default: MQ_conditions.txt', required=False)
args = parser.parse_args()

variables = {}
varNames = {}
varList = []
exceptions = []
cfg_files = []
other_files = []
executable = "./MABE"
HPCC_parameters = []
HPCC_LONGJOB = True

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
			if line[0] == "SETTINGS":
				cfg_files = line[2].split(',')
				for f in cfg_files:
					if not(os.path.isfile(f)):
						print('settings file: "' + f + '" seems to be missing!')
						exit()
			if line[0] == "OTHERFILES":
				other_files = line[2].split(',')
				for f in other_files:
					if not(os.path.isfile(f)):
						print('other file: "' + f + '" seems to be missing!')
						exit()
			if line[0] == "HPCC_LONGJOB":
				HPCC_LONGJOB = (line[2] == "TRUE")
			if line[0] == "HPCC_PARAMETERS":
				newParameter = ""
				for i in line[2:]:
					newParameter += i + ' '
				HPCC_parameters.append(newParameter[:-1])


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

print("excluding:")

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
		print("  " + condString[1:-1])
		
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

print("")
print("including:")
for c in conditions:
	print("  " + c[1:-1])
print("")

print("the following settings files will be included:")
for f in cfg_files:
  print("  "+f)
print("")
	
if not (args.runLocal or args.runHPCC):
	print("")
	print("If you wish to run, use a run option (runLocal or runHPCC)")
	print("")


userName = pwd.getpwuid( os.getuid() )[ 0 ] # get the user name
print(userName)
absLocalDir = os.getcwd() # this is needed so we can get back to the launch direcotry
localDirParts = absLocalDir.split('/') # turn path into list so we can trim of verything before ~
while localDirParts[0] != userName:
	localDirParts = localDirParts[1:] # trim off parts from the front until we get the the user name
	if len(localDirParts) == 1 and localDirParts[0] != userName:
		print('You must be in your home area to run MQ.py.')
		exit(1);
localDir = '~/'+'/'.join(localDirParts[1:]) # this is the launch directory from ~/ : this command appends '~/' onto the remaining list with '/' between each element.

pathToScratch = '/mnt/scratch/' # used for HPCC runs

# This loop cycles though all of the combinations and constructs to required calls to
# run MABE.

for i in range(len(combinations)):
	for rep in reps:
		if (args.runLocal):
			cfg_files_str = ' '.join(cfg_files) # turn cgf_files list into a space separated string
			print("running:")
			print("  " + executable + " -f " + cfg_files_str + " -p GLOBAL-outputDirectory " + conditions[i][1:-1] + "/" + str(rep) + "/ " + "GLOBAL-randomSeed " + str(rep) + " " + combinations[i][1:])
			call(["mkdir","-p",displayName + "_" + conditions[i][1:-1] + "/" + str(rep)]) # make rep directory (this will also make the condition directory if it's not here already)
			if not args.noRun:
				params = combinations[i][1:].split() # turn combinations string into a list
				call([executable, "-f"] + cfg_files + ["-p", "GLOBAL-outputDirectory" , displayName + "_" + conditions[i][1:-1] + "/" + str(rep) + "/" , "GLOBAL-randomSeed" , str(rep)] + params)
		if (args.runHPCC):
			os.chdir(absLocalDir) # go to the local directory (after each job is launched, we are in the work directory) 
			if (displayName == ""):
				realDisplayName = "C" + str(i) + "_" + str(rep) + "__" + conditions[i][1:-1]
				conditionDirectoryName = "C" + str(i) + "__" + conditions[i][1:-1]
			else:
				realDisplayName = displayName + "_C" + str(i) + "_" + str(rep) + "__" + conditions[i][1:-1]
				conditionDirectoryName = displayName + "_C" + str(i) + "__" + conditions[i][1:-1]
				
			timeNow = str(datetime.datetime.now().year)+'_'+str(datetime.datetime.now().month)+'_'+str(datetime.datetime.now().day)+'_'+str(datetime.datetime.now().hour)+'_'+str(datetime.datetime.now().minute)+'_'+str(datetime.datetime.now().second)

			workDir = pathToScratch+userName+'/'+realDisplayName+'_'+str(rep)+'__'+timeNow
			
			outputDir = workDir+'/'+conditionDirectoryName+'/'+str(rep)+'/' # this is where data files will actually be writen (on scratch)
			
			if os.path.exists(workDir): # if there was already a workDir, get rid of it.
				shutil.rmtree(workDir)
			os.makedirs(outputDir)
			shutil.copy(executable, workDir) # copy the executable to scratch
			for f in cfg_files:
				shutil.copy(f, workDir) # copy the settings files to scratch
			for f in other_files:
				shutil.copy(f, workDir) # copy other files to scratch

			if not(os.path.exists(conditionDirectoryName)): # if the local conditions directory is not already here, make it
				os.makedirs(conditionDirectoryName)

			if os.path.exists(conditionDirectoryName+'/'+str(rep)): # if there is already a link for this rep from the local conditions directory to scratch, remove it
				os.unlink(conditionDirectoryName+'/'+str(rep))
						

			os.symlink(workDir+'/'+conditionDirectoryName+'/'+str(rep),conditionDirectoryName+'/'+str(rep)) # create local link from the local conditions directory to the rep directory in work directory
			
			os.chdir(workDir) # goto the work dir (on scratch)

			qsubFileName = "MQ.qsub"

			# make the qsub file on scratch
			makeQsubFile(realDisplayName = realDisplayName, conditionDirectoryName = conditionDirectoryName, rep = rep ,qsubFileName = qsubFileName, cfg_files = cfg_files, workDir = workDir, conditions = combinations[i][1:])
			
			print("submitting:")
			print("  " + realDisplayName + " :")
			print("  workDir = " + workDir)
			print("  qsub " + qsubFileName)
			if not args.noRun:
				call(["qsub", qsubFileName]) # run the job

if args.noRun:
	print("")
	print("  You are using -noRun, so no jobs have been started!")

