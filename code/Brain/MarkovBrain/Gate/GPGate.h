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


class GPGate : public AbstractGate {
 private:

	int operation; // <link> stores the kind of GP operation (Add, Sub, Mult...)
	std::vector<double> constValues; // list of constant values
 public:

	static std::shared_ptr<ParameterLink<double>> constValueMinPL;
	static std::shared_ptr<ParameterLink<double>> constValueMaxPL;
	static std::shared_ptr<ParameterLink<std::string>> IO_RangesPL;

	GPGate() = delete;
	GPGate(std::shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
		operation = 0;
	}
	//GPGate(shared_ptr<AbstractGenome> genome, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID);
	GPGate(std::pair<std::vector<int>, std::vector<int>> _addresses, int _operation, std::vector<double> _constValues, int gateID, std::shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~GPGate() = default;
	virtual void update(std::vector<double> & states, std::vector<double> & nextStates) override;
	virtual std::string description() override;
	virtual std::string gateType() override{
		return "GeneticPrograming";
	}
	virtual std::shared_ptr<AbstractGate> makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr) override;

};

