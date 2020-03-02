//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "VoidGate.h"

/* this gate behaves like a deterministic gate with a constant externally set error which may set a single output to 0 */

shared_ptr<ParameterLink<double>> VoidGate::voidGate_ProbabilityPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_VOID-epsilonSource", 0.05, "if value is in [0,1], chance that output will be randomized. if < 1 or < 0, epsilon value is determined by the genome. If positive, then the genome handler will advance int(value) number of sites - 1 from the current location, before reading. If the value is negative, then int(abs(value)) indicates the absolute index of the site to be used i.e.(site at abs(value) in genome");
shared_ptr<ParameterLink<string>> VoidGate::IO_RangesPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_VOID-IO_Ranges", (string)"1-4,1-4", "range of number of inputs and outputs (min inputs-max inputs,min outputs-max outputs)");

VoidGate::VoidGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID, double _epsilon, shared_ptr<ParametersTable> _PT) :
		DeterministicGate(addresses, _table, _ID, _PT) {  // use DeterministicGate constructor to build set up everything (including a table of 0s and 1s)
	epsilon = _epsilon;  // in case you want to have different epsilon for different gates (who am I to judge?)
}

void VoidGate::update(vector<double> & nodes, vector<double> & nextNodes) {
	int input = vectorToBitToInt(nodes,inputs,true); // converts the input values into an index (true indicates to reverse order)

	vector<int> outputRow = table[input]; // pick the output row

	if (Random::P(epsilon)) { // if chance...
		outputRow[Random::getIndex(outputs.size())] = 0;  // pick one output randomly and set it to 0
	}
	for (size_t i = 0; i < outputs.size(); i++) { // add output row to nextNodes
		nextNodes[outputs[i]] += outputRow[i];
	}
}

shared_ptr<AbstractGate> VoidGate::makeCopy(shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newGate = make_shared<VoidGate>(_PT);
	newGate->table = table;
	newGate->ID = ID;
	newGate->inputs = inputs;
	newGate->outputs = outputs;
	newGate->defaultOutput = defaultOutput; 
	newGate->epsilon = epsilon; 
	return newGate;
}


