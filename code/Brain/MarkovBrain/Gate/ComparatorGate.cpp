//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "ComparatorGate.h"

std::shared_ptr<ParameterLink<int>> ComparatorGate::allowedComparisonsPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_COMPARATOR-allowedComparisons", 111, "a three digit binary, first allows < comparison, second =, and third > (i.e. 010 or '10' allows only equals)");

std::shared_ptr<ParameterLink<std::string>> ComparatorGate::initalValuePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_COMPARATOR-initalValue", (std::string)"G", "comparison value when gate is reset, G = extract from genome");
std::shared_ptr<ParameterLink<std::string>> ComparatorGate::initalValueRangePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_COMPARATOR-initalValueRange", (std::string)"-100,100", "min and max int values for inital value if initalValue == G");

std::shared_ptr<ParameterLink<int>> ComparatorGate::comparatorModePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_COMPARATOR-comparatorMode", 0, "if 0, let genome decide; if 1 only allow programable; if 2, only allow direct comparison");

ComparatorGate::ComparatorGate(int _programAddress, int _valueAddress, int _outputAddress, int _action, int _mode, int _initalValue, int _ID, std::shared_ptr<ParametersTable> _PT) :
	AbstractGate(_PT) {
	ID = _ID;
	inputs = { _programAddress, _valueAddress };
	outputs = { _outputAddress };

	mode = _mode;

	if (initalValuePL->get(PT) == "G") {
		initalValue = _initalValue;
	}
	else {
		initalValue = std::stoi(initalValuePL->get(PT));
	}
	value = (double)initalValue;

	// figure out action from allowedActions and _action

	int allowedComparisons = allowedComparisonsPL->get(PT);

	// allowedComparisons cases:
	// 111 all actions allowed pick from range [0,1,2]
	if (allowedComparisons == 111) {
		action = _action%3;
	}

	// 100 only <, ie 0
	else if (allowedComparisons == 100) {
		action = 0;
	}
	// 010 only =, ie 1
	else if (allowedComparisons == 10) {
		action = 1;
	}
	// 001 only >, ie 2
	else if (allowedComparisons == 1) {
		action = 2;
	}

	// 011, pick from [0,1] where 0 = = and 1 = >
	else if (allowedComparisons == 11) {
		action = (_action %2) ? 2 : 1;
	}
	// 101, pick from [0,1] where 0 = < and 1 = >
	else if (allowedComparisons == 101) {
		action = (_action % 2) ? 2 : 0;
	}
	// 110, pick from [0,1] where 0 = < and 1 = =
	else if (allowedComparisons == 110) {
		action = (_action % 2) ? 1 : 0;
	}
	// any other value is invalid (including 000) report error
	else {
		std::cout << "  In ComparatorGate constructor :: allowedComparisons parameter has value " << allowedComparisons << " but this value must be from be a value from [001,010,011,100,101,110,111].\n  exiting..." << std::endl;
		exit(1);
	}
}

void ComparatorGate::update(std::vector<double> & nodes, std::vector<double> & nextNodes) {  //this translates the input bits of the current states to the output bits of the next states

	if (mode == 1) { // programable, compare input 1 to stored value
		if (nodes[inputs[0]] > 0) { // program on positive value
			//std::cout << "program " << value << std::endl;
			value = nodes[inputs[1]]; // set value
			// no output ction needed // nextNodes[outputs[0]] += 0; // output 0 when gate is being programed
		}
		else {
			//std::cout << "mode " << mode << "  action " << action << " ("<<value<<")    " << nodes[inputs[0]] << " : " << nodes[inputs[1]] << " -> " << nextNodes[outputs[0]];
			if (action == 0) { // less than action
				nextNodes[outputs[0]] += (nodes[inputs[1]] < value);
			}
			if (action == 1) { // = action
				nextNodes[outputs[0]] += (nodes[inputs[1]] == value);
			}
			if (action == 2) { // greater than action
				nextNodes[outputs[0]] += (nodes[inputs[1]] > value);
			}
			//std::cout << " ... " << nextNodes[outputs[0]] << std::endl;
		}
	}
	else { // mode == 2, directly compare inputs 0 and 1
		//std::cout << "mode " << mode << "  action " << action << "   " << nodes[inputs[0]] << " : " << nodes[inputs[1]] << " -> " << nextNodes[outputs[0]];
		if (action == 0) { // less than action
			nextNodes[outputs[0]] += (nodes[inputs[0]] < nodes[inputs[1]]);
		}
		if (action == 1) { // = action
			nextNodes[outputs[0]] += (nodes[inputs[0]] == nodes[inputs[1]]);
		}
		if (action == 2) { // greater than action
			nextNodes[outputs[0]] += (nodes[inputs[0]] > nodes[inputs[1]]);
		}
		//std::cout << " ... " << nextNodes[outputs[0]] << std::endl;
	}
}

std::shared_ptr<AbstractGate> ComparatorGate::makeCopy(std::shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newGate = std::make_shared<ComparatorGate>(_PT);
	newGate->action = action;
	newGate->initalValue = initalValue;
	newGate->value = value;
	newGate->ID = ID;
	newGate->inputs = inputs;
	newGate->outputs = outputs;
	return newGate;
}
