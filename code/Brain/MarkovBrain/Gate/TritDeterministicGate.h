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

class TritDeterministicGate : public AbstractGate {
 public:

	static std::shared_ptr<ParameterLink<std::string>> IO_RangesPL;

	std::vector<std::vector<int>> table;

	TritDeterministicGate() = delete;
	TritDeterministicGate(std::shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
		table = {};
	}
	TritDeterministicGate(std::pair<std::vector<int>, std::vector<int>> addresses, std::vector<std::vector<int>> _table, int _ID, std::shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~TritDeterministicGate() = default;
	virtual void update(std::vector<double> & states, std::vector<double> & nextStates) override;

	//void setupForBits(int* Ins, int nrOfIns, int Out, int logic);

	virtual std::string gateType() override{
		return "TritDeterministic";
	}

	virtual std::shared_ptr<AbstractGate> makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr) override;

	//double voidOutput;
};




