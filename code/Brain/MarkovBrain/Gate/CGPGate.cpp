//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "CGPGate.h"

std::shared_ptr<ParameterLink<std::string>> CGPGate::availableOperatorsPL = 
Parameters::register_parameter("BRAIN_MARKOV_GATES_CARTESIANGENETICPROGRAMING-availableOperators", (std::string)"all", 
	"which opperators are allowed? all indicates, allow all opperators or, "
	"choose from:\n"
	"  ADD (a+b), MULT (a*b), SUBTRACT (a-b), DIVIDE(a/b)\n"
	"  SIN (sin(a)), COS (sin(a)), GREATER (1 if a > b, else 0), RAND (double[a..b))\n"
	"  IF (b is a > 0, else 0), INV (-a), CONST (const)");

std::shared_ptr<ParameterLink<double>> CGPGate::constValueMinPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_CARTESIANGENETICPROGRAMING-constValueMin", -10.0, "for the const values, min value the genome can generate");
std::shared_ptr<ParameterLink<double>> CGPGate::constValueMaxPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_CARTESIANGENETICPROGRAMING-constValueMax", 10.0, "for the const values, max value the genome can generate");
std::shared_ptr<ParameterLink<double>> CGPGate::magnitudeMinPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_CARTESIANGENETICPROGRAMING-magnitudeMin", -100.0, "results from gate with be clipped between magnitudeMin and magnitudeMax");
std::shared_ptr<ParameterLink<double>> CGPGate::magnitudeMaxPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_CARTESIANGENETICPROGRAMING-magnitudeMax", 100.0, "for the const values, max value the genome can generate");

/* *** GP Gate implementation *** */

//GPGate::GPGate(shared_ptr<AbstractGenome> genome, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {
CGPGate::CGPGate(std::pair<std::vector<int>, std::vector<int>> _addresses, int _operation,double _constValue, int gateID, std::shared_ptr<ParametersTable> _PT) : 
	AbstractGate(_PT) {

	std::vector<std::string> tempOpList;
	convertCSVListToVector(availableOperatorsPL->get(PT), tempOpList);
	for (auto o : tempOpList) {
		if (o == "all") {
			availableOperators = { 0,1,2,3,4,5,6,7,8,9 };
		}
		else if (o == "CONST") {
			availableOperators.push_back(0);
		}
		else if (o == "ADD") {
			availableOperators.push_back(1);
		}
		else if (o == "SUBTRACT") {
			availableOperators.push_back(2);
		}
		else if (o == "MULT") {
			availableOperators.push_back(3);
		}
		else if (o == "DIVIDE") {
			availableOperators.push_back(4);
		}
		else if (o == "SIN") {
			availableOperators.push_back(5);
		}
		else if (o == "COS") {
			availableOperators.push_back(6);
		}
		else if (o == "GREATER") {
			availableOperators.push_back(7);
		}
		else if (o == "IF") {
			availableOperators.push_back(8);
		}
		else if (o == "INV") {
			availableOperators.push_back(9);
		}
		else if (o == "RAND") {
			availableOperators.push_back(10);
		}
		else {
			std::cout << "  while making a Markov Brain:CGP gate, found a bad op in availableOperators (" << o << "). exiting..." << std::endl;
			exit(1);
		}
	}

	ID = gateID;

	inputs = _addresses.first;
	outputs = _addresses.second;

	operation = _operation%opsCount;
	constValue = _constValue;
	magnitudeMin = magnitudeMinPL->get(PT);
	magnitudeMax = magnitudeMaxPL->get(PT);
}

void CGPGate::update(std::vector<double>& states, std::vector<double>& nextStates) {
	int index = 0;
	double retValue = 0;
	switch (operation) {
	case 0:  //constant
		retValue = constValue;
		break;
	case 1:  // add
		retValue = states[inputs[0]] + states[inputs[1]];
		break;
	case 2:  // sub
		retValue = states[inputs[0]] - states[inputs[1]];
		break;
	case 3:  // mult
		retValue = states[inputs[0]] * states[inputs[1]];
		break;
	case 4:  // div
		if (states[inputs[1]] == 0) {
			retValue = 0;
		}
		else {
			retValue = states[inputs[0]] / states[inputs[1]];
		}
		break;
	case 5:  // sin
		retValue = sin(states[inputs[0]]);
		break;
	case 6:  // cos
		retValue = cos(states[inputs[0]]);
		break;
	case 7:  // greater
		retValue = (states[inputs[0]] > states[inputs[1]]) ? 1 : 0;
		break;
	case 8:  // if
		retValue = (states[inputs[0]] > 0) ? states[inputs[1]] : 0;
		break;
	case 9:  // inv
		retValue = -1 * states[inputs[0]];
		break;
	case 10:  // random
		retValue = Random::getDouble(states[inputs[0]],states[inputs[1]]);
		break;
	default:
		std::cout << "in CGP Gate :: bad operation value " << operation << ". exiting..." << std::endl;
		exit(1);
	}
	
	//std::cout << "states:";
	//for (auto v : states) {
	//	std::cout << " " << v;
	//}
	//std::cout << std::endl;

	//std::cout << "gate: " << ID << "  op: " << operation << " (" << inputs[0] << "," << inputs[1] << ")  " << states[inputs[0]] << ":" << states[inputs[1]] << " -> " << retValue << std::endl;

	nextStates[outputs[0]] += std::min(magnitudeMax,std::max(magnitudeMin,retValue));
	
	//std::cout << "nextStates:";
	//for (auto v : nextStates) {
	//	std::cout << " " << v;
	//}
	//std::cout << std::endl;
}

std::string CGPGate::description() {
	std::cout << "in GP description" << std::endl;
	std::string gateTypeName[9] = { "fixed constants", "+", "-", "*", "/", "Sin", "Cos", "Log", "Exp" };
	std::string constString = " constant: " + std::to_string(constValue) + "\n";
	return "Gate " + std::to_string(ID) + " is a (" + gateTypeName[operation] + ") " + gateType() + "Gate\n" + AbstractGate::descriptionIO() + constString;
}


std::shared_ptr<AbstractGate> CGPGate::makeCopy(std::shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newGate = std::make_shared<CGPGate>(_PT);
	newGate->ID = ID;
	newGate->inputs = inputs;
	newGate->outputs = outputs;
	newGate->constValue = constValue;
	newGate->operation = operation;
	return newGate;
}
