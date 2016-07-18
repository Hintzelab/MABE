# -*- coding: utf-8 -*-
# <nbformat>3.0</nbformat>
#%matplotlib inline

import argparse
parser = argparse.ArgumentParser()


parser.add_argument('-path', type=str, metavar='PATH', default = '',  help='path to files - default : none (will read files in current directory)', required=False)
parser.add_argument('-conditions', type=str, metavar=('CONDITION'), default = [''],  help='names of condition directories - default: none (will use files in path directly)',nargs='+', required=False)
parser.add_argument('-conditionNames', type=str, metavar=('CONDITION_NAME'), default = [''],  help='names to dispaly. must have same number of elements as conditions if defined - default: none (will use conditionNames)', nargs='+',required=False)
parser.add_argument('-files', type=str, metavar='FILE_PREFIX', default = ['ave','dominant'], help='file prefex(s) - default: ave dominant', nargs='+', required=False)
parser.add_argument('-repRange', type=int, metavar=('FIRST','LAST'), default = [1,0],  help='replicate range - default: none (will use files in path directly)', nargs=2, required=False)
parser.add_argument('-repList', type=str, metavar='REP', default = [],  help='replicate list. useful if you are missing a replicat. cannot be used with repRange - default: none (will use files in path directly)', nargs='+', required=False)

parser.add_argument('-save', type=str, choices=('pdf','png'), default = '',  help='save files rather then display as either pdf or png - default: none (display image)', required=False)

parser.add_argument('-data', type=str, metavar='COLUMN_NAME', default = [''],  help='column names of data to be graphed - default : none (will attempt to graph all columns from all files)',nargs='+', required=False)
parser.add_argument('-dataFromFile', type=str, metavar='FILE_NAME', default = 'ave',  help='this file will be used to determine with column names of data will be graphed. If this file is not in files, then all data will be plotted - default : ave', required=False)
parser.add_argument('-xAxis', type=str, metavar='COLUMN_NAME', default = 'update',  help='column name of data to be used on x axis - default : update', required=False)
parser.add_argument('-dataIndex', type=str, metavar='COLUMN_NAME', default = 'update',  help='column name of data to be used as index when generating averages - default : update', required=False)

parser.add_argument('-pltWhat', type=str, metavar='{ave,error,reps}',choices=('ave','error','reps'), default = ['ave','error'], help='what should be ploted. ave (averages), error, reps (show data for all reps) - default : ave error', nargs='+', required=False)
parser.add_argument('-pltStyle', type=str, choices=('line','point','randomLine','randomPoint'), default = 'line', help='plot style. Random is useful if plotting multiple data on the same plot - default : line', required=False)
parser.add_argument('-errorStyle', type=str, choices=('region','bar','barX','barXY'), default = 'region', help='how error is ploted - default : region', required=False)
#parser.add_argument('-errorMethod', type=str, choices=('stderr'), default = ['stderr'], help='what error is ploted - default : region', required=False)

parser.add_argument('-numCol', type=str, metavar='#', default = '3', help='if ploting a multi plot (default), how many columns in plot - default : 3', required=False)
parser.add_argument('-combineConditions', action='store_true', default = False, help='if ploting multiple conditions, adding this flag will combine data from files with same name - default (if not set) : OFF', required=False)
parser.add_argument('-combineData', action='store_true', default = False, help='if ploting multiple data lines, adding this flag will combine data into one plot - default (if not set) : OFF', required=False)

parser.add_argument('-verbose', action='store_true', default = False, help='adding this flag will provide more text output while running (useful if you are working with a lot of data to make sure that you are not hanging) - default (if not set) : OFF', required=False)

parser.add_argument('-legendLocation', type=str, choices=('ur','ul','lr','ll','cr','cl','lc','uc','c'), default = 'lr', help='if ledgends are needed this is determins placement (first letter u = upper, c = center, l = lower. second letter l = left, c = center, r = right) - default : lr (lower right)', required=False)
parser.add_argument('-legendLineWeight', type=int, default = -1, help='changes line thickness in legend - default : lineWeight', required=False)
parser.add_argument('-lineWeight', type=int, default = 1, help='changes line thickness of lines in plots - default : 1', required=False)

parser.add_argument('-grid', action='store_true', default = False, help='if set, this flag cause a grid to be displayed on plots - default : OFF', required=False)
parser.add_argument('-fontSizeMajor', type=int, default = 15, help='size of "Major" fonts (main title) - default : 15', required=False)
parser.add_argument('-fontSizeMinor', type=int, default = 10, help='size of "Minor" fonts (subplot titles and lables) - default : 10', required=False)
parser.add_argument('-fontSizeTicks', type=int, default = 8, help='size of font for axis ticks - default : 8', required=False)

parser.add_argument('-whereValue', type=str, default = 'update', help='only plot data where this column has values defined by whereRange - default : update', required=False)
parser.add_argument('-whereRange', type=int, default = [], help='only plot data where column with name set by whereValue has values defined this range. Single value, just this value. Two values, inclusive range. Three values, inclusive range with step. - default : none', nargs='+', required=False)

## trick to allow "-" in an argument name! Parse it seperatly and then remove from sys.argv
#tempPltStyle = '-'
#if ('-pltStyle' in sys.argv):
#	pltStyleIndex = sys.argv.index('-pltStyle')
#	tempPltStyle = sys.argv[pltStyleIndex+1]
#	sys.argv.remove(sys.argv[pltStyleIndex+1])
#	sys.argv.remove('-pltStyle')
## end of trick	

args = parser.parse_args()
#args.pltStyle = tempPltStyle

if args.save != "":
	import matplotlib
	matplotlib.use('Agg')

import matplotlib.pyplot as plt
import numpy as np
import math
from matplotlib.backends.backend_pdf import PdfPages
import matplotlib.font_manager as font_manager
import pandas
import sys
import getopt
#from scipy import stats

import ast


def MultiPlot(data, NamesList, ConditionsList, dataIndex, CombineData = False, PltWhat = ['ave','error'], PltStyle = 'line', ErrorMethod = 'stderr', ErrorStyle = 'region', Reps = [''], XCoordinateName = '', Columns = 3, title = '', legendLocation = "lower right"):
	MajorFontSize = args.fontSizeMajor
	MinorFontSize = args.fontSizeMinor
	TickFontSize = args.fontSizeTicks

	styleListColor = [(0,0,1),(0,1,0),(1,0,0),(.25,.75,0),(0,.25,.75),(.75,0,.25),(0,0,0),(1,0,0),(0,1,0),(0,0,1),(.25,.75,0),(0,.25,.75),(.75,0,.25),(0,0,0),(1,0,0),(0,1,0),(0,0,1),(.5,.5,0),(0,.5,.5),(.5,0,.5),(0,0,0)]
	styleListPoint = ['o','*','s','D','^','.','o','*','s','D','^','.','o','*','s','D','^','.','o','*','s','D','^','.','o','*','s','D','^','.']
	styleListLine = ['-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-',]
	styleListRandomLine = ['-^','-o','-*','-s','-D','-^','-.','-o','-*','-s','-D','-^','-.','-o','-*','-s','-D','-^','-.','-o','-*','-s','-D','-^','-.']
	styleListRandomPoint = ['^','.','o','*','s','D','^','.','o','*','s','D','^','.','o','*','s','D','^','.','o','*','s','D','^','.']
	PltColor = (0,0,0)

	if PltStyle == 'line':
		styleList = styleListLine
		PltStyle = '-'
	if PltStyle == 'point':
		styleList = styleListPoint
		PltStyle = 'o'
	if PltStyle == 'randomLine':
		styleList = styleListRandomLine
		PltStyle = '-'
	if PltStyle == 'randomPoint':
		styleList = styleListRandomPoint
		PltStyle = 'o'

	fig = plt.figure(figsize=(20,10))                                                # create a new figure
	fig.subplots_adjust(hspace=.35)

	if XCoordinateName in NamesList:
		NamesList.remove(XCoordinateName)
	
	title += '    x axis = ' + XCoordinateName
	plt.suptitle(title, fontsize=MajorFontSize, fontweight='bold')
	
	if len(NamesList) == 1:
		Columns = 1;
	if len(NamesList) == 2:
		Columns = 2;
	
	Rows = math.ceil(float(len(NamesList))/float(Columns))      # calcualate how many rows we need
	for conditionCount in range(len(ConditionsList)):
		for nameCount in range(len(NamesList)):
			plt.tick_params(labelsize=TickFontSize)
			if not CombineData:
				ax = plt.subplot(Rows,Columns,nameCount+1)
				plt.title(NamesList[nameCount], fontsize=MinorFontSize) 	              # set the title for this plot
				ax.title.set_position([.5, 1])
				if (len(ConditionsList) > 1):
					PltStyle = styleList[conditionCount]
					PltColor = styleListColor[conditionCount]
				ThisLabel = ConditionsList[conditionCount]
			elif len(ConditionsList) > 1 or len(NamesList) > 1:
				PltStyle = styleList[conditionCount + (nameCount * len(ConditionsList))]
				PltColor = styleListColor[conditionCount + (nameCount * len(ConditionsList))]
				if (len(ConditionsList) == 1):
					ThisLabel = NamesList[nameCount]
				else:
					ThisLabel = ConditionsList[conditionCount] + ' ' + NamesList[nameCount]
			if args.grid:
				plt.grid(b=True, which='major', color=(0,0,0), linestyle='-', alpha = .25)
			if 'reps' in PltWhat:
				for Rep in Reps:
					plt.plot(data[data["repName"] == Rep][data["con"] == ConditionsList[conditionCount]][XCoordinateName],
						data[data["repName"] == Rep][data["con"] == ConditionsList[conditionCount]][NamesList[nameCount]],
						PltStyle, alpha = .25, color = PltColor)
			if ('ave' in PltWhat) or ('error' in PltWhat):
				aveLine = data[data["con"] == ConditionsList[conditionCount]].pivot(index = dataIndex, columns ='repName', values = NamesList[nameCount]).mean(axis=1)
				aveXCoordinate = data[data["con"] == ConditionsList[conditionCount]].pivot(index = dataIndex, columns ='repName', values = XCoordinateName).mean(axis=1)
				plt.plot(aveXCoordinate, aveLine, PltStyle, color = PltColor, linewidth = args.lineWeight, label = ThisLabel)
			if 'error' in PltWhat:
				if (ErrorMethod == "stderr"):
					errorLineY = data[data["con"] == ConditionsList[conditionCount]].pivot(index = dataIndex, columns='repName', values = NamesList[nameCount]).std(axis=1)
					errorLineX = data[data["con"] == ConditionsList[conditionCount]].pivot(index = dataIndex, columns='repName', values = XCoordinateName).std(axis=1)
				else:
					print ('ERROR: errorMethod "' + ErrorMethod + '" not found.')
					exit()
				if (ErrorStyle == 'bar'):
					plt.errorbar(aveXCoordinate, aveLine,yerr = errorLineY,color = PltColor, alpha = .5,fmt='.')
				if (ErrorStyle == 'barX'):
					plt.errorbar(aveXCoordinate, aveLine,xerr = errorLineX,color = PltColor, alpha = .5,fmt='.')
				if (ErrorStyle == 'barXY'):
					plt.errorbar(aveXCoordinate, aveLine,xerr = errorLineX,yerr = errorLineY,color = PltColor, alpha = .5,fmt='.')
				if (ErrorStyle == 'region'):
					plt.fill_between(aveXCoordinate, aveLine - errorLineY,aveLine + errorLineY, color = PltColor, alpha = .15)
			if ((len(ConditionsList) > 1) or (CombineData))and legendLocation != '':
				plt.xlabel(XCoordinateName, fontsize=MinorFontSize)
				leg = plt.legend(loc=legendLocation, shadow=True)                    # add a legend
				if (args.legendLineWeight > 0):
					for legobj in leg.legendHandles:
						legobj.set_linewidth(args.legendLineWeight)
	return plt.gcf()                                            # gcf = get current figure - return that.		

plt.rcParams['figure.figsize'] = (10,8)

path = args.path


if args.repRange != [1,0] and args.repList != []:
	print ('Error in input. -repRange and -repList are mutualy exclusive, please only define one!')
	exit()
rangeStart = args.repRange[0]
rangeEnd = args.repRange[1]
if args.repRange == [1,0] and args.repList != []:
	reps = args.repList
else:
	reps = range(rangeStart,rangeEnd+1)
if list(reps) == []:
	reps = ['']
else:
	reps = [str(i) + '/' for i in reps]

files = args.files

conFileNames = args.conditions
if conFileNames != ['']:
	for i in range(len(conFileNames)):
		if conFileNames[i][-1] is not '/':
			conFileNames[i]+='/'

if args.conditionNames == ['']:
	cons = []
	for name in conFileNames:
		cons.append(name[:-1])
else:
	cons = args.conditionNames

if (len(cons) != len(conFileNames)):
	print ('Error in input. -conditions and -conditionNames must have the same number of arguments')
	exit()

#if conFileNames != ['']:
	#conFileNames = [i+'/' for i in conFileNames]
	####cons = [i+'__' for i in cons]

realLedgendList = ['upper right','upper left','lower right','lower left','center right','center left','lower center','upper center','center']
abrvLedgendList = ['ur','ul','lr','ll','cr','cl','lc','uc','c']
args.legendLocation = realLedgendList[abrvLedgendList.index(args.legendLocation)]

namesList = args.data

godFrames = {}
for file in files:
	godFrames[file]=pandas.DataFrame()
conCount = 0
for con in cons:
	for file in files:
		for rep in reps:
			if args.verbose:
				print ("loading file: " + path + conFileNames[conCount] + rep + file + ".csv")
			tempFrame =  pandas.read_csv(path + conFileNames[conCount] + rep + file + ".csv")
			tempFrame["repName"]=rep
			tempFrame["con"]=con
			godFrames[file] = godFrames[file].append(tempFrame, ignore_index=True)
		if args.dataFromFile == file and args.data == ['']:
			namesList = list(tempFrame.columns.values)
			namesList.remove("repName")
			namesList.remove("con")
	conCount = conCount + 1

if namesList == ['']:
	namesList = list(godFrames[files[0]].columns.values)
	namesList.remove("repName")
	namesList.remove("con")
	if args.xAxis in namesList:
		namesList.remove(args.xAxis)
if args.dataIndex == 'undefined':
	args.dataIndex = args.xAxis

for name in godFrames:
	godFrames[name].reindex()
	if len(args.whereRange)==1:
		godFrames[name]=godFrames[name][godFrames[name][args.whereValue]==args.whereRange[0]]
	if len(args.whereRange)==2:
		godFrames[name]=godFrames[name][godFrames[name][args.whereValue] >= args.whereRange[0]]
		godFrames[name]=godFrames[name][godFrames[name][args.whereValue] <= args.whereRange[1]]
	if len(args.whereRange)==3:
		cropRange = list(range(args.whereRange[0],args.whereRange[1]+1,args.whereRange[2]))
		godFrames[name]=godFrames[name][godFrames[name][args.whereValue].isin(cropRange)]

allGraphs = {}

if args.combineConditions:
	for file in files:
		if args.verbose:
			print ("generating plot for: " + file)
		allGraphs[file] = MultiPlot(data = godFrames[file], PltWhat = args.pltWhat, ConditionsList = cons, CombineData = args.combineData, PltStyle = args.pltStyle, ErrorMethod = 'stderr', ErrorStyle = args.errorStyle, Reps = reps, NamesList = namesList, XCoordinateName = args.xAxis, dataIndex = args.dataIndex, Columns = args.numCol, title = file,legendLocation = args.legendLocation)#plt.gcf()

else:
	for con in cons:
		for file in files:
			if args.verbose:
				print ("generating plot for: " + con + "__" + file)
			allGraphs[con+'__'+file] = MultiPlot(data = godFrames[file], PltWhat = args.pltWhat, ConditionsList = [con], CombineData = args.combineData, PltStyle = args.pltStyle, ErrorMethod = 'stderr', ErrorStyle = args.errorStyle, Reps = reps, NamesList = namesList, XCoordinateName = args.xAxis, dataIndex = args.dataIndex, Columns = args.numCol, title = con + "__" + file)#plt.gcf()

if args.save == '':
	plt.show()

######## SAVE TO FILE
customOptions = None
useLOD = False
customTitle = '';
if customTitle != '':
 customTitle = customTitle + ' '

######## SAVE TO A PNG FILE

if args.save == 'png':
	if customOptions == None:
		for g in allGraphs:
			allGraphs[g].savefig(customTitle + 'MABE_Graph_' + g + '.png', dpi=100)
 #else:
	#customGraph.savefig(customTitle + 'MABE_CustomGraph.png', dpi=100)

######## SAVE TO A PDF FILE

if args.save == 'pdf':
	if customOptions == None:
		pp = PdfPages(customTitle + 'MABE_Graphs.pdf')
		for g in allGraphs:
			pp.savefig(allGraphs[g])
		pp.close()

 #else:
	#pp = PdfPages(customTitle + 'MABE_CustomGraph.pdf')
	#pp.savefig(customGraph)
	#pp.close()
