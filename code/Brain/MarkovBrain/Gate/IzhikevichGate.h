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

#pragma once

#include "AbstractGate.h"

class IzhikevichGate: public AbstractGate {  //conventional probabilistic gate
public:

	// u=-15,v=-65,a=.02,b=.2,c=-50,d=2,threshold=30,IValues=IValues,time=2000

	static std::shared_ptr<ParameterLink<std::string>> UInitalPL;
	static std::shared_ptr<ParameterLink<std::string>> VInitalPL;
	static std::shared_ptr<ParameterLink<std::string>> APL;
	static std::shared_ptr<ParameterLink<std::string>> BPL;
	static std::shared_ptr<ParameterLink<std::string>> CPL;
	static std::shared_ptr<ParameterLink<std::string>> DPL;

	static std::shared_ptr<ParameterLink<std::string>> thresholdPL;

	static std::shared_ptr<ParameterLink<std::string>> V2_scalePL; // .04
	static std::shared_ptr<ParameterLink<std::string>> V_scalePL;  // 5
	static std::shared_ptr<ParameterLink<std::string>> V_constPL; // 140

	static std::shared_ptr<ParameterLink<std::string>> inputCount_PL; // min:max inputs per gate
	static std::shared_ptr<ParameterLink<std::string>> inputWeightsRange_PL; // min:max

	static std::shared_ptr<ParameterLink<bool>> record_behaviorPL;
	static std::shared_ptr<ParameterLink<std::string>> record_behavior_file_namePL;
	bool record_behavior = false;
	std::string record_behavior_file_name;

	double initV = 0;
	double V = 0;
	double initU = 0;
	double U = 0;
	double A = 0;
	double B = 0;
	double C = 0;
	double D = 0;
	double threshold = 0;

	double V2_scale = 0;
	double V_scale = 0;
	double V_const = 0;

	int inputCount = 0;

	std::vector<double> weights;

	IzhikevichGate() = delete;
	IzhikevichGate(std::shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
	}
	IzhikevichGate(std::vector<int> _inAddresses, std::vector<double> _weights, int _outputAddress, double _initU, double _initV, double _A, double _B, double _C, double _D, double _threshold, double _V2_scale, double _V_scale, double _V_const, int _ID, std::shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~IzhikevichGate() = default;
	virtual void update(std::vector<double> & states, std::vector<double> & nextStates) override;

	void resetGate() override {
		V = initV;
		U = initU;
	}

	virtual std::string gateType() override{
		return "Izhikevich";
	}
	virtual std::shared_ptr<AbstractGate> makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr) override;
};
