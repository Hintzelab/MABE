//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "AnnGate.h"


std::shared_ptr<ParameterLink<bool>> AnnGate::bitBehaviorPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_ANN-bitBehavior", false, "if true, preform BIT operator on all inputs and output to and from this gate");

std::shared_ptr<ParameterLink<std::string>> AnnGate::I_RangePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_ANN-Input_Range", (std::string)"1-4", "range of number of inputs (min inputs-max inputs)");

std::shared_ptr<ParameterLink<std::string>> AnnGate::biasRangePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_ANN-biasRange", (std::string)"-1.0,1.0", "bias will be generated per node in this range and used to initalize gate on update");

std::shared_ptr<ParameterLink<int>> AnnGate::discretizeOutputPL = Parameters::register_parameter(
	"BRAIN_MARKOV_GATES_ANN-discretizeOutput", 0, "should recurrent nodes be discretized when being copied?\n"
	"if 0, no, leave them be.\n"
	"if 1 then map <= 0 to 0, and > 0 to 1\n"
	"if > then 1, values are mapped to new equally spaced values in range [-1..1] such that each bin has the same sized range\n"
	"    i.e. if 3 bin bounderies will be (-1.0,-.333-,.333-,1.0) and resulting values will be (-1.0,0.0,1.0)\n"
	"Note that this process ends up in a skewed mapping. mappings will always include -1.0 and 1.0. even values > 1 will result in remappings that do not have 0");

std::shared_ptr<ParameterLink<std::string>> AnnGate::weightRangeMappingPL = Parameters::register_parameter(
	"BRAIN_MARKOV_GATES_ANN-weightRangeMapping", (std::string)"0,1,0,1,0",
	"comma seperated list of exactly five (double) values. weight values from genome will be extracted in the range [0..sum(list)].\n"
	"values between 0 and the first value will map to -1\n"
	"values between the first value and first+second will map to [-1..0]\n"
	"... i.e. each value in the list is the ratio of possible values that will map to each value/range in [-1,[-1..0],0,[0..1],1]\n"
	"the result is that this list sets the ratio of each type of weight (-1,variable(-1..0),0,variable(0..1),1)");

std::shared_ptr<ParameterLink<std::string>> AnnGate::activationFunctionPL = Parameters::register_parameter(
	"BRAIN_MARKOV_GATES_ANN-activationFunction", (std::string)"tanh",
	"choose from linear(i.e. none),tanh,tanh(0-1),bit,triangle");

AnnGate::AnnGate(std::vector<int> _inputs, int _output, std::vector<double> _weights, double _initalValue, int _ID, std::shared_ptr<ParametersTable> _PT) :
	AbstractGate(_PT) {

	ID = _ID;
	inputs = _inputs;
	outputs = { _output };
	weights = _weights;

	std::vector<double> biasRange;
	convertCSVListToVector(biasRangePL->get(_PT), biasRange);
	
	initalValue = _initalValue;
	discretizeOutput = discretizeOutputPL->get(PT);
	activationFunction = activationFunctionPL->get(PT);

	convertCSVListToVector(weightRangeMappingPL->get(), weightRangeMapping);

	// weightRangeMappingSums is a list of bounderies between the weight ranges [-1,[-1..0],0,[0..1],1]
	weightRangeMappingSums.push_back(weightRangeMapping[0]);
	weightRangeMappingSums.push_back(weightRangeMappingSums[0] + weightRangeMapping[1]);
	weightRangeMappingSums.push_back(weightRangeMappingSums[1] + weightRangeMapping[2]);
	weightRangeMappingSums.push_back(weightRangeMappingSums[2] + weightRangeMapping[3]);
	weightRangeMappingSums.push_back(weightRangeMappingSums[3] + weightRangeMapping[4]);

	bitBehavior = bitBehaviorPL->get(PT);
}

void AnnGate::update(std::vector<double>& nodes, std::vector<double>& nextNodes) {  //this translates the input bits of the current states to the output bits of the next states
	
	double result = initalValue; // initalize this nodes return value
	// accumulate weighted inputs
	if (bitBehavior) {
		for (int i = 0; i < inputs.size(); i++) {
			result += weights[i] * Bit(nodes[inputs[i]]);
		}
	}
	else {
		for (int i = 0; i < inputs.size(); i++) {
			result += weights[i] * nodes[inputs[i]];
		}
	}

	// select activation function (if none/linear, do nothing)
	if (activationFunction == "none" || activationFunction == "linear") {
		// if none / linear, do nothing
	}
	else if (activationFunction == "tanh") {
		result = tanh(result);
	}
	else if (activationFunction == "tanh(0-1)") {
		result = tanh(result) * .5 + .5;
	}
	else if (activationFunction == "bit") {
		result = Bit(result);
	}
	else if (activationFunction == "triangle") {
		result = std::max(1.0-std::abs(result*2.0),-1.0);
	}

	// apply discertize rule is set
	if (discretizeOutput == 1) {
		result = Bit(result);
	}
	else if (discretizeOutput > 1) {
		// move value is in to range 0 to discretizeOutput
		result = ((result + 1.0) / 2.0) * discretizeOutput;
		// use int to discretize
		result = (double)(int)(result);
		if (result == discretizeOutput) { // if node value was exactly 1
			result--;
		}
		// move back to [0..1] (with / discretizeOutput-1) and then to [-1...1] (with * 2 - 1)
		result = (result / (double)(discretizeOutput - 1) * 2.0) - 1.0;
	}

	if (bitBehavior) {
		nextNodes[outputs[0]] += Bit(result);
	}
	else {
		nextNodes[outputs[0]] += result;
	}
}

std::shared_ptr<AbstractGate> AnnGate::makeCopy(std::shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newGate = std::make_shared<AnnGate>(_PT);
	newGate->inputs = inputs;
	newGate->outputs = outputs;
	newGate->weights = weights;
	newGate->initalValue = initalValue;
	newGate->discretizeOutput = discretizeOutput;
	newGate->weightRangeMapping = weightRangeMapping;
	newGate->weightRangeMappingSums = weightRangeMappingSums;
	newGate->bitBehavior = bitBehavior;
	return newGate;
}
