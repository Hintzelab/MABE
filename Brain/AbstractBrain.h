//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__Brain__
#define __BasicMarkovBrainTemplate__Brain__

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

//#include "Gate/GateBuilder.h"
//#include "GateListBuilder/GateListBuilder.h"
#include "../Genome/AbstractGenome.h"
#include "../Utilities/Parameters.h"
#include "../Global.h"

using namespace std;
class AbstractBrain {
public:
	static shared_ptr<ParameterLink<string>> brainTypeStrPL;

	const shared_ptr<ParametersTable> PT;

	vector<string> aveFileColumns;

	bool recordActivity;
	string recordActivityFileName;

	int nrInputValues;
	int nrOutputValues;
	vector<double> inputValues;
	vector<double> outputValues;

	bool buildFromGenome = true; // should this brain be built from a genome (or self generate)

	AbstractBrain() = delete;

	AbstractBrain(int ins, int outs, shared_ptr<ParametersTable> _PT = nullptr) : PT(_PT) {
		nrInputValues = ins;
		nrOutputValues = outs;
		recordActivity = false;

		inputValues.resize(nrInputValues);
		outputValues.resize(nrOutputValues);

	}

	virtual ~AbstractBrain() = default;

//	virtual static shared_ptr<AbstractBrain> brainFactory(int ins, int outs, int hidden, shared_ptr<ParametersTable> _PT = nullptr){
//		cout << "  You are calling AbstractBrain::brainFactory()... this is not allowed (you can not construct an AbstractBrain!).\n Exiting."<<endl;
//		exit(1);
//	}

	virtual void update() = 0;

	virtual string description() = 0;  // returns a desription of this brain in it's current state
	virtual DataMap getStats() = 0;  // returns a vector of string pairs of any stats that can then be used for data tracking (etc.)

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) = 0;
	virtual void initalizeGenome(shared_ptr<AbstractGenome> _genome) = 0;

	virtual shared_ptr<AbstractBrain> makeMutatedBrainFrom(shared_ptr<AbstractBrain> parent){
		return parent;
	}

	virtual shared_ptr<AbstractBrain> makeMutatedBrainFromMany(vector<shared_ptr<AbstractBrain>> parents){
		return parents[0];
	}

	virtual void inline resetBrain() {
		resetInputs();
		resetOutputs();
	}

	virtual void inline setRecordActivity(bool _recordActivity) {
		recordActivity = _recordActivity;
	}

	virtual void inline setRecordFileName(string _recordActivityFileName) {
		recordActivityFileName = _recordActivityFileName;
	}

	virtual void inline resetOutputs() {
		for (int i = 0; i < nrOutputValues; i++) {
			outputValues[i] = 0.0;
		}
	}

	virtual void inline resetInputs() {
		for (int i = 0; i < nrInputValues; i++) {
			inputValues[i] = 0.0;
		}
	}

	inline void setInput(const int& inputAddress, const double& value) {
		if (inputAddress < nrInputValues) {
			inputValues[inputAddress] = value;
		} else {
			cout << "in Brain::setInput() : Writing to invalid input (" << inputAddress << ") - this brain needs more inputs!\nExiting" << endl;
			exit(1);
		}
	}

	inline double readInput(const int& inputAddress) {
		if (inputAddress < nrInputValues) {
			return inputValues[inputAddress];
		} else {
			cout << "in Brain::readInput() : Reading from invalid input (" << inputAddress << ") - this brain needs more inputs!\nExiting" << endl;
			exit(1);
		}
	}

	inline void setOutput(const int& outputAddress, const double& value) {
		if (outputAddress < nrOutputValues) {
			outputValues[outputAddress] = value;
		} else {
			cout << "in Brain::setOutput() : Writing to invalid output (" << outputAddress << ") - this brain needs more outputs!\nExiting" << endl;
			exit(1);
		}
	}

	inline double readOutput(const int& outputAddress) {
		if (outputAddress < nrOutputValues) {
			return outputValues[outputAddress];
		} else {
			cout << "in Brain::readOutput() : Reading from invalid output (" << outputAddress << ") - this brain needs more outputs!\nExiting" << endl;
			exit(1);
		}
	}

//	// converts the value of each value in nodes[] to bit and converts the bits to an int
//	// useful to generate values for lookups, useful for caching
//	int allNodesToBitToInt() {
//		int result = 0;
//		for (int i = 0; i < nrOfBrainNodes; i++)
//			result = (result) + Bit(nodes[i]);
//		return result;
//
//	}

	virtual shared_ptr<AbstractBrain> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) {
		cout << "ERROR IN AbstractBrain::makeCopy() - You are using the abstract copy constructor for brains. You must define your own" << endl;
		exit(1);
	}

	virtual bool requireGenome(){
		return true;
	}
};

#endif /* defined(__BasicMarkovBrainTemplate__Brain__) */
