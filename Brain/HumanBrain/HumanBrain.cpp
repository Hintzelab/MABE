//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "HumanBrain.h"

shared_ptr<ParameterLink<bool>> HumanBrain::useActionMapPL = Parameters::register_parameter("BRAIN_HUMAN-useActionMap", false, "if true, an action map will be used to translate user input");
shared_ptr<ParameterLink<string>> HumanBrain::actionMapFileNamePL = Parameters::register_parameter("BRAIN_HUMAN-actionMapFileName", (string) "actionMap.txt", "if useActionMap = true, use this file");

HumanBrain::HumanBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT) :
		AbstractBrain(_nrInNodes, _nrOutNodes, _PT) {
	useActionMap = useActionMapPL->get(PT);
	actionMapFileName = actionMapFileNamePL->get(PT);


	if (useActionMap) {  // if using an action map, load map with lines of format char output1 output2 output3... file must match brain # of outputs
		string fileName = actionMapFileName;
		ifstream FILE(fileName);
		string rawLine;
		double readDouble;
		char readChar;
		vector<double> action;
		if (FILE.is_open()) {  // if the file named by actionMapFileName can be opened
			while (getline(FILE, rawLine)) {  // keep loading one line from the file at a time into "line" until we get to the end of the file
				std::stringstream ss(rawLine);
				ss >> readChar;  // pull one char, this will be the key to the map
				action.clear();
				for (int i = 0; i < nrOutputValues; i++) {  // pull one double for each output
					ss >> readDouble;  // read one double
					action.push_back(readDouble);
				}
				ss >> actionNames[readChar];
				actionMap[readChar] = action;
			}
		} else {
			cout << "\n\nERROR:HumanBrain constructor, unable to open file \"" << fileName << "\"\n\nExiting\n" << endl;
			exit(1);
		}
	}

// columns to be added to ave file
	popFileColumns.clear();
}

shared_ptr<AbstractBrain> HumanBrain::makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) {
	shared_ptr<HumanBrain> newBrain = make_shared<HumanBrain>(nrInputValues, nrOutputValues);
	return newBrain;
}

void HumanBrain::resetBrain() {
	AbstractBrain::resetBrain();
}

void HumanBrain::update() {
	cout << "Inputs: ";
	for (int i = 0; i < nrInputValues; i++) {
		cout << inputValues[i] << " ";
	}
	cout << "\nLast Outputs: ";
	for (int i = 0; i < nrOutputValues; i++) {
		cout << outputValues[i] << " ";
	}
	cout << endl;

	outputValues.assign(nrOutputValues, 0.0);
	char key;
	if (useActionMap) {
		cout << "please enter action (* for options): ";
		cin >> key;
		while (actionMap.find(key) == actionMap.end()) {
			if (key == '*') {
				cout << "actions:" << endl;
				for (auto iter = actionMap.begin(); iter != actionMap.end(); iter++) {
					cout << iter->first << " " << actionNames[iter->first] << " (";
					for (size_t index = 0; index < iter->second.size(); index++) {
						cout << iter->second[index] << " ";
					}
					cout << ")" << endl;
				}
			} else {
				cout << "action not found." << endl;
			}
			cout << "please enter action: ";
			cin >> key;
		}
		// if we get here, we have a good key, move it into outputs
		for (int i = 0; i < nrOutputValues; i++) {  // pull one double for each output
			outputValues[i] = actionMap[key][i];
		}
	} else {  // not using action map
		cout << "please enter outputs (separated by space): ";
		string inputString;
		getline(cin, inputString);
		stringstream ss(inputString);
		for (int i = 0; i < nrOutputValues; i++) {  // pull one double for each output
			ss >> outputValues[i];
		}
	}
}

string HumanBrain::description() {
	string S = "Human Brain\n";
	return S;
}

DataMap HumanBrain::getStats(string& prefix) {
	DataMap dataMap;
	return (dataMap);
}

void HumanBrain::initializeGenomes(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) {
// do nothing;
}

shared_ptr<AbstractBrain> HumanBrain::makeCopy(shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newBrain = make_shared<HumanBrain>(nrInputValues, nrOutputValues, _PT);
	return newBrain;
}


