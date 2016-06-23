//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#include "DeterministicGate.h"

DeterministicGate::DeterministicGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID, shared_ptr<ParametersTable> _PT) :
	AbstractGate(_PT) {
	ID = _ID;
	inputs = addresses.first;
	outputs = addresses.second;
	table = _table;
}

//void DeterministicGate::setupForBits(int* Ins, int nrOfIns, int Out, int logic) {
//	inputs.resize(nrOfIns);
//	for (int i = 0; i < nrOfIns; i++)
//		inputs[i] = Ins[i];
//	outputs.resize(1);
//	outputs[0] = Out;
//	table.resize(1 << nrOfIns);
//	for (int i = 0; i < (1 << nrOfIns); i++) {
//		table[i].resize(1);
//		table[i][0] = (logic >> i) & 1;
//	}
//}

void DeterministicGate::update(vector<double> & nodes, vector<double> & nextNodes) {
	int input = vectorToBitToInt(nodes,inputs,true); // converts the input values into an index (true indicates to reverse order)
	for (size_t i = 0; i < outputs.size(); i++) {
		nextNodes[outputs[i]] += table[input][i];
	}
}

shared_ptr<AbstractGate> DeterministicGate::makeCopy(shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT; 
	}
	auto newGate = make_shared<DeterministicGate>(_PT); 
	newGate->table = table; 
	newGate->ID = ID;	
	newGate->inputs = inputs;
	newGate->outputs = outputs;
	return newGate;
}

