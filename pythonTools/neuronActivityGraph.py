
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


import pandas as pd
import numpy as np


import argparse
parser = argparse.ArgumentParser()

parser.add_argument('-inPath', type=str, metavar='PATH', default = '',  help='path to data files - default : none (current directory)', required=False)
parser.add_argument('-outPath', type=str, metavar='PATH', default = '',  help='path where plots will be written - default : none (current directory)', required=False)
parser.add_argument('-dataFileName', type=str, metavar='PATH', default = 'neuron_data.txt',  help='name of data file - default : neuron_data.txt', required=False)
parser.add_argument('-outFileName', type=str, metavar='PATH', default = 'neuronActivity.png',  help='name of plot to be created - default : neuron_data.txt', required=False)


args = parser.parse_args()

data = pd.read_csv(args.inPath + args.dataFileName, delimiter = ',')

numGates = 0
IDList = data['ID'].values.tolist()
for value in IDList:
	numGates = max(numGates,value)
	
numGates += 1

thSourceList = data['thresholdFromNode'].values.tolist()
chSourceList = data['deliveryChargeFromNode'].values.tolist()

insList = data['inConnections'].values.tolist()
outsList = data['outConnections'].values.tolist()

typeList = data['thresholdActivates'].values.tolist()

firelist = data['fire'].values.tolist()
threshlist = data['thresholdValue'].values.tolist()
curchlist = data['currentCharge'].values.tolist()

firelistBreakOut = []
thBO = []
cchBO = []
maxth = []
maxcch = []

minth = []
mincch = []

for i in range(numGates):
	firelistBreakOut.append([])
	thBO.append([])
	maxth.append(1)
	minth.append(-1)
	cchBO.append([])
	maxcch.append(1)
	mincch.append(-1)

count = 0
time = 0


while count < len(firelist):
	for i in range(numGates):
		firelistBreakOut[i].append(0)
		firelistBreakOut[i][time]+=(float(firelist[count]-.5)) * 1000000
		
		thBO[i].append(0)
		thBO[i][time]+=threshlist[count]
		maxth[i] = max(maxth[i],threshlist[count]+1)
		minth[i] = min(minth[i],threshlist[count]-1)

		cchBO[i].append(0)
		cchBO[i][time]+=curchlist[count]
		maxcch[i] = max(maxcch[i],curchlist[count]+1)
		mincch[i] = min(mincch[i],curchlist[count]-1)

		count+=1
	time+=1
		
fig = plt.figure(figsize=(24,6),facecolor='white')
fig.subplots_adjust(hspace = .15,left=0, right=1, top=1, bottom=0)

for i in range(numGates):
	plt.subplot(numGates,1,i+1)
	#plt.ylim(-.2,1.2)
	#plt.xlim(0,1000)
	title = 'gate ID: ' + str(i) + '   inputs: ' + str(insList[i]) + '   outputs: ' + str(outsList[i])
	if (thSourceList[i] != -1):
		title += '   threshold from: [' + str(thSourceList[i]) + ']'
	if (chSourceList[i] != -1):
		title += '   delivery charge from: [' + str(chSourceList[i]) + ']'

	#plt.title(title)
	XRANGE = range(0,len(firelistBreakOut[i]))
	#plt.plot(XRANGE,firelistBreakOut[i], ".", color = (0,0,0), linewidth = 1, label = "fire", alpha = .25)
	if typeList[i] is 1:
		typeColor = (0,1,0) # activate = green
	else:
		typeColor = (1,0,0) # represive = red
	plt.fill_between(XRANGE,firelistBreakOut[i], np.linspace(-1000000, -100000, len(firelistBreakOut[i])), color = (typeColor,typeColor,typeColor), alpha = .5, linewidth = 0)
	
	plt.ylim(max(min(minth[i],mincch[i]),-100),min(max(minth[i],maxcch[i]),100000))
	#plt.ylim(min(minth[i],mincch[i]),max(minth[i],maxcch[i]))
	plt.xlim(0,len(firelistBreakOut[i]))
	#plt.ylim(-10,10)
	#plt.xlim(0,300)

	plt.plot(XRANGE,thBO[i], color = (1,1,1), linewidth = 2, alpha = .75)
	plt.plot(XRANGE,thBO[i], color = (0,0,0), linewidth = 1, label = "tresh")
	plt.plot(XRANGE,cchBO[i], color = (1,1,1), linewidth = 2, alpha = .75)
	plt.plot(XRANGE,cchBO[i], color = (.3,.3,1), linewidth = 1, label = "cch")
	plt.plot(XRANGE,thBO[i], color = (0,0,0), linewidth = 1, alpha = .5)

	plt.tick_params(
    axis='x',          # changes apply to the x-axis
    which='both',      # both major and minor ticks are affected
    bottom='off',      # ticks along the bottom edge are off
    top='off',         # ticks along the top edge are off
    labelbottom='off') # labels along the bottom edge are off

	plt.tick_params(
    axis='y',          # changes apply to the x-axis
    which='both',      # both major and minor ticks are affected
    bottom='off',      # ticks along the bottom edge are off
    top='off',         # ticks along the top edge are off
    labelbottom='off') # labels along the bottom edge are off

	
	
#plt.show()
plt.savefig(args.outPath+args.outFileName, dpi=300)
