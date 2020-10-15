//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "DecomposableGate.h"

std::shared_ptr<ParameterLink<std::string>> DecomposableGate::IO_RangesPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_DECOMPOSABLE-IO_Ranges", (std::string)"1-4,1-4", "range of number of inputs and outputs (min inputs-max inputs,min outputs-max outputs)");

DecomposableGate::DecomposableGate(std::pair<std::vector<int>, std::vector<int>> addresses, std::vector<std::vector<int>> rawTable, int _ID, std::vector<std::vector<double>> factorsListHandover, std::shared_ptr<ParametersTable> _PT) :
	AbstractGate(_PT) {
	ID = _ID;
	int i, j;
	inputs = addresses.first;
	outputs = addresses.second;

	int numInputs = inputs.size();
	int numOutputs = outputs.size();

	table.resize(1 << numInputs);
	//normalize each row
	for (i = 0; i < (1 << numInputs); i++) {  //for each row (each possible input bit string)
		table[i].resize(1 << numOutputs);
		// first sum the row
		double S = 0;
		for (j = 0; j < (1 << numOutputs); j++) {
			S += (double) rawTable[i][j];
		}
		// now normalize the row
		if (S == 0.0) {  //if all the inputs on this row are 0, then give them all a probability of 1/(2^(number of outputs))
			for (j = 0; j < (1 << numOutputs); j++)
				table[i][j] = 1.0 / (double) (1 << numOutputs);
		} else {  //otherwise divide all values in a row by the sum of the row
			for (j = 0; j < (1 << numOutputs); j++)
				table[i][j] = (double) rawTable[i][j] / S;
		}
	}

}

void DecomposableGate::update(std::vector<double> & nodes, std::vector<double> & nextNodes) {  //this translates the input bits of the current states to the output bits of the next states
	int input = vectorToBitToInt(nodes,inputs,true); // converts the input values into an index (true indicates to reverse order)
	int outputColumn = 0;
	double r = Random::getDouble(1);  // r will determine with set of outputs will be chosen
	while (r > table[input][outputColumn]) {
		r -= table[input][outputColumn];  // this goes across the probability table in row for the given input and subtracts each
		// value in the table from r until r is less than a value it reaches
		outputColumn++;  // we have not found the correct output so move to the next output
	}
	for (size_t i = 0; i < outputs.size(); i++)  //for each output...
		nextNodes[outputs[i]] += 1.0 * ((outputColumn >> (outputs.size() - 1 - i)) & 1);  // convert output (the column number) to bits and pack into next states
																						   // but always put the last bit in the first input (to maintain consistancy)
}

std::shared_ptr<AbstractGate> DecomposableGate::makeCopy(std::shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newGate = std::make_shared<DecomposableGate>(_PT);
	newGate->table = table;
	newGate->ID = ID;
	newGate->inputs = inputs;
	newGate->outputs = outputs;
        newGate->factorList=factorList;
	return newGate;
}
