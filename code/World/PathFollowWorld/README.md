## PathFollowWorld
agents are tested to see if they can follow a path.
maps are loaded where locations are either off path, forward, left, right, or goal
agents start on one end of the path (indicated by the map) facing the correct direction
agents have a down sensor that allows them to see the value on the current location

agents have 3 outputs where 100 = left, 010 = right, 110 = forward, XX1 = reverse (X = 0 or 1)

## Score
score in PathFollowWorld is number of forward and turn locations visited + time left if goal is reached and all locations were visited
NOTE: since turns are interaly converted to forwards when visted, agents actually only recive points for being on forwards.

## Parameters

parameters can be used to alter input formatting and other aspects of world.
from settings_world.cfg:

```
% WORLD_PATHFOLLOW
  addFlippedMaps = 0                         #(bool) if addFlippedMaps, than a copy of each loaded map flipped horizontaly will be added to the maps list
  clearVisted = 0                            #(bool) if clearVisted is true, then world markers will be erased when a location is occupied and visting this location
                                             #  again will incure the emptySpace cost.
                                             #  note that map values > 1, i.e. turn signals and end of map signal are set to 1 (forward signal) when visted to provide
                                             #  time to take a turn action.
  emptySpaceCost = 0.25                      #(double) score lost anytime agent is on an empty location (including non-empty locations that become empty)
  evaluationsPerGeneration = 1               #(int) how many times should each organism be tested in each generation?
  extraSteps = 50                            #(int) how many many steps, beyond those needed to perfectly solve the map, does the agent get to solve each map?
  inputMode = single                         #(string) how are inputs delived from world to organism?
                                             #  single: 1 input : -1 (off), 0(forward), or [1, signValueMax](turn symbol)
                                             #  mixed:  4 inputs: offPathBit,onPathBit,(0(not turn), or [1,signValueMax](turn symbol))
                                             #  binary: 3+ inputs: offPathBit,onPathBit,onTurnBit, bits for turn symbol(0 if not turn)
  mapNames = path1.txt,path2.txt             #(string) list of text files with paths. in path files, 0 = empty,
                                             #  X = start position, 1 = forward path, 2 = turn right, 3 = turn right, 4 = end of path
  swapSymbolsAfter = 1.0                     #(double) if swapSignals < 1.0, than the turn symbols will be swapped after (minimum number of steps * swapSignalsAfter)
  symbolValueMax = 7                         #(int) number of symbols that will be used when generating turn symbols. range is [1,signValueMax]
                                             #  if inputMode is binary it is best if this value is a power of 2 minus 1
  useRandomTurnSymbols = 1                   #(bool) if true, random symbols will be determined per map (and per eval) for left and right.
                                             #  symbols will be the same for all agents in a generation.
                                             #  if false, symbolValueMax is ignored and 1 and 2 (or 01 and 10) are always used
```

## Parameters Notes (not in parameter descriptions):
   - if 'useRandomTurnSymbols' values are selected per generation per map,
       all agents will see same symbols per map)
   - if 'evaluationsPerGeneration' > 1, new symbols are generated for each evaluation
   - if symbol is delivered as binary, then not turn will be represented by 0.
       if symbolValueMax = 3, 3 symbols will be used with 2 bits (i.e. 01,10,11)
       if symbolValueMax = 4, 4 symbols will be used with 3 bits (i.e. 001,010,011,100)

## Map Files
map files are raw text files with the following rules
the first line of the file is the map size (x,y)
the second line of the file is inital facing direction for the agent
     where 0 = north, 1 = north east, 2 = east, and so on till 7 = north west
the following lines are the map:
0 = empty
1 = forward
2 = right
3 = left
4 = goal
X = startingLocation

the hash (or pound sign) in the first potision of a line indicates a comment which are ignored
blank lines are ignored

example map:
---
6,6<br>
2<br>
000000<br>
0X1120<br>
000002<br>
000002<br>
003120<br>
040000

---

## Visualizing Behavior
the directory Processing_Visualizer contains a script that can be run with processing:
you can get processing here: [https://processing.org/](https://processing.org/)
When you first open processing  you may need to click on the top right pull down
menu and select "Add Mode..." and then install (Python Mode for Processing) 

In order to use this script, you will need to run the world in visualize mode.
The best way to do this is the follow steps:
1) run MABE with PathFollowWorld and be sure that the Archivist is set to LODwAP
2) create a new text file with the name "population_loader.plf"
3) add one line to that file: MASTER = greatest 1 by ID from { 'LOD_organisms.csv' }
4) run MABE as you did before, but add the following paramers
(remeber -p is the commen line flag for paramers)
-p GLOBAL-initPop = population_loader.plf GLOBAL-mode = visualize
5) this should generate 'pathVisualization.txt'
6) double click on the processing script to open processing
6) edit the processing script so that the path to your pathVisualization.txt file is correct
7) press play on the processing interface (top left)
left and right arrows can be used to change the speed

If you have run with mq, the simplest way to visualize is to copy the LOD_data.csv and LOD_organisms.csv for the condition/replicate you want to see to the directory with MABE and your settings files, and then follow the same steps.

