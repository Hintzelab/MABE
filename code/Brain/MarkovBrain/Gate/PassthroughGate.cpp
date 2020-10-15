//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "PassthroughGate.h"

PassThroughGate::PassThroughGate(int _inAddress, int _outputAddress, int _ID, std::shared_ptr<ParametersTable> _PT) :
	AbstractGate(_PT) {
	ID = _ID;
	inputs = { _inAddress };
	outputs = { _outputAddress };
}

void PassThroughGate::update(std::vector<double> & nodes, std::vector<double> & nextNodes) {  //this translates the input bits of the current states to the output bits of the next states
	//std::cout << "passthough... " << nodes[inputs[0]] << " : " << nextNodes[outputs[0]];
	nextNodes[outputs[0]] += nodes[inputs[0]];
	//std::cout << " -> " << nextNodes[outputs[0]] << std::endl;
}

std::shared_ptr<AbstractGate> PassThroughGate::makeCopy(std::shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newGate = std::make_shared<PassThroughGate>(_PT);
	newGate->ID = ID;
	newGate->inputs = inputs;
	newGate->outputs = outputs;
	return newGate;
}
