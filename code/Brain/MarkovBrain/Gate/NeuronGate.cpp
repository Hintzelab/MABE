//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "NeuronGate.h"

std::shared_ptr<ParameterLink<int>> NeuronGate::defaultNumInputsMinPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-numInputsMin", 1, "min number of inputs to neuronGate");
std::shared_ptr<ParameterLink<int>> NeuronGate::defaultNumInputsMaxPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-numInputsMax", 4, "max number of inputs to neuronGate");
std::shared_ptr<ParameterLink<int>> NeuronGate::defaultDischargeBehaviorPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-dischargeBehavior", -1, "what happens when gate fires (-1: let genome decide, 0: clear charge, 1: 'reduce' by delivery charge, 2: 1/2 charge)");
std::shared_ptr<ParameterLink<double>> NeuronGate::defaultThresholdMinPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-thresholdMin", -2.0, "lowest possible value for threshold");
std::shared_ptr<ParameterLink<double>> NeuronGate::defaultThresholdMaxPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-thresholdMax", 2.0, "highest possible value for threshold");
std::shared_ptr<ParameterLink<bool>> NeuronGate::defaultAllowRepressionPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-allowRepression", true, "if false, neuronGates only fire if charge exceeds threshold; if true, neuronGates can also exist which always fire, except when charge exceeds threshold");
std::shared_ptr<ParameterLink<double>> NeuronGate::defaultDecayRateMinPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-decayRateMin", 0.0, "min % current charge will decay each time gate update is called (towards 0) per update");
std::shared_ptr<ParameterLink<double>> NeuronGate::defaultDecayRateMaxPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-decayRateMax", 1.0, "max % current charge will decay each time gate update is called (towards 0) per update");
std::shared_ptr<ParameterLink<double>> NeuronGate::defaultDeliveryChargeMinPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-deliveryChargeMin", -2.0, "output charge");
std::shared_ptr<ParameterLink<double>> NeuronGate::defaultDeliveryChargeMaxPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-deliveryChargeMax", 2.0, "output charge");
std::shared_ptr<ParameterLink<double>> NeuronGate::defaultDeliveryErrorPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-deliveryError", 0.0, "error in deliveryCharge... charge -= random[0,deliveryError)");


std::shared_ptr<ParameterLink<int>> NeuronGate::defaultThresholdFromNodePL = Parameters::register_parameter(
	"BRAIN_MARKOV_GATES_NEURON-thresholdFromNode", -1, "if 0, genome will determine threshold value within threshold range\nif 1, gate will have additional input, determined by genome, which will be clamped to threshold range and used as threshold value, if -1, genome will decide between options 0 and 1");
std::shared_ptr<ParameterLink<int>> NeuronGate::defaultDeliveryChargeFromNodePL = Parameters::register_parameter(
	"BRAIN_MARKOV_GATES_NEURON-deliveryChargeFromNode", -1, "if 0, genome will determine deliveryCharge value within deliveryCharge range\nif 1, gate will have additional input, determined by genome, which will be clamped to deliveryCharge range and used as deliveryCharge value, if -1, genome will decide between options 0 and 1");


std::shared_ptr<ParameterLink<bool>> NeuronGate::record_behaviorPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-record_behavior", false, "if true, record neuron behavior (NOTE: this can generate a lot of data!)");
std::shared_ptr<ParameterLink<std::string>> NeuronGate::record_behavior_file_namePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-record_behavior_fileName", (std::string) "neuron_behavior.csv","Name of file where neron behaviors are saved");

void NeuronGate::update(std::vector<double> & nodes, std::vector<double> & nextnodes) {
	bool fire = false;
	currentCharge += -1 * Trit(currentCharge) * decayRate;
	for (auto i : inputs) {
		currentCharge += nodes[i];
	}

	if (thresholdFromNode != -1) {
		thresholdValue = nodes[thresholdFromNode];
		thresholdValue = std::max(defaultThresholdMin, std::min(defaultThresholdMax, thresholdValue));
	}

	if (thresholdActivates) {  // fire if currCharge is greater than a positive threshold or less than a negative threshold
		if (currentCharge > thresholdValue) {
			fire = true;
		}
	}
	else {  // threshold represses. fire always unless currCharge is greater than a positive threshold (or less than a negative threshold)
		if (currentCharge < thresholdValue) {  // if thresholdValue is positive
			fire = true;
		}
	}

	std::string stateNow = "";

	if (record_behavior) {
		stateNow += std::to_string(ID);
		stateNow += "," + std::to_string(fire);
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
		stateNow += "," + std::to_string(thresholdValue);
		stateNow += "," + std::to_string(currentCharge);
		stateNow += "," + std::to_string(dischargeBehavior);
		stateNow += "," + std::to_string(decayRate);
		stateNow += "," + std::to_string(deliveryError);
		stateNow += "," + std::to_string(thresholdActivates);
		stateNow += "," + std::to_string(thresholdFromNode);
		stateNow += "," + std::to_string(deliveryChargeFromNode);
	}

	double localDeliveryCharge = 0;
	if (fire) {
		//cout << "neuron: " << ID << "  ";
		if (deliveryChargeFromNode == -1) {
			localDeliveryCharge += deliveryCharge;
			//cout << " -1 " << localDeliveryCharge << endl;
		}
		else {
			localDeliveryCharge += nodes[deliveryChargeFromNode];
			//cout << " deliveryChargeFromNode [" << deliveryChargeFromNode << "]  " << localDeliveryCharge << endl;
		}

		// clip to [min,max]
		localDeliveryCharge *= (1.0 - Random::getDouble(0, deliveryError));
		localDeliveryCharge = std::max(defaultDeliveryChargeMin, std::min(defaultDeliveryChargeMax, localDeliveryCharge));

		nextnodes[outputs[0]] += localDeliveryCharge;

		if (dischargeBehavior == 0) {
			currentCharge = 0;
		}
		if (dischargeBehavior == 1) { // "reduce" (i.e. move closer to 0) current charge, but thresholdValue amt
			int currentChargeSign = Trit(currentCharge);
			currentCharge = ((currentChargeSign * currentCharge) - (Trit(localDeliveryCharge) * localDeliveryCharge)) * currentChargeSign;
		}
		if (dischargeBehavior == 2) {
			currentCharge = currentCharge * .5;
		}
	}
	if (record_behavior) {
		stateNow += "," + std::to_string(localDeliveryCharge);
		FileManager::openAndWriteToFile(record_behavior_file_name, stateNow, "ID,fire,inCount,outCount,inConnections,outConnections,thresholdValue,currentCharge,dischargeBehavior,decayRate,deliveryError,thresholdActivates,thresholdFromNode,deliveryChargeFromNode,deliveryCharge");  //fileName, data, header
	}
}

//void NeuronGate::update(vector<double> & nodes, vector<double> & nextnodes) {
//	//cout << description() << endl;
//	bool fire = false;
//	// decay first
//	//cout << currentCharge;
//	currentCharge += -1 * Trit(currentCharge) * decayRate;
//	//cout << "->" << currentCharge << endl;
//	// add inputs to currCharge
//	//cout << currentCharge;
//	for (auto i : inputs) {
//		currentCharge += nodes[i];
//	}
//	//cout << "->" << currentCharge << endl;
//	// if currCharge is >= Th, fire
//	//   reduce currCharge
//
//	if (thresholdFromNode != -1) {
//		//cout << "threshold set from (" << thresholdFromNode << ") = ";
//		thresholdValue = nodes[thresholdFromNode];
//		//cout << thresholdValue << endl;
//	}
//
//	if (thresholdActivates) {  // fire if currCharge is greater than a positive threshold or less than a negative threshold
//		//cout << "  A+  ";
//		//cout << currentCharge << " ?? " << thresholdValue;
//		if (((Trit(currentCharge) * currentCharge) > (Trit(thresholdValue) * thresholdValue)) && (Trit(currentCharge) == Trit(thresholdValue) || (thresholdValue == 0))) {
//			//if (thresholdValue != 0){
//				//cout << "    ++FIRE++";
//				fire = true;
//			//} // note if (Trit(thresholdValue) == 0), fire for any value currentCharge != 0;
//		}
//		//cout << endl;
//	} else {  // threshold represses. fire always unless currCharge is greater than a positive threshold (or less than a negative threshold)
//		//cout << "  R-  ";
//		//cout << currentCharge << " ?? " << thresholdValue;
//		if (Trit(thresholdValue) == 1 && Trit(currentCharge) == 1) {  // if thresholdValue is positive
//			if (currentCharge < thresholdValue) {
//				//cout << "    --FIRE--";
//				fire = true;
//			}
//		} else if (Trit(thresholdValue) == -1 && Trit(currentCharge) == -1){  // if threshold is negative
//			if (currentCharge > thresholdValue) {
//				//cout << "    --FIRE--";
//				fire = true;
//			}
//		} // Trit(thresholdValue) == 0, do nothing, threshold 0 never fires.
//		  // if signs of thresholdValue and currentCharge do not match, do not fire!
//		//cout << endl;
//	}
//
//    if (Global::modePL->lookup() == "visualize") {
//            string stateNow = "";
//            stateNow += to_string(ID);
//            stateNow += "," + to_string(fire);
//            stateNow += "," + to_string(inputs.size());
//            stateNow += "," + to_string(outputs.size())+",\"[";
//            for (int i = 0; i < (int)inputs.size(); i++) {
//                    stateNow += to_string(inputs[i]) + ",";
//            }
//            stateNow.pop_back();
//            stateNow += "]\",\"[";
//            for (int i = 0; i < (int)outputs.size(); i++) {
//                    stateNow += to_string(outputs[i]) + ",";
//            }
//            stateNow.pop_back();
//            stateNow += "]\"";
//            stateNow += "," + to_string(thresholdValue);
//            stateNow += "," + to_string(currentCharge);
//            stateNow += "," + to_string(dischargeBehavior);
//            stateNow += "," + to_string(decayRate);
//            stateNow += "," + to_string(deliveryCharge);
//            stateNow += "," + to_string(deliveryError);
//            stateNow += "," + to_string(thresholdActivates);
//            stateNow += "," + to_string(thresholdFromNode);
//            stateNow += "," + to_string(deliveryChargeFromNode);
//
//            FileManager::openAndWriteToFile("neuron_data.txt", stateNow, "ID,fire,inCount,outCount,inConnections,outConnections,thresholdValue,currentCharge,dischargeBehavior,decayRate,deliveryCharge,deliveryError,thresholdActivates,thresholdFromNode,deliveryChargeFromNode");  //fileName, data, header - used when you want to output formatted data (i.e. genomes)
//    }
//
//	if (fire) {
//		//cout << "Fire!" <<  endl;
//		double localDdeliveryCharge = 0;
//		if (deliveryChargeFromNode == -1) {
//			localDdeliveryCharge += deliveryCharge - Random::getDouble(0, deliveryError);
//		} else {
//			//cout << "charge from (" << deliveryChargeFromNode << ") = " << nodes[deliveryChargeFromNode] << endl;
//			//cout << "  " << nextnodes[outputs[0]];
//			localDdeliveryCharge += nodes[deliveryChargeFromNode] - Random::getDouble(0, deliveryError);
//			//cout << "  " << nextnodes[outputs[0]] << endl;
//		}
//		nextnodes[outputs[0]] += localDdeliveryCharge;
//		if (dischargeBehavior == 0) {
//			currentCharge = 0;
//		}
//		if (dischargeBehavior == 1) { // "reduce" (i.e. move closer to 0) current charge, but thresholdValue amt
//			int currentChargeSign = Trit(currentCharge);
//			currentCharge = ((currentChargeSign * currentCharge) - (Trit(localDdeliveryCharge) * localDdeliveryCharge)) * currentChargeSign;
//		}
//		if (dischargeBehavior == 2) {
//			currentCharge = currentCharge * .5;
//		}
//	}
//}

std::shared_ptr<AbstractGate> NeuronGate::makeCopy(std::shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newGate = std::make_shared<NeuronGate>(_PT);
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
	newGate->defaultThresholdMin = defaultThresholdMin;
	newGate->defaultThresholdMax = defaultThresholdMax;

	return newGate;
}
