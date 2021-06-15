//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License
//
//	IzhikevichGate is based on a model created by Eugene M. Izhikevich
//	The description of the model was taken from: Simple Model of Spiking Neurons 
//	from: IEEE TRANSACTIONS ON NEURAL NETWORKS, VOL. 14, NO. 6, NOVEMBER 2003
//

#include "IzhikevichGate.h"


std::shared_ptr<ParameterLink<std::string>> IzhikevichGate::UInitalPL =
Parameters::register_parameter("BRAIN_MARKOV_GATES_IZHIKEVICH-UInital", std::string("-15"),
	"initalValue for U, if single value, use this value, if X:Y use genome to pick value in range (-15)");

std::shared_ptr<ParameterLink<std::string>> IzhikevichGate::VInitalPL =
Parameters::register_parameter("BRAIN_MARKOV_GATES_IZHIKEVICH-VInital", std::string("-75:-45"),
	"initalValue for V, if single value, use this value, if X:Y use genome to pick value in range");

std::shared_ptr<ParameterLink<std::string>> IzhikevichGate::APL =
Parameters::register_parameter("BRAIN_MARKOV_GATES_IZHIKEVICH-A", std::string(".02:.1"),
	"value for a, if single value, use this value, if X:Y use genome to pick value in range");

std::shared_ptr<ParameterLink<std::string>> IzhikevichGate::BPL =
Parameters::register_parameter("BRAIN_MARKOV_GATES_IZHIKEVICH-B", std::string(".2:3"),
	"value for b, if single value, use this value, if X:Y use genome to pick value in range");

std::shared_ptr<ParameterLink<std::string>> IzhikevichGate::CPL =
Parameters::register_parameter("BRAIN_MARKOV_GATES_IZHIKEVICH-C", std::string("-75:-45"),
	"value for c, if single value, use this value, if X:Y use genome to pick value in range");

std::shared_ptr<ParameterLink<std::string>> IzhikevichGate::DPL =
Parameters::register_parameter("BRAIN_MARKOV_GATES_IZHIKEVICH-D", std::string("0:10"),
	"value for d, if single value, use this value, if X:Y use genome to pick value in range");

std::shared_ptr<ParameterLink<std::string>> IzhikevichGate::thresholdPL =
Parameters::register_parameter("BRAIN_MARKOV_GATES_IZHIKEVICH-threshold", std::string("30.0"),
	"value for threshold, if single value, use this value, if X:Y use genome to pick value in range (30)");

std::shared_ptr<ParameterLink<std::string>> IzhikevichGate::V2_scalePL =
Parameters::register_parameter("BRAIN_MARKOV_GATES_IZHIKEVICH-V2_scale", std::string(".04"),
	"value for V2_scale, if single value, use this value, if X:Y use genome to pick value in range (.04)");

std::shared_ptr<ParameterLink<std::string>> IzhikevichGate::V_scalePL =
Parameters::register_parameter("BRAIN_MARKOV_GATES_IZHIKEVICH-V_scale", std::string("5"),
	"value for V_scale, if single value, use this value, if X:Y use genome to pick value in range (5)");

std::shared_ptr<ParameterLink<std::string>> IzhikevichGate::V_constPL =
Parameters::register_parameter("BRAIN_MARKOV_GATES_IZHIKEVICH-V_const", std::string("140"),
	"value for V_const, if single value, use this value, if X:Y use genome to pick value in range (140)");

std::shared_ptr<ParameterLink<std::string>> IzhikevichGate::inputCount_PL =
Parameters::register_parameter("BRAIN_MARKOV_GATES_IZHIKEVICH-inputCount", std::string("1:4"),
	"value for number of inputs, if single value, use this value, if X:Y use genome to pick value in range");

std::shared_ptr<ParameterLink<std::string>> IzhikevichGate::inputWeightsRange_PL =
Parameters::register_parameter("BRAIN_MARKOV_GATES_IZHIKEVICH-inputWeightsRange", std::string("-20:20"),
	"value for weights on input values, if single value, use this value, if X:Y use genome to pick value in range");

std::shared_ptr<ParameterLink<bool>> IzhikevichGate::record_behaviorPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_IZHIKEVICH-record_behavior", false, "if true, record IzhikevichGate gate behavior (NOTE: this can generate a lot of data!)");
std::shared_ptr<ParameterLink<std::string>> IzhikevichGate::record_behavior_file_namePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_IZHIKEVICH-record_behavior_fileName", (std::string) "izhikevichGate_behavior.csv", "Name of file where neron behaviors are saved");

IzhikevichGate::IzhikevichGate(std::vector<int> _inAddresses, std::vector<double> _weights, int _outputAddress, double _initU, double _initV, double _A, double _B, double _C, double _D, double _threshold, double _V2_scale, double _V_scale, double _V_const, int _ID, std::shared_ptr<ParametersTable> _PT):
	AbstractGate(_PT) {
	ID = _ID;
	inputs = _inAddresses;
	weights = _weights;
	outputs = { _outputAddress };
	initV = _initV;
	V = _initV;
	initU = _initU;
	U = _initU;
	A = _A;
	B = _B;
	C = _C;
	D = _D;
	threshold = _threshold;

	V2_scale = _V2_scale;
	V_scale = _V_scale;
	V_const = _V_const;

	inputCount = 0;

	record_behavior = record_behaviorPL->get(PT);
	record_behavior_file_name = record_behavior_file_namePL->get(PT);


	if (0) {
		std::cout << "ins: ";
		for (auto xx : _inAddresses) {
			std::cout << xx << "  ";
		}
		std::cout << "   weights: ";
		for (auto xx : _weights) {
			std::cout << xx << "  ";
		}
		std::cout << "   outs: ";
		for (auto xx : outputs) {
			std::cout << xx << "  ";
		}
		std::cout << "   V" << initV << " " << V << " : ";
		std::cout << "   U" << initU << " " << U << " : ";
		std::cout << "   A:" << A << " B:" << B << " C:" << C << " D:" << D << " th:" << threshold << std::endl;
	}
}

void IzhikevichGate::update(std::vector<double> & nodes, std::vector<double> & nextNodes) {  //this translates the input bits of the current states to the output bits of the next states
	//if (Global::update > 36) {
	//	std::cout << "pre:  " << V << "  " << U << std::endl;
	//}
	// collect inputs
	double I = 0;
	for (int i = 0; i < inputs.size(); i++) {
		I += weights[i] * nodes[inputs[i]];
	}

	// upate neuron
	//std::cout << V2_scale << " * " << V << "^2 + " << V_scale << " * " <<  V  << " + " << V_const << " - " << U << " + " <<  I <<  " = " << ((V2_scale * V * V) + (V_scale * V) + V_const) - U + I << std::endl;
	double dV = ((V2_scale * V * V) + (V_scale * V) + V_const) - U + I;
	//std::cout << A << " * (" << B << " * " << V << " - " << U << ")" << " = " << A * ((B * V) - U) << std::endl;
	double dU = A * ((B * V) - U);
	V += dV;
	U += dU;

	if (V != V) { // correct for nan
		V = 0;
	}
	if (U != U) { // correct for nan
		U = 0;
	}

	std::string stateNow = "";

	if (record_behavior) {
		stateNow += std::to_string(ID);
		// I/O
		stateNow += "," + std::to_string(inputs.size());
		stateNow += "," + std::to_string(outputs.size()) + ",\"[";
		for (int i = 0; i < (int)inputs.size(); i++) {
			stateNow += std::to_string(inputs[i]) + ",";
		}
		stateNow.pop_back();
		stateNow += "]\",\"[";
		for (int i = 0; i < (int)outputs.size(); i++) {
			stateNow += std::to_string(outputs[i]) + ",";
		}
		stateNow.pop_back();
		stateNow += "]\"";

		// state
		stateNow += "," + std::to_string(V >= threshold); // fire
		stateNow += "," + std::to_string(V);
		stateNow += "," + std::to_string(U);
		stateNow += "," + std::to_string(A);
		stateNow += "," + std::to_string(B);
		stateNow += "," + std::to_string(C);
		stateNow += "," + std::to_string(D);
		stateNow += "," + std::to_string(threshold);

		stateNow += "," + std::to_string(V2_scale);
		stateNow += "," + std::to_string(V_scale);
		stateNow += "," + std::to_string(V_const);
		stateNow += "," + std::to_string(I);

		FileManager::writeToFile(record_behavior_file_name, stateNow, "ID,inCount,outCount,inConnections,outConnections,fire,V,U,A,B,C,D,threshold,V2_scale,V_scale,V_const,input");  //fileName, data, header
	}

	int fired = 0;
	if (V >= threshold) {
		fired = 1;
		V = C;
		U = U + D;
	}

	nextNodes[outputs[0]] += fired;
	//if (Global::update > 46) {
	//	std::cout << "post: " << V << "  " << U << std::endl;
	//}

}

std::shared_ptr<AbstractGate> IzhikevichGate::makeCopy(std::shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newGate = std::make_shared<IzhikevichGate>(_PT);
	newGate->ID = ID;
	newGate->inputs = inputs;
	newGate->outputs = outputs;
	newGate->initV = initV;
	newGate->V = initV;
	newGate->initU = initU;
	newGate->U = initU;
	newGate->A = A;
	newGate->B = B;
	newGate->C = C;
	newGate->D = D;
	newGate->threshold = threshold;

	return newGate;
}
