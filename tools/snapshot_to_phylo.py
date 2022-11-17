#############################################################
## snapshot_to_phylo.py
## convert a set of snapshot_data files to a phylogny
##   files snapPhylo.dot and snapPhylo.json are produced
#############################################################


import pandas as pd

import argparse
parser = argparse.ArgumentParser()


parser.add_argument('-step', type=int, default = -1, help='time between snapshot_data files', required=False)
parser.add_argument('-end', type=int, default = -1, help='time on the last snapshot_data file', required=False)
parser.add_argument('-cutoff', type=int, default = 100, help='agents must have progony after at least this number of generations to be displayed', required=False)

parser.add_argument('-data_1_name', type=str, default = 'NONE', help='if set, will determine the red value of the nodes', required=False)
parser.add_argument('-data_2_name', type=str, default = 'NONE', help='if set, will determine the green value of the nodes', required=False)
parser.add_argument('-data_3_name', type=str, default = 'NONE', help='if set, will determine the blue value of the nodes', required=False)

parser.add_argument('-data_1_range', type=float, metavar=('MIN','MAX'), default = [0,1],  help='data will be scaled so that <= MIN is black and >= MAX is red', nargs=2, required=False)
parser.add_argument('-data_2_range', type=float, metavar=('MIN','MAX'), default = [0,1],  help='data will be scaled so that <= MIN is black and >= MAX is green', nargs=2, required=False)
parser.add_argument('-data_3_range', type=float, metavar=('MIN','MAX'), default = [0,1],  help='data will be scaled so that <= MIN is black and >= MAX is blue', nargs=2, required=False)

args = parser.parse_args()

showDot = True               # show dot format graph description
showJson = True              # show json format graph description

step = args.step           # time between snapshot_data files 
end = args.end             # time on the last snapshot_data file
cutoff = args.cutoff       # in generations

data_1_name = args.data_1_name    # this will show up in the red chanel
data_2_name = args.data_2_name    # this will show up in the green chanel
data_3_name = args.data_3_name    # this will show up in the blue chanel

data_1_range = args.data_1_range
data_2_range = args.data_2_range
data_3_range = args.data_3_range



def rgb_to_hex(rgb):
    return '%02x%02x%02x' % rgb
def rgb_dec_to_hex(rgb):
    rgb = ( int(rgb[0]*255), int(rgb[1]*255), int(rgb[2]*255) )
    return '%02x%02x%02x' % rgb



IDs = [[-1]]
ancestors = [[-1]]
counts = [[0]]
colors = [[(0,0,0)]]

data_1 = [[0]]
data_2 = [[0]]
data_3 = [[0]]


# load data

print('reading files ',end='')
for i in range(int(end/step)+1):
    fileName = 'snapshot_data_'+str(i*step)+'.csv'
    print('.',flush=True,end='' )
    df = pd.read_csv(fileName)
    
    IDs.append(list(df['ID']))
    ancestors.append(list(df['snapshotAncestors_LIST']))
    
    counts.append([0]*len(IDs[-1]))
    
    if data_1_name != 'NONE':
        data_1.append([ max(0,min(1, (v - data_1_range[0]) / (data_1_range[1]- data_1_range[0]) )) for v in list(df[data_1_name]) ])
    else:
        data_1.append([ 0 for v in range(len(IDs[-1])) ])
    if data_2_name != 'NONE':
        data_2.append([ max(0,min(1, (v - data_2_range[0]) / (data_2_range[1]- data_2_range[0]) )) for v in list(df[data_2_name]) ])
    else:
        data_2.append([ 0 for v in range(len(IDs[-1])) ])
    if data_3_name != 'NONE':
        data_3.append([ max(0,min(1, (v - data_3_range[0]) / (data_3_range[1]- data_3_range[0]) )) for v in list(df[data_3_name]) ])
    else:
        data_3.append([ 0 for v in range(len(IDs[-1])) ])

print()


# get phylo counts
print('calculating phylo depths ',end='')
for i in range(int(end/step)+1,0,-1):
    print('.',flush=True, end = '')
    for j in range(len(IDs[i])): # for each org in the current pop
        counts[i-1][IDs[i-1].index(ancestors[i][j])] = max(
            counts[i-1][IDs[i-1].index(ancestors[i][j])],
            counts[i][j]+1)
print()



if showDot:
	outString = ''

	adj_cutoff = int(cutoff/step)

	for i in range(1,len(IDs)-adj_cutoff):
		for j in range(len(IDs[i])):
			if counts[i][j] >= int(cutoff/step):
				outString+=str(ancestors[i][j])+' -> ' + str(IDs[i][j])+'\n'
				outString+=str(IDs[i][j])+'[style=filled,color="#'+rgb_dec_to_hex((data_1[i][j],data_2[i][j],data_3[i][j]))+'"]\n'
				if (data_1[i][j]>1 or data_2[i][j]>1 or data_3[i][j]>1):
					print('color over',IDs[i][j],data_1[i][j],data_2[i][j],data_3[i][j])


	print('saving snapPhylo.dot')
	
	text_file = open('snapPhylo.dot', 'w')
	text_file.write('digraph G {\n' + outString + '\n}\n')
	text_file.close()
    


if showJson:
	jsonData = {0:{}}
	for l in outString[:-1].split('\n'):
		ll = l.split(' ')
		if len(ll)>1:
			if ll[0] == '-1':
				jsonData[0][(int(ll[2]))]=[]
			elif int(ll[0]) in jsonData: 
				jsonData[int(ll[0])][(int(ll[2]))]=[]
			else:
				jsonData[int(ll[0])] = {int(ll[2]):[]}

	keys = list(reversed(sorted(jsonData.keys())))

	for k in keys:
		# find where this belongs...
		for elem in keys:
			#print(k,elem)
			if (elem in jsonData) and (k in jsonData[elem]):
				
				#print (k,elem,jsonData[elem])

				jsonData[elem].pop(k)
				jsonData[elem][k] = (jsonData[k])
				jsonData.pop(k)
				break
				
				
				
				
	def printIt(dict,outStr,indent):
		indent+=4
		
		
		for d in dict:
			if dict[d] == []:
				outStr+=(' '*indent+'{"name": "'+str(d)+'"},\n')
			else:
				outStr+=(' '*indent+'{"name": "'+str(d)+'",\n')
				outStr+=(' '*indent+'"children": [\n')
				outStr = printIt(dict[d],outStr,indent)
				outStr = outStr[:-2]+'\n'
				outStr+=(' '*indent+']},\n')
		return(outStr)
				
	outStr = printIt(jsonData,outStr = '',indent = 0)


	print('saving snapPhylo.json')
	
	text_file = open('snapPhylo.json', 'w')
	text_file.write(outStr[:-2]+'\n')
	text_file.close()
	
