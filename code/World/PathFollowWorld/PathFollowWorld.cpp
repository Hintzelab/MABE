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

shared_ptr<ParameterLink<int>> PathFollowWorld::symbolValueMaxPL =
Parameters::register_parameter("WORLD_PATHFOLLOW-symbolValueMax", 100,
    "number of symbols that will be used when generating turn symbols. range is [1,signValueMax]\n"
    "if inputMode is binary it is best if this value is a power of 2 minus 1");

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
    (std::string)"../code/World/pathFollowWorld/path1.txt,../code/World/pathFollowWorld/path2.txt",
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
        ss.str(std::string());
ss << rawLine;
    }
    else if (!file.eof()) {
    std::cout << "in loadSS, file is not open!\n  Exiting." << std::endl;
    exit(1);
    }
    //std::cout << "from file:  " << rawLine << std::endl;
    return file.eof();
}


void PathFollowWorld::loadMaps(std::vector<string>& mapNames, std::vector<Vector2d<int>>& maps, std::vector<std::pair<int, int>>& mapSizes, std::vector<int>& initalDirections, std::vector<std::pair<int, int>>& startLocations) {
    for (auto mapName : mapNames) {

        std::string rawLine;
        std::stringstream ss;

        std::ifstream FILE(mapName);
        bool atEOF = loadLineFromFile(FILE, rawLine, ss);
        std::vector<int> tempVect;
        int tempInt;

        convertCSVListToVector(rawLine, tempVect);

        mapSizes.push_back({ tempVect[0], tempVect[1] });
        maps.push_back(Vector2d<int>(tempVect[0], tempVect[1]));

        atEOF = loadLineFromFile(FILE, rawLine, ss);
        initalDirections.push_back(std::stoi(rawLine));

        atEOF = loadLineFromFile(FILE, rawLine, ss);

        size_t y = 0;
        while (!atEOF) {
            for (size_t x = 0; x < rawLine.size(); x++) {
                if (rawLine[x] == 'X') { // this is the start location, put 'path' here
                    maps.back()((int)x, (int)y) = 1;
                    startLocations.push_back({ x,y });
                }
                else {
                    maps.back()((int)x, (int)y) = rawLine[x] - 48;
                    //convertString(std::to_string(rawLine[x]), maps.back()((int)x, (int)y));// tempInt);
                }
            }
            y += 1;
            atEOF = loadLineFromFile(FILE, rawLine, ss);
        }
    }
}



// the constructor gets called once when MABE starts up. use this to set things up
PathFollowWorld::PathFollowWorld(shared_ptr<ParametersTable> PT_) : AbstractWorld(PT_) {

    //localize a parameter value for faster access
    evaluationsPerGeneration = evaluationsPerGenerationPL->get(PT);

    extraSteps = extraStepsPL->get(PT);
    emptySpaceCost = emptySpaceCostPL->get(PT);
    clearVisted = clearVistedPL->get(PT);

    symbolValueMax = symbolValueMaxPL->get(PT);
    useRandomTurnSymbols = useRandomTurnSymbolsPL->get(PT);
    if (!useRandomTurnSymbols) {
        symbolValueMax = 2;
    }
    inputMode = inputModePL->get(PT);

    swapSymbolsAfter = swapSymbolsAfterPL->get(PT);

    convertCSVListToVector(mapNamesPL->get(PT), mapNames);

    std::cout << "In pathFollowWorld, loading maps:" << std::endl;
    loadMaps(mapNames, maps, mapSizes, initalDirections, startLocations);
    addFlippedMaps = addFlippedMapsPL->get(PT);

    int mapsCount = maps.size();
    maxScores.resize(mapsCount,0);
    minSteps.resize(mapsCount,0);
    forwardCounts.resize(mapsCount,0);
    turnCounts.resize(mapsCount, 0);

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
        maxScores[i] = forwardCounts[i] + extraSteps; // max score an agent can get (before nomalization) is collect all forward counts and have all extra steps left over

        std::cout << "\n" << mapNames[i] << "  " << startLocations[i].first << "," << startLocations[i].second << std::endl;
        std::cout << "min steps: " << minSteps[i] << "  turn count: " << turnCounts[i] << "  forward counts: " << forwardCounts[i] << std::endl;
        maps[i].showGrid();

        if (addFlippedMapsPL->get(PT)) {
            // if flipped maps are flipped horizontaly, flip last loaded map and append to back of list
            // so all orignal maps will be listed first, then all flipped maps
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
            maps.push_back(newMap);
            mapNames.push_back(mapNames[i] + "_R"); // use the current map name + "_R"
            mapSizes.push_back(mapSizes[i]);
            startLocations.push_back({ (xSize - 1) - startLocations[i].first, startLocations[i].second }); // flip x, leave y alone
            minSteps.push_back(minSteps[i]);
            maxScores.push_back(maxScores[i]);
            forwardCounts.push_back(forwardCounts[i]);
            turnCounts.push_back(turnCounts[i]);
            initalDirections.push_back(initalDirections[i]);
            // brute force update for initalDirections on flipped maps
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
            else if (initalDirections.back() == 7) {
                initalDirections.back() = 1;
            }
        }
    }

    //for (int i = 0; i < maps.size(); i++) {
    //    std::cout << "\n" << mapNames[i] << "  " << startLocations[i].first << "," << startLocations[i].second << std::endl;
    //    std::cout << "min steps: " << minSteps[i] << "  forward counts: " << forwardCounts[i] << std::endl;
    //    std::cout << "size: " << mapSizes[i].first << ", " << mapSizes[i].second << std::endl;
    //    maps[i].showGrid();
    //}

    randomValues.resize(evaluationsPerGeneration*maps.size());

    if (inputMode == "single") {
        inputCount = 1;
    }
    else {
        inputCount = 3; // down = empty + down = forward sign + down = turn
        if (inputMode == "mixed") {
            inputCount += 1; // + turn symbol
        }
        else if (inputMode == "binary") {
            outputsNeededForTurnSign = 0;
            int temp = symbolValueMax;
            while (temp > 0) {
                outputsNeededForTurnSign++;
                temp = temp >> 1;
            }
        inputCount += outputsNeededForTurnSign; // the 1 for this is a turn signal
        }
    }
    
    outputCount = 3; // ??1 = reverse, 000 = no action, 010 = right, 100 = left, 110 = forward

    // popFileColumns tell MABE what data should be saved to pop.csv files
    popFileColumns.clear();
    popFileColumns.push_back("score");
    popFileColumns.push_back("reachGoal");
    popFileColumns.push_back("completion");
}

// the evaluate function gets called every generation. evaluate should set values on organisms datamaps
// that will be used by other parts of MABE for things like reproduction and archiving
auto PathFollowWorld::evaluate(map<string, shared_ptr<Group>>& groups, int analyze, int visualize, int debug) -> void {

    int sign2; // remapping for 2s in the map
    int sign3; // remapping for 3s in the map

    // if randomizeTurnSigns, create a pair of random signals for each map
    if (useRandomTurnSymbols && currentUpdate < Global::update) {
        for (int t = 0; t < evaluationsPerGeneration * maps.size(); t++) {
            randomValues[t].first = Random::getInt(1, symbolValueMax);
            randomValues[t].second = Random::getInt(1, symbolValueMax - 1); // one less
            if (randomValues[t].first == randomValues[t].second) {
                randomValues[t].second = symbolValueMax; // if values match, then set to greatest value
            }
            if (debug) {
                std::cout << "\non update: " << Global::update << " test: " << t << " has random values: " << randomValues[t].first << " " << randomValues[t].second << std::endl;
            }
        }
        currentUpdate = Global::update;
    }

    int popSize = groups[groupName]->population.size();
    
    // in this world, organisms do not interact, so we can just iterate over the population
    // on each iteration, each agent will visit every world evaluationsPerGeneration times
    for (int orgID = 0; orgID < popSize; orgID++) {

        // create a shortcut to access the organism and organisms brain
        auto org = groups[groupName]->population[orgID];
        auto brain = org->brains[brainName];
        
        int xPos, yPos, direction, out0, out1, out2;
        double score, reachGoal;
        int thisForwardCount;

        // evaluate this organism some number of times based on evaluationsPerGeneration
        for (int trial = 0; trial < evaluationsPerGeneration; trial++) {
            for (size_t mapID = 0; mapID < maps.size(); mapID++) {
                // new map! reset score and reachGoal
                score = 0;
                reachGoal = 0;

                // set starting location using value from file for this map
                xPos = startLocations[mapID].first;
                yPos = startLocations[mapID].second;
                direction = initalDirections[mapID];
                thisForwardCount = 0;

                // make a copy of the map so we can change it
                auto mapCopy = maps[mapID];
                
                // if useRandomTurnSymbols, pull values for turn symbol values from randomValues
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
                    std::string os = "new";
                    FileManager::writeToFile("pathVisualization.txt", os);
                }

                // clear the brain - resets brain state including memory
                brain->resetBrain();


                int firstTurn = -1;
                bool swapped = false;

                for (int step = 0; step < (minSteps[mapID] + extraSteps); step++) {

                    if ((swapSymbolsAfter < 1.0) && (swapped == false) && (step > ((double)minSteps[mapID] * swapSymbolsAfter)) ) {
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

                        if (debug) {
                            std::cout << "at location: " << xPos << "," << yPos << "  direction: " << direction << std::endl;
                            std::cout << "forward steps taken: " << thisForwardCount << "  current score: " << score << std::endl;
                            std::cout << "value @ this location: " << mapCopy(xPos, yPos) << std::endl;
                        }
                    }


                    int inputValue; // value at agents current location
                    if (clearVisted) {
                        inputValue = mapCopy(xPos, yPos);
                    }
                    else {
                        inputValue = maps[mapID](xPos, yPos);
                    }

                    if (inputMode == "single") {
                        // map values are 0 = empty, 1 = forward, 2 = left, 3 = right,
                        // but output is -1 = empty, 0 = forward, 1+ = turn, so we need to do some conversion...
                        inputValue--;
                        if (inputValue == 1) { // value in map was 2
                            inputValue = sign2;
                        }
                        else if (inputValue == 2) { // value in map was 3
                            inputValue = sign3;
                        }
                        else if (inputValue == 3) { // value in map was 4
                            inputValue = 0; // end marker now looks like any other forward location so it does not appear as a turn
                        }
                        brain->setInput(0, inputValue);
                    }
                    else { // (inputMode == "mixed" || "binary") {
                        
                        brain->setInput(0, inputValue == 0); // is location empty?
                        brain->setInput(1, inputValue == 1 || inputValue == 4); // is location path or goal?
                        
                        if (inputMode == "mixed") {
                            if (inputValue == 2) {
                                brain->setInput(2, 1); // is location a turn?
                                brain->setInput(3, sign2);
                            }
                            else if (inputValue == 3) {
                                brain->setInput(2, 1); // is location a turn?
                                brain->setInput(3, sign3);
                            }
                            else { // if not a turn
                                brain->setInput(2, 0); // is location a turn?
                                brain->setInput(3, 0);
                            }
                        } // end inputMode = "mixed"
                        else { // inputMode == "binary"

                            int val;
                            bool isTurn;

                            if (inputValue == 2) {
                                val = sign2;
                                isTurn = true;
                            }
                            else if (inputValue == 3) {
                                val = sign3;
                                isTurn = true;
                            }
                            else { // if not a turn
                                val = 0;
                                isTurn = false;
                            }

                            if (isTurn == false) {
                                brain->setInput(2, 0); // set input turn
                                for (int xx = 0; xx < outputsNeededForTurnSign; xx++) {
                                    brain->setInput(3 + xx, 0); // set turn symbol inputs to 0
                                }
                            }
                            if (isTurn == true) {
                                brain->setInput(2, 1); // set input turn
                                int c = 0;
                                for (int xx = 0; xx < outputsNeededForTurnSign; xx++) {
                                    brain->setInput(3 + xx, val & 1); // set turn symbol inputs
                                    val = val >> 1;
                                    ++c;
                                }
                            }
                        } // end else inputMode == "binary"
                    } // end else (inputMode == "mixed" || "binary")

                    // now that agent has inputs, update score based on map value at this location,
                    // and update map
                    // if map location = 1, +1 score, and change map location value to 0
                    // if map location = 4, goal, set step = steps (so while loop will end)
                    //    also add any remaning steps to score, if all path locations were visited
                    // if map location > 1, set location value to 1 (i.e. turn markers become 1s),
                    //    the value will be 1 next update if this agent turns, which will provide +1 score
                    if (mapCopy(xPos, yPos) > 1) {
                        if (firstTurn == -1) { // if this is the first turn the agent has seen in this map, recored map value
                            firstTurn = mapCopy(xPos, yPos);
                        }
                        if (mapCopy(xPos, yPos) == 4) { // if we get to the goal, and visted all locations get extra points for time left
                            if (thisForwardCount >= forwardCounts[mapID]) { // if all forward locations have been visited...
                                reachGoal = 1;
                                score += (minSteps[mapID] + extraSteps) - step; // add points for time left
                            }
                            step = minSteps[mapID] + extraSteps; // if agent steps on 4, end now
                        }
                        mapCopy(xPos, yPos) = 1; // set this location value to 1 so that on the next update agents do not pay emptySpaceCost
                    }
                    else if (mapCopy(xPos, yPos) == 1) {
                        score += 1;
                        thisForwardCount += 1;
                        if (clearVisted) {
                            mapCopy(xPos, yPos) = 0; // revisting will cost agent emptySpaceCost
                        }
                        else { // clear visted is off, 1s on map (move forward) will be changed to -2, agents will still see marker on original map
                            // agents will not get extra points or lose points for revisting this location
                            mapCopy(xPos, yPos) = -2; // -2 value will remain unchanged so that this location will not score (positive or negitive) in the future
                        }
                    }
                    else if (mapCopy(xPos, yPos) == 0){
                        // if current location is empty, pay emptySpaceCost
                        score -= emptySpaceCost;
                    }

                    brain->update();

                    out0 = Bit(brain->readOutput(0));
                    out1 = Bit(brain->readOutput(1));
                    out2 = Bit(brain->readOutput(2));
                    if (debug) {
                        std::cout << "outputs: " << out0 << "," << out1 << "," << out2 << std::endl;
                    }

                    if (out2 == 1) { // reverse
                        xPos = std::max(0, std::min(xPos - dx[direction], mapSizes[mapID].first-1));
                        yPos = std::max(0, std::min(yPos - dy[direction], mapSizes[mapID].second-1));
                    }
                    else if (out0 == 1 && out1 == 1) { // forward
                        xPos = std::max(0, std::min(xPos + dx[direction], mapSizes[mapID].first-1));
                        yPos = std::max(0, std::min(yPos + dy[direction], mapSizes[mapID].second-1));
                    }
                    else if (out0 == 1 && out1 == 0) { // left
                        direction = loopMod(direction - 1, 8);
                    }
                    else if (out0 == 0 && out1 == 1) { // right
                        direction = loopMod(direction + 1, 8);
                    }
                }
                org->dataMap.append("completion", (double)thisForwardCount / (double)forwardCounts[mapID]);
                if (reachGoal) {
                    org->dataMap.append("score", score / maxScores[mapID]);
                }
                else {
                    org->dataMap.append("score", (.5 * score) / maxScores[mapID]);
                }
                org->dataMap.append("reachGoal", reachGoal);
                if (debug || visualize) {
                    std::cout << "completion: " << (double)thisForwardCount / (double)forwardCounts[mapID] << std::endl;
                    std::cout << "score: " << score / maxScores[mapID] << std::endl;
                    std::cout << "reachGoal: " << reachGoal << std::endl;
                    std::cout << "sign2: " << sign2 << "   sign3: " << sign3 << std::endl;
                    if (visualize) {
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
    } // end of population loop
}

// the requiredGroups function lets MABE know how to set up populations of organisms that this world needs
auto PathFollowWorld::requiredGroups() -> unordered_map<string,unordered_set<string>> {
    std::cout << "pathFollowWorld requires a brain named " + brainName + " with " + std::to_string(inputCount) + " inputs, and " + std::to_string(outputCount) << " outputs." << std::endl;
    return { { groupName, { "B:" + brainName + "," + std::to_string(inputCount) + "," + std::to_string(outputCount)} } };
    // inputs: empty, path, turn bits 0 -> 4; outputs: left, righ
}
