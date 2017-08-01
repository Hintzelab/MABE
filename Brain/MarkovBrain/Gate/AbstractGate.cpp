//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "AbstractGate.h"

#include <iostream>

// *** General tools for All Gates ***

// converts values attained from genome for inputs and outputs to vaild brain state ids
// uses nodeMap to accomplish the remaping
void AbstractGate::applyNodeMap(vector<int> nodeMap, int maxNodes) {
	for (size_t i = 0; i < inputs.size(); i++) {
		inputs[i] = nodeMap[inputs[i]] % maxNodes;
	}
	for (size_t i = 0; i < outputs.size(); i++) {
		outputs[i] = nodeMap[outputs[i]] % maxNodes;
	}
}

shared_ptr<AbstractGate> AbstractGate::makeCopy(shared_ptr<ParametersTable> _PT) {
	cout << "ERROR IN AbstractGate::makeCopy() - You are using the abstract copy constructor for gates. You must define your own" << endl; 
	exit(1); 
}

void AbstractGate::resetGate() {
	//nothing to reset here!
}

vector<int> AbstractGate::getIns() {
	return inputs;
}

vector<int> AbstractGate::getOuts() {
	return outputs;
}

string AbstractGate::descriptionIO() {
	string S = "IN:";
	for (size_t i = 0; i < inputs.size(); i++)
		S = S + " " + to_string(inputs[i]);
	S = S + "\n";
	S = S + "OUT:";
	for (size_t i = 0; i < outputs.size(); i++)
		S = S + " " + to_string(outputs[i]);
	S = S + "\n";
	//S = S + getCodingRegions();
	return S;
}

string AbstractGate::description() {
	return "Gate " + to_string(ID) + " is a " + gateType() + "Gate\n" + descriptionIO();
}
