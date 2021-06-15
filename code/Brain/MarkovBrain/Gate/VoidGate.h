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

#include "DeterministicGate.h"

class VoidGate: public DeterministicGate {
public:

	static std::shared_ptr<ParameterLink<double>> voidGate_ProbabilityPL;
	static std::shared_ptr<ParameterLink<std::string>> IO_RangesPL;

	std::vector<int> defaultOutput;
	double epsilon;
	VoidGate() = delete;
	VoidGate(std::shared_ptr<ParametersTable> _PT = nullptr) :
		DeterministicGate(_PT) {
		epsilon = 0; 
	}
	VoidGate(std::pair<std::vector<int>, std::vector<int>> addresses, std::vector<std::vector<int>> _table, int _ID, double _epsilon, std::shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~VoidGate() = default;
	virtual void update(std::vector<double> & states, std::vector<double> & nextStates) override;
	virtual std::string gateType() override{
			return "Void";
		}
	virtual std::shared_ptr<AbstractGate> makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr) override;

};
