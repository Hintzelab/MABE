//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once

#include "AbstractGate.h"

class NeuronGate: public AbstractGate {
public:

	static std::shared_ptr<ParameterLink<int>> defaultNumInputsMinPL;
	static std::shared_ptr<ParameterLink<int>> defaultNumInputsMaxPL;
	static std::shared_ptr<ParameterLink<int>> defaultDischargeBehaviorPL;
	static std::shared_ptr<ParameterLink<double>> defaultThresholdMinPL;
	static std::shared_ptr<ParameterLink<double>> defaultThresholdMaxPL;
	static std::shared_ptr<ParameterLink<int>> defaultThresholdFromNodePL;

	static std::shared_ptr<ParameterLink<bool>> defaultAllowRepressionPL;
	static std::shared_ptr<ParameterLink<double>> defaultDecayRateMinPL;
	static std::shared_ptr<ParameterLink<double>> defaultDecayRateMaxPL;
	static std::shared_ptr<ParameterLink<double>> defaultDeliveryChargeMinPL;
	static std::shared_ptr<ParameterLink<double>> defaultDeliveryChargeMaxPL;
	static std::shared_ptr<ParameterLink<int>> defaultDeliveryChargeFromNodePL;
	static std::shared_ptr<ParameterLink<double>> defaultDeliveryErrorPL;

	static std::shared_ptr<ParameterLink<bool>> record_behaviorPL;
	static std::shared_ptr<ParameterLink<std::string>> record_behavior_file_namePL;

	bool record_behavior;
	std::string record_behavior_file_name;

	int dischargeBehavior;  // what to do when the gate delivers a charge
	double thresholdValue;  // threshold when this gate will fire (if negative, then fire when currentCharge < threshold)
	bool thresholdActivates;  // if true, fire when currentCharge is >= threshold, if false, fire unless currentCharge >
	double decayRate;  // every brain update reduce currentCharge by decayRate
	double deliveryCharge;  // charge delivered when this gate fires
	double deliveryError;  // delivery charge is reduced by random[0...deliveryError)

	double currentCharge;

	int thresholdFromNode;
	int deliveryChargeFromNode;

	double defaultThresholdMin;
	double defaultThresholdMax;
	double defaultDeliveryChargeMin;
	double defaultDeliveryChargeMax;
	//double costOfDecay;
	//double costOfDelivery;

	NeuronGate() = delete;
	NeuronGate(std::shared_ptr<ParametersTable> _PT) :
		AbstractGate(_PT) {
		dischargeBehavior = 0; 
		thresholdValue = 0;
		thresholdActivates = 0;
		decayRate = 0;
		deliveryCharge = 0;
		deliveryError = 0;
		currentCharge = 0;
		thresholdFromNode = 0;
		deliveryChargeFromNode = 0;

		defaultThresholdMin = defaultThresholdMinPL->get(PT);
		defaultThresholdMax = defaultThresholdMaxPL->get(PT);
		defaultDeliveryChargeMin = defaultDeliveryChargeMinPL->get(PT);
		defaultDeliveryChargeMax = defaultDeliveryChargeMaxPL->get(PT);

		record_behavior = record_behaviorPL->get(PT);
		record_behavior_file_name = record_behavior_file_namePL->get(PT);
	}
	NeuronGate(std::vector<int> ins, int out, int _dischargeBehavior, double _thresholdValue, bool _thresholdActivates, double _decayRate, double _deliveryCharge, double _deliveryError, int _thresholdFromNode, int _deliveryChargeFromNode, int _ID, std::shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
		ID = _ID;
		inputs = ins;
		outputs.clear();
		outputs.push_back(out);
		dischargeBehavior = _dischargeBehavior;
		thresholdValue = _thresholdValue;
		thresholdActivates = _thresholdActivates;
		decayRate = _decayRate;
		deliveryCharge = _deliveryCharge;
		deliveryError = _deliveryError;
		currentCharge = 0;

		thresholdFromNode = _thresholdFromNode;
		deliveryChargeFromNode = _deliveryChargeFromNode;

		defaultThresholdMin = defaultThresholdMinPL->get(PT);
		defaultThresholdMax = defaultThresholdMaxPL->get(PT);
		defaultDeliveryChargeMin = defaultDeliveryChargeMinPL->get(PT);
		defaultDeliveryChargeMax = defaultDeliveryChargeMaxPL->get(PT);

		record_behavior = record_behaviorPL->get(PT);
		record_behavior_file_name = record_behavior_file_namePL->get(PT);
	}

	virtual ~NeuronGate() = default;

	virtual void update(std::vector<double> & nodes, std::vector<double> & nextnodes) override;

	virtual std::string description() override {
		std::string s = "Gate " + std::to_string(ID) + " is a Neuron Gate with " + std::to_string(inputs.size()) + " inputs (";
		for (auto a : inputs) {
			s += std::to_string(a) + ",";
		}
		s.pop_back();
		s += ") and output " + std::to_string(outputs[0]) + "\n";
		s += "currentCharge: " + std::to_string(currentCharge) + "\n";
		s += "thresholdValue: " + std::to_string(thresholdValue) + "\n";
		s += "thresholdActivates: " + std::to_string(thresholdActivates) + "\n";
		s += "dischargeBehavior: " + std::to_string(dischargeBehavior) + "\n";
		s += "decayRate: " + std::to_string(decayRate) + "\n";
		s += "deliveryCharge: " + std::to_string(deliveryCharge) + "\n";
		s += "deliveryError: " + std::to_string(deliveryError) + "\n";
		return s;
	}

	virtual std::string gateType() override{
		return "Neuron";
	}

	void applyNodeMap(std::vector<int> nodeMap, int maxNodes) override {
		AbstractGate::applyNodeMap(nodeMap, maxNodes);
		if (thresholdFromNode != -1) {
			thresholdFromNode = nodeMap[thresholdFromNode] % maxNodes;
		}
		if (deliveryChargeFromNode != -1) {
			deliveryChargeFromNode = nodeMap[deliveryChargeFromNode] % maxNodes;
		}
	}

	void resetGate() override {
		currentCharge = 0;
	}

	virtual std::pair<std::vector<int>, std::vector<int>> getConnectionsLists() override{
		std::pair<std::vector<int>, std::vector<int>> connectionsLists;
		connectionsLists.first = inputs;
		if (thresholdFromNode != -1) {
			connectionsLists.first.push_back(thresholdFromNode);
		}
		if (deliveryChargeFromNode != -1) {
			connectionsLists.first.push_back(deliveryChargeFromNode);
		}
		connectionsLists.second = outputs;
		return connectionsLists;
	}

	virtual std::shared_ptr<AbstractGate> makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr) override;


};

