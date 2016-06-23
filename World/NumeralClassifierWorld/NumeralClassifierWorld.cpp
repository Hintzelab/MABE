//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "../../Utilities/Utilities.h"
#include "NumeralClassifierWorld.h"

shared_ptr<ParameterLink<int>> NumeralClassifierWorld::defaulttestsPreWorldEvalPL = Parameters::register_parameter("WORLD_NUMERALCLASSIFIER-testsPreWorldEval", 5, "number of values each brain attempts to evaluate in a world evaluation");
shared_ptr<ParameterLink<int>> NumeralClassifierWorld::defaultWorldUpdatesPL = Parameters::register_parameter("WORLD_NUMERALCLASSIFIER-WorldUpdates", 100, "number of world updates brain has to evaluate each value");
shared_ptr<ParameterLink<int>> NumeralClassifierWorld::defaultRetinaTypePL = Parameters::register_parameter("WORLD_NUMERALCLASSIFIER-retinaType", 3, "1 = center only, 2 = 3 across, 3 = 3x3, 4 = 5x5, 5 = 7x7");
shared_ptr<ParameterLink<string>> NumeralClassifierWorld::numeralDataFileNamePL = Parameters::register_parameter("WORLD_NUMERALCLASSIFIER-dataFileName", (string) "World/NumeralClassifierWorld/mnist.train.discrete.28x28-only100", "name of file with numeral data");

NumeralClassifierWorld::NumeralClassifierWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {
	worldUpdates = (PT == nullptr) ? defaultWorldUpdatesPL->lookup() : PT->lookupInt("WORLD_NUMERALCLASSIFIER-WorldUpdates");
	testsPreWorldEval = (PT == nullptr) ? defaulttestsPreWorldEvalPL->lookup() : PT->lookupInt("WORLD_NUMERALCLASSIFIER-testsPreWorldEval");
	retinaType = (PT == nullptr) ? defaultRetinaTypePL->lookup() : PT->lookupInt("WORLD_NUMERALCLASSIFIER-retinaType");  //1 = center only, 2 = 3 across, 3 = 3x3, 4 = 5x5, 5 = 7x7
	numeralDataFileName = (PT == nullptr) ? numeralDataFileNamePL->lookup() : PT->lookupString("WORLD_NUMERALCLASSIFIER-dataFileName");

	outputNodesCount = 13;  // moveX(1), moveY(1), 0->9(10), done(1)
	switch (retinaType) {
	case 1:
		retinaSensors = 1;
		stepSize = 1;
		break;
	case 2:
		retinaSensors = 3;
		stepSize = 1;
		break;
	case 3:
		retinaSensors = 9;
		stepSize = 3;
		break;
	case 4:
		retinaSensors = 25;
		stepSize = 5;
		break;
	case 5:
		retinaSensors = 49;
		stepSize = 7;
		break;
	default:
		cout << "\n\nIn NumeralClassifierWorld constructor, undefined retinaType \"" << retinaType << "\".\n\nExiting!\n" << endl;
		exit(1);
	}
	inputNodesCount = retinaSensors + 4;  // 3 x 3 retina + up + down + left + right

	cout << "NumeralClassifierWorld requires brains with at least " << inputNodesCount + outputNodesCount << " nodes.\n";
	if (inputNodesCount == 0) {
		cout << "    " << inputNodesCount << " Inputs\t No Inputs\n";
		cout << "    " << outputNodesCount << " Outputs\t nodes 0 to " << outputNodesCount - 1 << "\n";
	} else {
		cout << "    " << inputNodesCount << " Inputs\t nodes 0 to " << inputNodesCount - 1 << "\n";
		cout << "    " << outputNodesCount << " Outputs\t nodes " << inputNodesCount << " to " << inputNodesCount + outputNodesCount - 1 << "\n";
	}

	// LOAD NUMBERS

	numeralData.resize(10);

	string fileName = numeralDataFileName;
	ifstream FILE(fileName);
	string rawLine;
	int readInt;
	bool readBit;
	if (FILE.is_open()) {  // if the file named by configFileName can be opened
		while (getline(FILE, rawLine)) {  // keep loading one line from the file at a time into "line" until we get to the end of the file
			std::stringstream ss(rawLine);
			ss >> readInt;  // pull number from line (this identifies what number is in the image)
			for (int i = 0; i < 28; i++) {  //data is 28x28 pixels
				getline(FILE, rawLine);  // get next line
				std::stringstream ss(rawLine);
				for (int j = 0; j < 28; j++) {
					ss >> readBit;  // read one bit
					numeralData[readInt].push_back(readBit);
				}
			}
			getline(FILE, rawLine);  // get past blank line (after each number in file)
		}
	} else {
		cout << "\n\nERROR: NumeralClassifierWorld constructor, unable to open file \"" << fileName << "\"\n\nExiting\n" << endl;
		exit(1);
	}

	cout << numeralData.size() << endl;
	bool printNumbers = false;
	for (int i = 0; i < 10; i++) {
		cout << "  " << i << " : " << numeralData[i].size() / (28 * 28) << endl;
		if (printNumbers) {
			for (int index = 0; index < ((int) numeralData[i].size()) / (28 * 28); index++) {
				for (int r = 0; r < 28; r++) {  //data is 28x28 pixels
					for (int c = 0; c < 28; c++) {
						cout << numeralData[i][(index * 28 * 28) + (r * 28) + c];
					}
					cout << endl;
				}
			}
		}
	}

//	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("score");

	for (int i = 0; i < 10; i++) {
		aveFileColumns.push_back("correct" + to_string(i));
		aveFileColumns.push_back("incorrect" + to_string(i));
	}
	aveFileColumns.push_back("totalCorrect");
	aveFileColumns.push_back("totalIncorrect");
}

void NumeralClassifierWorld::runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug) {
	// numeralClassifierWorld assumes there will only ever be one agent being tested at a time. It uses org by default.
	double score = 0.0;
	int currentX, currentY;  // = { Random::getIndex(28), Random::getIndex(28) };  // place organism somewhere in the world

	// set up vars needed to run
	int numeralPick;  // number being tested
	int whichNumeral;  // which particular number from the set is being tested

	vector<int> correct;  // for each number (0 to 9) number of times organism is correct (at most # of evaluations)
	vector<int> incorrect;  // for each number (0 to 9) number of times organism is incorrect (at most 9 * # evaluations)
	vector<int> counts;  // how many times each number shows up.

	correct.resize(10);
	incorrect.resize(10);

	counts.resize(10);

	int nodesAssignmentCounter;  // this world can has number of brainState inputs set by parameter. This counter is used while assigning inputs
	// make sure the brain does not have values from last run
	org->brain->resetBrain();
	for (int test = 0; test < testsPreWorldEval; test++) {  //run agent for "worldUpdates" brain updates
		numeralPick = Random::getIndex(10);  // pick a number
		counts[numeralPick]++;
		whichNumeral = Random::getIndex(numeralData[numeralPick].size() / (28 * 28));
		currentX = Random::getIndex(28);  // place organism somewhere in the world
		currentY = Random::getIndex(28);  // place organism somewhere in the world

		for (int worldUpdate = 0; worldUpdate < worldUpdates; worldUpdate++) {

			nodesAssignmentCounter = 0;  // get ready to start assigning inputs
			//set retina nodes
			for (int i = 0; i < retinaSensors; i++) {  // fill first nodes with food values at here location
				int checkX = currentX + retinalOffsets[i].first;
				int checkY = currentY + retinalOffsets[i].second;
				if (checkX >= 0 && checkX < 28 && checkY >= 0 && checkY < 28) {  // if we are on the image
					org->brain->setInput(nodesAssignmentCounter++, numeralData[numeralPick][(whichNumeral * 28 * 28) + (checkY * 28) + (checkX)]);
				} else {  //if we are not on the number, assign 0
					org->brain->setInput(nodesAssignmentCounter++, 0);
				}
			}
			//set edge nodes
			//up and down
			int rayX;
			int rayY;
			if (currentX < 0 || currentX >= 28) {  // we are off the grid in x
				org->brain->setInput(nodesAssignmentCounter++, -1);  // up sensor
				org->brain->setInput(nodesAssignmentCounter++, -1);  // down sensor
			} else {  // cast up and down rays
				// up first
				int foundBlackUp = -1;
				int distance = 0;
				rayX = currentX;
				rayY = currentY;

				while (rayY >= 0 && foundBlackUp == -1) {
					//cout << "up ray@: " << rayX << " " << rayY << endl;
					if (rayY < 28 && (numeralData[numeralPick][(whichNumeral * 28 * 28) + (rayY * 28) + (rayX)] == 1)) {
						foundBlackUp = distance;
					}
					rayY--;
					distance++;
				}

				// down
				rayX = currentX;  // reset ray
				rayY = currentY;
				int foundBlackDown = -1;
				distance = 0;
				while (rayY < 28 && foundBlackDown == -1) {
					//cout << "down ray@: " << rayX << " " << rayY << endl;
					if (rayY >= 0 && (numeralData[numeralPick][(whichNumeral * 28 * 28) + (rayY * 28) + (rayX)] == 1)) {
						foundBlackDown = distance;
					}
					rayY++;
					distance++;
				}
				org->brain->setInput(nodesAssignmentCounter++, foundBlackUp);  // up sensor
				org->brain->setInput(nodesAssignmentCounter++, foundBlackDown);  // down sensor

			}

			//left and right
			if (currentY < 0 || currentY >= 28) {  // we are off the grid in y
				org->brain->setInput(nodesAssignmentCounter++, -1);  // left sensor
				org->brain->setInput(nodesAssignmentCounter++, -1);  // right sensor
			} else {  // cast rays
				// left first
				rayX = currentX;
				rayY = currentY;
				int foundBlackLeft = -1;
				int distance = 0;
				while (rayX >= 0 && foundBlackLeft == -1) {
					if (rayX < 28 && (numeralData[numeralPick][(whichNumeral * 28 * 28) + (rayY * 28) + (rayX)] == 1)) {
						foundBlackLeft = distance;
					}
					rayX--;
					distance++;
				}

				// right
				rayX = currentX;
				rayY = currentY;
				int foundBlackRight = -1;
				distance = 0;
				while (rayX < 28 && foundBlackRight == -1) {
					if (rayX >= 0 && (numeralData[numeralPick][(whichNumeral * 28 * 28) + (rayY * 28) + (rayX)] == 1)) {
						foundBlackRight = distance;
					}
					rayX++;
					distance++;
				}
				org->brain->setInput(nodesAssignmentCounter++, foundBlackLeft);  // left sensor
				org->brain->setInput(nodesAssignmentCounter++, foundBlackRight);  // right sensor
			}

//		if (clearOutputs) {
//			org->brain->resetOutputs();
//		}

			if (debug) {
				cout << "\n----------------------------\n";
				cout << "\ngeneration update: " << Global::update << "  world update: " << worldUpdate << "\n";
				cout << "currentLocation: " << currentX << "," << currentY << "\n";
				cout << "inNodes: ";
				for (int i = 0; i < inputNodesCount; i++) {
					cout << org->brain->readInput(i) << " ";
				}
				cout << "\nlast outNodes: ";
				for (int i = 0; i < outputNodesCount; i++) {
					cout << org->brain->readOutput(i) << " ";
				}
				cout << "\n\n  -- brain update --\n\n";
			}

			org->brain->update();  // just run the update!

			// move organism

			currentX += Trit(org->brain->readOutput(0)) * stepSize;  // left and right
			currentY += Trit(org->brain->readOutput(1)) * stepSize;  // up and down

			if (debug) {
				cout << "outNodes: ";
				for (int i = 0; i < outputNodesCount; i++) {
					cout << org->brain->readOutput(i) << " ";
				}
				cout << "\n  -- world update --\n\n";
				/////////////////////////////////////////////
				///////////////////
				cout << "numeralPick: " << numeralPick << "   whichNumeral " << whichNumeral << endl;
				vector<vector<int>> image;
				image.resize(28);
				for (int i = 0; i < 28; i++) {
					image[i].resize(28);
				}
				int index = whichNumeral * 28 * 28;
				for (int r = 0; r < 28; r++) {  //data is 28x28 pixels
					for (int c = 0; c < 28; c++) {
						image[r][c] = numeralData[numeralPick][index + (r * 28) + c];
						//cout << numeralData[numeralPick][index + (r * 28) + c];
					}
				}
				for (int i = 0; i < retinaSensors; i++) {  // fill first nodes with food values at here location
					int checkX = currentX + retinalOffsets[i].first;
					int checkY = currentY + retinalOffsets[i].second;
					if (checkX >= 0 && checkX < 28 && checkY >= 0 && checkY < 28) {  // if we are on the image
						image[checkY][checkX] = image[checkY][checkX] + 2;
					} else {  //if we are not on the number, do nothing

					}
					/////////////////////////////////////////////
					///////////////////
				}
				for (int r = 0; r < 28; r++) {  //data is 28x28 pixels
					for (int c = 0; c < 28; c++) {
						cout << image[r][c];
					}
					cout << endl;
				}
				cout << endl;
				cout << "loc: " << currentX << "," << currentY << "\n";

			}  // end of show loop
		}  // end of test loop
		for (int i = 0; i < 10; i++) {
			if (numeralPick == i && org->brain->readOutput(2 + i) > 0) {
				correct[i]++;
			}
			if (numeralPick != i && org->brain->readOutput(2 + i) > 0) {
				incorrect[i]++;
			}
		}
	}  // end of world loop

	for (int i = 0; i < 10; i++) {
		score += (double) correct[i];
		score -= ((double) incorrect[i]) / 10.0;
	}

	if (score < 0.0) {
		score = 0.0;
	}

	int total_correct = 0;
	int total_incorrect = 0;
	string temp_name;
	for (int i = 0; i < 10; i++) {
		total_correct += correct[i];
		total_incorrect += incorrect[i];

		temp_name = "allcorrect" + to_string(i);  // make food names i.e. food1, food2, etc.
		double val;
		(counts[i] > 0) ? val = (double) correct[i] / (double) counts[i] : val = 0;
		org->dataMap.Append(temp_name, val);
		temp_name = "allincorrect" + to_string(i);  // make food names i.e. food1, food2, etc.
		(counts[i] < testsPreWorldEval) ? val = (double) incorrect[i] / ((double) testsPreWorldEval - counts[i]) : val = 0;
		org->dataMap.Append(temp_name, val);
	}

	org->dataMap.Append("alltotalCorrect", total_correct);  // total food eaten (regardless of type)
	org->dataMap.Append("alltotalIncorrect", total_incorrect);  // total food eaten (regardless of type)

	if (score < 0.0) {
		score = 0.0;
	}
	org->score = score;
	org->dataMap.Append("allscore", score);
}
