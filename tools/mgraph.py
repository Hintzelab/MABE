# -*- coding: utf-8 -*-
# <nbformat>3.0</nbformat>
#%matplotlib inline


#check dependencies
from utils import pyreq
pyreq.require("matplotlib,pandas")


#gather command line arguments
import argparse
parser = argparse.ArgumentParser()

#data file specification parameters (change which files are loaded)
parser.add_argument('-path', type=str, metavar='PATH', default = '',  help='path to files - default : none (will read files in current directory)', required=False)
parser.add_argument('-conditions', type=str, metavar=('CONDITION'), default = [''],  help='names of condition directories - default: none (will use files in path directly)',nargs='+', required=False)
parser.add_argument('-files', type=str, metavar='FILE(s)', default = ['pop.csv'], help='file name(s) - default: pop.csv dominant.csv', nargs='+', required=False)
parser.add_argument('-repRange', type=int, metavar=('FIRST','LAST'), default = [1,0],  help='replicate range - default: none (will use files in path directly)', nargs=2, required=False)
parser.add_argument('-repList', type=str, metavar='REP', default = [],  help='replicate list. useful if you are missing a replicate. cannot be used with repRange - default: none (will use files in path directly)', nargs='+', required=False)

#data filtering parameters (change what data is displayed)
parser.add_argument('-data', type=str, metavar='COLUMN_NAME', default = [''],  help='column names of data to be graphed. Can contain wildcards(*) but then arguments should be closed in single quotes(\'\')- default : none (will attempt to graph all columns from first file, and those columns in all other files)',nargs='+', required=False)
parser.add_argument('-dataFromFile', type=str, metavar='FILE_NAME', default = '',  help='this file will be used to determine with column names of data will be graphed. If this file is not in files, then all data will be plotted - default : NONE', required=False)
parser.add_argument('-ignoreData', type=str, metavar='COLUMN_NAME', default = [''],  help='column names of data to be ignored (this will override data). Can contain wildcards(*) but then arguments should be closed in single quotes(\'\')- default : none (will attempt to graph all columns from first file, and those columns in all other files)',nargs='+', required=False)
parser.add_argument('-whereValue', type=str, default = 'update', help='only plot data where this column has values defined by whereRange - default : update', required=False)
parser.add_argument('-whereRange', type=int, default = [], help='only plot data where column with name set by whereValue has values defined this range. Single value, just this value. Two values, inclusive range. Three values, inclusive range with step. - default : none', nargs='+', required=False)
parser.add_argument('-whereRangeLimitToData', action='store_true', default = False, help='set whereRange max based on rep with least data - default : OFF', required=False)
parser.add_argument('-lastOnly', action='store_true', default = False, help='shows only the last data point of all conditions - default (if not set) : OFF', required=False)

#data display parameters (change how data is displayed)
parser.add_argument('-xAxis', type=str, metavar='COLUMN_NAME', default = 'update',  help='column name of data to be used on x axis - default : update', required=False)
parser.add_argument('-dataIndex', type=str, metavar='COLUMN_NAME', default = 'update',  help='column name of data to be used as index when generating averages - default : update', required=False)
parser.add_argument('-yRange', type=float, default = [], help='if set, determines the range on the y axis; expects 2 values - default : none', nargs='+', required=False)
parser.add_argument('-xRange', type=float, default = [], help='if set, determines the range on the x axis; expects 2 values - default : none', nargs='+', required=False)
parser.add_argument('-pltWhat', type=str, metavar='{ave,std,sem,95conf,99conf,reps}',choices=('ave','std','sem','95conf','99conf','reps'), default = ['ave','95conf'], help='what should be ploted. ave (averages), std (Standard Deviation), sem (Standard Error from the Mean), 95conf (95 percent confidence intervals), 99conf (99 percent confidence intervals), reps (show data for all reps) - default : ave 95conf', nargs='+', required=False)
parser.add_argument('-integrate', type=str, default = [], metavar='_DATA', help='attempt to integrate associated data with _AVE data of same name (e.g. if "_VAR", plot "_VAR" with "_AVE" so "score_VAR" plots with "score_AVE"). Integrated data will not appear in its own plot. This will only work on single reps. -integrate will work with combine conditions (two or more reps can be loaded as conditions).', nargs='+', required=False)
parser.add_argument('-combineConditions', action='store_true', default = False, help='if ploting multiple conditions, adding this flag will combine data from files with same name - default (if not set) : OFF', required=False)
parser.add_argument('-combineData', action='store_true', default = False, help='if ploting multiple data lines, adding this flag will combine data into one plot - default (if not set) : OFF', required=False)

#plot parameters (changes aspects of the plot independent of data)
parser.add_argument('-title', type=str, default = 'NONE',  help='title of image - default: none (MGraph will make something up)', required=False)
parser.add_argument('-conditionNames', type=str, metavar=('CONDITION_NAME'), default = [''],  help='names to dispaly. must have same number of elements as conditions if defined - default: none (will use conditionNames)', nargs='+',required=False)
parser.add_argument('-imageSize', type=float, default = [10,10], help='size of image to be created - default : 10 10', nargs=2, required=False)
parser.add_argument('-pltStyle', type=str, choices=('line','point','randomLine','randomPoint'), default = 'line', help='plot style. Random is useful if plotting multiple data on the same plot - default : line', required=False)
parser.add_argument('-errorStyle', type=str, choices=('region','bar','barX','barXY'), default = 'region', help='how error is ploted - default : region', required=False)
parser.add_argument('-numCol', type=int, metavar='#', default = 3, help='if ploting a multi plot (default), how many columns in plot - default : 3', required=False)
parser.add_argument('-legendLocation', type=str, choices=('ur','ul','lr','ll','cr','cl','lc','uc','c','off'), default = 'lr', help='if legends are needed this is determins placement (first letter u = upper, c = center, l = lower. second letter l = left, c = center, r = right, off = off) - default : lr (lower right)', required=False)
parser.add_argument('-legendLineWeight', type=int, default = -1, help='changes line thickness in legend - default : lineWeight', required=False)
parser.add_argument('-lineWeight', type=int, default = 1, help='changes line thickness of lines in plots - default : 1', required=False)
parser.add_argument('-grid', action='store_true', default = False, help='if set, this flag cause a grid to be displayed on plots - default : OFF', required=False)
parser.add_argument('-fontSizeMajor', type=int, default = 15, help='size of "Major" fonts (main title) - default : 15', required=False)
parser.add_argument('-fontSizeMinor', type=int, default = 10, help='size of "Minor" fonts (subplot titles and lables) - default : 10', required=False)
parser.add_argument('-fontSizeTicks', type=int, default = 8, help='size of font for axis ticks - default : 8', required=False)
parser.add_argument('-fontSizeLegend', type=int, default = 8, help='size of font in legend - default : 8', required=False)

#utility parameters (does not change data or display)
parser.add_argument('-showDataNames', action='store_true', default = False, help='print the names of the columns in the first file listed - default : OFF', required=False)
parser.add_argument('-verbose', action='store_true', default = False, help='adding this flag will provide more text output while running (useful if you are working with a lot of data to make sure that you are not hanging) - default (if not set) : OFF', required=False)
parser.add_argument('-save', type=str, choices=('pdf','png'), default = '',  help='save files rather then display as either pdf or png - default: none (display image)', required=False)
parser.add_argument('-saveName', type=str, default = '',  help='if saveFile is png or pdf, and only one file is being created, use this for the file name. - default: none (mGraph will make up a name)', required=False)

args = parser.parse_args()

# check for invalid argument settings
if args.repRange != [1,0] and args.repList != []:
    print ('Error in input. -repRange and -repList are mutually exclusive, please only define one!')
    exit()
if args.saveName != "" and args.save == "png" and len(args.files) > 1:
    print("\n\n-saveName was provided, but more then one image file will be created because more then input file was listed and -save is png.\n\n  either save as type pdf (for a combined image file),\n  or run mGraph for each input file\n  or remove -saveName")
    exit()

#continue with script...
if args.save != "":
    from matplotlib import use
    use('Agg')

# imports
from pandas import read_csv, concat
import pandas
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from matplotlib.backends.backend_pdf import PdfPages
from math import ceil
from fnmatch import fnmatchcase


def isolate_condition(df, con):
    return df[df['con'] == con]


def add_error_bars(ErrorStyle, x_axis_values, aveLine, errorLineY, PltColor):
    if (ErrorStyle == 'bar'): plt.errorbar(x_axis_values, aveLine, yerr=errorLineY, color=PltColor, alpha=0.5, fmt='.')
    if (ErrorStyle == 'barX'): plt.errorbar(x_axis_values, aveLine, xerr=x_axis_values, color=PltColor, alpha=0.5, fmt='.')
    if (ErrorStyle == 'barXY'): plt.errorbar(x_axis_values, aveLine, xerr=x_axis_values, yerr=errorLineY, color=PltColor, alpha=0.5, fmt='.')
    if (ErrorStyle == 'region'): plt.fill_between(x_axis_values, aveLine-errorLineY, aveLine+errorLineY, color=PltColor, alpha=0.15)


def MultiPlot(data, NamesList, ConditionsList, dataIndex, CombineData = False, PltWhat = ['ave','95conf'], PltStyle = 'line', ErrorStyle = 'region', Reps = [''], XCoordinateName = '', Columns = 3, title = '', legendLocation = "lower right", xRange = [], yRange = [], integrateNames = [], imageSize = [10,10]):
    
    global args
    MajorFontSize = args.fontSizeMajor
    MinorFontSize = args.fontSizeMinor
    TickFontSize = args.fontSizeTicks
    LegendFontSize = args.fontSizeLegend

    colorMap = cm.gist_rainbow
    styleListPoint = ['o','*','s','D','^','.']
    styleListLine = ['-']
    styleListRandomLine = ['-^','-.','-o','-*','-s','-D']
    styleListRandomPoint = ['^','.','o','*','s','D']
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

    while len(ConditionsList)*len(NamesList) > len(styleList): #HACK len(ConditionsList)*len(NamesList) is not a proven upper bound
        styleList = styleList + styleList

    fig = plt.figure(figsize=(imageSize[0],imageSize[1])) # create a new figure
    fig.subplots_adjust(hspace=.35)

    if XCoordinateName in NamesList:
        NamesList.remove(XCoordinateName)
        if args.verbose:
            print('removing xAxis column: ',XCoordinateName,' from list of columns to be plotted.',flush=True)

    if dataIndex in NamesList:
        NamesList.remove(dataIndex)
        if args.verbose:
            print('removing dataAxis column: ',dataIndex,' from list of columns to be plotted.',flush=True)
        

    if args.lastOnly:
        title += '    x axis = conditions'
    else:
        title += '    x axis = ' + XCoordinateName

    if (args.title != 'NONE'):
        title = args.title

    plt.suptitle(title, fontsize=MajorFontSize, fontweight='bold')

    allNamesList = NamesList
    for integrateName in integrateNames: # remove all integrateName columns
        NamesList = [x for x in NamesList if not integrateName in x]

    if len(NamesList) == 1:
        Columns = 1
    if (len(NamesList) == 2) and (int(Columns) > 2):
        Columns = 2
                            
    Rows = ceil(float(len(NamesList))/float(Columns)) # calculate how many rows we need
    
    for conditionCount, con in enumerate(ConditionsList):

        df_cond = isolate_condition(data, con).groupby(dataIndex)

        ### this is a hack. df_cond.mean() removes any columns that can not be averaged (i.e. lists, strings, etc...)
        ### so, we will run it all the time. That way, invalid columns will be removed.
        if True:#any([x in PltWhat for x in ['ave', 'std', 'sem', '95conf', '99conf']]):
            df_mean = df_cond.mean()

        if 'std' in PltWhat:
            df_std = df_cond.std()

        if any([x in PltWhat for x in ['sem', '95conf', '99conf']]):
            df_sem = df_cond.sem()

        x_axis_values = isolate_condition(data, con).pivot(index = dataIndex, columns ='repName', values = XCoordinateName).mean(axis=1) #HACK needs to be optimized

        for nameCount, name in enumerate(NamesList):
            ThisLabel= ''
            if not CombineData:
                ThisLabel = con
                ax = plt.subplot(Rows, Columns, nameCount + 1)#BUG , label=ThisLabel) #label ensures unique axes are created
                plt.title(name, fontsize=MinorFontSize)  # set the title for this plot
                ax.title.set_position([.5, 1])
                if (len(ConditionsList) > 1):
                    PltStyle = styleList[conditionCount]
                    PltColor = colorMap(conditionCount/len(ConditionsList)) #styleListColor[conditionCount]
            elif len(ConditionsList) > 1 or len(NamesList) > 1:
                PltStyle = styleList[conditionCount + (nameCount * len(ConditionsList))]
                PltColor = colorMap((conditionCount + (nameCount * len(ConditionsList)))/(len(ConditionsList)+len(NamesList))) #styleListColor[conditionCount + (nameCount * len(ConditionsList))]
                if (len(ConditionsList) == 1):
                    ThisLabel = name
                else:
                    ThisLabel = con + ' ' + name

            if args.grid:
                plt.grid(b=True, which='major', color=(0,0,0), linestyle='-', alpha = .25)
            
            
            if not name in df_mean.columns.values:
                if name == dataIndex:
                    print('warrning: it appears you are attempting to plot ',name,' which is the data index. This is not allowed. Actually if you fix this I will give you $10.',flush=True)
                    if not CombineData:
                        plt.title(name+'\n(invalid, dataIndex...\nsee command line output)', fontsize=MinorFontSize)  # set the title for this plot 

                else:
                    print('warrning: it appears that ',name,' is non-numeric (perhaps a list) so its values are not being plotted.',flush=True)
                    if not CombineData:
                        plt.title(name+'   (INVALID DATA FORMAT)', fontsize=MinorFontSize)  # set the title for this plot 
            else:
                    
                
                if args.lastOnly:
                    quantity = df_mean.loc[:, name].tail(1).iloc[0]
                    # quantity = quantity.iloc[0]
                    if 'std' in PltWhat:
                        quantityErr = df_std.loc[:, name].tail(1)
                        plt.bar([conditionCount], [quantity], yerr=quantityErr) #TODO should allow sem, 95conf, 99conf
                    else:
                        plt.bar([conditionCount], [quantity])
                else:
                    if 'reps' in PltWhat:
                        firstRep = 1
                        for Rep in Reps:							
                            if firstRep == 1:
                                firstRep = 0
                                plt.plot(data[data["repName"] == Rep][data["con"] == con][XCoordinateName], data[data["repName"] == Rep][data["con"] == con][name], PltStyle, alpha = .25, color = PltColor, label = ThisLabel + "_rep")
                            else:
                                plt.plot(data[data["repName"] == Rep][data["con"] == con][XCoordinateName], data[data["repName"] == Rep][data["con"] == con][name], PltStyle, alpha = .25, color = PltColor, label = '_nolegend_')

                    # any plot that is dependant on the average line must trigger this 'if' statment
                    if any([x in PltWhat for x in ['ave', 'std', 'sem', '95conf', '99conf']]):
                        aveLine = df_mean.loc[:, name]

                        for integrateName in integrateNames: # remove all integrateName columns
                            VARNAME = name[0:-4]+integrateName
                            if VARNAME in allNamesList:
                                if args.verbose: print('     '+VARNAME+'  found, adding to plot for data: ' + name + ' condition: ' + con,flush=True)
                                errorLineY = df_mean.loc[:, VARNAME]
                                plt.fill_between(x_axis_values, aveLine - errorLineY,aveLine + errorLineY, color = PltColor, alpha = .15) 
                    if 'std' in PltWhat:
                        errorLineY = df_std.loc[:, name]
                        add_error_bars(ErrorStyle, x_axis_values, aveLine, errorLineY, PltColor)

                    if ('ave' in PltWhat):
                        plt.plot(x_axis_values, aveLine, PltStyle, markersize = 10, color = PltColor, linewidth = args.lineWeight, label = ThisLabel)

                    if ('sem' in PltWhat):
                        errorLineY = df_sem.loc[:, name]
                        add_error_bars(ErrorStyle, x_axis_values, aveLine, errorLineY, PltColor)

                    if ('95conf' in PltWhat):
                        errorLineY = df_sem.loc[:, name].multiply(1.96)
                        add_error_bars(ErrorStyle, x_axis_values, aveLine, errorLineY, PltColor)

                    if ('99conf' in PltWhat):
                        errorLineY = df_sem.loc[:, name].multiply(2.58)
                        add_error_bars(ErrorStyle, x_axis_values, aveLine, errorLineY, PltColor)
                
                if ((len(ConditionsList) > 1) or (CombineData))and legendLocation != '':
                    if args.lastOnly:
                        plt.xlabel('Conditions', fontsize=MinorFontSize)
                    else:
                        plt.xlabel(XCoordinateName, fontsize=MinorFontSize)
                    leg = plt.legend(fontsize=LegendFontSize,loc=legendLocation)   # add a legend
                    if (args.legendLineWeight > 0):
                        for legobj in leg.legendHandles:
                            legobj.set_linewidth(args.legendLineWeight)
                
                if args.lastOnly: ## combineConditions
                    plt.xticks(range(len(ConditionsList)), ConditionsList, rotation=45, ha='right')
                else:
                    plt.ticklabel_format(useOffset=False, style='plain')
                
                plt.tick_params(labelsize=TickFontSize)

                if len(xRange) == 2:
                    plt.xlim(xRange[0],xRange[1])

                if len(yRange) == 2:
                    plt.ylim(yRange[0],yRange[1])
            
    return plt.gcf() # gcf = get current figure - return that.		


def get_rep_list(args):
    rangeStart = args.repRange[0]
    rangeEnd = args.repRange[1]

    if args.repList:
        reps = args.repList
    else:
        reps = range(rangeStart, rangeEnd + 1)
    
    if not reps:
        reps = ['']
    else:
        reps = [str(i) + '/' for i in reps]
    
    return reps


def get_con_names(args):
    folder_names = args.conditions

    if folder_names != ['']:
        for i, folder_name in enumerate(folder_names):
            if folder_name[-1] is not '/':
                folder_names[i] += '/'

    if args.conditionNames == ['']:
        user_names = [name[:-1] for name in folder_names]
    else:
        user_names = args.conditionNames
        if (len(user_names) != len(folder_names)):
            print ('Error in input. -conditions and -conditionNames must have the same number of arguments')
            exit()

    return folder_names, user_names

if args.dataFromFile == '':
    args.dataFromFile = args.files[0]
  
def get_data_names(args, condition_folder_names, replicates):
    exemplar_path = args.path + condition_folder_names[0] + replicates[0] + args.dataFromFile
    if args.verbose: print('getting column names from',exemplar_path,flush=True)
    with open(exemplar_path, 'r') as fileReader:
        names_from_file = fileReader.readline().strip().split(",")
    
    if args.showDataNames:
        print('showing data column names:')
        print(*names_from_file, sep=",")
        exit()

    namesList = []
    if args.data != ['']:
        user_names = args.data
        for u_name in user_names:
            if '*' in u_name or '[' in u_name or ']' in u_name:
                if args.verbose: print("found column name with wildcard: " + u_name,flush=True)
                for f_name in names_from_file:
                    if fnmatchcase(f_name,u_name):
                        if args.verbose: print("   ... found match, adding " + f_name + " to data.",flush=True)
                        namesList.append(f_name)
            else:
                namesList.append(u_name)
    else:
        namesList = names_from_file
    
    if args.xAxis in namesList:
        namesList.remove(args.xAxis)

    for u_name in args.ignoreData:
        if '*' in u_name or '[' in u_name or ']' in u_name:
            if args.verbose: print("found ignore data with wildcard: " + u_name,flush=True)
            for f_name in namesList:
                if fnmatchcase(f_name,u_name):
                    if args.verbose: print("   ... found match, removing " + f_name + ".",flush=True)
                    namesList.remove(f_name)
    return namesList


def find_alternate_data_names(search_from, match_to):
    alternate_names = []
    for name in match_to:
        if name in search_from:
            alternate_names.append(name)
        else:
            if args.verbose: print("  can't find: '" + name + "'",flush=True)
            if name[-4:]=="_AVE":
                short_name = name[0:-4]
                if short_name in search_from:
                    if args.verbose: print("         but I did find: '" + short_name + "'",flush=True)
                    alternate_names.append(short_name)
    return alternate_names


def load_data(args, condition_folder_names, condition_user_names, replicates, file_names, data_names):
    df_dict = {}
    updateMin = 'undefined'

    for f in file_names:
        df_dict[f] = []
        alt_names = []
        for c_f, c_u in zip(condition_folder_names, condition_user_names):
            for r in replicates:
                complete_path = args.path + c_f + r + f
                if args.verbose: print ("loading file: " + complete_path,flush=True)
                df_all = read_csv(complete_path)
                last_x_value = df_all[args.xAxis].iat[-1]
                if updateMin == 'undefined':
                    updateMin = last_x_value
                if (last_x_value < updateMin):
                    updateMin = last_x_value
                    if args.verbose: print(c_u + " " + r + " has data until: " + str(last_x_value) + " new shortest!",flush=True)
                
                if args.xAxis == args.dataIndex:
                    extraColumns = [args.dataIndex]
                else:
                    extraColumns = [args.xAxis]+[args.dataIndex]
                                    
                if f == args.dataFromFile:
                    df_keep = df_all[list(set([data_name for data_name in data_names]+extraColumns))]
                else:
                    if not alt_names:
                        alt_names = find_alternate_data_names(df_all.columns,data_names)
                    df_keep = pandas.DataFrame(df_all[list(set([alt_name for alt_name in alt_names]+extraColumns))])
                
                df_keep = pandas.DataFrame(df_keep)

                df_keep["repName"] = r
                df_keep["con"] = c_u
                df_dict[f].append(df_keep)
    return {f:concat([df for df in df_dict[f]], ignore_index=True) for f in df_dict}, updateMin


def main(args):
    plt.rcParams['figure.figsize'] = (6,6)

    integrateNames = args.integrate
    imageSize = args.imageSize
    realLegendList = ['upper right','upper left','lower right','lower left','center right','center left','lower center','upper center','center','']
    abrvLegendList = ['ur','ul','lr','ll','cr','cl','lc','uc','c','off']
    args.legendLocation = realLegendList[abrvLegendList.index(args.legendLocation)]

    # ---Load Data -------------------------
    if args.path != '' and args.path[-1] != '/': args.path += '/' #new
    reps = get_rep_list(args) #new
    files = args.files
    conFolderNames, conUserNames = get_con_names(args) #new
    dataColumnNames = get_data_names(args, conFolderNames, reps) #new
    godFrames, updateMin = load_data(args, conFolderNames, conUserNames, reps, files, dataColumnNames) #new
    # --------------------------------------

    if len(args.whereRange) == 0 and args.whereRangeLimitToData:
        args.whereRange.append(0)
        args.whereRange.append(updateMin)

    if len(args.whereRange) >= 2 and args.whereRangeLimitToData:
        args.whereRange[1] = updateMin


    for name in godFrames:
        godFrames[name].reindex(copy=False)
        if len(args.whereRange)==1:
            godFrames[name] = godFrames[name][godFrames[name][args.whereValue]==args.whereRange[0]]
        if len(args.whereRange)==2:
            godFrames[name] = godFrames[name][godFrames[name][args.whereValue] >= args.whereRange[0]]
            godFrames[name] = godFrames[name][godFrames[name][args.whereValue] <= args.whereRange[1]]
        if len(args.whereRange)==3:
            cropRange = list(range(args.whereRange[0],args.whereRange[1]+1,args.whereRange[2]))
            godFrames[name] = godFrames[name][godFrames[name][args.whereValue].isin(cropRange)]

    allGraphs = {}

    if args.combineConditions:
        for file in files:
            if args.verbose: print ("generating plot for: " + file,flush=True)
            thisNamesList = find_alternate_data_names(isolate_condition(godFrames[file], conUserNames[0]).columns, dataColumnNames)
            allGraphs[file] = MultiPlot(data = godFrames[file], PltWhat = args.pltWhat, ConditionsList = conUserNames, CombineData = args.combineData, PltStyle = args.pltStyle, ErrorStyle = args.errorStyle, Reps = reps, NamesList = thisNamesList, XCoordinateName = args.xAxis, dataIndex = args.dataIndex, Columns = args.numCol, title = file,legendLocation = args.legendLocation, xRange = args.xRange, yRange = args.yRange, integrateNames = integrateNames, imageSize = imageSize)
    else:
        for con in conUserNames:
            for file in files:
                if args.verbose: print ("generating plot for: " + con + "__" + file,flush=True)
                thisNamesList = find_alternate_data_names(isolate_condition(godFrames[file], conUserNames[0]).columns, dataColumnNames)
                allGraphs[con+'__'+file] = MultiPlot(data = godFrames[file], PltWhat = args.pltWhat, ConditionsList = [con], CombineData = args.combineData, PltStyle = args.pltStyle, ErrorStyle = args.errorStyle, Reps = reps, NamesList = thisNamesList, XCoordinateName = args.xAxis, dataIndex = args.dataIndex, Columns = args.numCol, title = con + "__" + file,legendLocation = args.legendLocation, xRange = args.xRange, yRange = args.yRange, integrateNames = integrateNames, imageSize = imageSize)

    #plt.tight_layout()
    if args.save == '':
        plt.show()	

    ######## SAVE TO A PNG FILE
    if args.save == 'png':
        for g in allGraphs:
            if g[-4:] == '.csv':
                if (args.saveName == ""):
                    allGraphs[g].savefig(args.title+'_MGraph_' + g[:-4].replace('/','_') + '.png', dpi=100)
                else:
                    allGraphs[g].savefig(args.saveName + '.png', dpi=100)
            else:
                if (args.title != "NONE"):
                    allGraphs[g].savefig(args.title+'_MGraph_' + g.replace('/','_') + '.png', dpi=100)
                else:
                    allGraphs[g].savefig('MGraph_' + g.replace('/','_') + '.png', dpi=100)

    ######## SAVE TO A PDF FILE
    if args.save == 'pdf':
        if (args.saveName == ""):
            pp = PdfPages(args.title + '_MGraph.pdf')
        else:
            pp = PdfPages(args.saveName+'.pdf')
        for g in allGraphs:
            pp.savefig(allGraphs[g])
        pp.close()


if __name__ == "__main__":
    main(args)
