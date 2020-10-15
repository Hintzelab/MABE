//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "DecomposableDirectGate.h"

std::shared_ptr<ParameterLink<std::string>> DecomposableDirectGate::IO_RangesPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_DECOMPOSABLE_DIRECT-IO_Ranges", (std::string)"1-4,1-4", "range of number of inputs and outputs (min inputs-max inputs,min outputs-max outputs)");

DecomposableDirectGate::DecomposableDirectGate(std::pair<std::vector<int>, std::vector<int>> addresses, std::vector<std::vector<double>> newFactorsList, int _ID, std::shared_ptr<ParametersTable> _PT) :
	AbstractGate(_PT) {
	ID = _ID;
	int i, j;
	inputs = addresses.first;
	outputs = addresses.second;

	int numInputs = inputs.size();
	int numOutputs = outputs.size();

	factorsList.resize(newFactorsList.size());
	for (int rowi(0); rowi < newFactorsList.size(); rowi++) {  //for each row (each possible input bit string)
    //factorsList[rowi].resize(newFactorsList[0].size());
		factorsList[rowi] = newFactorsList[rowi];
	}
}

void DecomposableDirectGate::update(std::vector<double> & nodes, std::vector<double> & nextNodes) {  //this translates the input bits of the current states to the output bits of the next states
	int input = vectorToBitToInt(nodes,inputs,true); // converts the input values into an index (true indicates to reverse order)
  // each output 'wire' has an associated factor (probability) of firing. Test each one, and fire each ouput as necessary
  double r;
	for (size_t outputi = 0; outputi < outputs.size(); outputi++) { //for each output...
    r = Random::getDouble(1);
    if (Random::getDouble(1) >= factorsList[input][outputi]) nextNodes[outputs[outputi]] += 1.0; // sum the outputs
  }
}

std::shared_ptr<AbstractGate> DecomposableDirectGate::makeCopy(std::shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newGate = std::make_shared<DecomposableDirectGate>(_PT);
	newGate->factorsList = factorsList;
	newGate->ID = ID;
	newGate->inputs = inputs;
	newGate->outputs = outputs;
	return newGate;
}
