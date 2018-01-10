#!/usr/bin/python

import matplotlib.pyplot as plt
import numpy as np
import math
from matplotlib.backends.backend_pdf import PdfPages
import csv
import pandas

from ast import literal_eval


import graphviz as gv
import functools
graph = functools.partial(gv.Graph, format='svg')
digraph = functools.partial(gv.Digraph, format='svg')


styles = {
    'graph': {
		'size':'10',
		'splines': 'line',
		'ratio':'3',
        'label': 'A Fancy Graph',
        'fontsize': '16',
        'fontcolor': 'white',
        'bgcolor': '#000000',
        'rankdir': 'BT',
    },
    'nodes': {
        'fontname': 'Helvetica',
        'shape': 'circle',
        'fontcolor': 'black',
        'fontsize': '15',
        'color': 'white',
        'style': 'filled',
        'fillcolor': 'white',
		'width' : '.01',
		'height' : '.01',
    },
    'edges': {
        'style': 'solid',
        'color': 'white',
		'penwidth':'2',
        'arrowhead': 'normal',
        'fontname': 'Courier',
        'fontsize': '15',
        'fontcolor': 'white',
    }
}


def apply_styles(graph, styles):
    graph.graph_attr.update(
        ('graph' in styles and styles['graph']) or {}
    )
    graph.node_attr.update(
        ('nodes' in styles and styles['nodes']) or {}
    )
    graph.edge_attr.update(
        ('edges' in styles and styles['edges']) or {}
    )
    return graph

def add_nodes(graph, nodes):
    for n in nodes:
        if isinstance(n, tuple):
            graph.node(n[0], **n[1])
        else:
            graph.node(n)
    return graph

def add_edges(graph, edges):
    for e in edges:
        if isinstance(e[0], tuple):
            graph.edge(*e[0], **e[1])
        else:
            graph.edge(*e)
    return graph

nodes = ["-1"]
edges = []

for time in np.arange(0,201,3):
  #file_name = 'root__snapshotData_' + str(time) + '.csv'
  file_name = 'root__SSwD_data_' + str(time) + '.csv'
  data_csv = pandas.read_csv(file_name)
  num_orgs = 0

  for n in data_csv['ID']:
    nodes.append(str(n))
    num_orgs = num_orgs + 1

  print (num_orgs)

  for i in range(num_orgs):
    #for a in literal_eval(data_csv['snapshotAncestors_LIST'][i]):
    for a in literal_eval(data_csv['ancestors_LIST'][i]):
      edges.append((str(data_csv['ID'][i]),str(a)))

#print(nodes)
#print(edges)
#print(gv.ENGINES)
apply_styles(add_edges(
    #add_nodes(digraph(engine = 'neato'), nodes),
    add_nodes(digraph(engine = 'dot'), nodes),
    edges
),styles).render('img/g4')


f = open('workfile2.sif', 'w')
for edge in edges:
  f.write(str(edge[0]) + " x " + str(edge[1]) + "\n")


  

