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


class ComparatorGate: public AbstractGate {  //conventional probabilistic gate
public:

	static std::shared_ptr<ParameterLink<int>> allowedComparisonsPL;
	static std::shared_ptr<ParameterLink<std::string>> initalValuePL;
	static std::shared_ptr<ParameterLink<std::string>> initalValueRangePL;
	static std::shared_ptr<ParameterLink<int>> comparatorModePL;

	int action = 1; // 0 = <, 1 = =, 2 = >
	int mode = 0; // if 0, get mode from genome, if 1 than programable, if 2 than direct
	double value = 0;
	int initalValue = 0;

	ComparatorGate() = delete;
	ComparatorGate(std::shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
	}
	ComparatorGate(int _programAddress, int _valueAddress, int _outputAddress, int _action, int _mode, int _initalValue, int _ID, std::shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~ComparatorGate() = default;
	virtual void update(std::vector<double> & states, std::vector<double> & nextStates) override;

	void resetGate() override {
		value = initalValue;
	}

	virtual std::string gateType() override{
		return "Comparator";
	}
	virtual std::shared_ptr<AbstractGate> makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr) override;
};
