//
//  BlockCatch World
//  written by Cliff Bohm
//

#include <algorithm>    // std::rotate

#include "BlockCatchWorld.h"

std::shared_ptr<ParameterLink<int>> BlockCatchWorld::visualizeBestPL = Parameters::register_parameter("WORLD_BLOCKCATCH-visualizeBest", -1, "visualize best scoring organism every visualizeBest generations, excluding generation 0.\n"
	"if -1, do not visualize on steps (this parameter does not effect visualize mode)");

std::shared_ptr<ParameterLink<std::string>> BlockCatchWorld::scoreMethodPL = Parameters::register_parameter("WORLD_BLOCKCATCH-scoreMethod", (std::string)"ANY_ANY", "How should score be calculated?\n"
	"ANY_ANY - a hit is defined as any part of the pattern contacting at part of the paddle\n"
	"VISIBLE_ANY - a hit is defined as any visible part of the pattern contacting any part of the paddle\n"
	"VISIBLE_SENSOR - a hit is defined as any visible part of the pattern contacting any sensor on the paddle\n"
	"SUM_ALL_ALL - a hit will be recorded for each part of the pattern that contacts any part of the paddle\n"
	"SUM_VISIBLE_SENSOR - a hit will be recorded for each visible part of the pattern that contacts a sensor on the paddle\n"
	"SUM_VISIBLE_NON_SENSOR - a hit will be recorded for each visible part of the pattern that contacts a non-sensor on the paddle.\n"
	"NOTE: correct and incorrect are calculated diffrently if any of the SUM methods are used, see the README file!");

std::shared_ptr<ParameterLink<std::string>> BlockCatchWorld::paddlePL = Parameters::register_parameter("WORLD_BLOCKCATCH-paddle", (std::string)"110011", "list of 1 (for sensor) and 0 (for non-sensor) used to define the paddle");

std::shared_ptr<ParameterLink<std::string>> BlockCatchWorld::catchPatternsPL = Parameters::register_parameter("WORLD_BLOCKCATCH-patternsToCatch", (std::string)"1L,1R", "a comma separted list of block patterns to be caught\n"
	"each pattern is followed by a list of at least one L, R or, H. These letters will determine\n"
	"movement (left, right, hold). for example, 101LLRH = pattern 3 wide with a hole in the middle\n"
	"which moves left, left, right, no movement, left, left, right, hold...\n"
	"the letters L and R may be followed by a number, this will allow the pattern to move more faster.\n"
	"patterns may be defined with : to allow for animated patterns (i.e. 10:01L will define a pattern\n"
	"that ossilates between 10 and 01 while moving left.");
std::shared_ptr<ParameterLink<std::string>> BlockCatchWorld::missPatternsPL = Parameters::register_parameter("WORLD_BLOCKCATCH-patternsToMiss", (std::string)"11L,11R",
	"a comma separted list of block pattern sizes to be missed (see blockPatternsToCatch)");

std::shared_ptr<ParameterLink<int>> BlockCatchWorld::worldXMaxPL = Parameters::register_parameter("WORLD_BLOCKCATCH-worldXMax", 20, "max width of world");
std::shared_ptr<ParameterLink<int>> BlockCatchWorld::worldXMinPL = Parameters::register_parameter("WORLD_BLOCKCATCH-worldXMin", 20, "min width of world");
std::shared_ptr<ParameterLink<int>> BlockCatchWorld::startYMaxPL = Parameters::register_parameter("WORLD_BLOCKCATCH-startYMax", 20, "highest height patterns can start");
std::shared_ptr<ParameterLink<int>> BlockCatchWorld::startYMinPL = Parameters::register_parameter("WORLD_BLOCKCATCH-startYMin", 20, "lowest height patterns can start");
std::shared_ptr<ParameterLink<std::string>> BlockCatchWorld::patternStartPositionsPL = Parameters::register_parameter("WORLD_BLOCKCATCH-patternStartPositions", (std::string) "ALL",
	"Where should the patters start (and if random, how many times should each pattern be tested)\n"
	"ALL = each pattern will be tested for each possible start position\n"
	"ALL_CLEAR = each pattern will be tested for each possible start position that does not overlap the paddle\n"
	"RANDOM_X = each pattern will be tested X times, starting from a random location (locations may repeat)\n"
	"RANDOM_CLEAR_X = each pattern will be tested X times, starting from a random location that does not overlap the paddle\n"
	"note: with either random method, locations may repeat)");


std::shared_ptr<ParameterLink<std::string>> BlockCatchWorld::groupNamePL = Parameters::register_parameter("WORLD_BLOCKCATCH_NAMES-groupName", (std::string)"root::", "name of group to be evaluated");
std::shared_ptr<ParameterLink<std::string>> BlockCatchWorld::brainNamePL = Parameters::register_parameter("WORLD_BLOCKCATCH_NAMES-brainName", (std::string)"root::", "name of brains used to control organisms");


void BlockCatchWorld::debugDisplay(int worldX, int time, std::vector<std::vector<int>> patternBuffer, int frameIndex, std::vector<int> sensorArray, std::vector<int> gapArray){
	for (int ii = 0; ii < worldX; ii++) { // the first line has the pattern
		if (patternBuffer[frameIndex][ii] == 1) {
			std::cout << "-"; // if pattern and visible
		}
		else if (patternBuffer[frameIndex][ii] == 2) {
			std::cout << "o"; // if pattern and invisible
		}
		else {
			bool inSensor = false;
			for (auto s : sensorArray) {
				if (s == ii) {
					inSensor = true;
				}
			}
			if (inSensor) {
				std::cout << "|"; // if not in pattern - and over sensor
			}
			else {
				std::cout << "."; // if not in pattern - and not over sensor
			}
		}
	} // end draw pattern
	std::cout << " " << time << std::endl; // print height of this line at end of line
	// done draw first line

	for (int tt = time - 1; tt >= 0; tt--) { // draw the remander of the lines
		for (int ii = 0; ii < worldX; ii++) {
			bool inSensor = false;
			for (auto s : sensorArray) {
				if (s == ii) {
					inSensor = true;
				}
			}
			if (inSensor) {
				std::cout << "|"; // if over a sensor
			}
			else {
				std::cout << "."; // if not over a sensor
			}
		}
		std::cout << " " << tt << std::endl; // print height of this line at end of line
	} // done draw remaining lines

	std::vector<int> paddleBuffer(worldX, 0); // buffer to convert sensorArray into an interable
	for (auto sensor : sensorArray) {
		paddleBuffer[sensor] = 1; // mark a sensor paddle location
	}
	for (auto gap : gapArray) {
		paddleBuffer[gap] = 2; // mark an non-sensor paddle location
	}

	for (int ii : paddleBuffer) { // interate over paddleBuffer to draw last line
		if (ii == 1) {
			std::cout << "^"; // if location is a sensor
		}
		else if (ii == 2) {
			std::cout << "*"; // if location is on paddle, but not a sensor
		}
		else {
			std::cout << " "; // if loction is not on paddle
		}
	}
	std::cout << std::endl;
}

void BlockCatchWorld::visualizeDisplay(bool catchPattern, int worldX, int startYMax, int endTime, int time, int patternIndex, int catchPatternsCount,
	int correct, int incorrect, std::vector<std::vector<int>> patternBuffer, int frameIndex,
	std::vector<int> sensorArray, std::vector<int> gapArray){
	// encode current time (needed to determine pattern altitide)
	std::string visualizeData = std::to_string(worldX) + "," + std::to_string(time) + ",";
	// encode if this pattern should be caught or missed
	if (catchPattern) {
		visualizeData += "catch,";
	}
	else {
		visualizeData += "miss,";
	}

	// add current correct and incorrect counts
	visualizeData += std::to_string(correct) + "," + std::to_string(incorrect) + "\n";

	// encode pattern
	for (int ii = 0; ii < worldX; ii++) {
		if (patternBuffer[frameIndex][ii] == 1) {
			visualizeData += std::to_string(ii) + ",";
		}
	}
	visualizeData += "\n";

	// encode pattern holes
	for (int ii = 0; ii < worldX; ii++) {
		if (patternBuffer[frameIndex][ii] == 2) {
			visualizeData += std::to_string(ii) + ",";
		}
	}
	visualizeData += "\n";

	// encode paddle sensors
	for (auto loc : sensorArray) {
		visualizeData += std::to_string(loc) + ",";
	}
	visualizeData += "\n";

	// encode paddle gap
	for (auto loc : gapArray) {
		visualizeData += std::to_string(loc) + ",";
	}

	// save visualization to file
	FileManager::writeToFile("CatchPassVisualize_" + std::to_string(Global::update) + ".txt", visualizeData);
}

void BlockCatchWorld::loadPatterns(int& patternCounter, std::vector<std::string> patterns) {
	for (auto pat : patterns) {
		int patternFrameCount = 0; // counter create index for frames in this pattern
		bool patternDone = false; // this will be set true when we hit a movement instruction
		for (auto c : pat) {
			if (c == ':') { // we are done with current frame, set up for next frame
				patternFrameCount++;
				allPatterns[patternCounter].push_back({}); // push back an empty vector
			}
			else if (c == '0' && patternDone == false) { // invisible pattern location
				allPatterns[patternCounter][patternFrameCount].push_back(2);
			}
			else if (c == '1' && patternDone == false) { // visible pattern location
				allPatterns[patternCounter][patternFrameCount].push_back(1);
			}
			else if (c == 'L') { // left movement
				patternDirections[patternCounter].push_back(-1);
				patternDone = true;
			}
			else if (c == 'R') { // right movement
				patternDirections[patternCounter].push_back(1);
				patternDone = true;
			}
			else if (c == 'H') { // hold (no movement)
				patternDirections[patternCounter].push_back(0);
				patternDone = true;
			}
			else if (c >= '0' && c <= '9') { // numbers after pattern has been encoded make movement "faster"
				int tempNumber;
				std::string s(1, c);
				convertString(s, tempNumber);
				if (std::abs(patternDirections[patternCounter].back()) == 1) { // the last char was a direction
					patternDirections[patternCounter].back() *= tempNumber;
				}
				else { // the last char was another number. current value needs to shift (in base 10) and add (or subtract) this number
					patternDirections[patternCounter].back() = (patternDirections[patternCounter].back() * 10);
					if (patternDirections[patternCounter].back() > 0) { // if the current number is left
						patternDirections[patternCounter].back() += tempNumber;
					}
					else if (patternDirections[patternCounter].back() < 0) { // if the current direction is right
						patternDirections[patternCounter].back() -= tempNumber;
					}
				}
			} // done resolve direction number
		} // done pattern
		patternCounter++; // advance pattern counter
	} // done convert patterns
}

BlockCatchWorld::BlockCatchWorld(std::shared_ptr<ParametersTable> _PT) : AbstractWorld(_PT) {
	groupName = groupNamePL->get(PT);
	brainName = brainNamePL->get(PT);

	worldXMax = worldXMaxPL->get(PT);
	worldXMin = worldXMinPL->get(PT);
	startYMax = startYMaxPL->get(PT);
	startYMin = startYMinPL->get(PT);

	visualizeBest = visualizeBestPL->get(PT);

	std::string scoreMethodStr = scoreMethodPL->get(PT);

	if (scoreMethodStr == "ANY_ANY") {
		scoreMethod = 0;
	}
	else if (scoreMethodStr == "VISIBLE_ANY") {
		scoreMethod = 1;
	}
	else if (scoreMethodStr == "VISIBLE_SENSOR") {
		scoreMethod = 2;
	}
	else if (scoreMethodStr == "SUM_ALL_ALL") {
		scoreMethod = 3;
	}
	else if (scoreMethodStr == "SUM_VISIBLE_SENSOR") {
		scoreMethod = 4;
	}
	else if (scoreMethodStr == "SUM_VISIBLE_NON_SENSOR") {
		scoreMethod = 5;
	}
	else {
		std::cout << "  While setting up BlockCatchWorld...  scoreMethod \"" << scoreMethodStr << "\" is not recognized.\n  exiting." << std::endl;
		exit(1);
	}

	/////////////////////////////////////////////////////////////////
	// load the paddle
	/////////////////////////////////////////////////////////////////
	std::string paddleStr = paddlePL->get(PT);
	std::cout << "\nSetting up BlockCatchWorld...\n  paddle : " << paddleStr << std::endl;

	// get width of paddle
	paddleWidth = paddleStr.size();

	for (auto c : paddleStr) {
		if (c == '0') {
			paddleShape.push_back(0);
		}
		else if (c == '1') {
			paddleShape.push_back(1);
			numberOfSensors++;
		}
		else {
			std::cout << "  While setting up BlockCatchWorld...  paddle parameter contains an illegual character \"" << c << "\".\n  exiting." << std::endl;
			exit(1);
		}
	}

	/////////////////////////////////////////////////////////////////
	// load catch and miss patterns
	/////////////////////////////////////////////////////////////////
	std::vector<std::string> catchPatterns;
	std::vector<std::string> missPatterns;
	convertCSVListToVector(catchPatternsPL->get(PT), catchPatterns, ',');
	convertCSVListToVector(missPatternsPL->get(PT), missPatterns, ',');

	patternsCount = catchPatterns.size() + missPatterns.size();
	catchPatternsCount = catchPatterns.size();

	std::cout << "  patterns :" << std::endl;
	for (auto cp : catchPatterns) {
		std::cout << "    catch: " << cp << std::endl;
	}
	for (auto mp : missPatterns) {
		std::cout << "    miss: " << mp << std::endl;
	}
	std::cout << std::endl;

	allPatterns.resize(patternsCount, { {} }); // push back a vector with an empty vector in it.
	patternDirections.resize(patternsCount);

	int patternCounter = 0; // counter creates index for allPatterns (both catch and miss patterns)

	// convert strings for catch patterns (including frames and movement)
	loadPatterns(patternCounter, catchPatterns);

	// convert strings for miss patterns (including frames and movement)
	loadPatterns(patternCounter, missPatterns);

	/////////////////////////////////////////////////////////////////
	// record patternSizes and check that all frames in all patterns are the same size
	/////////////////////////////////////////////////////////////////
	for (auto p : allPatterns) {
		patternSizes.push_back(p[0].size()); // remember the size of this pattern
		for (auto frame : p) { // make sure that all frames in this pattern are the same size
			if (frame.size() != p[0].size()) {
				std::cout << "  While setting up BlockCatchWorld... one or more patterns has frames of diffrnt sizes.\n  All frames in a pattern must have the same size.\n  exiting." << std::endl;
				exit(1);
			}
		}
	}
	

	///////////////////////////////////////////////
	// where will patterns start ?
	///////////////////////////////////////////////
	std::string patternStartPositionsStr = patternStartPositionsPL->get(PT);
	if (patternStartPositionsStr == "ALL") {
		patternStartPositions = 0;
		if (worldXMin != worldXMax) {
			std::cout << "  While setting up BlockCatchWorld... a non-random patternStartPosition is being used, but worldXMin does not equale worldXMax.\n  set worldXMin equal to worldXMax or use a diffrent patterStartPosition setting.\n  exiting." << std::endl;
			exit(1);
		}
		// larest block must be smaller then the width of the world!
		for (auto p : patternSizes) {
			if (p > worldXMax) {
				std::cout << "  While setting up BlockCatchWorld... a pattern of size " << p << " was set, which is larger then worldXMax.\n"
					"  please correct and try again... exiting." << std::endl;
				exit(1);
			}
		}
		repeats = worldXMax;
	}
	else if (patternStartPositionsStr == "ALL_CLEAR") {
		patternStartPositions = 1;
		if (worldXMin != worldXMax) {
			std::cout << "  While setting up BlockCatchWorld... a non-random patternStartPosition is being used, but worldXMin does not equale worldXMax.\n  set worldXMin equal to worldXMax or use a diffrent patterStartPosition setting.\n  exiting." << std::endl;
			exit(1);
		}
		for (auto p : patternSizes) {
			if (p > (worldXMin - paddleWidth)) {
				std::cout << "  While setting up BlockCatchWorld... a pattern of size " << p << " was set, and patternStartsPositions method it ALL_CLEAR.\n  patterns can not be larger then " << worldXMin - paddleWidth << " (worldXMin - paddleWidth).\n"
					"  please correct and try again... exiting." << std::endl;
				exit(1);
			}
		}
		// note, repeats will need to be set for each pattern
	}
	else if (patternStartPositionsStr.size() > 7 && patternStartPositionsStr.substr(0, 13) == "RANDOM_CLEAR_") {
		patternStartPositions = 3;
		auto testConvert = convertString(patternStartPositionsStr.substr(13, patternStartPositionsStr.size() - 13), repeats);
		if (!testConvert) {
			std::cout << "  While setting up BlockCatchWorld...  the parameter patternStartPositions \"" << patternStartPositionsStr << "\" has a tests number that can not be converted.\n  exiting." << std::endl;
			exit(1);
		}
		for (auto p : patternSizes) {
			if (p > (worldXMin - paddleWidth)) {
				std::cout << "  While setting up BlockCatchWorld... a pattern of size " << p << " was set, and patternStartsPositions method it RANDOM_CLEAR_X.\n  patterns can not be larger then " << worldXMin - paddleWidth << " (worldXMin - paddleWidth).\n"
					"  please correct and try again... exiting." << std::endl;
				exit(1);
			}
		}
	}
	else if (patternStartPositionsStr.size() > 7 && patternStartPositionsStr.substr(0, 7) == "RANDOM_") {
		patternStartPositions = 2;
		auto testConvert = convertString(patternStartPositionsStr.substr(7, patternStartPositionsStr.size() - 7), repeats);
		if (!testConvert) {
			std::cout << "  While setting up BlockCatchWorld...  the parameter patternStartPositions \"" << patternStartPositionsStr << "\" has a tests number that can not be converted.\n  exiting." << std::endl;
			exit(1);
		}
		for (auto p : patternSizes) {
			if (p > worldXMax) {
				std::cout << "  While setting up BlockCatchWorld... a pattern of size " << p << " was set, which is larger then worldXMax.\n"
					"  please correct and try again... exiting." << std::endl;
				exit(1);
			}
		}
	}
	else {
		std::cout << "  While setting up BlockCatchWorld...  the parameter patternStartPositions \"" << patternStartPositionsStr << "\" is not recognized.\n  exiting." << std::endl;
		exit(1);
	}

	/////////////////////////////////////////////////////////////////
	// columns to be added to pop file
	/////////////////////////////////////////////////////////////////
	popFileColumns.clear();
	popFileColumns.push_back("score");
	popFileColumns.push_back("correct");
	popFileColumns.push_back("incorrect");
	for (int i = 0; i < patternsCount; i++) {
		popFileColumns.push_back("correct_" + std::to_string(i));
		popFileColumns.push_back("incorrect_" + std::to_string(i));
	}
}



void BlockCatchWorld::evaluateSolo(std::shared_ptr<Organism> org, int analyse, int visualize, int debug) {
	int correct = 0; // total number of correct catches/misses
	int incorrect = 0; // total number of incorrect catches/misses
	std::vector<int> correctPer(patternsCount, 0); // total number of correct catches/misses per pattern
	std::vector<int> incorrectPer(patternsCount, 0); // total number of incorrect catches/misses per pattern

	if (visualize) { // save state of world before we get started.
		FileManager::writeToFile("CatchPassVisualize_"+std::to_string(Global::update)+".txt",
			"reset\n" +
			std::to_string(worldXMax) + "," + std::to_string(startYMax) + "," +
			std::to_string(Global::update) + "," + std::to_string(org->ID));
	}

	double fitness = 1.0;
	auto brain = org->brains[brainName];
	int action;
	for (int patternIndex = 0; patternIndex < patternsCount; patternIndex++) { // for patternIndex in number of patterns
		int directionCounter = 0;

		// determine number of tests for this pattern if patternStartPosiont is ALL_CLEAR
		if (patternStartPositions == 1){
			repeats = (worldXMax - (patternSizes[patternIndex] + paddleWidth)) + 1;
		}
		
		for (int repeat = 0; repeat < repeats; repeat++) {

			//get worldX and start height for pattern;
			int worldX = Random::getInt(worldXMin, worldXMax);
			int endTime = Random::getInt(startYMin, startYMax);

			// allocate space for all frames in this pattern
			std::vector<std::vector<int>> patternBuffer(allPatterns[patternIndex].size(), std::vector<int>(worldX, 0));

			// get paddle offset based on patterStartPosition parameter 
			int patternOffset;
			if (patternStartPositions == 0) { // ALL
				patternOffset = repeat;
			}
			else if (patternStartPositions == 1) { // ALL_CLEAR
				patternOffset = paddleWidth + repeat;
			}
			else if (patternStartPositions == 2) { // RANDOM
				patternOffset = Random::getIndex(worldX);
			}
			else if (patternStartPositions == 3) { // RANDOM_CLEAR
				patternOffset = Random::getInt(paddleWidth, worldX - patternSizes[patternIndex]);
			}
			// load pattern into buffer
			for (size_t frame = 0; frame < allPatterns[patternIndex].size(); frame++) { // for each frame in pattern
				for (int loc = 0; loc < patternSizes[patternIndex]; loc++) { // for each location
					patternBuffer[frame][loopMod(patternOffset + loc, worldX)] = allPatterns[patternIndex][frame][loc];
				}
			}

			std::vector<int> sensorArray; // locations for the paddle sensors relitive to paddleLoc
			std::vector<int> gapArray; // locations for the paddle sensors relitive to paddleLoc

			// load the paddle buffer and gap buffer with paddle at left
			for (size_t i = 0; i < paddleShape.size(); i++) {
				if (paddleShape[i] == 1) {
					sensorArray.push_back(loopMod(i, worldX));
				}
				else if (paddleShape[i] == 0) {
					gapArray.push_back(loopMod(i, worldX));
				}
			}

			int frameIndex = 0;

			if (1) { // perhaps add parameter for this
				brain->resetBrain();
			}

			if (visualize) { // save visualize data to file for inital world state
				visualizeDisplay(patternIndex < catchPatternsCount, worldX, startYMax, endTime, endTime,
					patternIndex, catchPatternsCount, correct, incorrect, patternBuffer, frameIndex,
					sensorArray, gapArray);
			} // end visualize

			for (int time = endTime; time >= 0; time--) { // run world for 34 updates with current pattern
				int brianInputIndex = 0; // used to iterate over brain inputs

				for (auto sensor : sensorArray) {
					brain->setInput(brianInputIndex++, (patternBuffer[frameIndex][sensor] == 1));
				}

				if (debug) { // if debug then draw world - only draw from the height of the pattern down to the paddle
					std::cout << "time :" << time << " << pattern: " << patternIndex << " << frame: " << frameIndex << std::endl;
					debugDisplay(worldX, time, patternBuffer, frameIndex, sensorArray, gapArray);
				} // done debug

				brain->update();

				action = Bit(brain->readOutput(0)) + (Bit(brain->readOutput(1)) << 1); // convert 2 bits of brain output to a value in range[0,3]
				
				switch (action) { // for cases 0 and 3 do nothing (i.e. 0,0 or 1,1)
				case 1: // left
					for (int i = 0; i < sensorArray.size(); i++) {
						sensorArray[i] = loopMod(sensorArray[i] - 1, worldX);
					}
					for (int i = 0; i < gapArray.size(); i++) {
						gapArray[i] = loopMod(gapArray[i] - 1, worldX);
					}
					break;
				case 2: // right
					for (int i = 0; i < sensorArray.size(); i++) {
						sensorArray[i] = (sensorArray[i] + 1) % worldX;
					}
					for (int i = 0; i < gapArray.size(); i++) {
						gapArray[i] = (gapArray[i] + 1) % worldX;
					}
				}

				// move all frames in patternBuffer
				if (patternDirections[patternIndex][directionCounter] < 0) { // if block is moving left
					for (size_t i = 0; i < patternBuffer.size(); i++) {
						std::rotate(patternBuffer[i].begin(), patternBuffer[i].begin() + loopMod(std::abs(patternDirections[patternIndex][directionCounter]),worldX), patternBuffer[i].end()); // shift block left. if anything gets cut off, shift it to the other side
					}
				}
				else if (patternDirections[patternIndex][directionCounter] > 0) { // block is moving right
					for (size_t i = 0; i < patternBuffer.size(); i++) {
						std::rotate(patternBuffer[i].begin(), patternBuffer[i].begin() + (patternBuffer[i].size() - loopMod(patternDirections[patternIndex][directionCounter],worldX)), patternBuffer[i].end()); // shift block left. if anything gets cut off, shift it to the other side
					}
				} // else no movement - do nothing

				directionCounter = (directionCounter + 1) % patternDirections[patternIndex].size(); // move to next direction in patternDirections list
				frameIndex = (frameIndex + 1) % patternBuffer.size(); // move to next frame in this pattern

				if (visualize) { // save visualize data to file
					visualizeDisplay(patternIndex < catchPatternsCount, worldX, startYMax, endTime, time,
						patternIndex, catchPatternsCount, correct, incorrect, patternBuffer, frameIndex,
						sensorArray, gapArray);
				} // end visualize

			} // end single pattern evaluation

			//check for hit
			int hit = 0;
			if (scoreMethod <= 2) { // scoreing method is not summing
				if (scoreMethod == 0) { // ANY_ANY - at least one element in the pattern (visible or invisible) contacts one sensor or non-sensor
					for (int sensorIndex = 0; sensorIndex < sensorArray.size() && hit == 0; sensorIndex++) { // for locaiton in paddle and while no hit has been detected
						if (patternBuffer[frameIndex][sensorArray[sensorIndex]] > 0) { // if location overlaps pattern
							hit = 1; // set hit true
						}
					}
					for (int gapIndex = 0; gapIndex < gapArray.size() && hit == 0; gapIndex++) { // for locaiton in paddle and while no hit has been detected
						if (patternBuffer[frameIndex][gapArray[gapIndex]] > 0) { // if location overlaps pattern
							hit = 1; // set hit true
						}
					}
				}
				else if (scoreMethod == 1) { // VISIBLE_ANY = at least one visible element in pattern contacts at least one sensor or non-sensor
					for (int sensorIndex = 0; sensorIndex < sensorArray.size() && hit == 0; sensorIndex++) { // for locaiton in paddle and while no hit has been detected
						if (patternBuffer[frameIndex][sensorArray[sensorIndex]] == 1) { // if location overlaps visible pattern
							hit = 1; // set hit true
						}
					}
					for (int gapIndex = 0; gapIndex < gapArray.size() && hit == 0; gapIndex++) { // for locaiton in paddle and while no hit has been detected
						if (patternBuffer[frameIndex][gapArray[gapIndex]] == 1) { // if location overlaps visible pattern
							hit = 1; // set hit true
						}
					}
				}
				else if (scoreMethod == 2) { // VISIBLE_SENSOR = at least one visible element of pattern contacts atlease one sensor
					for (int sensorIndex = 0; sensorIndex < sensorArray.size() && hit == 0; sensorIndex++) { // for locaiton in paddle and while no hit has been detected
						if (patternBuffer[frameIndex][sensorArray[sensorIndex]] == 1) { // if location overlaps pattern
							hit = 1; // set hit true
						}
					}
				}
				// now set correct and/or incorrect (for non-sum scoring methods)
				if (patternIndex < catchPatternsCount) { // if patternIndex is < catchPatternsCount we should be catching this
					if (hit) {
						correct++;
						correctPer[patternIndex]++;
					}
					else {
						incorrect++;
						incorrectPer[patternIndex]++;
					}
				}
				else { // this is in the set of patterns to miss
					if (hit) {
						incorrect++;
						incorrectPer[patternIndex]++;
					}
					else {
						correct++;
						correctPer[patternIndex]++;
					}
				}
			}
			else {
				if (scoreMethod == 3) { // SUM_ALL_ALL = for each pattern location (visible or invisible) that overlaps a sensor or non-sensor record a hit
					for (int sensorIndex = 0; sensorIndex < sensorArray.size(); sensorIndex++) { // for locaiton in paddle and while no hit has been detected
						if (patternBuffer[frameIndex][sensorArray[sensorIndex]] > 0) { // if location overlaps pattern
							hit++; // record a hit
						}
					}
					for (int gapIndex = 0; gapIndex < gapArray.size(); gapIndex++) { // for locaiton in paddle and while no hit has been detected
						if (patternBuffer[frameIndex][gapArray[gapIndex]] > 0) { // if location overlaps pattern
							hit++; // record a hit
						}
					}
				}
				else if (scoreMethod == 4) { // SUM_VISIBLE_SENSOR = for each visible pattern location that overlaps a sensor record a hit
					for (int sensorIndex = 0; sensorIndex < sensorArray.size(); sensorIndex++) { // for locaiton in paddle and while no hit has been detected
						if (patternBuffer[frameIndex][sensorArray[sensorIndex]] == 1) { // if location overlaps pattern
							hit++; // record a hit
						}
					}
				}
				else if (scoreMethod == 5) { // SUM_VISIBLE_NON_SENSOR = for each visible pattern location that overlaps a non-sensor record a hit
					for (int gapIndex = 0; gapIndex < gapArray.size(); gapIndex++) { // for locaiton in paddle and while no hit has been detected
						if (patternBuffer[frameIndex][gapArray[gapIndex]] == 1) { // if location overlaps pattern
							hit++; // record a hit
						}
					}
				}
				// now set correct and/or incorrect (for sum scoring methods)
				// in this mode, correct are accumulated for each hit on a to catch pattern
				// and incorrect are accumulated for each hit on a to miss pattern 
				if (patternIndex < catchPatternsCount) { // if patternIndex is < catchPatternsCount we should be catching this
					correct+= hit;
					correctPer[patternIndex] += hit;
				}
				else { // this is in the set of patterns to miss
					incorrect+= hit;
					incorrectPer[patternIndex]+= hit;
				}
			}
			// else condition (i.e. an undefined score method) is checked for in constructor


			if (debug) { // if debug then draw world
				std::cout << "hit : " << hit << "  score: " << std::pow(1.05, correct - incorrect) << std::endl;
			}
		} // end repeats
	} // end all patterns

	org->dataMap.set("score", std::pow(1.05,correct - incorrect));
	org->dataMap.set("correct", correct);
	org->dataMap.set("incorrect", incorrect);
	
	for (int i = 0; i < allPatterns.size(); i++) {
		org->dataMap.set("correct_" + std::to_string(i), correctPer[i]);
		org->dataMap.set("incorrect_" + std::to_string(i), incorrectPer[i]);
	}

	if (visualize) { // mark end of data... EOD
		FileManager::writeToFile("CatchPassVisualize_" + std::to_string(Global::update) + ".txt", "EOD");
	}

}

void BlockCatchWorld::evaluate(std::map<std::string, std::shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
	int popSize = groups[groupName]->population.size();
	for (int i = 0; i < popSize; i++) {
		evaluateSolo(groups[groupName]->population[i], analyse, visualize, debug);
	}

	if (visualizeBest > 0 && Global::update % visualizeBest == 0 && Global::update > 0) {
		// get best org (org with best score)
		double bestScore = groups[groupNamePL->get(PT)]->population[0]->dataMap.getAverage("score");
		auto bestOrg = groups[groupNamePL->get(PT)]->population[0];
		for (auto org : groups[groupNamePL->get(PT)]->population) {
			double orgScore = org->dataMap.getAverage("score");
			if (bestScore != orgScore) {
				if (bestScore < orgScore) {
					bestOrg = org;
					bestScore = orgScore;
				}
			}
		}
		std::cout << "  running visualization for org with ID: " << bestOrg->ID << " and score: " << bestScore << std::endl;
		auto testOrg = bestOrg->makeCopy(bestOrg->PT); // make a copy so we don't mess up the data map
		evaluateSolo(testOrg, analyse, 1, debug);
	}
}

std::unordered_map<std::string, std::unordered_set<std::string>> BlockCatchWorld::requiredGroups() {
  return { { groupNamePL->get(PT),{ "B:" + brainNamePL->get(PT) + ","+std::to_string(numberOfSensors)+",2"} } };
}

