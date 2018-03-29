//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "GPGate.h"

shared_ptr<ParameterLink<double>> GPGate::constValueMinPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_GENETICPROGRAMING-constValueMin", -1.0, "for the const values, min value the genome can generate");
shared_ptr<ParameterLink<double>> GPGate::constValueMaxPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_GENETICPROGRAMING-constValueMax", 1.0, "for the const values, max value the genome can generate");
shared_ptr<ParameterLink<string>> GPGate::IO_RangesPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_GENETICPROGRAMING-IO_Ranges", (string)"1-4,1-4", "range of number of inputs and outputs (min inputs-max inputs,min outputs-max outputs)");

/* *** GP Gate implementation *** */

//GPGate::GPGate(shared_ptr<AbstractGenome> genome, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {
GPGate::GPGate(pair<vector<int>, vector<int>> _addresses, int _operation, vector<double> _constValues, int gateID, shared_ptr<ParametersTable> _PT) :
	AbstractGate(_PT) {

	ID = gateID;

	inputs = _addresses.first;
	outputs = _addresses.second;

	operation = _operation;
	constValues = _constValues;
}

void GPGate::update(vector<double> & states, vector<double> & nextStates) {
	double retValue = states[inputs[0]];
	int index = 0;
	size_t i, o;
	bool writeValueAtEnd = true;
	switch (operation) {
		case 0:  //constant
			writeValueAtEnd = false;
			for (o = 0; o < outputs.size(); o++)
				nextStates[outputs[o]] = constValues[o];
			break;
		case 1:  // add
			for (i = 1; i < inputs.size(); i++){
				retValue += states[inputs[i]];
			}

			break;
		case 2:  // sub
			for (i = 1; i < inputs.size(); i++)
				retValue -= states[inputs[i]];
			break;
		case 3:  // mult
			for (i = 1; i < inputs.size(); i++)
				retValue *= states[inputs[i]];
			break;
		case 4:  // div
			for (i = 1; i < inputs.size(); i++)
				if (states[inputs[i]] == 0){
					retValue = 0;
				} else {
					retValue /= states[inputs[i]];
				}
			break;
		case 5:  // sin
			writeValueAtEnd = false;
			for (o = 0; o < outputs.size(); o++) {
				nextStates[outputs[o]] += sin(states[inputs[index]]);
				index = (index + 1) % (int) inputs.size();
			}
			break;
		case 6:  // cos
			writeValueAtEnd = false;
			for (o = 0; o < outputs.size(); o++) {
				nextStates[outputs[o]] += cos(states[inputs[index]]);
				index = (index + 1) % (int) inputs.size();
			}
			break;
		case 7:  // log
			writeValueAtEnd = false;
			for (o = 0; o < outputs.size(); o++) {
				if (inputs[index] > 0.0)
					if (states[inputs[index]] > 0){
											nextStates[outputs[o]] += log(states[inputs[index]]);
					}
				index = (index + 1) % (int) inputs.size();
			}
			break;
		case 8:  // exp
			writeValueAtEnd = false;
			for (o = 0; o < outputs.size(); o++) {
				nextStates[outputs[o]] += 1;//exp(states[inputs[index]]);
				index = (index + 1) % (int) inputs.size();
			}
			break;
	}
	if (writeValueAtEnd) {
		for (size_t o = 0; o < outputs.size(); o++)
			nextStates[outputs[o]] += retValue;
	}
}

string GPGate::description() {
	cout << "in GP description" << endl;
	string gateTypeName[9] = { "fixed constants", "+", "-", "*", "/", "Sin", "Cos", "Log", "Exp" };
	string constString = " constants: " + to_string(constValues[0]) + " " + to_string(constValues[1]) + " "+ to_string(constValues[2]) + " " + to_string(constValues[3]) + "\n";
	return "Gate " + to_string(ID) + " is a (" + gateTypeName[operation] + ") " + gateType() + "Gate\n" + AbstractGate::descriptionIO() + constString;
}


shared_ptr<AbstractGate> GPGate::makeCopy(shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newGate = make_shared<GPGate>(_PT);
	newGate->ID = ID;
	newGate->inputs = inputs;
	newGate->outputs = outputs;
	newGate->constValues = constValues;
	newGate->operation = operation;
	return newGate;
}
