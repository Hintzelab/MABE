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

	static shared_ptr<ParameterLink<int>> defaultNumInputsMinPL;
	static shared_ptr<ParameterLink<int>> defaultNumInputsMaxPL;
	static shared_ptr<ParameterLink<int>> defaultDischargeBehaviorPL;
	static shared_ptr<ParameterLink<double>> defaultThresholdMinPL;
	static shared_ptr<ParameterLink<double>> defaultThresholdMaxPL;
	static shared_ptr<ParameterLink<bool>> defaultThresholdFromNodePL;

	static shared_ptr<ParameterLink<bool>> defaultAllowRepressionPL;
	static shared_ptr<ParameterLink<double>> defaultDecayRateMinPL;
	static shared_ptr<ParameterLink<double>> defaultDecayRateMaxPL;
	static shared_ptr<ParameterLink<double>> defaultDeliveryChargeMinPL;
	static shared_ptr<ParameterLink<double>> defaultDeliveryChargeMaxPL;
	static shared_ptr<ParameterLink<bool>> defaultDeliveryChargeFromNodePL;
	static shared_ptr<ParameterLink<double>> defaultDeliveryErrorPL;

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
	NeuronGate(shared_ptr<ParametersTable> _PT) :
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
	}
	NeuronGate(vector<int> ins, int out, int _dischargeBehavior, double _thresholdValue, bool _thresholdActivates, double _decayRate, double _deliveryCharge, double _deliveryError, int _thresholdFromNode, int _deliveryChargeFromNode, int _ID, shared_ptr<ParametersTable> _PT = nullptr) :
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
	}

	virtual ~NeuronGate() = default;

	virtual void update(vector<double> & nodes, vector<double> & nextnodes) override;

	virtual string description() override {
		string s = "Gate " + to_string(ID) + " is a Neuron Gate with " + to_string(inputs.size()) + " inputs (";
		for (auto a : inputs) {
			s += to_string(a) + ",";
		}
		s.pop_back();
		s += ") and output " + to_string(outputs[0]) + "\n";
		s += "currentCharge: " + to_string(currentCharge) + "\n";
		s += "thresholdValue: " + to_string(thresholdValue) + "\n";
		s += "thresholdActivates: " + to_string(thresholdActivates) + "\n";
		s += "dischargeBehavior: " + to_string(dischargeBehavior) + "\n";
		s += "decayRate: " + to_string(decayRate) + "\n";
		s += "deliveryCharge: " + to_string(deliveryCharge) + "\n";
		s += "deliveryError: " + to_string(deliveryError) + "\n";
		return s;
	}

	virtual string gateType() override{
		return "Neuron";
	}

	void applyNodeMap(vector<int> nodeMap, int maxNodes) override {
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

	virtual pair<vector<int>,vector<int>> getConnectionsLists() override{
		pair<vector<int>,vector<int>> connectionsLists;
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

	virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;


};

