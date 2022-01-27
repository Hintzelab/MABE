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
from subprocess import Popen as callNoWait
import glob
import argparse
import os
import sys
#import pwd
import shutil
import datetime
import getpass  # for getuser() gets current username
import itertools # for generating combinations of conditions
import re
from utils import pyreq
import subprocess # invoking command line module installation

pyreq.require("colorama,psutil") # quits if not found, even after it installs. must run this script again

# colored warning and error printing
import colorama
from colorama import Fore, Back, Style
import psutil

if 'cmd' in psutil.Process().parent().name().lower(): # do only for cmd.exe, unnecessary for powershell.exe,bash,zsh,etc.
    colorama.init(convert=True)

def printError(msg,end='\n'):
    print(Style.BRIGHT+Fore.RED+"Error: "+msg+Style.RESET_ALL,end=end)

def printWarning(msg,end='\n'):
    print(Style.BRIGHT+Fore.YELLOW+"Warning: "+msg+Style.RESET_ALL,end=end)

# regular expression patterns
ptrnCommand = re.compile(r'^\s*([A-Z]+)\s') # ex: gets 'VAR' from 'VAR = UD GLOBAL-msg "a message"'
ptrnSpaceSeparatedEquals = re.compile(r'\s(\s*\".*\"|[^\s]+)') # ex: gets ['=','UD','GLOBAL-updated','a message']
#ptrnCSVs = re.compile(r'\s*,?\s*([^\s",]+|\"([^"\\]|\\.)*?\")\s*,?\s*') # ex: gets ['1.2','2',"a \"special\" msg"] from '1.2,2,"a \"special\" msg"'
ptrnCSVs = re.compile(r'\s*,?\s*(\[(.*?)\]|([^\s",]+|\"([^"\\]|\\.)*?\"))\s*,?\s*') # ex: gets ['1.2','2',"a \"special\" msg"] from '1.2,2,"a \"special\" msg"'
ptrnGlobalUpdates = re.compile(r'GLOBAL-updates\s+[0-9]+') # ex: None or 'GLOBAL-updates    300'

def makeQsubFile(realDisplayName, conditionDirectoryName, rep, slurmFileName, executable, cfg_files, workDir, conditions, padSizeReps):
    outFile = open(slurmFileName, 'w')
    outFile.write('#!/bin/bash -login\n')
    for p in HPCC_parameters:
        outFile.write(p + '\n')
    #outFile.write('#PBS -o ' + realDisplayName + '.out\n')
    outFile.write('#SBATCH --output=' + realDisplayName + '.out\n')
    outFile.write('#SBATCH --job-name=' + realDisplayName + '\n')

    outFile.write('\n' +
                  'shopt -s expand_aliases\n' +
                  '\n' +
                  'cd ' + workDir +
                  '\n')

    includeFileString = ""
    if (len(cfg_files) > 0):
        includeFileString += "-f "
        for fileName in cfg_files:
            includeFileString += fileName + ' '

    if HPCC_LONGJOB:
        #outFile.write('# 4 hours * 60 minutes * 6 seconds - 60 seconds * 20 minutes\n' +
        #              'export BLCR_WAIT_SEC=$(( 4 * 60 * 60 - 60 * 20 ))\n' +
        #              #'export BLCR_WAIT_SEC=$( 30 * 60 )\n'+
        #              'export PBS_JOBSCRIPT="$0"\n' +
        #              '\n' +
        #              'longjob ' +  executable + ' ' + includeFileString + '-p GLOBAL-outputPrefix ' + conditionDirectoryName + '/' + str(rep).zfill(padSizeReps) + '/ GLOBAL-randomSeed ' + str(rep) + ' ' + conditions + '\n')
        local_dir = conditionDirectoryName + '/' + str(rep).zfill(padSizeReps)
        mabe_call = executable + ' ' + includeFileString + '-p GLOBAL-outputPrefix ' + local_dir + '/ GLOBAL-randomSeed ' + str(rep) + ' ' + conditions
        longjob_contents = '''
ulimit -s 8192
cd ${{SLURM_SUBMIT_DIR}}
export SLURM_JOBSCRIPT="{FILE_NAME}" # used for resubmission
######################## start dmtcp_coordinator #######################
fname=port.$SLURM_JOBID # store port number
dmtcp_coordinator --daemon --exit-on-last -p 0 --port-file $fname $@ 1>/dev/null 2>&1   # start coordinater
h=`hostname` # get coordinator's host name
p=`cat $fname` # get coordinator's port number
export DMTCP_COORD_HOST=$h # save coordinators host info in an environment variable
export DMTCP_COORD_PORT=$p # save coordinators port info in an environment variable
export DMTCP_CHECKPOINT_DIR="./" # save ckpt files into unique locations
####################### BODY of the JOB ######################
# prepare work environment of the job
# if first time launch, use "dmtcp_launch" otherwise use "dmtcp_restart"


export CKPT_WAIT_SEC=$(( 4 * 60 * 60 - 10 * 60 )) # when to ckpt, in seconds (just under 4 hrs)
#export CKPT_WAIT_SEC=$(( 600 )) # when to ckpt, in seconds (just under 4 hrs)


# Launch or restart the execution
if [ ! -f ${{DMTCP_CHECKPOINT_DIR}}/ckpt_*.dmtcp ] # if no ckpt file exists, it is first time run, use dmtcp_launch
then
  # first time run, use dmtcp_launch to start the job and run on background
  dmtcp_launch -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT --rm --ckpt-open-files {MABE_CALL} &
  #wait for an inverval of checkpoint seconds to start checkpointing
  sleep $CKPT_WAIT_SEC

  # if program is still running, do the checkpoint and resubmit
  if dmtcp_command -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT -s 1>/dev/null 2>&1
  then
    # start checkpointing
    dmtcp_command -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT --ckpt-open-files --bcheckpoint
    # kill the running job after checkpointing
    dmtcp_command -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT --quit
    # resubmit the job
    sbatch $SLURM_JOBSCRIPT
  else
    echo "job finished"
  fi
else            # it is a restart run
  # clean up artifacts (resulting files that could be in the middle of being written to)
  # clean up any generated mabe files that have been checkpointed
  rm {LOCAL_DIR}/*.csv
  # restart job with checkpoint files ckpt_*.dmtcp and run in background
  dmtcp_restart -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT ckpt_*.dmtcp &
  # wait for a checkpoint interval to start checkpointing
  sleep $CKPT_WAIT_SEC
  # if program is still running, do the checkpoint and resubmit
  if dmtcp_command -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT -s 1>/dev/null 2>&1
  #if dmtcp_command -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT -s 1>/dev/null
  then  
    # clean up old ckpt files before start checkpointing
    rm -r ckpt_*.dmtcp
    # checkpointing the job
    dmtcp_command -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT --ckpt-open-files -bc
    # kill the running program and quit
    dmtcp_command -h $DMTCP_COORD_HOST -p $DMTCP_COORD_PORT --quit
    # resubmit this script to slurm
    sbatch $SLURM_JOBSCRIPT
  else
    echo "job finished"
  fi
fi
# show the job status info
#scontrol show job $SLURM_JOB_ID
'''.format(FILE_NAME=slurmFileName, MABE_CALL=mabe_call, LOCAL_DIR=local_dir)
        outFile.write(longjob_contents)
    else:
        outFile.write(executable + ' ' + includeFileString + '-p GLOBAL-outputPrefix ' + conditionDirectoryName + '/' + str(rep).zfill(padSizeReps) + '/ GLOBAL-randomSeed ' + str(rep) + ' ' + conditions + '\n')
    outFile.write('ret=$?\n\n' +
                  'sacct -j ${SLURM_JOB_ID}\n' +
                  '\n' +
                  'exit $ret\n')
    outFile.close()


parser = argparse.ArgumentParser()
parser.add_argument('-n', '--runNo', action='store_true', default=False,
                    help='if set, will do everything (i.e. create files and directories) but launch jobs, allows you to do a dry run - default : false(will run)', required=False)
parser.add_argument('-l', '--runLocal', action='store_true', default=False,
                    help='if set, will run jobs localy - default : false(no action)', required=False)
parser.add_argument('-t', '--runTest', action='store_true', default=False,
                    help='if set, will run jobs localy with 1 rep and 2 updates set - default : false(no action)', required=False)
parser.add_argument('-d', '--runHPCC', action='store_true', default=False,
                    help='if set, will deploy jobs with qsub on HPCC - default : false(no action)', required=False)
parser.add_argument('-f', '--file', type=str, metavar='FILE_NAME', default='mq_conditions.txt',
                    help='file which defines conditions - default: mq_conditions.txt', required=False)
parser.add_argument('-i', '--indefinite', action='store_true', default=False,
                    help='run jobs until they self-terminate (longjob) - default : false', required=False)
args = parser.parse_args()

variables = {}
variablesNonConditionsVersion = {}
varNames = {}
varList = []
exceptions = []
condition_sets = []
condition_sets_skipped = []
constantDefs = ''
populationLoaderDefs = ''
cfg_files = []
other_files = []
executable = "./mabe"
HPCC_parameters = []
HPCC_LONGJOB = False
using_conditions = False ## either use VAR/EXCEPT or CONDITIONS, but not both. Use of condition values overrides VAR values.

def hasMatchedSymbols(rawString,sym1,sym2):
    numberOpen = rawString.count(sym1)
    numberClosed = rawString.count(sym2)
    return numberOpen == numberClosed
def hasUnmatchedSymbols(rawString):
    problemPairs = []
    for eachSymbolSet in '[],()'.split(','):
        sym1,sym2 = eachSymbolSet[0],eachSymbolSet[1]
        if not hasMatchedSymbols(rawString,sym1,sym2):
            problemPairs.append((sym1,sym2))
    if len(problemPairs) == 0:
        return False
    else:
        return problemPairs
def stripIllegalDirnameChars(rawString):
    for eachChar in list(':[](),\'"\\!@#$%^&*=+` <>?{}'):
        rawString = rawString.replace(eachChar,'')
    return rawString

varDeprecationWarningPrinted = False
with open(args.file) as openfileobject:
    for rawline in openfileobject:

        line = rawline.split()
        if (len(line) > 0):
            if line[0] == "REPS":
                firstRep = int(line[2])
                lastRep = int(line[3])
            if line[0] == "JOBNAME":
                displayName = line[2]
                if displayName == "NONE":
                    displayName = ""
            if line[0] == "VAR": # VAR = PUN GLOBAL-poisonValue 0.0,1.0,1.5
                everythingEqualsAndAfterAsList = ptrnSpaceSeparatedEquals.findall(rawline) # 0:'=',1:variable,2:MABE-variable,3:values
                if everythingEqualsAndAfterAsList[0] != '=':
                    printError("VARs require an assignment for readability. Ex: CONDITIONS = TSK=1.0")
                    exit(1)
                var,mabeVar = everythingEqualsAndAfterAsList[1:3] # get variable and mabe-variable
                varList.append(var)
                varNames[var] = mabeVar
                if len(everythingEqualsAndAfterAsList) > 3: # allow for users to not specify any values
                    variables[var] = everythingEqualsAndAfterAsList[3]
                    variablesNonConditionsVersion[var] = [e[0] for e in ptrnCSVs.findall(variables[var])]
                    if not varDeprecationWarningPrinted:
                        varDeprecationWarningPrinted = True
                        printWarning("VAR statement with values deprecated. Please use the new syntax.")
                else:
                    using_conditions = True # can't use standard VAR/EXCEPT when you don't specify values
            if line[0] == "EXCEPT": # EXCEPT = UH=1,UI=1
                everythingEqualsAndAfterAsList = ptrnSpaceSeparatedEquals.findall(rawline) # 0:'=',1:variable,2:MABE-variable,3:values
                if everythingEqualsAndAfterAsList[0] != '=':
                    printError("EXCEPT requires an assignment for readability. Ex: CONDITIONS = TSK=1.0")
                    exit(1)
                new_skip_condition_set = []
                exceptions.append(','.join(everythingEqualsAndAfterAsList[1:]).replace('=',',').split(','))
                for eachVar in everythingEqualsAndAfterAsList[1:]:
                    if eachVar.count('=') > 1:
                        printError("more than 1 '=' character found in EXCEPT values (probably in a string?) and we haven't considered this problem yet.")
                        sys.exit(1)
                    variable,rawValues=eachVar.split('=')
                    values = [e[0] for e in ptrnCSVs.findall(rawValues)]
                    new_skip_condition_set.append([variable]+values)
                condition_sets_skipped.append(new_skip_condition_set) # results as: condition_sets=[[['PUN','0.0','1.0','1.5'], ['UH','1'], ['UI','1']], /* next condition set here... */ ]
            if line[0] == "CONDITIONS": # CONDITIONS = PUN=0.0,1.0,1.5;UH=1;UI=1
                using_conditions = True
                everythingEqualsAndAfterAsList = ptrnSpaceSeparatedEquals.findall(rawline) # 0:'=',1:variable,2:MABE-variable,3:values
                if everythingEqualsAndAfterAsList[0] != '=':
                    printError("CONDITIONS require an assignment for readability. Ex: CONDITIONS = TSK=1.0")
                    exit(1)
                new_condition_set = []
                for eachVar in everythingEqualsAndAfterAsList[1:]:
                    #if eachVar.count('=') > 1:
                    #    printError("more than 1 '=' character found in CONDITIONS values (probably in a string?) and we haven't considered this problem yet.")
                    #    exit(1)
                    #variable,rawValues=eachVar.split('=')
                    variable = eachVar.split('=')[0]
                    rawValues = eachVar[len(variable)+1:]
                    problemPairs = hasUnmatchedSymbols(rawValues)
                    if problemPairs:
                        printWarning("The following value(s) have unmatched {symbols} symbols.".format(symbols=','.join([e[0]+e[1] for e in problemPairs])))
                        printWarning(rawValues)
                        sys.exit(1)
                    #values = [e[0].strip('[]') for e in ptrnCSVs.findall(rawValues)] ## removed to leave [] untouched
                    values = [e[0] for e in ptrnCSVs.findall(rawValues)]
                    new_condition_set.append([variable]+values)
                condition_sets.append(new_condition_set) # results as: condition_sets=[[['PUN','0.0','1.0','1.5'], ['UH','1'], ['UI','1']], /* next condition set here... */ ]
            if line[0] == "EXECUTABLE":
                executable = line[2]
            if line[0] == "CONSTANT":
                constantDefs += ' '+' '.join(line[2:])
            if line[0] == "SETTINGS":
                cfg_files = line[2].split(',')
                for f in cfg_files:
                    if not(os.path.isfile(f)) and f.find('{{rep}}')==-1:
                        print('settings file: "' + f +
                              '" seems to be missing!')
                        exit()
            if line[0] == "OTHERFILES":
                other_files = line[2].split(',')
                for f in other_files:
                    if not(os.path.isfile(f)) and f.find('{{rep}}')==-1:
                        print('other file: "' + f + '" seems to be missing!')
                        exit()
            #if line[0] == "HPCC_LONGJOB":
            #    HPCC_LONGJOB = (line[2] == "TRUE")
            if line[0] == "HPCC_PARAMETERS":
                newParameter = ""
                for i in line[2:]:
                    newParameter += i + ' '
                HPCC_parameters.append(newParameter[:-1])
            if line[0] == "PLF":
                populationLoaderDefs += '\n '+' '.join(line[2:])

padSizeReps = len(str(lastRep))

ex_names = []
for ex in exceptions:
    ex_index = 0
    while ex_index < len(ex):
        if ex[ex_index] not in ex_names:
            ex_names.append(ex[ex_index])
        ex_index += 2

cond_var_names=set()
for cond_set in condition_sets:
    for cond_def in cond_set:
        cond_var_names.add(cond_def[0]) # add the variable name part of the definition (always [0])

for ex_name in ex_names:
    found_ex_name = False
    for v in varList:
        if v == ex_name:
            found_ex_name = True
    if not found_ex_name:
        print('exception rules contain variable with name: "' +
              ex_name + '". But this variable is not defined. Exiting.')
        exit()

for cond_var_name in cond_var_names:
    if cond_var_name not in varList:
        print('conditions contains variable with name: "' +
              cond_var_name + '". But this variable is not defined. Exiting.')
        exit()

print("\nSetting up your jobs...\n")
for v in varList:
    if v in variablesNonConditionsVersion:
        print(v + " (" + varNames[v] + ") = " + str(variablesNonConditionsVersion[v]))

print("")

reps = range(firstRep, lastRep + 1)

lengthsList = []
indexList = []
for key in varList:
    if key in variables:
        lengthsList.append(len(variablesNonConditionsVersion[key]))
        indexList.append(0)

combinations = []
conditions = []

if len(varList) + len(condition_sets) == 0:
    using_conditions = True
    conditions.append('')
    combinations.append('C')

if not using_conditions:
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
            varString += " " + varNames[key] + " " + \
                str(variablesNonConditionsVersion[key][indexList[keyCount]])
            condString += "_" + key + "_" + \
                str(variablesNonConditionsVersion[key][indexList[keyCount]]) + "_"
            keyCount += 1

        cond_is_ex = False
        if len(exceptions) > 0:
            for rule in exceptions:
                ruleIndex = 0
                rule_is_ex = True
                while ruleIndex < len(rule):
                    keyCount = 0
                    for key in varList:
                        if (rule[ruleIndex] == key) and (str(rule[ruleIndex + 1]) != str(variablesNonConditionsVersion[key][indexList[keyCount]])):
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
        else:
            print("  " + condString[1:-1])

        checkIndex = len(indexList) - 1

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
elif using_conditions: # This section is for parsing the CONDITIONS lines
    # calculate skip sets
    skipCombinations = []
    for skip_set in condition_sets_skipped:
        assignmentsInCategories = []
        for condition_var in skip_set:
            varString = varNames[condition_var[0]]+' '
            category=[]
            for var_value in condition_var[1:]: # loop over all values, skipping the variable name
                category.append( varString+var_value ) # store MABE-like parameter assignment
            assignmentsInCategories.append(category)
        skipCombinations += itertools.product(*assignmentsInCategories)
    # calculate include sets
    for condition_set in condition_sets:
        assignmentsInCategories = []
        for condition_var in condition_set:
            varString = varNames[condition_var[0]]+' '
            condString = '_'+condition_var[0]+'_'
            category=[]
            for var_value in condition_var[1:]: # loop over all values, skipping the variable name
                category.append( (varString+var_value, condString+var_value+'_') ) # store MABE-like parameter assignment, and condition path strings
            assignmentsInCategories.append(category)
        allCombinations = itertools.product(*assignmentsInCategories)
        for each_combination in allCombinations: # each_combination is tuple: (mabe-like param assignment, condition string)
            matchesi = [0]*len(skipCombinations) # boolean mask for which skips sets match, assume all match
            for test_value in [e[0] for e in each_combination]:
                for sci,skip_combo in enumerate(skipCombinations):
                    if test_value in skip_combo:
                        matchesi[sci] += 1
            should_not_skip = True
            for sci,skip_combo in enumerate(skipCombinations):
                if matchesi[sci] == len(skip_combo):
                    should_not_skip = False
                    break
            if should_not_skip:
                conditions.append(''.join([e[1] for e in each_combination])) # store full condition path string for folder name generation
                combinations.append(' '+' '.join([e[0] for e in each_combination])) # store MABE-like full parameter string

padSizeCombinations = len(str(len(combinations)))
i = 0 

print("")
print("including:")
for c in conditions:
    if (displayName == ""):
        conditionDirectoryName = "C" + str(i).zfill(padSizeCombinations) + "__" + stripIllegalDirnameChars(c[1:-1])
    else:
        conditionDirectoryName = displayName + "_C" + str(i).zfill(padSizeCombinations) + "__" + stripIllegalDirnameChars(c[1:-1])
    print("  " + conditionDirectoryName)
    i = i+1
print("")

print("the following settings files will be included:")
for f in cfg_files:
    print("  " + f)
print("")

print("running these " + str(len(conditions)) + " conditions will result in " + str(len(conditions)* len(reps)) + " jobs")     
print("")

if not (args.runLocal or args.runHPCC):
    print("")
    print("If you wish to run, use a run option (runLocal or runHPCC)")
    print("")


userName = getpass.getuser()
print(userName)
absLocalDir = os.getcwd()  # this is needed so we can get back to the launch direcotry
# turn path into list so we can trim of verything before ~
localDirParts = absLocalDir.split('/')
# this is the launch directory from ~/ : this command appends '~/' onto the remaining list with '/' between each element.
localDir = '~/' + '/'.join(localDirParts[1:])

pathToScratch = '/mnt/scratch/'  # used for HPCC runs

# This loop cycles though all of the combinations and constructs to required calls to
# run MABE.
if args.runTest:
    reps = range(1,2)
    match = ptrnGlobalUpdates.search(constantDefs)
    if match is None:
        constantDefs += " GLOBAL-updates 2"
    else:
        constantDefs = constantDefs.replace(match.group(), "GLOBAL-updates 2")
for i in range(len(combinations)):
    for rep in reps:
        if not args.runNo:
            ## perform keyword replacement such as {{rep}}
            replacedPopulationLoaderDefs = populationLoaderDefs.replace('{{rep}}',str(rep))
            replacedOther_files = [s.replace('{{rep}}',str(rep)) for s in other_files]
            replacedConstantDefs = constantDefs.replace('{{rep}}',str(rep))
            if (len(populationLoaderDefs)): ## if any have been specified, then create the file and dump specified to file
                os.chdir(absLocalDir)
                with open('population_loader.plf','w') as file:
                    file.write(replacedPopulationLoaderDefs+'\n')
        if args.runLocal or args.runTest:
            for eachFile in replacedOther_files:
                if os.path.relpath(eachFile) != os.path.basename(eachFile):
                    shutil.copy(eachFile, os.path.basename(eachFile))
            # turn cgf_files list into a space separated string
            cfg_files_str = ' '.join(cfg_files)
            if (displayName == ""):
                conditionDirectoryName = "C" + str(i).zfill(padSizeCombinations) + "__" + stripIllegalDirnameChars(conditions[i][1:-1])
            else:
                conditionDirectoryName = displayName + "_C" + str(i).zfill(padSizeCombinations) + "__" + stripIllegalDirnameChars(conditions[i][1:-1])
            print("running:")
            print("  " + executable + " -f " + cfg_files_str + " -p GLOBAL-outputPrefix " + conditionDirectoryName + "/" + str(rep).zfill(padSizeReps) + "/ " + "GLOBAL-randomSeed " + str(rep) + " " + combinations[i][1:] + replacedConstantDefs)
            # make rep directory (this will also make the condition directory if it's not here already)
            call(["mkdir","-p", conditionDirectoryName + "/" + str(rep).zfill(padSizeReps)])
            if not args.runNo:
                sys.stdout.flush() # force flush before running MABE, otherwise sometimes MABE output shows before the above
                # turn combinations string into a list
                params = combinations[i][1:].split()
                params = [e.strip('"') for e in params]
                call([executable, "-f"] + cfg_files + ["-p", "GLOBAL-outputPrefix" , conditionDirectoryName + "/" + str(rep).zfill(padSizeReps) + "/" , "GLOBAL-randomSeed" , str(rep)] + params + replacedConstantDefs.split())
        if args.runHPCC or args.indefinite:
            if args.indefinite:
                HPCC_LONGJOB = True
            # go to the local directory (after each job is launched, we are in the work directory)
            os.chdir(absLocalDir)
            if (displayName == ""):
                realDisplayName = "C" + str(i).zfill(padSizeCombinations) + "_" + str(rep).zfill(padSizeReps) + "__" + stripIllegalDirnameChars(conditions[i][1:-1])
                conditionDirectoryName = "C" + str(i).zfill(padSizeCombinations) + "__" + stripIllegalDirnameChars(conditions[i][1:-1])
            else:
                realDisplayName = displayName + "_C" + str(i).zfill(padSizeCombinations) + "_" + str(rep).zfill(padSizeReps) + "__" + stripIllegalDirnameChars(conditions[i][1:-1])
                conditionDirectoryName = displayName + "_C" + str(i).zfill(padSizeCombinations) + "__" + stripIllegalDirnameChars(conditions[i][1:-1])
                
            timeNow = str(datetime.datetime.now().year) + '_' + str(datetime.datetime.now().month) + '_' + str(datetime.datetime.now().day) + \
                '_' + str(datetime.datetime.now().hour) + '_' + str(
                    datetime.datetime.now().minute) + '_' + str(datetime.datetime.now().second)

            workDir = pathToScratch + userName + '/' + \
                realDisplayName + '_' + str(rep).zfill(padSizeReps) + '__' + timeNow

            # this is where data files will actually be writen (on scratch)
            outputDir = workDir + '/' + \
                conditionDirectoryName + '/' + str(rep).zfill(padSizeReps) + '/'

            # if there was already a workDir, get rid of it.
            if os.path.exists(workDir):
                shutil.rmtree(workDir)
            os.makedirs(outputDir)
            shutil.copy(executable, workDir)  # copy the executable to scratch
            for f in cfg_files:
                shutil.copy(f, workDir)  # copy the settings files to scratch
            for f in replacedOther_files:
                shutil.copy(f, workDir)  # copy other files to scratch
            if (len(replacedPopulationLoaderDefs)):
                shutil.copy('population_loader.plf', workDir)

            # if the local conditions directory is not already here, make it
            if not(os.path.exists(conditionDirectoryName)):
                os.makedirs(conditionDirectoryName)

            # if there is already a link for this rep from the local conditions directory to scratch, remove it
            if os.path.exists(conditionDirectoryName + '/' + str(rep)):
                os.unlink(conditionDirectoryName + '/' + str(rep))

            # create local link from the local conditions directory to the rep directory in work directory
            os.symlink(workDir+'/'+conditionDirectoryName+'/'+str(rep).zfill(padSizeReps),conditionDirectoryName+'/'+str(rep).zfill(padSizeReps))

            os.chdir(workDir)  # goto the work dir (on scratch)

            slurmFileName = "slurm.sb"

            # make the qsub file on scratch
            makeQsubFile(realDisplayName = realDisplayName, conditionDirectoryName = conditionDirectoryName, rep = rep ,slurmFileName = slurmFileName, executable = executable, cfg_files = cfg_files, workDir = workDir, conditions = combinations[i][1:] + replacedConstantDefs, padSizeReps = padSizeReps)

            print("submitting:")
            print("  " + realDisplayName + " :")
            print("  workDir = " + workDir)
            print("  sbatch " + slurmFileName)
            if not args.runNo:
                call(["sbatch", slurmFileName])  # run the job

if args.runNo:
    print("")
    print("  You are using --runNo, so no jobs have been started!")
