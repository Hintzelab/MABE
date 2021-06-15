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


class AnnGate : public AbstractGate {  //conventional probabilistic gate
public:

	static std::shared_ptr<ParameterLink<bool>> bitBehaviorPL;
	static std::shared_ptr<ParameterLink<std::string>> biasRangePL;
	static std::shared_ptr<ParameterLink<int>> discretizeOutputPL;
	static std::shared_ptr<ParameterLink<std::string>> weightRangeMappingPL;
	static std::shared_ptr<ParameterLink<std::string>> I_RangePL;
	static std::shared_ptr<ParameterLink<std::string>> activationFunctionPL;

	std::vector<double> weights = {};


	bool bitBehavior = false;
	double initalValue = 0.0;
	int discretizeOutput = 0;
	std::vector<double> weightRangeMapping = {};
	std::vector<double> weightRangeMappingSums = {};
	std::string activationFunction = "tanh";

	AnnGate() = delete;
	AnnGate(std::shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {}
	AnnGate(std::vector<int> _inputs, int _output, std::vector<double> _weights, double _initalValue, int _ID, std::shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~AnnGate() = default;
	virtual void update(std::vector<double> & states, std::vector<double> & nextStates) override;

	void resetGate() override {
	}

	virtual std::string gateType() override{
		return "ANN";
	}
	virtual std::shared_ptr<AbstractGate> makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr) override;
};
