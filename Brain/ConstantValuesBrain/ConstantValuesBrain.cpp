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

shared_ptr<ParameterLink<string>> ConstantValuesBrain::genomeNamePL = Parameters::register_parameter("BRAIN_CONSTANT_NAMES-genomeName", (string)"root", "name of genome used to encode this brain\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");

ConstantValuesBrain::ConstantValuesBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT) :
		AbstractBrain(_nrInNodes, _nrOutNodes, _PT) {
	valueMin = (PT == nullptr) ? valueMinPL->lookup() : PT->lookupDouble("BRAIN_CONSTANT-valueMin");
	valueMax = (PT == nullptr) ? valueMaxPL->lookup() : PT->lookupDouble("BRAIN_CONSTANT-valueMax");
	valueType = (PT == nullptr) ? valueTypePL->lookup() : PT->lookupInt("BRAIN_CONSTANT-valueType");
	samplesPerValue = (PT == nullptr) ? samplesPerValuePL->lookup() : PT->lookupInt("BRAIN_CONSTANT-samplesPerValue");

	initializeUniform = (PT == nullptr) ? initializeUniformPL->lookup() : PT->lookupBool("BRAIN_CONSTANT-initializeUniform");
	initializeConstant = (PT == nullptr) ? initializeConstantPL->lookup() : PT->lookupBool("BRAIN_CONSTANT-initializeConstant");
	initializeConstantValue = (PT == nullptr) ? initializeConstantValuePL->lookup() : PT->lookupInt("BRAIN_CONSTANT-initializeConstantValue");

	genomeName = (PT == nullptr) ? genomeNamePL->lookup() : PT->lookupString("BRAIN_CONSTANT_NAMES-genomeName");

// columns to be added to ave file
	popFileColumns.clear();
	for (int i = 0; i < nrOutputValues; i++) {
		popFileColumns.push_back("brainValue" + to_string(i));
	}
}

shared_ptr<AbstractBrain> ConstantValuesBrain::makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) {
	shared_ptr<ConstantValuesBrain> newBrain = make_shared<ConstantValuesBrain>(nrInputValues, nrOutputValues);
	auto genomeHandler = _genomes[genomeName]->newHandler(_genomes[genomeName], true);

	double tempValue;

	for (int i = 0; i < nrOutputValues; i++) {
		tempValue = 0;
		for (int j = 0; j < samplesPerValue; j++) {
			if (valueType == 0) {
				tempValue += (double)genomeHandler->readInt(valueMin, valueMax);
			} else if (valueType == 1.0) {
				tempValue += (double)genomeHandler->readDouble(valueMin, valueMax);
			} else {
				cout << "  ERROR! BRAIN_CONSTANT-valueType is invalid. current value: " << valueType << endl;
				exit(1);
			}
		}
		if (valueType == 0) {
			newBrain->outputValues[i] = int(tempValue / samplesPerValue);
		}
		if (valueType == 1) {
			newBrain->outputValues[i] = tempValue / samplesPerValue;
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

DataMap ConstantValuesBrain::getStats(string& prefix) {
	DataMap dataMap;
//	dataPairs.push_back("outputValues");
//	string valuesList = "\"[";
//	for (int i = 0; i < nrOutNodes; i++) {
//		valuesList += to_string(nextNodes[outputNodesList[i]]) + ",";
//	}
//	valuesList.pop_back();
//	valuesList += "]\"";
//
//	dataPairs.push_back(to_string(valuesList));

	for (int i = 0; i < nrOutputValues; i++) {
		dataMap.Set(prefix + "brainValue" + to_string(i),outputValues[i]);
	}
	return (dataMap);
}

void ConstantValuesBrain::initalizeGenomes(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) {
	if (initializeConstant) {
		if (initializeConstantValue < valueMin) {
			cout << "ERROR: initializeConstantValue must be greater then or equal to valueMin" << endl;
			exit(1);
		}
		if (initializeConstantValue > valueMax) {
			cout << "ERROR: initializeConstantValue must be less then or equal to valueMax" << endl;
			exit(1);
		}
		auto handler = _genomes[genomeName]->newHandler(_genomes[genomeName]);
		while (!handler->atEOG()) {
			if (valueType == 1) {
				handler->writeInt(initializeConstantValue, (int)valueMin, (int)valueMax);
			} else if (valueType == 0) {
				handler->writeInt(initializeConstantValue, (int)valueMin, (int)valueMax);
			}
		}
		handler->resetHandler();
		handler->writeInt(initializeConstantValue, (int)valueMin, (int)valueMax);
	} else if (initializeUniform) {
		auto handler = _genomes[genomeName]->newHandler(_genomes[genomeName]);
		int count = 0;
		double randomValue = 0;
		if (valueType == 1) {
			randomValue = Random::getDouble(valueMin, valueMax);
		} else if (valueType == 0) {
			randomValue = Random::getInt((int)valueMin, (int)valueMax);
		}
//		if (count == samplesPerValue) {
//			if (valueType == 1) {
//				randomValue = Random::getDouble(valueMin, valueMax);
//			} else if (valueType == 0) {
//				randomValue = Random::getInt(valueMin, valueMax);
//			}
//		}
		while (!handler->atEOG()) {
			if (count == samplesPerValue) {
				count = 0;
				if (valueType == 1) {
					randomValue = Random::getDouble(valueMin, valueMax);
				} else if (valueType == 0) {
					randomValue = Random::getInt((int)valueMin, (int)valueMax);
				}
			}
			if (valueType == 1) {
				handler->writeInt((int)randomValue, (int)valueMin, (int)valueMax);
			} else if (valueType == 0) {
				handler->writeInt((int)randomValue, (int)valueMin, (int)valueMax);
			}
			count++;
		}
	} else {
		_genomes[genomeName]->fillRandom();
	}
}

