//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

// PathFollowWorld
// agents are tested to see if they can follow a path.
// maps are loaded where locations are either off path, forward, left, right, or goal
// agents start on one end of the path (indicated by the map) facing the correct direction
// agents have a down sensor that allows them to see the value on the current location
//
// agents have 3 outputs where 100 = left, 010 = right, 110 = forward, XX1 = reverse (X = 0 or 1)
//
// parameters can be used to alter input formatting and other aspects of world.
//
// Parameters Notes (not in parameter descriptions):
//   - if 'useRandomTurnSymbols' values are selected per generation per map,
//       all agents will see same symbols per map)
//   - if 'evaluationsPerGeneration' > 1, new symbols are generated for each evaluation
//   - if symbol is delivered as binary, then not turn will be represented by 0.
//       if symbolValueMax = 3, 3 symbols will be used with 2 bits (i.e. 01,10,11)
//       if symbolValueMax = 4, 4 symbols will be used with 3 bits (i.e. 001,010,011,100)
//
// score in PathFollowWorld is number of forward and turn locations visited + time left if goal is reached and all locations were visited
// NOTE: since turns are interaly converted to forwards when visted, agents actually only recive points for being on forwards.


#include "PathFollowWorld.h"

shared_ptr<ParameterLink<int>> PathFollowWorld::evaluationsPerGenerationPL =
    Parameters::register_parameter("WORLD_PATHFOLLOW-evaluationsPerGeneration", 3,
    "how many times should each organism be tested in each generation?");

shared_ptr<ParameterLink<int>> PathFollowWorld::extraStepsPL =
Parameters::register_parameter("WORLD_PATHFOLLOW-extraSteps", 50,
    "how many many steps, beyond those needed to perfectly solve the map, does the agent get to solve each map?");

shared_ptr<ParameterLink<double>> PathFollowWorld::emptySpaceCostPL =
Parameters::register_parameter("WORLD_PATHFOLLOW-emptySpaceCost", .25,
    "score lost anytime agent is on an empty location (including non-empty locations that become empty)");

shared_ptr<ParameterLink<bool>> PathFollowWorld::clearVistedPL =
Parameters::register_parameter("WORLD_PATHFOLLOW-clearVisted", true,
    "if clearVisted is true, then world markers will be erased when a location is occupied and visting this location again will incure the emptySpace cost.\n"
    "note that map values > 1, i.e. turn signals and end of map signal are set to 1 (forward signal) when visted to provide time to take a turn action.");

shared_ptr<ParameterLink<int>> PathFollowWorld::turnSymbolsCountPL =
Parameters::register_parameter("WORLD_PATHFOLLOW-turnSymbolsCount", 4,
    "number of symbols that will be used when generating turn symbols.\n"
    "if inputMode is binary it is best if this value is a power of 2");

shared_ptr<ParameterLink<bool>> PathFollowWorld::useRandomTurnSymbolsPL =
Parameters::register_parameter("WORLD_PATHFOLLOW-useRandomTurnSymbols", true,
    "if true, random symbols will be determined per map (and per eval) for left and right.\n"
    "symbols will be the same for all agents in a generation.\n"
    "if false, symbolValueMax is ignored and 1 and 2  (or 01 and 10) are always used");

shared_ptr<ParameterLink<std::string>> PathFollowWorld::inputModePL =
Parameters::register_parameter("WORLD_PATHFOLLOW-inputMode", (std::string)"single",
    "how are inputs delived from world to organism?\n"
    "single: 1 input : -1 (off), 0(forward), or [1, signValueMax](turn symbol)\n"
    "mixed:  4 inputs: offPathBit,onPathBit,(0(not turn), or [1,signValueMax](turn symbol))\n"
    "binary: 3+ inputs: offPathBit,onPathBit,onTurnBit, bits for turn symbol(0 if not turn)");

shared_ptr<ParameterLink<double>> PathFollowWorld::swapSymbolsAfterPL =
Parameters::register_parameter("WORLD_PATHFOLLOW-swapSymbolsAfter", 1.0,
    "if swapSignals < 1.0, than the turn symbols will be swapped after (minimum number of steps * swapSignalsAfter)");

shared_ptr<ParameterLink<std::string>> PathFollowWorld::mapNamesPL =
Parameters::register_parameter("WORLD_PATHFOLLOW-mapNames",
    (std::string)"../code/World/PathFollowWorld/path1.txt,../code/World/PathFollowWorld/path2.txt",
    "list of text files with paths. in path files, X = start position, 0 = empty, 1 = forward path, 2 = turn right, 3 = turn right, 4 = end of path");

shared_ptr<ParameterLink<bool>> PathFollowWorld::addFlippedMapsPL =
Parameters::register_parameter("WORLD_PATHFOLLOW-addFlippedMaps", true,
    "if addFlippedMaps, than a copy of each loaded map flipped horizontaly will be added to the maps list");

// load single line from file, lines that are empty or start with # are skipped
inline bool loadLineFromFile(std::ifstream& file, std::string& rawLine, std::stringstream& ss) {
    rawLine.clear();
    if (file.is_open() && !file.eof()) {
        while ((rawLine.size() == 0 || rawLine[0] == '#') && !file.eof()) {
            getline(file, rawLine);
            // remove all whitespace from rawLine
            rawLine.erase(remove_if(rawLine.begin(), rawLine.end(), ::isspace), rawLine.end());
        }
        ss.str(std::string()); // clear conntents of ss (if any)
        ss << rawLine;
    }
    else if (!file.eof()) {
    std::cout << "in loadSS, file is not open!\n  Exiting." << std::endl;
    exit(1);
    }
    return file.eof(); // ss contains the current line, this return tells us if we are at end of file
}


void PathFollowWorld::loadMaps(std::vector<string>& mapNames, std::vector<Vector2d<int>>& maps, std::vector<std::pair<int, int>>& mapSizes, std::vector<int>& initalDirections, std::vector<std::pair<int, int>>& startLocations) {
    for (auto mapName : mapNames) {

        std::string rawLine; // raw line and ss are used to hold file info file making maps
        std::stringstream ss;

        std::ifstream FILE(mapName);
        bool atEOF = loadLineFromFile(FILE, rawLine, ss); // load lines from file till end of file
        std::vector<int> tempVect;
        int tempInt;

        convertCSVListToVector(rawLine, tempVect); // convert the line to a comma seperated vector<int>

        mapSizes.push_back({ tempVect[0], tempVect[1] }); // first line of the map is it's size
        maps.push_back(Vector2d<int>(tempVect[0], tempVect[1])); // adds a "grid" of map size to maps

        atEOF = loadLineFromFile(FILE, rawLine, ss); // get the next line
        initalDirections.push_back(std::stoi(rawLine)); // ... contails initial facing direction

        atEOF = loadLineFromFile(FILE, rawLine, ss); // get the next line, from here we are loading the actual map

        size_t y = 0;
        while (!atEOF) { // while not at end of file, read lines from file and add them to map
            for (size_t x = 0; x < rawLine.size(); x++) { // for all positions (x values) in the current line ...
                if (rawLine[x] == 'X') { // if character in file is "X", this is the start location, X was used to make the map more human readable
                    maps.back()((int)x, (int)y) = 1;
                    startLocations.push_back({ x,y }); // make a note of this location in startLocations
                }
                else { 
                    maps.back()((int)x, (int)y) = rawLine[x] - 48; // We have the ascii value, -48 converts it to int with "0" == 0
                }
            }
            y += 1; // done with the current line, advance the y counter
            atEOF = loadLineFromFile(FILE, rawLine, ss); // get the next line
        }
    }
}



// the constructor gets called once when MABE starts up. use this to set things up
// this localizes parameters, loads maps, tells the archivist what to add to pop.csv files, etc.
PathFollowWorld::PathFollowWorld(shared_ptr<ParametersTable> PT_) : AbstractWorld(PT_) {

    //localize a parameter value for faster access
    evaluationsPerGeneration = evaluationsPerGenerationPL->get(PT);

    extraSteps = extraStepsPL->get(PT);
    emptySpaceCost = emptySpaceCostPL->get(PT);
    clearVisted = clearVistedPL->get(PT);

    turnSymbolsCount = turnSymbolsCountPL->get(PT);
    useRandomTurnSymbols = useRandomTurnSymbolsPL->get(PT);
    if (!useRandomTurnSymbols) {
        turnSymbolsCount = 2;
    }
    inputMode = inputModePL->get(PT);

    swapSymbolsAfter = swapSymbolsAfterPL->get(PT);

    convertCSVListToVector(mapNamesPL->get(PT), mapNames);

    std::cout << "In pathFollowWorld, loading maps:" << std::endl;
    loadMaps(mapNames, maps, mapSizes, initalDirections, startLocations);
    addFlippedMaps = addFlippedMapsPL->get(PT);

    int mapsCount = maps.size();
    maxScores.resize(mapsCount,0); // each map has a maxScore
    minSteps.resize(mapsCount,0); // each map has a minSteps
    forwardCounts.resize(mapsCount,0); // each maps has a forwardCounts (how many forward steps for a perfect score)
    turnCounts.resize(mapsCount, 0); // each maps has a turnCounts (how many turns for a perfect score)

    // for each map, collect stats stats (minSteps, forwardCounts, turnCounts)
    for (size_t i = 0; i < mapsCount; i++) {
        for (int y = 0; y < mapSizes[i].second; y++) {
            for (int x = 0; x < mapSizes[i].first; x++) {
                if ((maps[i](x, y) > 0) && (maps[i](x, y) < 4)) { // if location is not empty and not goal, i.e. it is 1, 2, or 3
                    forwardCounts[i]++;      // even if a turn, this will requre a step forward (after the turn)
                    minSteps[i] += 1;        // moving forward takes 1 time step
                    if (maps[i](x, y) > 1) { // if location > 1, it's a turn
                        turnCounts[i] += 1;      // count this turn
                        minSteps[i] += 1;    // a turn requires an additional time step
                    }
                }
            }
        }
        maxScores[i] = forwardCounts[i] + extraSteps; // max score an agent can get (before nomalization) is forward count + extra steps left over if they are prefect

        std::cout << "\n" << mapNames[i] << "  " << startLocations[i].first << "," << startLocations[i].second << std::endl;
        std::cout << "min steps: " << minSteps[i] << "  turn count: " << turnCounts[i] << "  forward counts: " << forwardCounts[i] << std::endl;
        maps[i].showGrid();
        
        if (addFlippedMapsPL->get(PT)) {
            // all flipped maps will appear in maps list after all non-flipped maps
            // maps are flipped horizontaly, along with the start position and start facing
            std::cout << "  adding flipped map..." << std::endl;
            int xSize = maps[i].x();
            int ySize = maps[i].y();
            Vector2d<int> newMap(xSize, ySize);
            for (int y = 0; y < mapSizes[i].second; y++) {
                for (int x = 0; x < mapSizes[i].first; x++) {
                    newMap(x, y) = maps[i]((xSize - 1) - x, y); // flip x, leave y alone
                    if (newMap(x, y) == 2) { // if map value is right turn, make it a left turn
                        newMap(x, y) = 3;
                    }
                    else if (newMap(x, y) == 3) { // if map value is left turn, make it a right turn
                        newMap(x, y) = 2;
                    }
                }
            }
            // now add this flipped map to maps
            maps.push_back(newMap); // append this map at the end of the maps list
            mapNames.push_back(mapNames[i] + "_R"); // use the current map name + "_R" (for reversed)
            mapSizes.push_back(mapSizes[i]);
            startLocations.push_back({ (xSize - 1) - startLocations[i].first, startLocations[i].second }); // flip x, leave y alone
            minSteps.push_back(minSteps[i]);
            maxScores.push_back(maxScores[i]);
            forwardCounts.push_back(forwardCounts[i]);
            turnCounts.push_back(turnCounts[i]);
            initalDirections.push_back(initalDirections[i]);
            // brute force update for initalDirections on flipped maps if we need to change it
            // 0, i.e. up, and 4, i.e. down don't need to change.
            if (initalDirections.back() == 1) {
                initalDirections.back() = 7;
            }
            else if (initalDirections.back() == 2) {
                initalDirections.back() = 6;
            }
            else if (initalDirections.back() == 3) {
                initalDirections.back() = 5;
            }
            else if (initalDirections.back() == 5) {
                initalDirections.back() = 3;
            }
            else if (initalDirections.back() == 6) {
                initalDirections.back() = 2;
            }
            else if (initalDirections.back() == 7) {
                initalDirections.back() = 1;
            }
        }
    }

    // we will fill randomValues (i.e. the turn symbols every generation, here we just make sure we have enough space
    randomValues.resize(evaluationsPerGeneration*maps.size());

    // determin the number of inputs based on the input mode
    if (inputMode == "single") { // input will be a single number (effectivly, the map value)
        inputCount = 1;
    }
    else {
        inputCount = 3; // location is empty + location is forward + location is turn
        if (inputMode == "mixed") {
            inputCount += 1; // + turn symbol. turn symbol is an int
        }
        else if (inputMode == "binary") { // turn symbol will be in bits, we need to know the number of bits
            outputsNeededForTurnSign = 0;
            int temp = turnSymbolsCount - 1; // e.g. if symbolValueMax is 2, this will be 1, or 0 and 1 (1 bit) will be needed
            while (temp > 0) {
                outputsNeededForTurnSign++;
                temp = temp >> 1;
            }
            inputCount += outputsNeededForTurnSign;
        }
    }
    
    outputCount = 3; // ??1 = reverse, 000 = no action, 010 = right, 100 = left, 110 = forward

    // popFileColumns tell MABE what data should be saved to pop.csv files
    popFileColumns.clear();
    popFileColumns.push_back("score");      // (score/maxScore)/mapCount
    popFileColumns.push_back("reachGoal");  // did the agent reach the goal Y/N
    popFileColumns.push_back("completion"); // (how much of the path did the agent visit) / (entire path length)
}

// the evaluate function gets called every generation. evaluate should set values on organisms datamaps
// that will be used by other parts of MABE for things like reproduction and archiving
// here we pick turn symbols once and these are used for all agents (to make sure we are doing a fair evaluation)
// then agents are tested (one-at-a-time) on each map and given a normalized score.
auto PathFollowWorld::evaluate(map<string, shared_ptr<Group>>& groups, int analyze, int visualize, int debug) -> void {

    int sign2; // remapping for 2s in the map
    int sign3; // remapping for 3s in the map

    // if randomizeTurnSigns, create a pair of random signals for each map
    if (useRandomTurnSymbols) {
        for (int t = 0; t < evaluationsPerGeneration * maps.size(); t++) {
            randomValues[t].first = Random::getInt(1, turnSymbolsCount);
            randomValues[t].second = Random::getInt(1, turnSymbolsCount - 1); // one less, so, if the same values are picked, we have somewhere to go
            if (randomValues[t].first == randomValues[t].second) {
                randomValues[t].second = turnSymbolsCount; // if values match, then set to max value - this is the only way the second symbol can be max
                                                           // i.e. in the case of a match, the max values "replaces" the chosen value
            }
            if (debug) {
                std::cout << "\non update: " << Global::update << " map: " << t << " has random values: " << randomValues[t].first << " " << randomValues[t].second << std::endl;
            }
        }
    }

    int popSize = groups["root::"]->population.size();
    
    // in this world, organisms do not interact, so we can just iterate over the population
    // on each iteration, each agent will run though every map evaluationsPerGeneration times
    // each runthough of a map will use different randomValues
    for (int orgID = 0; orgID < popSize; orgID++) {

        TS::intTimeSeries worldStates;

        // create a shortcut to access the organism and organisms brain
        auto org = groups["root::"]->population[orgID];
        auto brain = org->brains["root::"];

        if (analyze) {
            brain->setRecordActivity(true); // tell brain to record it's states
        }

        int xPos, yPos, direction, out0, out1, out2;
        double score, reachGoal;
        int thisForwardCount;

        // evaluate this organism on each map evaluationsPerGeneration times
        // a trial consists of one test on each map
        for (int trial = 0; trial < evaluationsPerGeneration; trial++) {
            for (size_t mapID = 0; mapID < maps.size(); mapID++) { // for each map
                
                // new map! reset some stuff
                score = 0;
                reachGoal = 0;
                thisForwardCount = 0;

                // set starting location using value from file for this map
                xPos = startLocations[mapID].first;
                yPos = startLocations[mapID].second;
                direction = initalDirections[mapID];

                // make a copy of the map so we have a local copy we can make notes on
                auto mapCopy = maps[mapID];
                
                // if useRandomTurnSymbols, pull values for turn symbol values from randomValues for this evaluation (trial) and map
                if (useRandomTurnSymbols) {
                    sign2 = randomValues[(trial * maps.size()) + mapID].first;
                    sign3 = randomValues[(trial * maps.size()) + mapID].second;
                }
                else { // use fixed values
                    sign2 = 1;
                    sign3 = 2;
                }

                if (debug) {
                    // show current map
                    mapCopy.showGrid();
                    std::cout << "at location: " << xPos << "," << yPos << "  direction: " << direction << std::endl;
                }

                if (visualize) {
                    std::string os = "new"; // initalize this map in the visualize file
                    FileManager::writeToFile("pathVisualization.txt", os);
                }

                // clear the brain - resets brain state including memory. marks end of lifetime in recorded brain states
                brain->resetBrain();


                int firstTurn = -1; // the agent has not stepped on a turn, so we don't know what the first turn is yet
                bool swapped = false; // have he symbols been swapped?

                for (int step = 0; step < (minSteps[mapID] + extraSteps); step++) { // this is how much time the agent has on this map

                    if ((swapSymbolsAfter < 1.0) && (swapped == false) && (step > ((double)minSteps[mapID] * swapSymbolsAfter)) ) {
                        // if we get far enough into the map, then swap the turn symbols
                        swapped = true;
                        auto temp = sign3;
                        sign3 = sign2;
                        sign2 = temp;
                    }

                    if (visualize) {
                        std::string os = "start\n";
                        os += std::to_string(direction) + "\n";
                        os += std::to_string(score) + "\n";
                        os += std::to_string(mapSizes[mapID].first) + "\n";
                        os += std::to_string(mapSizes[mapID].second) + "\n";

                        os += std::to_string(sign2) + "\n";
                        os += std::to_string(sign3) + "\n";

                        auto mapSource = mapCopy;
                        if (!clearVisted) {
                            mapSource = maps[mapID];;
                        }
                        // show grid, and other stats
                        for (int y = 0; y < mapSizes[mapID].second; y++) {
                            for (int x = 0; x < mapSizes[mapID].first; x++) {
                                auto hereValue = mapSource(x, y);
                                if (x == xPos && y == yPos) {
                                    if (debug) { std::cout << "* "; }
                                    os += "*";
                                }
                                else if (mapSource(x, y) == 0) {
                                    if (debug) { std::cout << "  "; }
                                    os += "0";
                                }
                                else if (mapSource(x, y) == 2) {
                                    if (debug) { std::cout << "R "; }
                                    os += "2";
                                }
                                else if (mapSource(x, y) == 3) {
                                    if (debug) { std::cout << "L ";; }
                                    os += "3";
                                }
                                else {
                                    if (debug) { std::cout << mapSource(x, y) << " "; }
                                    os += std::to_string(mapSource(x, y));
                                }
                            }
                            if (debug) { std::cout << std::endl; }
                            os += "\n";
                        }

                        FileManager::writeToFile("pathVisualization.txt", os);
                    }

                    if (debug) {
                        mapCopy.showGrid();
                        std::cout << "at location: " << xPos << "," << yPos << "  direction: " << direction << std::endl;
                        std::cout << "forward steps taken: " << thisForwardCount << "  current score: " << score << std::endl;
                        std::cout << "value @ this location: " << mapCopy(xPos, yPos) << std::endl;
                    }

                    int inputValue; // value at agents current location
                    if (clearVisted) {
                        inputValue = mapCopy(xPos, yPos); // if clear visited, get the current location value from the copy
                    }
                    else {
                        inputValue = maps[mapID](xPos, yPos); // ... else, pull from the real map
                    }

                    if (inputMode == "single") {
                        // map values are 0 = empty, 1 = forward, 2 = left, 3 = right,
                        // but output is -1 = empty, 0 = forward, 1 or more = turn, so we need to do some conversion...
                        inputValue--;
                        if (inputValue == 1) { // value in map was 2
                            inputValue = sign2;
                        }
                        else if (inputValue == 2) { // value in map was 3
                            inputValue = sign3;
                        }
                        else if (inputValue == 3) { // value in map was 4
                            inputValue = 0; // location is end, appears as forward (but this never happens)
                        }
                        brain->setInput(0, inputValue);
                    }
                    else { // inputMode == "mixed" || "binary"
                        
                        brain->setInput(0, inputValue == 0); // is location empty?
                        brain->setInput(1, inputValue == 1 || inputValue == 4); // is location path?   foraward and end look the same (but this never happens)
                        
                        if (inputMode == "mixed") {
                            if (inputValue == 2) {
                                brain->setInput(2, 1); // this location is a turn
                                brain->setInput(3, sign2);
                            }
                            else if (inputValue == 3) {
                                brain->setInput(2, 1); // this location is a turn?
                                brain->setInput(3, sign3);
                            }
                            else { // if not a turn
                                brain->setInput(2, 0); // this location is not a turn?
                                brain->setInput(3, 0);
                            }
                        } // end inputMode = "mixed"
                        else { // inputMode == "binary"

                            int val;
                            bool isTurn;

                            if (inputValue == 2) { // is turn
                                val = sign2 - 1; // signs are 1 to symbolValueMax, so subtract 1 now
                                isTurn = true;
                            }
                            else if (inputValue == 3) { // is turn
                                val = sign3 - 1; // signs are 1 to symbolValueMax, so subtract 1 now
                                isTurn = true;
                            }
                            else { // is not a turn
                                val = 0;
                                isTurn = false;
                            }
                            if (isTurn == false) { // set "outputsNeededForTurnSign" bits to 0 (this is not a turn)
                                brain->setInput(2, 0); // set input turn
                                for (int xx = 0; xx < outputsNeededForTurnSign; xx++) {
                                    brain->setInput(3 + xx, 0); // set turn symbol inputs to 0
                                }
                            }
                            if (isTurn == true) { // convert turn symbol "val" in to "outputsNeededForTurnSign" bits
                                brain->setInput(2, 1); // set input turn
                                for (int xx = 0; xx < outputsNeededForTurnSign; xx++) {
                                    brain->setInput(3 + xx, val & 1); // set turn symbol inputs
                                    val = val >> 1;
                                }
                            }

                        } // end else inputMode == "binary"
                    } // end else (inputMode == "mixed" || "binary")



                    // now that agent has inputs, update score based on map value at this location,
                    // and update map (note, agent gets first score before taking an action)
                    // if map location = 1, +1 score, and change map location value to 0
                    // if map location = 4, goal, set step = steps (so while loop will end)
                    //    also add any remaning steps to score, if all path locations were visited
                    // if map location > 1, set location value to 1 (i.e. turn markers become 1s),
                    //    the value will be 1 next update if this agent turns, which will provide +1 score
                    if (mapCopy(xPos, yPos) > 1) {
                        if (firstTurn == -1) { // if this is the first turn the agent has seen in this map, record map value
                            firstTurn = mapCopy(xPos, yPos);
                        }
                        if (mapCopy(xPos, yPos) == 4) { // if we get to the end of this map...
                            if (thisForwardCount >= forwardCounts[mapID]) { // ... and if all forward locations have been visited...
                                reachGoal = 1; // we only count a "reachGoal" if all locations on path were visited
                                score += (minSteps[mapID] + extraSteps) - step; // add points for steps left
                            }
                            step = minSteps[mapID] + extraSteps; // ... either way, end this map now - set step to force an exit from the step for loop
                        }
                        mapCopy(xPos, yPos) = 1; // set this location value to on mapCopy 1 so that on the next update agents do not pay emptySpaceCost
                    }
                    else if (mapCopy(xPos, yPos) == 1) { // if symbol on map copy is forward
                        score += 1;
                        thisForwardCount += 1;
                        mapCopy(xPos, yPos) = 0; // revisting will cost agent emptySpaceCost


                        // I don't this this is needed. in both cases agents should pay for revisting as though it were empty all the time
                        /*if (clearVisted) {
                            mapCopy(xPos, yPos) = 0; // revisting will cost agent emptySpaceCost
                        }
                        else { // clear visted is off, 1s on map (move forward) will be changed to -2, agents will still see marker on original map
                            // agents will not get extra points or lose points for revisting this location
                            mapCopy(xPos, yPos) = 0; // -2 value will remain unchanged so that this location will not score (positive or negitive) in the future
                        }
                        */
                    }
                    else if (mapCopy(xPos, yPos) == 0){
                        // if current location is empty, pay emptySpaceCost
                        score -= emptySpaceCost;
                    }

                    if (step < minSteps[mapID] + extraSteps) { // if there is still time (which also means agent has not arrived at end of map)
                        // collect world state
                        // worldStates are "on empty", "on foward" (or end), "on turn2", "on turn3", "what is sign2", "what is sign3"
                        // because of turn symbols, world states are NOT binary, but are finite int
                        if (analyze) {
                            worldStates.push_back({ inputValue == 0, inputValue == 1 || inputValue == 4, inputValue == 2, inputValue == 3, sign2,sign3 });
                        }

                        brain->update();

                        out0 = Bit(brain->readOutput(0));
                        out1 = Bit(brain->readOutput(1));
                        out2 = Bit(brain->readOutput(2));
                        if (debug) {
                            std::cout << "outputs: " << out0 << "," << out1 << "," << out2 << std::endl;
                        }

                        if (out2 == 1) { // step backwards, note world has a wall
                            xPos = std::max(0, std::min(xPos - dx[direction], mapSizes[mapID].first - 1));
                            yPos = std::max(0, std::min(yPos - dy[direction], mapSizes[mapID].second - 1));
                        }
                        else if (out0 == 1 && out1 == 1) { // step forwards, note world has a wall
                            xPos = std::max(0, std::min(xPos + dx[direction], mapSizes[mapID].first - 1));
                            yPos = std::max(0, std::min(yPos + dy[direction], mapSizes[mapID].second - 1));
                        }
                        else if (out0 == 1 && out1 == 0) { // turn left
                            direction = loopMod(direction - 1, 8);
                        }
                        else if (out0 == 0 && out1 == 1) { // turn right
                            direction = loopMod(direction + 1, 8);
                        }
                    }
                }
                org->dataMap.append("completion", (double)thisForwardCount / (double)forwardCounts[mapID]); // ratio of forwards visited / all forwards
                if (reachGoal) {
                    org->dataMap.append("score", score / maxScores[mapID]);
                }
                else {
                    org->dataMap.append("score", (.5 * score) / maxScores[mapID]); // agents only get 1/2 value for points if they don't get to the goal!
                }
                org->dataMap.append("reachGoal", reachGoal);
                if (debug || visualize) { // generate a report of this path
                    std::cout << "completion: " << (double)thisForwardCount / (double)forwardCounts[mapID] << std::endl;
                    std::cout << "score: " << score / maxScores[mapID] << std::endl;
                    std::cout << "reachGoal: " << reachGoal << std::endl;
                    std::cout << "sign2: " << sign2 << "   sign3: " << sign3 << std::endl;
                    if (visualize) { // save a report of this path
                        std::string os = std::to_string(score / maxScores[mapID]) + ",";
                        os += std::to_string(reachGoal) + ",";
                        os += std::to_string((double)thisForwardCount / (double)forwardCounts[mapID]) + ",";
                        os += std::to_string(firstTurn) + ",";
                        os += std::to_string(sign2) + ",";
                        os += std::to_string(sign3);
                        FileManager::writeToFile("visualizationData_" + std::to_string(Global::randomSeedPL->get(PT)) + ".txt", os,
                            "score,completion,reachGoal,firstTurn,sign2,sign3");

                    }
                }
            }
        }

        if (analyze) {
            int thisID = org->ID;

            std::cout << "organism with ID " << thisID << " scored " << org->dataMap.getAverage("score") << std::endl;
            
            auto lifeTimes = brain->getLifeTimes();
            
            auto inputStateSet = TS::remapToIntTimeSeries(brain->getInputStates(), TS::RemapRules::TRIT);

            auto outputStateSet = TS::remapToIntTimeSeries(brain->getOutputStates(), TS::RemapRules::TRIT);

            auto hiddenFullStatesSet = TS::remapToIntTimeSeries(brain->getHiddenStates(), TS::RemapRules::TRIT);
            auto hiddenAfterStateSet = TS::trimTimeSeries(hiddenFullStatesSet, TS::Position::FIRST, lifeTimes);
            auto hiddenBeforeStateSet = TS::trimTimeSeries(hiddenFullStatesSet, TS::Position::LAST, lifeTimes);

            std::vector<std::string> featureNames = { "onEmpty", "onFoward", "onTurn2", "onTurn3", "sign2", "sign3" };

            std::cout << "saving frag over time..." << std::endl;
            std::string header = "LOD_order, score,";
            std::string outStr = std::to_string(org->dataMap.getIntVector("ID")[0]) + "," + std::to_string(org->dataMap.getAverage("score")) + ",";
            std::vector<int> save_levelsThresholds = { 50,75,100 };
            for (auto th : save_levelsThresholds) {
                auto frag = FRAG::getFragmentationSet(worldStates, hiddenAfterStateSet, ((double)th) / 100.0, "feature");
                for (int f = 0; f < frag.size(); f++) {
                    //header += "Threshold_" + std::to_string(th) + "__feature_" + std::to_string(f) + ",";
                    header += "Threshold_" + std::to_string(th) + "__" + featureNames[f] + ",";
                    outStr += std::to_string(frag[f]) + ",";
                }
            }
            FileManager::writeToFile("fragOverTime.csv", outStr.substr(0, outStr.size() - 1), header.substr(0, header.size() - 1));

            std::cout << "saving brain connectome and structrue..." << std::endl;

            brain->saveConnectome("brainConnectome_id_" + std::to_string(thisID) + ".py");
            brain->saveStructure("brainStructure_id_" + std::to_string(thisID) + ".dot");

            std::string fileName = "StateToState_id_" + std::to_string(thisID) + ".txt";
            if (brain->recurrentOutput) {
                S2S::saveStateToState({ hiddenFullStatesSet, outputStateSet }, { inputStateSet }, lifeTimes, "H_O__I_" + fileName);
                S2S::saveStateToState({ hiddenFullStatesSet }, { inputStateSet }, lifeTimes, "H_I_" + fileName);
            }
            else {
                S2S::saveStateToState({ hiddenFullStatesSet, TS::extendTimeSeries(outputStateSet, lifeTimes, {0}, TS::Position::FIRST) }, { inputStateSet }, lifeTimes, "H_O__I_" + fileName);
                S2S::saveStateToState({ hiddenFullStatesSet }, { outputStateSet, inputStateSet }, lifeTimes, "H__O_I_" + fileName);
                S2S::saveStateToState({ hiddenFullStatesSet }, { inputStateSet }, lifeTimes, "H_I_" + fileName);
            }

            FRAG::saveFragMatrix(worldStates, hiddenAfterStateSet, "R_FragmentationMatrix_id_" + std::to_string(thisID) + ".py", "feature", { "onEmpty", "onFoward", "onTurn2", "onTurn3", "sign2", "sign3" });

            FileManager::writeToFile("score_id_" + std::to_string(thisID) + ".txt", std::to_string(org->dataMap.getAverage("score")));

            // save data flow information - 
            //std::vector<std::pair<double, double>> flowRanges = { {0,1},{0,.333},{.333,.666},{.666,1},{0,.5},{.5,1} };
            //std::vector<std::pair<double, double>> flowRanges = { {0,1},{0,.1},{.9,1} };
            std::vector<std::pair<double, double>> flowRanges = { {0,.25}, {.75,1}, {0,1} };//, { 0,.1 }, { .9,1 }};

            //std::cout << TS::TimeSeriesToString(TS::trimTimeSeries(brainStates, TS::Position::LAST, lifeTimes), ",",",") << std::endl;
            //std::cout << TS::TimeSeriesToString(TS::trimTimeSeries(brainStates, TS::Position::FIRST, lifeTimes), ",",",") << std::endl;
            if (brain->recurrentOutput) {
                FRAG::saveFragMatrixSet(
                    TS::Join({ TS::trimTimeSeries(hiddenFullStatesSet, TS::Position::FIRST, lifeTimes), TS::trimTimeSeries(outputStateSet, TS::Position::FIRST, lifeTimes) }),
                    TS::Join({ TS::trimTimeSeries(hiddenFullStatesSet, TS::Position::LAST, lifeTimes), inputStateSet, TS::trimTimeSeries(outputStateSet, TS::Position::LAST, lifeTimes) }),
                    lifeTimes, flowRanges, "flowMap_id_" + std::to_string(thisID) + ".py", "shared", -1);
            }
            else {
                FRAG::saveFragMatrixSet(
                    TS::Join(TS::trimTimeSeries(hiddenFullStatesSet, TS::Position::FIRST, lifeTimes), outputStateSet),
                    TS::Join(TS::trimTimeSeries(hiddenFullStatesSet, TS::Position::LAST, lifeTimes), inputStateSet),
                    lifeTimes, flowRanges, "flowMap_id_" + std::to_string(thisID) + ".py", "shared", -1);
            }

            std::cout << "saving brain states information..." << std::endl;
            std::string fileStr = "";
            if (brain->recurrentOutput) {

                auto discreetInput = inputStateSet;
                auto discreetOutputBefore = TS::trimTimeSeries(outputStateSet, TS::Position::LAST, lifeTimes);;
                auto discreetOutputAfter = TS::trimTimeSeries(outputStateSet, TS::Position::FIRST, lifeTimes);
                auto discreetHiddenBefore = TS::trimTimeSeries(hiddenFullStatesSet, TS::Position::LAST, lifeTimes);
                auto discreetHiddenAfter = TS::trimTimeSeries(hiddenFullStatesSet, TS::Position::FIRST, lifeTimes);
                int timeCounter = 0;
                int lifeCounter = 0;
                int lifeTimeCounter = 0;

                fileStr += "input,outputBefore,outputAfter,hiddenBefore,hiddenAfter,time,life,lifeTime\n";
                for (int i = 0; i < discreetInput.size(); i++) {
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetInput[i], ",") + "\",";
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetOutputBefore[i], ",") + "\",";
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetOutputAfter[i], ",") + "\",";
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetHiddenBefore[i], ",") + "\",";
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetHiddenAfter[i], ",") + "\",";

                    fileStr += std::to_string(timeCounter) + ",";
                    fileStr += std::to_string(lifeCounter) + ",";
                    fileStr += std::to_string(lifeTimeCounter) + "\n";

                    timeCounter++;
                    lifeTimeCounter++;
                    if (lifeTimes[lifeCounter] == lifeTimeCounter) { // if we are at the end of the current lifetime
                        lifeCounter++;
                        lifeTimeCounter = 0;
                    }
                }
            }
            else {

                auto discreetInput = inputStateSet;
                auto discreetOutput = outputStateSet;
                auto discreetHiddenBefore = TS::trimTimeSeries(hiddenFullStatesSet, TS::Position::LAST, lifeTimes);
                auto discreetHiddenAfter = TS::trimTimeSeries(hiddenFullStatesSet, TS::Position::FIRST, lifeTimes);
                int timeCounter = 0;
                int lifeCounter = 0;
                int lifeTimeCounter = 0;
                fileStr += "input,output,hiddenBefore,hiddenAfter,time,life,lifeTime\n";
                for (int i = 0; i < discreetInput.size(); i++) {
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetInput[i], ",") + "\",";
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetOutput[i], ",") + "\",";
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetHiddenBefore[i], ",") + "\",";
                    fileStr += "\"" + TS::TimeSeriesSampleToString(discreetHiddenAfter[i], ",") + "\",";

                    fileStr += std::to_string(timeCounter) + ",";
                    fileStr += std::to_string(lifeCounter) + ",";
                    fileStr += std::to_string(lifeTimeCounter) + "\n";

                    timeCounter++;
                    lifeTimeCounter++;
                    if (lifeTimes[lifeCounter]== lifeTimeCounter){ // if we are at the end of the current lifetime
                        lifeCounter++;
                        lifeTimeCounter = 0;
                    }
                }
            }

            FileManager::writeToFile("PathFollow_BrainActivity_id_" + std::to_string(thisID) + ".csv", fileStr);
            std::cout << "   done." << std::endl;

        } // end analyze
    } // end of population loop
}

// the requiredGroups function lets MABE know how to set up populations of organisms that this world needs
auto PathFollowWorld::requiredGroups() -> unordered_map<string,unordered_set<string>> {
    std::cout << "PathFollowWorld requires a brain named root:: with " + std::to_string(inputCount) + " inputs, and " + std::to_string(outputCount) << " outputs." << std::endl;
    return { { "root::", { "B:root::," + std::to_string(inputCount) + "," + std::to_string(outputCount)} } };
    // inputs: empty, path, turn bits 0 -> 4; outputs: left, righ
}
