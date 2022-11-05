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

#include <cmath>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

#include "../../Utilities/Random.h"
#include "../../Genome/AbstractGenome.h"
#include "../../Utilities/Utilities.h"

#include "../AbstractBrain.h"

class CGPBrain : public AbstractBrain {
public:
	static std::shared_ptr<ParameterLink<int>> nrRecurrentNodesPL;
	int nrRecurrentValues;

	static std::shared_ptr<ParameterLink<std::string>> availableOperatorsPL;

	static std::shared_ptr<ParameterLink<double>> magnitudeMaxPL;
	static std::shared_ptr<ParameterLink<double>> magnitudeMinPL;
	double magnitudeMax, magnitudeMin;

	static std::shared_ptr<ParameterLink<int>> opSpaceColumnsPL;
	static std::shared_ptr<ParameterLink<int>> opSpaceRowsPL;
	int opSpaceColumns, opSpaceRows;

	static std::shared_ptr<ParameterLink<bool>> readFromOutputsPL;
	bool readFromOutputs;


	static std::shared_ptr<ParameterLink<int>> discretizeRecurrentPL;
	static std::shared_ptr<ParameterLink<std::string>> discretizeRecurrentRangePL;
	int discretizeRecurrent;
	std::vector<double> discretizeRecurrentRange;

	static std::shared_ptr<ParameterLink<double>> inputMutationRatePL;
	static std::shared_ptr<ParameterLink<double>> constMutationRatePL;
	
	static std::shared_ptr<ParameterLink<double>> operatorMutationRatePL;
	double operatorMutationRate;
	
	static std::shared_ptr<ParameterLink<double>> outputMutationRatePL;
	double outputMutationRate;

	static std::shared_ptr<ParameterLink<double>> RNN_weightsMutationRatePL;
	static std::shared_ptr<ParameterLink<double>> RNN_biasMutationRatePL;
	static std::shared_ptr<ParameterLink<double>> RNN_biasMinPL;
	static std::shared_ptr<ParameterLink<double>> RNN_biasMaxPL;
	static std::shared_ptr<ParameterLink<double>> DETGATE_logicMutationRatePL;

	enum class OPTYPES { NONE, ADD, MULT, SUBTRACT, DIVIDE, SIN, COS, GREATER, RAND, IF, INV, CONST, RNN, DETGATE };
	std::unordered_map<std::string, OPTYPES> OPTYPES_MAP{ 
		{"NONE",OPTYPES::NONE},
		{"ADD",OPTYPES::ADD},
		{"MULT",OPTYPES::MULT},
		{"SUBTRACT",OPTYPES::SUBTRACT},
		{"DIVIDE",OPTYPES::DIVIDE},
		{"SIN",OPTYPES::SIN},
		{"COS",OPTYPES::COS},
		{"GREATER",OPTYPES::GREATER},
		{"RAND",OPTYPES::RAND},
		{"IF",OPTYPES::IF},
		{"INV",OPTYPES::INV},
		{"CONST",OPTYPES::CONST},
		{"RNN",OPTYPES::RNN},
		{"DETGATE",OPTYPES::DETGATE},
	};
	std::vector<std::vector<OPTYPES>> availableOperators; // per column, or if 1, all columns
	//SUM,MULT,SUBTRACT,DIVIDE,SIN,COS,THRESH,RAND,INV,RNN,DETGATE


	int nrInputTotal;  // inputs + last outputs (maybe) + recurrent
	int nrOutputTotal; // outputs + recurrent

	class AbstractOpt {
	public:
		OPTYPES type;
		std::string name;
		int numInputs;
		std::vector<int> inputs;
		int availableInputs;
		int row;
		int column;
		double inputMutationRate = inputMutationRatePL->get();
		AbstractOpt() : type(OPTYPES::NONE), name("NONE"), numInputs(-1), availableInputs(-1), row(-1), column(-1) {}
		AbstractOpt(int availableInputs_, OPTYPES type_, std::string name_, int numInputs_, int row_, int column_) : type(type_), name(name_), numInputs(numInputs_), availableInputs(availableInputs_), row(row_), column(column_) { // full construct
			inputs.resize(numInputs);
			for (int i = 0; i < numInputs; i++) {
				inputs[i] = Random::getIndex(availableInputs_);
			}
		}
		void mutateInputs() {
			int numInputMutations = Random::getBinomial(numInputs, inputMutationRate);
			for (int i = 0; i < numInputMutations; i++) {
				inputs[Random::getIndex(numInputs)] = Random::getIndex(availableInputs);
			}
		}
		virtual void mutate() = 0;// { // mutate this gate
		virtual double update(std::vector<double>& currentMatrixValues) = 0;// { // run update on this gate, return result
		virtual std::string serializeInputs() {
			std::string inputsStr = "";
			for (int i = 0; i < inputs.size(); i++) {
				inputsStr += std::to_string(inputs[i]);
				if (i < inputs.size() - 1) {
					inputsStr += "'";
				}
			}
			return(inputsStr);
		}
		virtual std::string serialize() { // name:inputs[:other contents] , use ' for lowest level seperator
			std::string returnData = name + ":" + std::to_string(availableInputs) + ":";
			returnData += std::to_string(row) + ":" + std::to_string(column) + ":";
			returnData += std::to_string(numInputs) + ":" + serializeInputs();
			return(returnData);
		}

		// use contentsData to fill in this op :: assumes that the op was already created of the correct type
		// name, availableInputs, row, column, numInputs
		virtual void deserializeContents(std::string contentsData) {
			std::vector<std::string> contentsList;
			convertCSVListToVector(contentsData, contentsList, ':');
			convertCSVListToVector(contentsList[5], inputs, '\'');
		}
	};

	class ADDOpt : public AbstractOpt {
	public:
		ADDOpt(int availableInputs_, int row_, int column_) : AbstractOpt(availableInputs_, OPTYPES::ADD, "ADD", 2, row_, column_) { // full construct
		}
		void mutate() override {
			mutateInputs();
		}
		double update(std::vector<double>& currentMatrixValues) override {
			return(currentMatrixValues[inputs[0]] + currentMatrixValues[inputs[1]]);
		}
	};

	class SUBTRACTOpt : public AbstractOpt {
	public:
		SUBTRACTOpt(int availableInputs_, int row_, int column_) : AbstractOpt(availableInputs_, OPTYPES::SUBTRACT, "SUBTRACT", 2, row_, column_) { // full construct
		}
		void mutate() override {
			mutateInputs();
		}
		double update(std::vector<double>& currentMatrixValues) override {
			return(currentMatrixValues[inputs[0]] - currentMatrixValues[inputs[1]]);
		}
	};

	class MULTOpt : public AbstractOpt {
	public:
		MULTOpt(int availableInputs_, int row_, int column_) : AbstractOpt(availableInputs_, OPTYPES::MULT, "MULT", 2, row_, column_) { // full construct
		}
		void mutate() override {
			mutateInputs();
		}
		double update(std::vector<double>& currentMatrixValues) override {
			return(currentMatrixValues[inputs[0]] * currentMatrixValues[inputs[1]]);
		}
	};

	class DIVIDEOpt : public AbstractOpt {
	public:
		DIVIDEOpt(int availableInputs_, int row_, int column_) : AbstractOpt(availableInputs_, OPTYPES::DIVIDE, "DIVIDE", 2, row_, column_) { // full construct
		}
		void mutate() override {
			mutateInputs();
		}
		double update(std::vector<double>& currentMatrixValues) override {
			if (currentMatrixValues[inputs[1]] == 0) {
				return 0;
			}
			else {
				return(currentMatrixValues[inputs[0]] / currentMatrixValues[inputs[1]]);
			}
		}
	};

	class SINOpt : public AbstractOpt {
	public:
		SINOpt(int availableInputs_, int row_, int column_) : AbstractOpt(availableInputs_, OPTYPES::SIN, "SIN", 1, row_, column_) { // full construct
		}
		void mutate() override {
			mutateInputs();
		}
		double update(std::vector<double>& currentMatrixValues) override {
			return(sin(currentMatrixValues[inputs[0]]));
		}
	};

	class COSOpt : public AbstractOpt {
	public:
		COSOpt(int availableInputs_, int row_, int column_) : AbstractOpt(availableInputs_, OPTYPES::COS, "COS", 1, row_, column_) { // full construct
		}
		void mutate() override {
			mutateInputs();
		}
		double update(std::vector<double>& currentMatrixValues) override {
			return(cos(currentMatrixValues[inputs[0]]));
		}
	};

	class GREATEROpt : public AbstractOpt {
	public:
		GREATEROpt(int availableInputs_, int row_, int column_) : AbstractOpt(availableInputs_, OPTYPES::GREATER, "GREATER", 2, row_, column_) { // full construct
		}
		void mutate() override {
			mutateInputs();
		}
		double update(std::vector<double>& currentMatrixValues) override {
			if (currentMatrixValues[inputs[0]] > currentMatrixValues[inputs[1]]) {
				return(1);
			}
			else {
				return(0);
			}
		}
	};

	class RANDOpt : public AbstractOpt {
	public:
		RANDOpt(int availableInputs_, int row_, int column_) : AbstractOpt(availableInputs_, OPTYPES::RAND, "RAND", 2, row_, column_) { // full construct
		}
		void mutate() override {
			mutateInputs();
		}
		double update(std::vector<double>& currentMatrixValues) override {
			if (currentMatrixValues[inputs[0]] <= currentMatrixValues[inputs[1]]) {
				return(Random::getDouble(currentMatrixValues[inputs[0]], currentMatrixValues[inputs[1]]));
			}
			else {
				return(Random::getDouble(currentMatrixValues[inputs[1]], currentMatrixValues[inputs[0]]));
			}
		}
	};

	class IFOpt : public AbstractOpt {
	public:
		IFOpt(int availableInputs_, int row_, int column_) : AbstractOpt(availableInputs_, OPTYPES::IF, "IF", 2, row_, column_) { // full construct
		}
		void mutate() override {
			mutateInputs();
		}
		double update(std::vector<double>& currentMatrixValues) override {
			if (currentMatrixValues[inputs[0]] > 0) {
				return(currentMatrixValues[inputs[1]]);
			}
			else {
				return(0);
			}
		}
	};

	class INVOpt : public AbstractOpt {
	public:
		INVOpt(int availableInputs_, int row_, int column_) : AbstractOpt(availableInputs_, OPTYPES::INV, "INV", 1, row_, column_) { // full construct
		}
		void mutate() override {
			mutateInputs();
		}
		double update(std::vector<double>& currentMatrixValues) override {
			return(-1 * currentMatrixValues[inputs[0]]);
		}
	};

	class CONSTOpt : public AbstractOpt {
	public:
		double valueMutationRate = constMutationRatePL->get();
		double value;
		CONSTOpt(int availableInputs_, int row_, int column_) : AbstractOpt(availableInputs_, OPTYPES::CONST, "CONST", 0, row_, column_) { // full construct
			value = Random::getDouble(-10, 10);
		}
		void mutate() override {
			if (Random::P(valueMutationRate)) {
				value = Random::getDouble(-10, 10);
			}
		}
		double update(std::vector<double>& currentMatrixValues) override {
			return(value);
		}
		virtual std::string serialize() {
			std::string returnData = name + ":" + std::to_string(availableInputs) + ":";
			returnData += std::to_string(row) + ":" + std::to_string(column) + ":";
			returnData += std::to_string(numInputs) + ":" + std::to_string(value);
			return(returnData);
		}
		// name, availableInputs, row, column, numInputs, value
		virtual void deserializeContents(std::string contentsData) {
			std::vector<std::string> temp;
			convertCSVListToVector(contentsData, temp, ':');
			std::stringstream ss(temp[5]);
			std::string remaining;
			if (!(ss >> value ? !(ss >> remaining) : false)) {
				std::cout << "problem converting const during CONSTOpt deserialize! exiting..." << std::endl;
				exit(1);
			}
		}
	};

	class RNNOpt : public AbstractOpt {
	public:
		std::vector<double> weights;
		double bias;
		double mutateRateWeights = RNN_weightsMutationRatePL->get();
		double mutateRateBias = RNN_biasMutationRatePL->get();
		RNNOpt(int availableInputs_, int row_, int column_) : AbstractOpt(availableInputs_, OPTYPES::RNN, "RNN", 8, row_, column_) { // full construct
			weights = std::vector<double>(numInputs);
			for (int i = 0; i < numInputs; i++) {
				weights[i] = Random::getDouble(-1, 1);
			}
			bias = Random::getDouble(RNN_biasMinPL->get(), RNN_biasMaxPL->get());
		}
		void mutate() override {
			mutateInputs();
			int numWeightMutations = Random::getBinomial(weights.size(), mutateRateWeights);
			for (int i = 0; i < numWeightMutations; i++) {
				weights[Random::getIndex(weights.size())] = Random::getDouble(-1, 1);
			}
			if (Random::P(mutateRateBias)) {
				bias = Random::getDouble(RNN_biasMinPL->get(), RNN_biasMaxPL->get());
			}
		}
		double update(std::vector<double>& currentMatrixValues) override {
			double sum = bias;
			for (int i = 0; i < numInputs; i++) {
				sum += currentMatrixValues[inputs[i]] * weights[i];
			}
			sum = tanh(sum);
			return(sum);
		}
		virtual std::string serialize() { // name:inputs:other contents
			std::string returnData = name + ":" + std::to_string(availableInputs) + ":";
			returnData += std::to_string(row) + ":" + std::to_string(column) + ":";
			returnData += std::to_string(numInputs) + ":" + serializeInputs() + ":";
			for (int i = 0; i < weights.size(); i++) {
				returnData += std::to_string(weights[i]);
				if (i < weights.size() - 1) {
					returnData += "'";
				}
			}
			returnData += ":" + std::to_string(bias);
			return(returnData);
		}		
		// name, availableInputs, row, column, numInputs, weights, bias
		virtual void deserializeContents(std::string contentsData) {
			std::vector<std::string> temp;
			convertCSVListToVector(contentsData, temp, ':');
			convertCSVListToVector(temp[5], inputs, '\'');
			convertCSVListToVector(temp[6], weights, '\'');
			std::stringstream ss(temp[7]);
			std::string remaining;
			if (!(ss >> bias ? !(ss >> remaining) : false)) {
				std::cout << "problem converting RNN bias during CONSTOpt deserialize! exiting..." << std::endl;
				exit(1);
			}
		}
	};

	class DETGATEOpt : public AbstractOpt {
	public:
		std::vector<bool> outputValues;
		double logicMutationRate = DETGATE_logicMutationRatePL->get();
		DETGATEOpt(int availableInputs_, int row_, int column_) : AbstractOpt(availableInputs_, OPTYPES::DETGATE, "DETGATE", 4, row_, column_) { // full construct
			outputValues = std::vector<bool>(std::pow(2,numInputs));
			for (int i = 0; i < outputValues.size(); i++) {
				outputValues[i] = Random::getInt(0, 1);
			}
		}
		void mutate() override {
			mutateInputs();
			int numOutputMutations = Random::getBinomial(outputValues.size(), logicMutationRate);
			for (int i = 0; i < numOutputMutations; i++) {
				outputValues[Random::getIndex(outputValues.size())] = Random::getInt(0, 1);
			}
		}
		double update(std::vector<double>& currentMatrixValues) override {
			int inputVal = 0;
			for (int i = 0; i < numInputs; i++) {
				inputVal = inputVal * 2 + Bit(currentMatrixValues[inputs[i]]);
			}
			//std::cout << "detGate inputVal: " << inputVal << std::endl;
			return(outputValues[inputVal]);
		}
		virtual std::string serialize() { // name|inputs|other contents
			std::string returnData = name + ":" + std::to_string(availableInputs) + ":";
			returnData += std::to_string(row) + ":" + std::to_string(column) + ":";
			returnData += std::to_string(numInputs) + ":" + serializeInputs() + ":";
			for (int i = 0; i < outputValues.size(); i++) {
				returnData += std::to_string(outputValues[i]);
				if (i < outputValues.size() - 1) {
					returnData += "'";
				}
			}
			return(returnData);
		}

		// name, availableInputs, row, column, numInputs, outputValues
		virtual void deserializeContents(std::string contentsData) {
			std::vector<std::string> temp;
			convertCSVListToVector(contentsData, temp, ':');
			convertCSVListToVector(temp[5], inputs, '\'');
			std::vector<int> temp_outputValues;
			convertCSVListToVector(temp[6], temp_outputValues, '\'');
			outputValues.resize(temp_outputValues.size());
			for (int i = 0; i < outputValues.size(); i++) {
				outputValues[i] = temp_outputValues[i] == 1;
			}
		}
	};


	// test one op - takes an op (with minimal monotinic inputs), an input values vector and expect value
	// print an X before test if test fails (this may not be a problem (e.g. RAND)
	double testOp(std::shared_ptr<AbstractOpt> thisOp, std::vector<double> values, double expected) {

		double result = thisOp->update(values);
		if (result != expected) {
			std::cout << "   X ";
		}
		else {
			std::cout << "     ";
		}
		std::cout << thisOp->name << " ";
		for (auto val : values) {
			std::cout << val << ", ";
		}
		std::cout << " = " << result << "(" << expected << ")" << std::endl;
		return (result);
	}

	std::shared_ptr<AbstractOpt> getNewOp(int availableInputs_, int row_, int column_, OPTYPES whichOp = OPTYPES::NONE) {
		if (whichOp == OPTYPES::NONE) {
			if (availableOperators.size() == 1) {
				whichOp = availableOperators[0][Random::getIndex(availableOperators[0].size())];
			}
			else {
				whichOp = availableOperators[column_][Random::getIndex(availableOperators[column_].size())];
			}
		}
		switch (whichOp) {
		case OPTYPES::ADD:
			return(std::make_shared<ADDOpt>(ADDOpt(availableInputs_, row_, column_)));
		case OPTYPES::MULT:
			return(std::make_shared<MULTOpt>(MULTOpt(availableInputs_, row_, column_)));
		case OPTYPES::SUBTRACT:
			return(std::make_shared<SUBTRACTOpt>(SUBTRACTOpt(availableInputs_, row_, column_)));
		case OPTYPES::DIVIDE:
			return(std::make_shared<DIVIDEOpt>(DIVIDEOpt(availableInputs_, row_, column_)));
		case OPTYPES::SIN:
			return(std::make_shared<SINOpt>(SINOpt(availableInputs_, row_, column_)));
		case OPTYPES::COS:
			return(std::make_shared<COSOpt>(COSOpt(availableInputs_, row_, column_)));
		case OPTYPES::GREATER:
			return(std::make_shared<GREATEROpt>(GREATEROpt(availableInputs_, row_, column_)));
		case OPTYPES::RAND:
			return(std::make_shared<RANDOpt>(RANDOpt(availableInputs_, row_, column_)));
		case OPTYPES::IF:
			return(std::make_shared<IFOpt>(IFOpt(availableInputs_, row_, column_)));
		case OPTYPES::INV:
			return(std::make_shared<INVOpt>(INVOpt(availableInputs_, row_, column_)));
		case OPTYPES::CONST:
			return(std::make_shared<CONSTOpt>(CONSTOpt(availableInputs_, row_, column_)));
		case OPTYPES::RNN:
			return(std::make_shared<RNNOpt>(RNNOpt(availableInputs_, row_, column_)));
		case OPTYPES::DETGATE:
			return(std::make_shared<DETGATEOpt>(DETGATEOpt(availableInputs_, row_, column_)));
		}
		std::cout << "in getNewOp : bad case with (int)value " << (int)whichOp << "  exiting..." << std::endl;
		exit(1);
	}

	std::vector<std::shared_ptr<AbstractOpt>> copyOpMatrix() {
		std::vector<std::shared_ptr<AbstractOpt>> newOpMatrix(opsMatrixSize);
		
		int c = 0;
		for (auto o : opsMatrix) {
			if (o != nullptr) {
				switch (o->type) {
				case OPTYPES::ADD:
					newOpMatrix[c++] = std::make_shared<ADDOpt>(ADDOpt(*std::dynamic_pointer_cast<ADDOpt>(o)));
					break;
				case OPTYPES::MULT:
					newOpMatrix[c++] = std::make_shared<MULTOpt>(MULTOpt(*std::dynamic_pointer_cast<MULTOpt>(o)));
					break;
				case OPTYPES::SUBTRACT:
					newOpMatrix[c++] = std::make_shared<SUBTRACTOpt>(SUBTRACTOpt(*std::dynamic_pointer_cast<SUBTRACTOpt>(o)));
					break;
				case OPTYPES::DIVIDE:
					newOpMatrix[c++] = std::make_shared<DIVIDEOpt>(DIVIDEOpt(*std::dynamic_pointer_cast<DIVIDEOpt>(o)));
					break;
				case OPTYPES::SIN:
					newOpMatrix[c++] = std::make_shared<SINOpt>(SINOpt(*std::dynamic_pointer_cast<SINOpt>(o)));
					break;
				case OPTYPES::COS:
					newOpMatrix[c++] = std::make_shared<COSOpt>(COSOpt(*std::dynamic_pointer_cast<COSOpt>(o)));
					break;
				case OPTYPES::GREATER:
					newOpMatrix[c++] = std::make_shared<GREATEROpt>(GREATEROpt(*std::dynamic_pointer_cast<GREATEROpt>(o)));
					break;
				case OPTYPES::RAND:
					newOpMatrix[c++] = std::make_shared<RANDOpt>(RANDOpt(*std::dynamic_pointer_cast<RANDOpt>(o)));
					break;
				case OPTYPES::IF:
					newOpMatrix[c++] = std::make_shared<IFOpt>(IFOpt(*std::dynamic_pointer_cast<IFOpt>(o)));
					break;
				case OPTYPES::INV:
					newOpMatrix[c++] = std::make_shared<INVOpt>(INVOpt(*std::dynamic_pointer_cast<INVOpt>(o)));
					break;
				case OPTYPES::CONST:
					newOpMatrix[c++] = std::make_shared<CONSTOpt>(CONSTOpt(*std::dynamic_pointer_cast<CONSTOpt>(o)));
					break;
				case OPTYPES::RNN:
					newOpMatrix[c++] = std::make_shared<RNNOpt>(RNNOpt(*std::dynamic_pointer_cast<RNNOpt>(o)));
					break;
				case OPTYPES::DETGATE:
					newOpMatrix[c++] = std::make_shared<DETGATEOpt>(DETGATEOpt(*std::dynamic_pointer_cast<DETGATEOpt>(o)));
					break;
				}
			}
		}
		return newOpMatrix;
	}

	void testOps() { // test all ops to make sure they work!
		bool testOps = true;
		if (testOps) {
			std::cout << "testing ops..." << std::endl << std::endl;

			std::shared_ptr<AbstractOpt> thisOp;

			thisOp = getNewOp(2, -1, -1, OPTYPES::ADD);
			thisOp->inputs = { 0,1 };
			testOp(thisOp, { 2,1 }, 3);
			testOp(thisOp, { -2,1 }, -1);

			thisOp = getNewOp(2, -1, -1, OPTYPES::MULT);
			thisOp->inputs = { 0,1 };
			testOp(thisOp, { 2,3 }, 6);

			thisOp = getNewOp(2, -1, -1, OPTYPES::SUBTRACT);
			thisOp->inputs = { 0,1 };
			testOp(thisOp, { 2,1 }, 1);
			testOp(thisOp, { -1,1 }, -2);

			thisOp = getNewOp(2, -1, -1, OPTYPES::DIVIDE);
			thisOp->inputs = { 0,1 };
			testOp(thisOp, { 1,2 }, .5);
			testOp(thisOp, { -10,2 }, -5);

			thisOp = getNewOp(1, -1, -1, OPTYPES::SIN);
			testOp(thisOp, { 22.56 }, sin(22.56));

			thisOp = getNewOp(1, -1, -1, OPTYPES::COS);
			testOp(thisOp, { 22.56 }, cos(22.56));

			thisOp = getNewOp(2, -1, -1, OPTYPES::GREATER);
			thisOp->inputs = { 0,1 };
			testOp(thisOp, { 5,2 }, 1);
			testOp(thisOp, { 1,2 }, 0);

			thisOp = getNewOp(2, -1, -1, OPTYPES::RAND);
			thisOp->inputs = { 0,1 };
			testOp(thisOp, { 0,1 }, 0);

			thisOp = getNewOp(2, -1, -1, OPTYPES::IF);
			thisOp->inputs = { 0,1 };
			testOp(thisOp, { -1,2 }, 0);
			testOp(thisOp, { 0,2 }, 0);
			testOp(thisOp, { 1,2 }, 2);

			thisOp = getNewOp(1, -1, -1, OPTYPES::INV);
			testOp(thisOp, { 3 }, -3);

			thisOp = getNewOp(0, -1, -1, OPTYPES::CONST);
			auto x = testOp(thisOp, {}, 0);
			testOp(thisOp, {}, x);

			
			thisOp = getNewOp(8, -1, -1, OPTYPES::RNN);
			auto tempRNNOP = std::dynamic_pointer_cast<RNNOpt>(thisOp);
			tempRNNOP->inputs = { 0,1,2,3,4,5,6,7 };
			tempRNNOP->weights = { 1,-1,1,-1,1,-1,1,.5 };
			tempRNNOP->bias = .2;
			testOp(thisOp, { 1,1,1,1,1,1,1,-1 }, tanh(.2+1-1+1-1+1-1+1-.5));
			
			thisOp = getNewOp(4, -1, -1, OPTYPES::DETGATE);
			auto tempDETGATEOP = std::dynamic_pointer_cast<DETGATEOpt>(thisOp);
			tempDETGATEOP->inputs = { 0,1,2,3 };
			tempDETGATEOP->outputValues = { 0,1,0,1,0,1,1,1,0,1,0,1,0,1,1,1 };
			testOp(thisOp, { 0,0,0,0 }, 0);
			testOp(thisOp, { 0,0,0,1 }, 1);
			testOp(thisOp, { 0,0,1,0 }, 0);
			testOp(thisOp, { 0,0,1,1 }, 1);
			testOp(thisOp, { 0,1,0,0 }, 0);
			testOp(thisOp, { 0,1,0,1 }, 1);
			testOp(thisOp, { 0,1,1,0 }, 1);
			testOp(thisOp, { 0,1,1,1 }, 1);
			testOp(thisOp, { 1,0,0,0 }, 0);
			testOp(thisOp, { 1,0,0,1 }, 1);
			testOp(thisOp, { 1,0,1,0 }, 0);
			testOp(thisOp, { 1,0,1,1 }, 1);
			testOp(thisOp, { 1,1,0,0 }, 0);
			testOp(thisOp, { 1,1,0,1 }, 1);
			testOp(thisOp, { 1,1,1,0 }, 1);
			testOp(thisOp, { 1,1,1,1 }, 1);

			std::cout << "\n\ndone testing..." << std::endl;
			exit(1);
		}
	}

	void discretizeVector(std::vector<double>& vect, int steps, std::vector<double>& range) {
		//std::cout << "A" << std::endl;
		for (size_t i = 0; i < vect.size(); i++) {
			if (steps == 1) {
				//std::cout << " - ";
				vect[i] = Bit(vect[i]);
			}
			else {
				//std::cout << " + ";
				double drr = range[1] - range[0];
				// move value to steps
				vect[i] = std::max(range[0], std::min(range[1], vect[i]));
				vect[i] = ((vect[i] - range[0]) / drr) * steps;
				// use int to discretize
				vect[i] = (double)(int)(vect[i]);

				if (vect[i] == steps) { // if node value was exactly max value, slid to next lower value
					vect[i]--;
				}
				// move back to [0..1] (with / steps-1) and then to [-1...1] (with * 2 - 1)
				vect[i] = (vect[i] / (double)(steps - 1) * drr) + range[0];
			}
		}
		//std::cout << "  B" << std::endl;

	}

	void graphBrain();
	void updateExecuteOrder();

	std::vector<std::shared_ptr<AbstractOpt>> opsMatrix; // list with all ops
	int opsMatrixSize; // 
	std::vector<int> outputLayerAddresses; // list with index for each output
	std::vector<double> currentMatrixValues; // updated value for each input and op
	int currentMatrixValuesSize;
	std::vector<double> nextValues; // output + recurrent
	std::vector<int> executeOrder;


	CGPBrain() = delete;

	CGPBrain(int _nrInNodes, int _nrOutNodes,
		std::shared_ptr<ParametersTable> PT_ = nullptr);
	CGPBrain(int _nrInNodes, int _nrOutNodes,
		std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>& _genomes,
		std::shared_ptr<ParametersTable> PT_ = nullptr);

	virtual ~CGPBrain() = default;

	virtual void update() override;

	virtual std::shared_ptr<AbstractBrain> makeBrain(
		std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>& _genomes) override {
		std::shared_ptr<CGPBrain> newBrain = std::make_shared<CGPBrain>(nrInputValues, nrOutputValues, _genomes, PT);
		return newBrain;
	}

	virtual std::unordered_set<std::string> requiredGenomes() override {
		return {};
	}

	virtual std::string description() override;
	virtual DataMap getStats(std::string& prefix) override;
	virtual std::string getType() override { return "CGP"; }

	virtual void resetBrain() override;
	
	virtual std::shared_ptr<AbstractBrain> makeBrainFrom(std::shared_ptr<AbstractBrain> parent, std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>& _genomes) {
		auto newBrain = parent->makeCopy(PT);


		//std::string name = "CGPBrain";
		
		//auto serialData = serialize(name);
		//serialData.openAndWriteToFile("tempFile.csv");
		
		//serialData = newBrain->serialize(name);
		//serialData.openAndWriteToFile("tempFile.csv");

		newBrain->mutate();

		//serialData = newBrain->serialize(name);
		//serialData.openAndWriteToFile("tempFile.csv");

		return(newBrain);
	}

	virtual DataMap serialize(std::string& name) {
		DataMap serialData;
		std::string outLayerAddressesStr = "";
		for (auto o : outputLayerAddresses) {
			outLayerAddressesStr += std::to_string(o) + "'";
		}
		serialData.set(name + "_outLayerAddresses", outLayerAddressesStr.substr(0, outLayerAddressesStr.size() - 1));
		std::string opsDescs = "";
		for (auto op : opsMatrix) {
			opsDescs += op->serialize() + "|";
		}
		serialData.set(name + "_ops", opsDescs.substr(0,opsDescs.size() - 1));
		std::vector<std::string> opsDescsList;
		convertCSVListToVector(opsDescs, opsDescsList,'|');
		//for (auto o : opsDescsList){
		//	std::cout << o << std::endl;
		//}

		// UNCOMMENT TO DISPLAY BRAINS TO DISPLAY WHEN SAVING
		//std::cout << std::endl;
		//std::cout << std::endl;
		//graphBrain();
		//std::cout << std::endl;
		//std::cout << std::endl;

		return(serialData);
	}

	virtual void deserialize(std::shared_ptr<ParametersTable> PT, std::unordered_map<std::string, std::string>& orgData, std::string& name) {
		
		// from org we need "name"_outLayerAddresses and "name"_ops
		if (orgData.find(name + "_outLayerAddresses") == orgData.end() || orgData.find(name + "_ops") == orgData.end()) {
			std::cout << "  In CGP::deserialize :: can not find either BRAIN_" + name + "_outLayerAddresses or BRAIN_" + name + "_ops.\n  exiting" << std::endl;
			exit(1);
		}		
		convertCSVListToVector(orgData[name + "_outLayerAddresses"], outputLayerAddresses,'\'');
		std::vector<std::string> ops_str;
		convertCSVListToVector(orgData[name + "_ops"], ops_str, '|'); // |,:,'
		int c = 0;
		for (auto o : ops_str) {
			std::vector<std::string> this_op_list;
			convertCSVListToVector(o, this_op_list,':');
			std::string avalibleInputs_row_column_str(this_op_list[1] + "," + this_op_list[2] + "," + this_op_list[3]);
			
			std::vector<int> avalibleInputs_row_column;
			convertCSVListToVector(avalibleInputs_row_column_str, avalibleInputs_row_column);
			// name, availableInputs, row, column, numInputs, outputValues
			opsMatrix[c] = getNewOp(avalibleInputs_row_column[0], avalibleInputs_row_column[1], avalibleInputs_row_column[2], OPTYPES_MAP[this_op_list[0]]);
			opsMatrix[c]->deserializeContents(o);
			c++;
		}
		updateExecuteOrder();

		//std::string garbo = "garbo";
		//auto bs = serialize(garbo);
		//bs.openAndWriteToFile("garbo.txt");
	}

	virtual void mutate() {
		//std::cout << "in mutate..." << std::endl;
		for (auto & cell : opsMatrix) {
			if (Random::P(operatorMutationRate)) {
				//std::cout << "NEW: " << cell->availableInputs << " " << cell->row << " " << cell->column << std::endl;
				cell = getNewOp(cell->availableInputs,cell->row,cell->column);
			}
			else {
				cell->mutate();
			}
		}
		//std::cout << "done mutate (cells)..." << std::endl;

		int numOutputMutations = Random::getBinomial(nrOutputTotal, outputMutationRate);
		for (int i = 0; i < numOutputMutations; i++) {
			outputLayerAddresses[Random::getIndex(nrOutputTotal)] = Random::getIndex(nrInputTotal + opsMatrixSize);
		}
		//std::cout << "done mutate..." << std::endl;
		updateExecuteOrder();
	}
	virtual std::shared_ptr<AbstractBrain> makeCopy(std::shared_ptr<ParametersTable> PT_ = nullptr) override;
	virtual void initializeGenomes(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>& _genomes);
};

inline std::shared_ptr<AbstractBrain>
CGPBrain_brainFactory(int ins, int outs, std::shared_ptr<ParametersTable> PT) {
	return std::make_shared<CGPBrain>(ins, outs, PT);
}
