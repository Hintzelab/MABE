//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "../ConstantValuesBrain/ConstantValuesBrain.h"

shared_ptr<ParameterLink<double>> ConstantValuesBrain::valueMinPL = Parameters::register_parameter("BRAIN_CONSTANT-valueMin", 0.0, "Minmum value that brain will deliver");
shared_ptr<ParameterLink<double>> ConstantValuesBrain::valueMaxPL = Parameters::register_parameter("BRAIN_CONSTANT-valueMax", 100.0, "Maximum value that brain will deliver");
shared_ptr<ParameterLink<int>> ConstantValuesBrain::valueTypePL = Parameters::register_parameter("BRAIN_CONSTANT-valueType", 0, "0 = int, 1 = double");
shared_ptr<ParameterLink<int>> ConstantValuesBrain::samplesPerValuePL = Parameters::register_parameter("BRAIN_CONSTANT-samplesPerValue", 1, "for each brain value, this many samples will be taken from genome and averaged");

shared_ptr<ParameterLink<bool>> ConstantValuesBrain::initializeUniformPL = Parameters::register_parameter("BRAIN_CONSTANT-initializeUniform", false, "Initialize genome randomly, with all samples having same value");
shared_ptr<ParameterLink<bool>> ConstantValuesBrain::initializeConstantPL = Parameters::register_parameter("BRAIN_CONSTANT-initializeConstant", false, "If true, all values in genome will be initialized to initial constant value.");
shared_ptr<ParameterLink<double>> ConstantValuesBrain::initializeConstantValuePL = Parameters::register_parameter("BRAIN_CONSTANT-initializeConstantValue", 0.0, "If initialized constant, this value is used to initialize entire genome.");

shared_ptr<ParameterLink<string>> ConstantValuesBrain::genomeNamePL = Parameters::register_parameter("BRAIN_CONSTANT_NAMES-genomeNameSpace", (string)"root::", "namespace used to set parameters for genome used to encode this brain");

ConstantValuesBrain::ConstantValuesBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT) :
		AbstractBrain(_nrInNodes, _nrOutNodes, _PT) {

// columns to be added to ave file
	popFileColumns.clear();
	for (int i = 0; i < nrOutputValues; i++) {
		popFileColumns.push_back("brainValue" + to_string(i));
	}
}

shared_ptr<AbstractBrain> ConstantValuesBrain::makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) {
	shared_ptr<ConstantValuesBrain> newBrain = make_shared<ConstantValuesBrain>(nrInputValues, nrOutputValues,PT);
	auto genomeHandler = _genomes[genomeNamePL->get(PT)]->newHandler(_genomes[genomeNamePL->get(PT)], true);
	auto samplesPerValue = samplesPerValuePL->get(PT);
	auto valueType = valueTypePL->get(PT);
	auto valueMin = valueMinPL->get(PT);
	auto valueMax = valueMaxPL->get(PT);

	double tempValue;

	for (int i = 0; i < nrOutputValues; i++) {
		tempValue = 0;
		for (int j = 0; j < samplesPerValue; j++) {
			if (valueType == 0) {
				tempValue += (double)genomeHandler->readInt((int)valueMin, (int)valueMax);
			} else if (valueType == 1) {
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
	for (int i = 0; i < nrOutputValues; i++) {
		dataMap.set(prefix + "brainValue" + to_string(i),outputValues[i]);
	}
	return (dataMap);
}

void ConstantValuesBrain::initializeGenomes(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) {

	auto samplesPerValue = samplesPerValuePL->get(PT);
	auto valueType = valueTypePL->get(PT);
	auto valueMin = valueMinPL->get(PT);
	auto valueMax = valueMaxPL->get(PT);
	auto initializeConstantValue = initializeConstantValuePL->get(PT);

	auto genomeName = genomeNamePL->get(PT);

	if (initializeConstantPL->get(PT)) {
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
				handler->writeDouble(initializeConstantValue, valueMin, valueMax);
			} else if (valueType == 0) {
				handler->writeInt((int)initializeConstantValue, (int)valueMin, (int)valueMax);
			}
		}
		//handler->resetHandler();
		//handler->writeInt(initializeConstantValue, (int)valueMin, (int)valueMax);
	} else if (initializeUniformPL->get(PT)) {
		auto handler = _genomes[genomeName]->newHandler(_genomes[genomeName]);
		int count = 0;
		double randomValue = 0;
		if (valueType == 1) {
			randomValue = Random::getDouble(valueMin, valueMax);
		} else if (valueType == 0) {
			randomValue = Random::getInt((int)valueMin, (int)valueMax);
		}
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
				handler->writeDouble(randomValue, valueMin, valueMax);
			} else if (valueType == 0) {
				handler->writeInt((int)randomValue, (int)valueMin, (int)valueMax);
			}
			count++;
		}
	} else {
		_genomes[genomeName]->fillRandom();
	}
}

shared_ptr<AbstractBrain> ConstantValuesBrain::makeCopy(shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newBrain = make_shared<ConstantValuesBrain>(nrInputValues, nrOutputValues, _PT);

	for (int i = 0; i < nrOutputValues; i++) {
		newBrain->outputValues[i] = outputValues[i];
	}

	return newBrain;
}