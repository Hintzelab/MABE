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


class CGPGate : public AbstractGate {
 private:

	int operation; // <link> stores the kind of GP operation (Add, Sub, Mult...)
	double constValue; // in case op is const
	int opsCount = 10;

 public:
	 static std::shared_ptr<ParameterLink<std::string>> availableOperatorsPL;
	 std::vector<int> availableOperators;
	 static std::shared_ptr<ParameterLink<double>> constValueMinPL;
	 static std::shared_ptr<ParameterLink<double>> constValueMaxPL;
	 static std::shared_ptr<ParameterLink<double>> magnitudeMinPL;
	 static std::shared_ptr<ParameterLink<double>> magnitudeMaxPL;
	 double magnitudeMin, magnitudeMax;

	CGPGate() = delete;
	CGPGate(std::shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
		operation = 0;
		constValue = 0;
		magnitudeMin = 0;
		magnitudeMax = 0;
	}
	//GPGate(shared_ptr<AbstractGenome> genome, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID);
	CGPGate(std::pair<std::vector<int>, std::vector<int>> _addresses, int _operation, double _constValue, int gateID, std::shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~CGPGate() = default;
	virtual void update(std::vector<double> & states, std::vector<double> & nextStates) override;
	virtual std::string description() override;
	virtual std::string gateType() override{
		return "CartesianGeneticPrograming";
	}
	virtual std::shared_ptr<AbstractGate> makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr) override;

};

