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
    Parameters::register_parameter("WORLD_PATHFOLLOW-evaluationsPerGeneration", 1,
    "how many times should each organism be tested in each generation? (useful for non-deterministic brains)");

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
Parameters::register_parameter("WORLD_PATHFOLLOW-turnSymbolsCount", 2,
    "number of symbols that will be used when generating turn symbols.\n"
    "if inputMode is binary it is best if this value is a power of 2");

shared_ptr<ParameterLink<int>> PathFollowWorld::randomTurnSymbolsPL =
Parameters::register_parameter("WORLD_PATHFOLLOW-randomTurnSymbols", -1,
    "if 1, random symbols pairs will be determined per map (and per eval) for left and right.\n"
    "symbols will be the same for all agents in a generation.\n"
    "if 0, symbolValueMax is ignored and 1 and 2  (or 01 and 10) are always used\n"
    "if -1 all possible symbols pairs will be used per map (and per eval) for left and right");

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

shared_ptr<ParameterLink<bool>> PathFollowWorld::saveFragOverTimePL =
Parameters::register_parameter("WORLD_PATHFOLLOW_ANALYZE-saveFragOverTime", false,
    "");
shared_ptr<ParameterLink<bool>> PathFollowWorld::saveBrainStructureAndConnectomePL =
Parameters::register_parameter("WORLD_PATHFOLLOW_ANALYZE-saveBrainStructureAndConnectome", true,
    "");
shared_ptr<ParameterLink<bool>> PathFollowWorld::saveStateToStatePL =
Parameters::register_parameter("WORLD_PATHFOLLOW_ANALYZE-saveStateToState", true,
    "");
shared_ptr<ParameterLink<bool>> PathFollowWorld::save_R_FragMatrixPL =
Parameters::register_parameter("WORLD_PATHFOLLOW_ANALYZE-save_R_FragMatrix", false,
    "");
shared_ptr<ParameterLink<bool>> PathFollowWorld::saveFlowMatrixPL =
Parameters::register_parameter("WORLD_PATHFOLLOW_ANALYZE-saveFlowMatrix", false,
    "");
shared_ptr<ParameterLink<bool>> PathFollowWorld::saveStatesPL =
Parameters::register_parameter("WORLD_PATHFOLLOW_ANALYZE-saveStates", true,
    "");
shared_ptr<ParameterLink<bool>> PathFollowWorld::saveVisualPL =
Parameters::register_parameter("WORLD_PATHFOLLOW_ANALYZE-saveVisual", true,
    "save visualization, even though we are in analyze mode");


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
    std::cout << "  in loadSS, file is not open!\n  Exiting." << std::endl;
    exit(1);
    }
    return file.eof(); // ss contains the current line, this return tells us if we are at end of file
}


void PathFollowWorld::loadMaps(std::vector<string>& mapNames, std::vector<Vector2d<int>>& maps, std::vector<std::pair<int, int>>& mapSizes, std::vector<int>& initalDirections, std::vector<std::pair<int, int>>& startLocations) {
    for (auto mapName : mapNames) {
        std::cout << "  loading maps: " << mapName << std::endl;
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
    randomTurnSymbols = randomTurnSymbolsPL->get(PT);

    inputMode = inputModePL->get(PT);

    swapSymbolsAfter = swapSymbolsAfterPL->get(PT);

    saveFragOverTime = saveFragOverTimePL->get(PT);
    saveBrainStructureAndConnectome = saveBrainStructureAndConnectomePL->get(PT);
    saveStateToState = saveStateToStatePL->get(PT);
    save_R_FragMatrix = save_R_FragMatrixPL->get(PT);
    saveFlowMatrix = saveFlowMatrixPL->get(PT);
    saveStates = saveStatesPL->get(PT);
    saveVisual = saveVisualPL->get(PT);

    convertCSVListToVector(mapNamesPL->get(PT), mapNames);

    std::cout << "In pathFollowWorld, loading maps (if 'done loading maps' does not appear, there is an issue in the map files)..." << std::endl;
    loadMaps(mapNames, maps, mapSizes, initalDirections, startLocations);
    std::cout << "  done loading maps." << std::endl;

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

    // if randomizeTurnSigns and , all possible pairs of signals
    if (randomTurnSymbols == -1) {
        turnSignalPairs.resize(turnSymbolsCount * (turnSymbolsCount - 1));
        int signalListCount = 0;
        for (int t1 = 0; t1 < turnSymbolsCount; t1++) {
            for (int t2 = 0; t2 < turnSymbolsCount; t2++) {
                if (t1 != t2) {
                    turnSignalPairs[signalListCount].first = t1;
                    turnSignalPairs[signalListCount].second = t2;
                    signalListCount++;
                }
            }
        }
        // if all pairs are seen, then eval per gen (i.e. number of times each map is seen) is number of pairs
        evaluationsPerGeneration = turnSignalPairs.size();

        // give a report
        std::cout << "  randomTurnSymbols set to -1, so all maps will see the following turn pairs:" << std::endl;
        for (auto p : turnSignalPairs) {
            std::cout << "    " << p.first << "," << p.second << std::endl;
        }
    }
    else if (randomTurnSymbols == 1) { // if randomTurnSymbols then each map will be visted evaluationsPerGeneration times
        turnSignalPairs.resize(evaluationsPerGeneration * maps.size());
        std::cout << "randomTurnSymbols set to 1, turn pairs will be determined ever generaion." << std::endl;
    }
    else { // randomTurnSymbols == 0, fixed signals
        turnSignalPairs = { {0,1} };
        std::cout << "randomTurnSymbols set to 0, turn pairs will be fixed." << std::endl;
    }

    // determin the number of inputs based on the input mode
    if (inputMode == "single") { // input will be a single number (effectivly, the map value)
        inputCount = 1;
    }
    else if (inputMode == "mixed") {
        inputCount = 4; // location is empty + location is forward + location is turn + turn symbol
    }
    else if (inputMode == "binary") { // turn symbol will be in bits, we need to know the number of bits
        outputsNeededForTurnSign = 0;
        int temp = turnSymbolsCount - 1; // e.g. if symbolValueMax is 2, this will be 1, or 0 and 1 (1 bit) will be needed
        while (temp > 0) {
            outputsNeededForTurnSign++;
            temp = temp >> 1;
        }
        inputCount = 3 + outputsNeededForTurnSign; // location is empty + location is forward + location is turn + bittized turn symbol
    }
    else {
        std::cout << "\nWORLD_PATHFOLLOW-inputMode parameter, not valid. Was given: " << inputMode << ", please correct and try again." << std::endl;
        exit(1);
    }
    
    outputCount = 3; // 001 = reverse, 010 = right, 100 = left, 110 = forward, remaining = no action

    // popFileColumns tell MABE what data should be saved to pop.csv files
    popFileColumns.clear();
    popFileColumns.push_back("score");      // (score/maxScore)/mapCount
    popFileColumns.push_back("reachGoal");  // did the agent reach the goal Y/N
    popFileColumns.push_back("completion"); // (how much of the path did the agent visit) / (entire path length)
    popFileColumns.push_back("correctTurnRate"); // rate at which agents make correct turns
}

// the evaluate function gets called every generation. evaluate should set values on organisms datamaps
// that will be used by other parts of MABE for things like reproduction and archiving
// here we pick turn symbols once and these are used for all agents (to make sure we are doing a fair evaluation)
// then agents are tested (one-at-a-time) on each map and given a normalized score.
auto PathFollowWorld::evaluate(map<string, shared_ptr<Group>>& groups, int analyze, int visualize, int debug) -> void {
    
    if (analyze && saveVisual) {
        visualize = true;
    }

    int sign2; // remapping for 2s in the map (left)
    int sign3; // remapping for 3s in the map (right)

    
    // if randomizeTurnSigns, create a pair of random signals for each map
    if (randomTurnSymbols == 1) {
        for (int t = 0; t < evaluationsPerGeneration * maps.size(); t++) {
            turnSignalPairs[t].first = Random::getIndex(turnSymbolsCount);
            turnSignalPairs[t].second = Random::getIndex(turnSymbolsCount - 1); // one less, so, if the same values are picked, we have somewhere to go
            if (turnSignalPairs[t].first == turnSignalPairs[t].second) {
                turnSignalPairs[t].second = turnSymbolsCount; // if values match, then set to max value - this is the only way the second symbol can be max
                                                           // i.e. in the case of a match, the max values "replaces" the chosen value
            }
            if (debug) {
                std::cout << "\non update: " << Global::update << " map: " << t << " has random values: " << turnSignalPairs[t].first << " " << turnSignalPairs[t].second << std::endl;
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

        int turnsIndex = 0; // used to itterate over turnSignalPairs

        int correctTurns = 0; // count of number of times agent turns correctly
        int totalTurns = 0; // count of number of turns seen by agent

        // evaluate this organism on each map evaluationsPerGeneration times
        // turnSignalPairs[turnsIndex] will provide the correct turn pair and is updated automaticly
        for (size_t mapID = 0; mapID < maps.size(); mapID++) { // for each map
            for (int evaluation = 0; evaluation < evaluationsPerGeneration; evaluation++) {
                //std::cout << randomValues.size() << "  trial: " << trial << "  mapID:" << mapID << "  trial * mapID: " << trial * maps.size() + mapID << "   " << randomValues[trial * maps.size() + mapID].first << "," << randomValues[trial * maps.size() + mapID].second << std::endl;
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

                sign2 = turnSignalPairs[turnsIndex].first;
                sign3 = turnSignalPairs[turnsIndex++].second;
                if (turnsIndex >= turnSignalPairs.size()) { // we are at the end of turnSignalPairs, reset to start
                    turnsIndex = 0;
                }

                if (debug) {
                    mapCopy.showGrid(); // show current map
                    std::cout << "at location: " << xPos << "," << yPos << "  direction: " << direction << std::endl;
                }

                if (visualize) {
                    std::string os = "new"; // initalize this map in the visualize file
                    FileManager::writeToFile("pathVisualization.txt", os);
                }

                // clear the brain - resets brain state including memory. marks end of lifetime in recorded brain states
                brain->resetBrain();


                int firstTurn = -1; // the agent has not stepped on a turn yet, so we don't know what the first seen turn is yet
                int lastTrun = -1; // the agent has not stepped on a turn yet, so we don't know what the last seen turn is yet
                bool swapped = false; // have he symbols been swapped?

                for (int step = 0; step < (minSteps[mapID] + extraSteps); step++) { // this is how much time the agent has on this map

                    if ((swapSymbolsAfter < 1.0) && (swapped == false) && (step > ((double)minSteps[mapID] * swapSymbolsAfter))) {
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

                        if (inputMode == "single" || inputMode == "mixed") {
                            os += std::to_string(sign2 + 1) + "\n";
                            os += std::to_string(sign3 + 1) + "\n";
                        }
                        else { // inputMode is binary
                            os += std::to_string(sign2) + "\n";
                            os += std::to_string(sign3) + "\n";
                        }

                        auto mapSource = mapCopy; // only used in visualize
                        if (!clearVisted) {
                            mapSource = maps[mapID];
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
                                    if (debug) { std::cout << "L "; }
                                    os += "2";
                                }
                                else if (mapSource(x, y) == 3) {
                                    if (debug) { std::cout << "R ";; }
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
                        if (clearVisted) {
                            mapCopy.showGrid();
                        }
                        else {
                            maps[mapID].showGrid();
                        }
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
                        if (inputValue == 1) { // value in map was 2 (left)
                            inputValue = sign2 + 1;
                        }
                        else if (inputValue == 2) { // value in map was 3 (right)
                            inputValue = sign3 + 1;
                        }
                        //else if (inputValue == 3) { // value in map was 4
                        //    inputValue = 0; // location is end, appears as forward (but this never happens)
                        //}
                        brain->setInput(0, inputValue);
                    }
                    else { // inputMode == "mixed" || "binary"

                        brain->setInput(0, inputValue == 0); // is location empty?
                        brain->setInput(1, inputValue == 1 || inputValue == 4); // is location path?   foraward and end look the same (but this never happens)

                        if (inputMode == "mixed") {
                            if (inputValue == 2) {
                                brain->setInput(2, 1); // this location is a turn
                                brain->setInput(3, sign2 + 1); // sign associated with left turn, add one so that 0 is unique for no turn
                            }
                            else if (inputValue == 3) {
                                brain->setInput(2, 1); // this location is a turn
                                brain->setInput(3, sign3 + 1); // sign associated with left turn, add one so that 0 is unique for no turn
                            }
                            else { // if not a turn
                                brain->setInput(2, 0); // this location is not a turn
                                brain->setInput(3, 0); // blank
                            }
                        } // end inputMode = "mixed"
                        else { // inputMode == "binary"

                            int val;
                            bool isTurn;

                            if (inputValue == 2) { // is left turn
                                val = sign2;
                                isTurn = true;
                            }
                            else if (inputValue == 3) { // is right turn
                                val = sign3;
                                isTurn = true;
                            }
                            else { // is not a turn
                                val = 0;
                                isTurn = false;
                            }

                            if (isTurn == false) { // set "outputsNeededForTurnSign" bits to 0 (this is not a turn)
                                brain->setInput(2, 0); // set input turn
                                for (int xx = 0; xx < outputsNeededForTurnSign; xx++) {
                                    brain->setInput(3 + xx, 0); // set inputs 3+ to 0s
                                }
                            }
                            if (isTurn == true) { // convert turn symbol "val" in to "outputsNeededForTurnSign" bits
                                brain->setInput(2, 1); // set input turn
                                for (int xx = 0; xx < outputsNeededForTurnSign; xx++) {
                                    brain->setInput(3 + xx, val & 1); // set inputs 3+ to turn symbol bits
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
                    // if map location is 1, set to 0, no more points.
                    
                    int mapValueHere = mapCopy(xPos, yPos); // used to check correct turn

                    if (mapCopy(xPos, yPos) > 1) {
                        lastTrun = mapCopy(xPos, yPos); // this is now the last turn seen
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
                        else { // this is a turn
                            totalTurns++;
                            mapCopy(xPos, yPos) = 1; // set this location value to on mapCopy 1 so that on the next update agents do not pay emptySpaceCost
                        }
                    }
                    else if (mapCopy(xPos, yPos) == 1) { // if symbol on map copy is forward
                        score += 1;
                        thisForwardCount += 1;
                        mapCopy(xPos, yPos) = 0; // revisting will cost agent emptySpaceCost
                    }
                    // if current location is empty, pay emptySpaceCost
                    else if (maps[mapID](xPos, yPos) == 0 || (mapCopy(xPos, yPos) == 0 && clearVisted)) {
                        // if current location is empty, pay emptySpaceCost
                        score -= emptySpaceCost;
                    }

                    if (step < minSteps[mapID] + extraSteps) { // if there is still time (which also means agent has not arrived at end of map)
                        // collect world state
                        // worldStates are "on empty", "on foward" (or end), "on left turn", "on right", "what is left signal", "what is right signal", "what was last turn signal"
                        // because of turn symbols, world states are NOT binary, but are finite int
                        if (analyze) {
                            worldStates.push_back({ inputValue == 0, inputValue == 1 || inputValue == 4, inputValue == 2, inputValue == 3, sign2, sign3, lastTrun});
                        }

                        brain->update();

                        out0 = Bit(brain->readOutput(0));
                        out1 = Bit(brain->readOutput(1));
                        out2 = Bit(brain->readOutput(2));
                        if (debug) {
                            std::cout << "outputs: " << out0 << "," << out1 << "," << out2 << std::endl;
                        }

                        if (out2 == 1) { // step backwards
                            xPos = std::max(0, std::min(xPos - dx[direction], mapSizes[mapID].first - 1));
                            yPos = std::max(0, std::min(yPos - dy[direction], mapSizes[mapID].second - 1));
                        }
                        else if (out0 == 1 && out1 == 1 && out2 == 0) { // step forwards
                            xPos = std::max(0, std::min(xPos + dx[direction], mapSizes[mapID].first - 1));
                            yPos = std::max(0, std::min(yPos + dy[direction], mapSizes[mapID].second - 1));
                        }
                        else if (out0 == 1 && out1 == 0 && out2 == 0) { // turn left
                            direction = loopMod(direction - 1, 8);
                            if (mapValueHere == 2) {
                                correctTurns++;
                            }
                        }
                        else if (out0 == 0 && out1 == 1 && out2 == 0) { // turn right
                            direction = loopMod(direction + 1, 8);
                            if (mapValueHere == 3) {
                                correctTurns++;
                            }
                        }
                        // else (0,0,0) do nothing
                    }

                } // current path finished
                org->dataMap.append("completion", (double)thisForwardCount / (double)forwardCounts[mapID]); // ratio of forwards visited / all forwards
                if (reachGoal) {
                    org->dataMap.append("score", score / maxScores[mapID]);
                }
                else {
                    org->dataMap.append("score", (.5 * score) / maxScores[mapID]); // agents only get 1/2 value for points if they don't get to the goal!
                }
                org->dataMap.append("reachGoal", reachGoal);
                org->dataMap.append("correctTurnRate", (double)correctTurns/(double)totalTurns);
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

            std::cout << "\nAlalyze Mode:  organism with ID " << thisID << " scored " << org->dataMap.getAverage("score") << std::endl;
            
            auto lifeTimes = brain->getLifeTimes();
            
            auto inputStateSet = TS::remapToIntTimeSeries(brain->getInputStates(), TS::RemapRules::TRIT);

            auto outputStateSet = TS::remapToIntTimeSeries(brain->getOutputStates(), TS::RemapRules::TRIT);

            auto hiddenFullStatesSet = TS::remapToIntTimeSeries(brain->getHiddenStates(), TS::RemapRules::UNIQUE);
            auto hiddenAfterStateSet = TS::trimTimeSeries(hiddenFullStatesSet, TS::Position::FIRST, lifeTimes);
            auto hiddenBeforeStateSet = TS::trimTimeSeries(hiddenFullStatesSet, TS::Position::LAST, lifeTimes);

            FileManager::writeToFile("score.txt", std::to_string(org->dataMap.getAverage("score")));

            if (saveFragOverTime) { // change to 1 to save frag over time
                std::cout << "  saving frag over time..." << std::endl;
                std::vector<std::string> featureNames = { "onEmpty", "onFoward", "onLeft", "onRight", "leftSig", "rightSig", "lastTurn"};
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
            }

            if (saveBrainStructureAndConnectome) {
                std::cout << "  saving brain connectome and structrue..." << std::endl;

                brain->saveConnectome("brainConnectome_id_" + std::to_string(thisID) + ".py");
                brain->saveStructure("brainStructure_id_" + std::to_string(thisID) + ".dot");
            }
            if (saveStateToState) {
                std::cout << "  saving state to state..." << std::endl;
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
            }
            if (save_R_FragMatrix) {
                std::cout << "  saving R frag matrix..." << std::endl;

                FRAG::saveFragMatrix(worldStates, hiddenAfterStateSet, "R_FragmentationMatrix_id_" + std::to_string(thisID) + ".py", "feature", { "on Empty", "on Foward", "on Left", "on Right", "left Sig", "right Sig", "last Turn" });

                FileManager::writeToFile("score_id_" + std::to_string(thisID) + ".txt", std::to_string(org->dataMap.getAverage("score")));
            }
            if (saveFlowMatrix) {
                std::cout << "  saving flow matix..." << std::endl;

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
            }
            if (saveStates) {
                std::cout << "  saving brain states information..." << std::endl;
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
                        if (lifeTimes[lifeCounter] == lifeTimeCounter) { // if we are at the end of the current lifetime
                            lifeCounter++;
                            lifeTimeCounter = 0;
                        }
                    }
                }

                FileManager::writeToFile("PathFollow_BrainActivity_id_" + std::to_string(thisID) + ".csv", fileStr);
            } 
            std::cout << "  ... analyze done" << std::endl;
        } // end analyze
    } // end of population loop
}

// the requiredGroups function lets MABE know how to set up populations of organisms that this world needs
auto PathFollowWorld::requiredGroups() -> unordered_map<string,unordered_set<string>> {
    std::cout << "PathFollowWorld requires a brain named root:: with " + std::to_string(inputCount) + " inputs, and " + std::to_string(outputCount) << " outputs." << std::endl;
    return { { "root::", { "B:root::," + std::to_string(inputCount) + "," + std::to_string(outputCount)} } };
    // inputs: empty, path, turn bits 0 -> 4; outputs: left, righ
}
