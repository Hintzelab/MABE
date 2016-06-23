//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "../ConstantValuesBrain/ConstantValuesBrain.h"

shared_ptr<ParameterLink<double>> ConstantValuesBrain::valueMinPL = Parameters::register_parameter("BRAIN_CONSTANT-valueMin", 0.0, "Minmum value that brain will deliver");
shared_ptr<ParameterLink<double>> ConstantValuesBrain::valueMaxPL = Parameters::register_parameter("BRAIN_CONSTANT-valueMax", 100.0, "Maximum value that brain will deliver");
shared_ptr<ParameterLink<int>> ConstantValuesBrain::valueTypePL = Parameters::register_parameter("BRAIN_CONSTANT-valueType", 0, "0 = int, 1 = double");
shared_ptr<ParameterLink<int>> ConstantValuesBrain::samplesPerValuePL = Parameters::register_parameter("BRAIN_CONSTANT-samplesPerValue", 1, "for each brain value, this many samples will be taken from genome and averaged");

shared_ptr<ParameterLink<bool>> ConstantValuesBrain::initializeUniformPL = Parameters::register_parameter("BRAIN_CONSTANT-initializeUniform", false, "Initialize genome randomly, with all samples having same value");
shared_ptr<ParameterLink<bool>> ConstantValuesBrain::initializeConstantPL = Parameters::register_parameter("BRAIN_CONSTANT-initializeConstant", false, "If true, all values in genome will be initialized to initial constant value.");
shared_ptr<ParameterLink<int>> ConstantValuesBrain::initializeConstantValuePL = Parameters::register_parameter("BRAIN_CONSTANT-initializeConstantValue", 0, "If initialized constant, this value is used to initialize entire genome.");

ConstantValuesBrain::ConstantValuesBrain(int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes, shared_ptr<ParametersTable> _PT) :
		AbstractBrain(_nrInNodes, _nrOutNodes, _nrHiddenNodes, _PT) {
	valueMin = (PT == nullptr) ? valueMinPL->lookup() : PT->lookupDouble("BRAIN_CONSTANT-valueMin");
	valueMax = (PT == nullptr) ? valueMaxPL->lookup() : PT->lookupDouble("BRAIN_CONSTANT-valueMax");
	valueType = (PT == nullptr) ? valueTypePL->lookup() : PT->lookupInt("BRAIN_CONSTANT-valueType");
	samplesPerValue = (PT == nullptr) ? samplesPerValuePL->lookup() : PT->lookupInt("BRAIN_CONSTANT-samplesPerValue");

	initializeUniform = (PT == nullptr) ? initializeUniformPL->lookup() : PT->lookupBool("BRAIN_CONSTANT-initializeUniform");
	initializeConstant = (PT == nullptr) ? initializeConstantPL->lookup() : PT->lookupBool("BRAIN_CONSTANT-initializeConstant");
	initializeConstantValue = (PT == nullptr) ? initializeConstantValuePL->lookup() : PT->lookupInt("BRAIN_CONSTANT-initializeConstantValue");

// columns to be added to ave file
	aveFileColumns.clear();
	for (int i = 0; i < nrOutNodes; i++) {
		aveFileColumns.push_back("brainValue" + to_string(i));
	}
}

shared_ptr<AbstractBrain> ConstantValuesBrain::makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) {
	shared_ptr<ConstantValuesBrain> newBrain = make_shared<ConstantValuesBrain>(nrInNodes, nrOutNodes, nrHiddenNodes);
	auto genomeHandler = _genome->newHandler(_genome, true);

	double tempValue;

	for (int i = 0; i < nrOutNodes; i++) {
		tempValue = 0;
		for (int j = 0; j < samplesPerValue; j++) {
			if (valueType == 0) {
				tempValue += genomeHandler->readInt(valueMin, valueMax);
			} else if (valueType == 1) {
				tempValue += genomeHandler->readDouble(valueMin, valueMax);
			} else {
				cout << "  ERROR! BRAIN_CONSTANT-valueType is invalid. current value: " << valueType << endl;
				exit(1);
			}
		}
		if (valueType == 0) {
			newBrain->nodes[outputNodesList[i]] = int(tempValue / samplesPerValue);
			newBrain->nextNodes[outputNodesList[i]] = int(tempValue / samplesPerValue);
		}
		if (valueType == 1) {
			newBrain->nodes[outputNodesList[i]] = tempValue / samplesPerValue;
			newBrain->nextNodes[outputNodesList[i]] = tempValue / samplesPerValue;
		}
	}
	return newBrain;
}

void ConstantValuesBrain::resetBrain() {
// do nothing! values never change!
}

void ConstantValuesBrain::update() {
// do nothing! output is already set!
}

void inline ConstantValuesBrain::resetOutputs() {
// do nothing! output is already set!
}

string ConstantValuesBrain::description() {
	string S = "Constant Values Brain\n";
	return S;
}

vector<string> ConstantValuesBrain::getStats() {
	vector<string> dataPairs;
//	dataPairs.push_back("outputValues");
//	string valuesList = "\"[";
//	for (int i = 0; i < nrOutNodes; i++) {
//		valuesList += to_string(nextNodes[outputNodesList[i]]) + ",";
//	}
//	valuesList.pop_back();
//	valuesList += "]\"";
//
//	dataPairs.push_back(to_string(valuesList));

	for (int i = 0; i < nrOutNodes; i++) {
		dataPairs.push_back("brainValue" + to_string(i));
		dataPairs.push_back(to_string(nextNodes[outputNodesList[i]]));
	}

	return (dataPairs);
}

void ConstantValuesBrain::initalizeGenome(shared_ptr<AbstractGenome> _genome) {
	if (initializeConstant) {
		if (initializeConstantValue < valueMin) {
			cout << "ERROR: initializeConstantValue must be greater then or equal to valueMin" << endl;
			exit(1);
		}
		if (initializeConstantValue > valueMax) {
			cout << "ERROR: initializeConstantValue must be less then or equal to valueMax" << endl;
			exit(1);
		}
		auto handler = _genome->newHandler(_genome);
		while (!handler->atEOG()) {
			if (valueType == 1) {
				handler->writeInt(initializeConstantValue, valueMin, valueMax);
			} else if (valueType == 0) {
				handler->writeInt(initializeConstantValue, valueMin, valueMax);
			}
		}
		handler->resetHandler();
		handler->writeInt(initializeConstantValue, valueMin, valueMax);
	} else if (initializeUniform) {
		auto handler = _genome->newHandler(_genome);
		int count = 0;
		double randomValue;
		if (valueType == 1) {
			randomValue = Random::getDouble(valueMin, valueMax);
		} else if (valueType == 0) {
			randomValue = Random::getInt(valueMin, valueMax);
		}
		if (count == samplesPerValue) {
			if (valueType == 1) {
				randomValue = Random::getDouble(valueMin, valueMax);
			} else if (valueType == 0) {
				randomValue = Random::getInt(valueMin, valueMax);
			}
		}
		while (!handler->atEOG()) {
			if (count == samplesPerValue) {
				count = 0;
				if (valueType == 1) {
					randomValue = Random::getDouble(valueMin, valueMax);
				} else if (valueType == 0) {
					randomValue = Random::getInt(valueMin, valueMax);
				}
			}
			if (valueType == 1) {
				handler->writeInt(randomValue, valueMin, valueMax);
			} else if (valueType == 0) {
				handler->writeInt(randomValue, valueMin, valueMax);
			}
			count++;
		}
	} else {
		_genome->fillRandom();
	}
}

