#!/usr/bin/python

import matplotlib.pyplot as plt
import numpy as np
import math
from matplotlib.backends.backend_pdf import PdfPages
import csv
import pandas

from ast import literal_eval


#nodes = [1,2,3,4,5]
#edges = [(1,2),(4,5),(2,3),(3,1)]

nodes = [-1]
edges = []

for time in np.arange(0,17000,100):
  file_name = 'data_' + str(time) + '.csv'
  data_csv = pandas.read_csv(file_name)
  num_orgs = 0

  for n in data_csv['ID']:
    nodes.append(n)
    num_orgs = num_orgs + 1

  print (num_orgs)

  for i in range(num_orgs):
    for a in literal_eval(data_csv['ancestors_LIST'][i]):
      edges.append((data_csv['ID'][i],a))

f = open('workfile2.sif', 'w')
for edge in edges:
  f.write(str(edge[0]) + " x " + str(edge[1]) + "\n")

#G=nx.DiGraph()
#G.add_nodes_from(nodes)
#G.add_edges_from(edges)

#H=nx.DiGraph(G)
#pos=nx.spring_layout(G, dim=2, k=None, pos=None, fixed=None, iterations=50, weight='weight', scale=1.0, center=None)
#nx.draw(H,node_size=1)
#nx.draw(H,node_size=1)
#plt.show()



####
#
# BuildMultiPlot(DataMap,NamesList,XCoordinateName='',Columns=1)
#   Builds a Figure with rows = # elements in Names list.
#   Each row contains a graph generated from DataMap[NamesList[row#]]
#
#   DataMap : a dictornary with data (all entires must have the same number of elements)
#   NamesList : list of names of elements from data to be graphed
#   XCoordinateName : If specified, this element from the DataMap will determin the XCoordinate scale
#   Columns : spread the graphs over this many columns, if not defined, there will be 1 column
#             # of rows is calcualted by the number of columns and the number of elements in NamesList
#
#   Return: the figure created
#
####

def BuildMultiPlotFromDict(DataMap,NamesList,XCoordinateName='',Columns=1):
  plt.figure()                                                # create a new figure

  Rows = math.ceil(float(len(NamesList))/float(Columns))      # calcualate how many rows we need
  
  if (XCoordinateName==''):                                   # if there is no XCoordinateName
    for count in range(len(NamesList)):                       # for each name
      plt.subplot(Rows,Columns,count+1)                       # go to the count-th row of in our figure (with len(NamesList) rows)
      plt.plot(DataMap[NamesList[count]],label=NamesList[count])
                                                              # plot the data for each element in name in it's own plot
      plt.title(NamesList[count], fontsize=12) 	              # set the title for this plot

  else:                                                       # else, there is a XCoordinateName
    for count in range(len(NamesList)):                       # for each name
      plt.subplot(Rows,Columns,count+1)                       # go to the count-th row of in our figure (with len(NamesList) rows)
      plt.plot(DataMap[XCoordinateName],DataMap[NamesList[count]],label=NamesList[count])
                                                              # plot the data for each element in name in it's own plot
      plt.title(NamesList[count], fontsize=12)                # set the title for this plot

  return plt.gcf()                                            # gcf = get current figure - return that.

####
#
# BuildPlot(DataMap,NamesList,XCoordinateName='',AddLegend="")
#   Builds a figure with a single plot graphing the elements from DataMap named by NamesList.
#   Axis are labled if possible, and a legend is added.
#
#   DataMap : a dictornary with data (all entires must have the same number of elements)
#   NamesList : list of names of elements from data to be graphed
#   XCoordinateName : If specified, this element from the DataMap will determin the XCoordinate scale
#   AddLegend : if defined, places a legend ('upper left','upper right','lower left','lower right')
#
#   Return: the figure created
#
####

def BuildPlotFromDict(DataMap,NamesList,XCoordinateName='',AddLegend=''):
  plt.figure()                                                # create a new figure

  if XCoordinateName=='':                                     # if there is no XCoordinateName
    for count in range(len(NamesList)):                       # for each name
      plt.plot(DataMap[NamesList[count]],label=NamesList[count])
                                                              # plot the data for each element in name in it's own plot

  else:                                                       # else, there is a XCoordinateName
    for count in range(len(NamesList)):                       # for each name
      plt.plot(DataMap[XCoordinateName],DataMap[NamesList[count]],label=NamesList[count])
                                                              # plot the data for each element in name in it's own plot
      plt.xlabel(XCoordinateName)                             # add a X axis label

  if (AddLegend!=""):
    plt.legend(loc=AddLegend, shadow=True)                    # add a legend
    
  return plt.gcf()                                            # gcf = get current figure - return that.

	  
######## LOAD DATA




#data_csv_file = pandas.read_csv(r'data.csv')
#ave_csv_file = pandas.read_csv(r'ave.csv')
#dominant_csv_file = pandas.read_csv(r'dominant.csv')

#BuildMultiPlotFromDict(data_csv_file,NamesList = ['score','switches','food1','food2','gates','genomeSize'],XCoordinateName='update',Columns=2)
#BuildMultiPlotFromDict(ave_csv_file,NamesList = ['score','gates','genomeSize'],XCoordinateName='update',Columns=2)
#BuildMultiPlotFromDict(dominant_csv_file,NamesList = ['score','switches','food1','food2','gates','genomeSize'],XCoordinateName='update',Columns=2)

#BuildPlotFromDict(DataMap = Data, NamesList = ['food1','food2','switches'],XCoordinateName = 'update',AddLegend='lower right')
#BuildPlotFromDict(DataMap = Data, NamesList = ['food1','food2','switches'],XCoordinateName = 'update',AddLegend='lower right')

#plt.show()


######## SAVE TO A PNG FILE
#plt.savefig('testGraph.png', dpi=100)


######## SAVE TO A PDF FILE

#pp = PdfPages('foo.pdf')
#pp.savefig(fig1)
#pp.savefig(fig2)
#pp.savefig(DisplaysFig)
#pp.close()

  

