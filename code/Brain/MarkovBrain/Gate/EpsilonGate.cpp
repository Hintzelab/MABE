//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "EpsilonGate.h"

/* this gate behaves like a deterministic gate with a constant externally set error which may cause the gate to deliver the "wrong" output values */

std::shared_ptr<ParameterLink<double>> EpsilonGate::EpsilonSourcePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_EPSILON-epsilonSource", 0.05, "chance output will be randomized. if > 1 or < 0, epsilon value is determined by the genome. If positive, then the genome handler will advance int(value) number of sites - 1 from the current location, before reading. If the value is negative, then int(abs(value)) indicates the absolute index of the site to be used i.e.(site at abs(value) in genome)");
std::shared_ptr<ParameterLink<std::string>> EpsilonGate::IO_RangesPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_EPSILON-IO_Ranges", (std::string)"1-4,1-4", "range of number of inputs and outputs (min inputs-max inputs,min outputs-max outputs)");


EpsilonGate::EpsilonGate(std::pair<std::vector<int>, std::vector<int>> addresses, std::vector<std::vector<int>> _table, int _ID, double _epsilon, std::shared_ptr<ParametersTable> _PT) :
		DeterministicGate(addresses, _table, _ID, _PT) {  // use DeterministicGate constructor to build set up everything (including a table of 0s and 1s)
	epsilon = _epsilon;  // in case you want to have different epsilon for different gates (who am I to judge?)
	// now to the specifics of this gate - we convert the table to a list of numbers (i.e. bitstrings) so we can do fast comparisons in the update
	defaultOutput.clear();  // clear the defaultOutput vector... the list of bitstrings
	for (size_t i = 0; i < table.size(); i++) {  //for every row of the table (which is 2^inputs)
		int D = 0;
		for (size_t j = 0; j < table[i].size(); j++) {  //for every space in the row
			D |= (Bit(table[i][j]) << j);  //translate the value in that space to a bit (0 or 1), then bitshift it [position in row] times, then take D, bit-or it with that number,
			//and make that the new D
		}
		defaultOutput.push_back(D);  //add D (which is a bitstring of length [2^outputs]) to defaultOutput. defaultoutputs is now of length (2^number of inputs)
	}
}

void EpsilonGate::update(std::vector<double> & nodes, std::vector<double> & nextNodes) {
	int input = vectorToBitToInt(nodes,inputs,true); // converts the input values into an index (true indicates to reverse order)
	if (Random::P(epsilon)) {  //if chance... then return a different set of output values
		int output = 0;
		do {
			output = Random::getIndex(table.size());
		} while (output == defaultOutput[input]);  // keep picking randomly until you pick a different output
		for (size_t i = 0; i < outputs.size(); i++) {
			nextNodes[outputs[i]] += 1.0 * (double) ((output >> i) & 1);
		}
	} else {
		//do the default output
		for (size_t i = 0; i < outputs.size(); i++) {
			nextNodes[outputs[i]] += table[input][i];
		}
	}
}

std::shared_ptr<AbstractGate> EpsilonGate::makeCopy(std::shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newGate = std::make_shared<EpsilonGate>(_PT);
	newGate->table = table;
	newGate->ID = ID;
	newGate->inputs = inputs;
	newGate->outputs = outputs;
	newGate->defaultOutput = defaultOutput; 
	newGate->epsilon = epsilon; 
	return newGate;
}
