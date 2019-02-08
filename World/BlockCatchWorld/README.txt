BLOCK CATCH WORLD README

Block Catch world is a vertiucaly cylindrical grid with height and width. patterns are dropped from
the top of the world and decend one grid height each world update. Patterns may also move left or right (see below).
Organisms are paddles with some grid width. Some of the grids that make up the paddle may be sensors.
Organisms exist at the 'bottom' of the world and their sensors look up.
Organisms are tasked with catching or dodging patterns depending on the pattern 'shape' and motion.

Score, Correct and Incorrect:
  Block Catch World keeps track of correct and incorrect catches. When a block reaches the bottom of the world
  one or more hits may be registed (see below). For each hit registered on a pattern that should be caught, the
  correct counter will be increased, as will the correct counter for that pattern. For each hit registered on
  a pattern that should not be caught, the incorrect counter will be increased, as will the incorrect counter
  for that pattern. For blocks that should be missed, hits increase the incorrect counters and misses increase the
  correct counters.

  correct, incorrect and all of the per pattern correct and incorect totals are recorded to each organisms dataMap.

  Score (in dataMap) is set to POW(1.05,correct-incorrect) (This formula seems to work well with roulette selection)
  Of course you can generate your own fitnes function if the optimizer allows.

Defining the paddle:
  paddle = 110011                            #(string) list of 1 (for sensor) an   d 0 (for non-sensor) used to define the paddle

  Note, the paddle always starts on the left side of the world and there is no setting for this.

defining patterns:
  There are two parameters for patterns; patternsToCatch and patternsToMiss. Each parameter takes a list of one or more patterns.
  A pattern is made by 1's (blocks visible to the paddle sensors) and 0's (blocks which can not be seen by the paddle sensors) and a
  movement list (which defines how the pattern should move).

  A pattern shape may be a single list of 1's and 0s or a list of 1 or more 'frames'. If there is more then one frame, then the pattern
  will be animated (the pattern to change shape as it's falling). All of the frames in a pattern must be the same size (the same total
  number of 1's and 0's)

  The movement list is comprised of R (move right one grid width for each world update), L (left) and H (hold) and integer values (after R
  or L - jump this many grid widths in a single update). There must be at lease one letter in the movement list. If there is more then
  one letter, then they will be cycle one letter ( and number if present) per world update.

  patternsToCatch = 11L,010R                 #(string) a comma separted list of block patterns to be caught
                                             #  each pattern is followed by a list of at least one L, R or, H. These letters will determine
                                             #  movement (left, right, hold). for example, 101LLRH = pattern 3 wide with a hole in the middle
                                             #  which moves left, left, right, no movement, left, left, right, hold...
                                             #  the letters L and R may be followed by a number, this will allow the pattern to move more faster

  patternsToCatch = 11:10:01L                # defines a block that animates 11->10->01->11->10... while moving left                

Scoring modes:
  the scoring method allows you to set what events will be considered 'hits' (i.e. success if the pattern is on the to catch list or failer if the pattern
  is on the to miss list). The scoreMethods are of two types, the first three only count a single hit for each pattern, the remainder (the SUM methods)
  can recored multiple hits per pattern.

  NOTE: If any of the SUM methods are being used then correct can only be accumulated with to catch bocks (when a hit is
  recorded and incorrect can only be accumulated with to miss blocks (when a hit is recoded).

  scoreMethod = ANY_ANY                      #(string) How should score be calculated?
                                             #  ANY_ANY - a hit is defined as any part of the pattern contacting at part of the paddle
                                             #  VISIBLE_ANY - a hit is defined as any visible part of the pattern contacting any part of the paddle
                                             #  VISIBLE_SENSOR - a hit is defined as any visible part of the pattern contacting any sensor on the paddle
                                             #  SUM_ALL_ALL - a hit will be recorded for each part of the pattern that contacts any part of the paddle
                                             #  SUM_VISIBLE_SENSOR - a hit will be recorded for each visible part of the pattern that contacts a sensor on the paddle
                                             #  SUM_VISIBLE_NON_SENSOR - a hit will be recorded for each visible part of the pattern that contacts a non=sensor on
                                             #  the paddle.
                                             #  NOTE: correct and incorrect are calculated diffrently if any of the SUM methods are used, see the README file!
											 
pattern start positions:

  patternStartPositions = ALL_CLEAR          #(string) Where should the patters start (and if random, how many times should each pattern be tested)
                                             #  ALL = each pattern will be tested for each possible start position
                                             #  ALL_CLEAR = each pattern will be tested for each possible start position that does not overlap the paddle
                                             #  RANDOM_X = each pattern will be tested X times, starting from a random location
                                             #  RANDOM_CLEAR_X = each pattern will be tested X times, starting from a random location that does not overlap the paddle
                                             #  with either random mode, locations may repeat
											 

Random World Size and Random World Time
  WorldXMin and WorldXMax can be set to the same value (default) for a fixed width world. If they are set to diffrent values, then the world width will be randomized
  each time a pattern starts falling. This alters the amount of time a pattern takes to move across the world.

  worldXMax = 20                             #(int) max width of world
  worldXMin = 20                             #(int) min width of world

  startWMin and startYMax can also be used to add randomness. If these are diffrent numbers, then the patterns will not always start at the same height. In this world,
  organims can not detect the height to the pattern, so this randomizes the total time that the organism has to catch or miss the current pattern.
  
  startYMax = 20                             #(int) highest height patterns can start
  startYMin = 10                             #(int) lowest height patterns can start

  
Visualize Mode
  Block Cathc World includes visualization code (run if mode = visualize) which can be viewed with the included processing script (see below)
  Visualization is saved into a file with the name CatchPassVisualize_###.txt, where ### is the MABE Global::update when the visualization was run.
  
  In addtion, the visualizeBest best parameter will allow you to do run time visualizations on the organism with the best score.
  
  visualizeBest = 200                        #(int) visualize best scoring organism every visualizeBest generations, excluding generation 0.
                                             #  if -1, do not visualize on steps (this parameter does not effect visualize mode)

Preocessing Script
In order to see the visualization, you must instal processing.
Once you do that you should be able to open the processing script. You will need to change the file path (near the top of the processing script) and then should
be able to press the play button in the processing interface.
