#!/usr/bin/python

import matplotlib.pyplot as plt
import numpy as np
import math
from matplotlib.backends.backend_pdf import PdfPages
import matplotlib.font_manager as font_manager
import pandas
import sys
import getopt


####
#
# usage()
#  Display usage
#
####

def usage():
	print('')
	print(sys.argv[0] + ' [-h][-t title][-s pdf|png][-l][-c "source,x-axis,[alpha X,][legend location,][style,][line width X,]data1,data2,..."]')
	print('')
	print('  -h show this help message')
	print('  -s do not display graph(s), save image(s) instead ("pdf" or "png" format)')
	print('  -l also load and graph Line of Decent (data.csv)')
	print('  -t add a prefix to a title')
	print('  -n number of columns in table (non-custom)')
	print('  -c make a custom graph - arguments must be in order, optional arguments can be left out')
	print('       source = ave | dom | LOD (for LOD, must also use -l flag)')
	print('       x-axis = which column to use for x-axis')
	print('       alpha = optional - transparency of ploted data (default "alpha .5")')
	print('             alpha [ 0 .. 1 ] ')
	print('       legend location = optional (default "lower right")')
	print('             upper left | upper right | lower left | lower right | center left | center right | upper center | lower center | center')
	print('       style = optional - style of plot (default "-dif")')
	print('             - | . | o | * | x | -- | -. | -o | -* | -x | .dif | -dif')
	print('             .dif = point plot with diffrent markers, -dif = line plot with diffrent markers')
	print('       line width = optional (default "line width 1")')
	print('             [1 .. ]')
	print('       data* = columns to graph')
	print('')

####
#
# BuildMultiPlot(DataMap,NamesList,XCoordinateName='',Columns=1,title = '')
#   Builds a Figure with rows = # elements in Names list.
#   Each row contains a graph generated from DataMap[NamesList[row#]]
#
#	MultiDataMap : a dictionary or dictionary with data (all entires must have the same number of elements)
#				e.g. if DataMap is Ave, this can be raw data - if this is not empty it will display at 20% alpha
#   DataMap : a dictionary with data (all entires must have the same number of elements)
#   NamesList : list of names of elements from data to be graphed
#   XCoordinateName : If specified, this element from the DataMap will determin the XCoordinate scale
#   Columns : spread the graphs over this many columns, if not defined, there will be 1 column
#				number of rows is calcualted by the number of columns and the number of elements in NamesList
#   title : title to display on top of image
#
#   Return: the figure created
#
####


def BuildMultiPlotFromDict(MultiDataMap,AveDataMap,NamesList,XCoordinateName='',Columns=1,title = ''):
	fig = plt.figure(figsize=(20,10))                                                # create a new figure
	fig.subplots_adjust(hspace=.35)

	if (title!=''):
		plt.suptitle(title, fontsize=20, fontweight='bold')

	Rows = math.ceil(float(len(NamesList))/float(Columns))      # calcualate how many rows we need
  
	if (XCoordinateName==''):                                   # if there is no XCoordinateName
		XLimit = len(AveDataMap[NamesList[0]])
		for count in range(len(NamesList)):                       # for each name
			ax = plt.subplot(Rows,Columns,count+1)                       # go to the count-th row of in our figure (with len(NamesList) rows)
			if len(AveDataMap) > 0:
				plt.plot(AveDataMap[NamesList[count]],label=NamesList[count])
			if len(MultiDataMap) > 0:
				for map in MultiDataMap:
					print ("X")
					plt.plot(MultiDataMap[name][NamesList[count]],label=NamesList[count],alpha = .2,color = (0,0,0))
	
                                                              # plot the data for each element in name in it's own plot
			plt.title(NamesList[count], fontsize=16) 	              # set the title for this plot
			ax.title.set_position([.5, .97])
			for label in (ax.get_xticklabels() + ax.get_yticklabels()):
				#label.set_fontname('Arial')
				label.set_fontsize(10)
			ax.xaxis.set_tick_params(pad=2)
			plt.xlim([0,XLimit])
	else:                                                       # else, there is a XCoordinateName
		XLimit = max(AveDataMap[XCoordinateName])
		for count in range(len(NamesList)):                       # for each name
			ax = plt.subplot(Rows,Columns,count+1)                       # go to the count-th row of in our figure (with len(NamesList) rows)
			if len(AveDataMap) > 0:
				plt.plot(AveDataMap[XCoordinateName],AveDataMap[NamesList[count]],label=NamesList[count])
			if len(MultiDataMap) > 0:
				for name in MultiDataMap:
					plt.plot(MultiDataMap[name][XCoordinateName],MultiDataMap[name][NamesList[count]],label=NamesList[count], alpha = .2,color = (0,0,0))
                                                             # plot the data for each element in name in it's own plot
			plt.title(NamesList[count], fontsize=16)                # set the title for this plot
			ax.title.set_position([.5, .97])
			for label in (ax.get_xticklabels() + ax.get_yticklabels()):
				#label.set_fontname('Arial')
				label.set_fontsize(10)
			ax.xaxis.set_tick_params(pad=2)
			plt.xlim([0,XLimit])

	return plt.gcf()                                            # gcf = get current figure - return that.

####
#
# BuildPlotFromDict(DataMap,NamesList,XCoordinateName='',AddLegend='',title = '',plotType='-',lineWeight = 2 , alpha = .5)
#   Builds a figure with a single plot graphing the elements from DataMap named by NamesList.
#   X Axis is labled if possible, and a legend is added.
#
#   DataMap : a dictionary with data (all entires must have the same number of elements)
#   NamesList : list of names of elements from data to be graphed
#   XCoordinateName : If specified, this element from the DataMap will determin the XCoordinate scale
#   AddLegend : if defined, places a legend ('upper left','upper right','lower left','lower right')
#   title : title to display on top of image
#   plotType : determines how the data will be plotted (- | . | o | * | x | -- | -. | -o | -* | -x | .dif | -dif)
#   lineWeight : thickness of lines (if any)
#   alpha : transparancy of plotted data
#
#   Return: the figure created
#
####

def BuildPlotFromDict(DataMap,NamesList,XCoordinateName='',AddLegend='',title = '',plotType='-',lineWeight = 2 , alpha = .5):
	plt.figure()                                                # create a new figure

	styleListPoint = ['o','*','s','D','^','.','o','*','s','D','^','.','o','*','s','D','^','.','o','*','s','D','^','.','o','*','s','D','^','.']
	styleListLine = ['-o','-*','-s','-D','-^','-.','-o','-*','-s','-D','-^','-.','-o','-*','-s','-D','-^','-.','-o','-*','-s','-D','-^','-.','-o','-*','-s','-D','-^','-.']
	styleListIndex = 0
	
	if XCoordinateName=='':                                     # if there is no XCoordinateName
		print ('no x-axis was provide in BuildPlotFromDict()')
		sys.exit()

		#for count in range(len(NamesList)):                       # for each name
		#	plt.plot(DataMap[NamesList[count]], plotType, linewidth = lineWeight, alpha = .5,label=NamesList[count])
                                                              # plot the data for each element in name in it's own plot

	else:                                                       # else, there is a XCoordinateName
		XLimit = max(DataMap[XCoordinateName])
		plt.xlim([0,XLimit])
		for count in range(len(NamesList)):                       # for each name
			if (plotType == '.dif'):
				plt.plot(DataMap[XCoordinateName],DataMap[NamesList[count]], styleListPoint[styleListIndex], linewidth = lineWeight, alpha = alpha, label=NamesList[count])
			elif (plotType == '-dif'):
				plt.plot(DataMap[XCoordinateName],DataMap[NamesList[count]], styleListLine[styleListIndex], linewidth = lineWeight, alpha = alpha, label=NamesList[count])
			else:
				plt.plot(DataMap[XCoordinateName],DataMap[NamesList[count]], plotType, linewidth = lineWeight, alpha = alpha, label=NamesList[count])
                                                              # plot the data for each element in name in it's own plot
			plt.xlabel(XCoordinateName)                             # add a X axis label
			styleListIndex = styleListIndex + 1
	if (AddLegend!=""):
		plt.legend(loc=AddLegend, shadow=True)                    # add a legend
  
	if (title!=''):
		plt.title(title)
	
	
	return plt.gcf()                                            # gcf = get current figure - return that.

	  
def main(argv=None):
	
### LOAD ARGUMENTS FROM COMMAND LINE
	if argv is None:
		argv = sys.argv
	try:
		opts, args = getopt.getopt(sys.argv[1:], "h-n:vt:vs:vc:vlr:v")
	except getopt.GetoptError as err:
		# print help information and exit:
		print()
		print (str(err)) # will print something like "option -a not recognized"
		usage()
		sys.exit(2)
	output = None
	graphRows = 2
	customOptions = None
	customTitle = ''
	replicates = None
	useLOD = False
	for option, value in opts:
		if option == "-v":
			verbose = True
		elif option in ("-h"):
			usage()
			sys.exit()
		elif option == "-t":
			customTitle = value
		elif option == "-n":
			graphRows = value
		elif option == "-c":
			customOptions = value
		elif option == "-r":
			replicates = value
		elif option in ("-s"):
			output = value
		elif option in ("-l"):
			useLOD = True
		else:
			assert False, "unhandled option"

### LOAD DATA

	allAve = {};
	allDom = {};
	allLOD = {};

	replicateNames = [];
	if replicates == None:
		replicateNames.append('./')
	else:
		replicateNames = replicates.split(',') # split up custom options
		
	for name in replicateNames:
		try:
			allAve[name] = pandas.read_csv(name+'ave.csv')
		except:
			print ('\nERROR: can not load aveage data, "' + name + 'ave.csv" can not be found\n')
			sys.exit()

		try:
			allDom[name] = pandas.read_csv(name+'dominant.csv')
		except:
			print ('\nERROR: can not load dominant data, "' + name + 'dominant.csv" can not be found\n')
			sys.exit()

		if useLOD:
			try:
				allLOD = pandas.read_csv(name+'data.csv')
			except:
				print ('\nERROR: can not load data on line of descent, "' + name + 'data.csv" can not be found\n')
				sys.exit()

	ave_csv_file = allAve[replicateNames[0]]
	dominant_csv_file = allDom[replicateNames[0]]

### MAKE DATA LIST (this defines the columns to graph)

	aveList = list(allAve[replicateNames[0]].columns.values)
	if "update" in aveList:
		aveList.remove('update')
	domList = aveList
	if useLOD:
		LODList = aveList

### FIND NUMBER OF ROWS

	rows = len(allAve[replicateNames[0]][aveList[0]])

### GET AVERAGES AND ERROR

# first make placeholders
	aveAve = allAve[replicateNames[0]].copy(deep=True)
	errorAve = aveAve.copy(deep=True)
	aveDom = aveAve.copy(deep=True)
	errorDom = aveAve.copy(deep=True)
	if useLOD:
		aveLOD = aveAve.copy(deep=True)
		errorLOD = aveAve.copy(deep=True)

# now visit all the places
	for row in list(range(0,rows)):
		for colName in aveList:
			aveSum = 0
			domSum = 0
			LODSum = 0
			for repName in replicateNames:
				aveSum = aveSum + allAve[repName][colName][row];
				domSum = domSum + allDom[repName][colName][row];
				if useLOD:
					LODSum = LODSum + allLOD[repName][colName][row];
			aveAve[colName][row] = aveSum/len(replicateNames)
			aveDom[colName][row] = domSum/len(replicateNames)
			if useLOD:
				aveLOD[colName][row] = LODSum/len(replicateNames)

			


### NOT A CUSTOM GRAPH

	if customOptions == None:	# make default graphs
		aveGraph = BuildMultiPlotFromDict(allAve,aveAve,NamesList = aveList,XCoordinateName='update',Columns=graphRows,title = customTitle + ' ' + 'Average')
		domGraph = BuildMultiPlotFromDict(allDom,aveDom,NamesList = domList,XCoordinateName='update',Columns=graphRows,title = customTitle + ' ' + 'Dominant')
		if useLOD:
			LODGraph = BuildMultiPlotFromDict(LOD_csv_file,NamesList = LODList,XCoordinateName='update',Columns=graphRows,title = customTitle + ' ' + 'Line of Descent')

### MAKE A CUSTOM GRAPH

	else:
		data_starts_at = 2 # if there are not optional settings, data to graph starts at [2]
		customList = customOptions.split(',') # split up custom options

### WHICH TYPE OF DATA ARE WE GRAPHING?

		if customList[0] == 'ave':
			source = aveAve;
		elif customList[0] == 'dom':
			source = aveDom
		elif customList[0] == 'LOD':
			if not useLOD:
				print ('\nERROR: must use -l option to use LOD in custom graph"\n')
				sys.exit()
			source = aveLOD
		else:
			print ('\nERROR: options for custom graph must start with "ave", "dom" or, "lod"\n')
			sys.exit()

### SET THE X-AXIS

		if customList[1] == '':
			print ('\nERROR: second option (x-axis) for custom graph was blank\n')
			sys.exit()
		elif customList[1] not in list(source.columns.values):
			print ('\nERROR: second option (x-axis) for custom graph "' + customList[1] + '" not found in data\n')
			sys.exit()

### SET OPTIONAL ALPHA	

		if customList[data_starts_at][:5] == 'alpha':
			_alpha = customList[data_starts_at]
			_alpha = _alpha[6:]
			_alpha = float(_alpha)
			data_starts_at = data_starts_at + 1
		else:
			_alpha = .5
			
### SET OPTIONAL LEGEND LOCATION

		if customList[data_starts_at] in ['upper right','upper left','lower left','lower right','center right','center left','lower center','upper center','center']:
			legLoc = customList[data_starts_at]
			data_starts_at = data_starts_at + 1
		else:
			legLoc = 'lower right'

### SET OPTIONAL PLOT STYLE
		if customList[data_starts_at] in ['-','.','o','*','x','--','-.','-o','-*','-x','.dif','-dif']:
			_plotType = customList[data_starts_at]
			data_starts_at = data_starts_at + 1
		else:
			_plotType = '-dif'

### SET OPTIONAL LINE WIDTH

		if customList[data_starts_at][:10] == 'line width':
			_lineWeight = int(customList[data_starts_at][11:])
			data_starts_at = data_starts_at + 1
		else:
			_lineWeight = 1

### EXTRACT NAMES OF COLUMNS TO GRAPH

		names = customList[data_starts_at:]
		for name in names:
			if name not in list(source.columns.values):
				print ('\nERROR: data column "' + name + '" for custom graph not found in "' + customList[0] + '" data\n')
				sys.exit()

### MAKE THE CUSTOM GRAPH

		customGraph = BuildPlotFromDict(source,NamesList = names, XCoordinateName=customList[1], AddLegend=legLoc, title = customTitle + ' from ' + customList[0], plotType = _plotType,lineWeight = _lineWeight, alpha = _alpha)
	

### IF -s IS NOT USED, THEN PRINT GRAPH(S) TO SCREEN

	if output == None:
		plt.show()
		
######## SAVE TO FILE
	
	if customTitle != '':
		customTitle = customTitle + ' '

######## SAVE TO A PNG FILE

	if output == 'png':
		if customOptions == None:
			aveGraph.savefig(customTitle + 'MABE_Graph_Ave.png', dpi=100)
			domGraph.savefig(customTitle + 'MABE_Graph_Dom.png', dpi=100)
			if useLOD:
				LODGraph.savefig(customTitle + 'MABE_Graph_LOD.png', dpi=100)
		else:
			customGraph.savefig(customTitle + 'MABE_CustomGraph.png', dpi=100)

######## SAVE TO A PDF FILE

	if output == 'pdf':
		if customOptions == None:
			pp = PdfPages(customTitle + 'MABE_Graphs.pdf')
			pp.savefig(aveGraph)
			pp.savefig(domGraph)
			if useLOD:
				pp.savefig(LODGraph)
		else:
			pp = PdfPages(customTitle + 'MABE_CustomGraph.pdf')
			pp.savefig(customGraph)

		pp.close()

######## MAIN

if __name__ == "__main__":
	#try:
		main()
	#except:
		#print ("\nERROR: error in input... use -h option for help\n")
		#sys.exit()
