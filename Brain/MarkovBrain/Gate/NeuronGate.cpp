//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "NeuronGate.h"

shared_ptr<ParameterLink<int>> NeuronGate::defaultNumInputsMinPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-numInputsMin", 3, "min number of inputs to neuronGate");
shared_ptr<ParameterLink<int>> NeuronGate::defaultNumInputsMaxPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-numInputsMax", 3, "max number of inputs to neuronGate");
shared_ptr<ParameterLink<int>> NeuronGate::defaultDischargeBehaviorPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-dischargeBehavior", 0, "what happens when gate fires (-1: let genome decide, 0: clear charge, 1: 'reduce' by delivery charge, 2: 1/2 charge)");
shared_ptr<ParameterLink<double>> NeuronGate::defaultThresholdMinPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-thresholdMin", -1.0, "lowest possible value for threshold");
shared_ptr<ParameterLink<double>> NeuronGate::defaultThresholdMaxPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-thresholdMax", 1.0, "highest possible value for threshold");
shared_ptr<ParameterLink<bool>> NeuronGate::defaultAllowRepressionPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-allowRepression", false, "if false, neuronGates only fire if charge exceeds threshold; if true, neuronGates can also exist which always fire, except when charge exceeds threshold");
shared_ptr<ParameterLink<double>> NeuronGate::defaultDecayRateMinPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-decayRateMin", 0.0, "amount charge will decay (towards 0) per update");
shared_ptr<ParameterLink<double>> NeuronGate::defaultDecayRateMaxPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-decayRateMax", 0.0, "amount charge will decay (towards 0) per update");
shared_ptr<ParameterLink<double>> NeuronGate::defaultDeliveryChargeMinPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-deliveryChargeMin", -1.0, "output charge");
shared_ptr<ParameterLink<double>> NeuronGate::defaultDeliveryChargeMaxPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-deliveryChargeMax", 1.0, "output charge");
shared_ptr<ParameterLink<double>> NeuronGate::defaultDeliveryErrorPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-deliveryError", 0.0, "error in deliveryCharge... charge -= random[0,deliveryError)");


shared_ptr<ParameterLink<bool>> NeuronGate::defaultThresholdFromNodePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-thresholdFromNode", false, "if true, gate will have additional input, which will be used as threshold");
shared_ptr<ParameterLink<bool>> NeuronGate::defaultDeliveryChargeFromNodePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-deliveryChargeFromNode", false, "if true, gate will have additional input, which will be used as deliveryCharge");


shared_ptr<AbstractGate> NeuronGate::makeCopy(shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newGate = make_shared<NeuronGate>(_PT);
	newGate->ID = ID;
	newGate->inputs = inputs;
	newGate->outputs = outputs;
	newGate->dischargeBehavior = dischargeBehavior;
	newGate->thresholdValue = thresholdValue;
	newGate->thresholdActivates = thresholdActivates;
	newGate->decayRate = decayRate;
	newGate->deliveryCharge = deliveryCharge;
	newGate->deliveryError = deliveryError;
	newGate->currentCharge = currentCharge;
	newGate->thresholdFromNode = thresholdFromNode;
	newGate->deliveryChargeFromNode = deliveryChargeFromNode;

	return newGate;
}
