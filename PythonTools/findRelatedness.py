
# coding: utf-8

# In[62]:


import matplotlib.pyplot as plt
import numpy as np
import math
from matplotlib.backends.backend_pdf import PdfPages
import matplotlib.font_manager as font_manager
import pandas
import sys
import getopt

import ast

def findMRCA(ID1,ID2,all):
    found = False
    currentTime = 0
    set1 = set([ID1])
    set2 = set([ID2])
    newSet1 = set1
    newSet2 = set2
    
    while (found == False and currentTime >= 0):
        if len(set1&set2) > 0:
            found = True
            #print ("found common:")
            #print (set1&set2)
            #print ("at time:")
            #print (currentTime)
            #print ("___________")
        else:
            tempSet = set()
            for ID in newSet1:
                if (ID != -1):
                    tempSet = tempSet.union(all[ID])
                else:
                    tempSet.add(-1)
            set1 = set1.union(tempSet)
            newSet1 = tempSet;
            tempSet = set()
            
            for ID in newSet2:
                if (ID != -1):
                    tempSet = tempSet.union(all[ID])
                else:
                    tempSet.add(-1)
            set2 = set2.union(tempSet)
            newSet2 = tempSet
            currentTime += 1
            
    if not found:
        print("NONE")
    return currentTime


def loadData(fileLocation,filePrefix,start,end,step):
    godFrame = pandas.DataFrame()

    for u in range(start,end+1,step):
        print(str(u)+" ",end="")
        tempFrame =  pandas.read_csv(fileLocation+filePrefix+str(u)+".csv")
        godFrame = godFrame.append(tempFrame, ignore_index=True)
    print()
    return godFrame


def makeRelatednessChart(Frame,lastPop):
    print("making Relatedness Chart")
    all = {}
    for ID in Frame["ID"]:
        x = Frame[Frame["ID"]==ID]["ancestors"]
        xx = set([ast.literal_eval(l) for l in x][0])
        all[ID]=xx
    print("ID lookup table constructed")
    relatednessChart = []
    for ID1 in lastPop:
        print("generating relatedness for ID " + str(ID1)+" ")
        newRow = []
        for ID2 in lastPop:
            newRow.append(findMRCA(ID1,ID2,all))
        relatednessChart.append(newRow)
    return relatednessChart

start = 0
end = 200
step = 1

fileLocation = "/home/cliff/eclipse/workspace/3BerrySexDiploid/"
filePrefix = "data_"

godFrame = loadData(fileLocation,filePrefix,start,end,step)

tempFrame =  pandas.read_csv(fileLocation+filePrefix+str(end)+".csv")
lastPop = []
for ID in tempFrame["ID"]:
    lastPop.append(ID)

relatednessChart = makeRelatednessChart(godFrame,lastPop)

print()
for r in range(len(relatednessChart)):
    print (str(lastPop[r]) + " ID: ",end="")
    for c in range(len(relatednessChart[r])):
        print (str(relatednessChart[r][c]) + "  ",end = "")
    print()


