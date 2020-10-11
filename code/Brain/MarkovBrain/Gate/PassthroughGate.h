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

class PassThroughGate: public AbstractGate {  //conventional probabilistic gate
public:

	PassThroughGate() = delete;
	PassThroughGate(std::shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
	}
	PassThroughGate(int _inAddress, int _outputAddress, int _ID, std::shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~PassThroughGate() = default;
	virtual void update(std::vector<double> & states, std::vector<double> & nextStates) override;

	void resetGate() override {
	}

	virtual std::string gateType() override{
		return "PassThrough";
	}
	virtual std::shared_ptr<AbstractGate> makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr) override;
};
