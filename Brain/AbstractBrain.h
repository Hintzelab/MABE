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
	static shared_ptr<ParameterLink<int>> hiddenNodesPL;
	static shared_ptr<ParameterLink<bool>> serialProcessingPL;

	const shared_ptr<ParametersTable> PT;

	vector<string> aveFileColumns;

	bool recordActivity;
	string recordActivityFileName;

	int nrOfBrainNodes;  // the number of states in THIS brain
						 // this is temporary! new node/memory/dataCell will fix this.
	int nrInNodes;
	int nrOutNodes;
	int nrHiddenNodes;
	vector<int> inputNodesList, outputNodesList;  //maps inputs to nodes and outputs to nodes
	vector<double> nodes;
	vector<double> nextNodes;

//	AbstractBrain() {
//		nrInNodes = nrOutNodes = nrHiddenNodes = nrOfBrainNodes = 0;
//		recordActivity = false;
//		cout << "ERROR: attempting to construct brain with no arguments. Check brain type for required parameters... most likely at least #in, #out and #hidden are required!\n\nExiting.\n" << endl;
//		exit(1);
//	}

	AbstractBrain() = delete;

	AbstractBrain(int ins, int outs, int hidden, shared_ptr<ParametersTable> _PT = nullptr) : PT(_PT) {
		nrInNodes = ins;
		nrOutNodes = outs;
		nrHiddenNodes = hidden;
		recordActivity = false;

		// setup default input and output nodes lists
		for (int i = 0; i < nrInNodes; i++) {
			inputNodesList.push_back(i);
		}
		for (int i = nrInNodes; i < nrInNodes + nrOutNodes; i++) {
			outputNodesList.push_back(i);
		}
		nrOfBrainNodes = ins + outs + hidden;
		nodes.resize(nrOfBrainNodes);
		nextNodes.resize(nrOfBrainNodes);

	}

	virtual ~AbstractBrain() = default;

//	virtual static shared_ptr<AbstractBrain> brainFactory(int ins, int outs, int hidden, shared_ptr<ParametersTable> _PT = nullptr){
//		cout << "  You are calling AbstractBrain::brainFactory()... this is not allowed (you can not construct an AbstractBrain!).\n Exiting."<<endl;
//		exit(1);
//	}

	virtual void update() = 0;

	virtual string description() = 0;  // returns a desription of this brain in it's current state
	virtual vector<string> getStats() =0;  // returns a vector of string pairs of any stats that can then be used for data tracking (etc.)

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) = 0;
	virtual void initalizeGenome(shared_ptr<AbstractGenome> _genome) = 0;

	virtual void inline resetBrain() {
		for (int i = 0; i < nrOfBrainNodes; i++) {
			nodes[i] = 0.0;
		}
	}

	virtual void inline setRecordActivity(bool _recordActivity) {
		recordActivity = _recordActivity;
	}

	virtual void inline setRecordFileName(string _recordActivityFileName) {
		recordActivityFileName = _recordActivityFileName;
	}

	virtual void inline resetOutputs() {
//		for (int i = nrInNodes; i < (nrInNodes + nrOutNodes); i++) {
//			nodes[i] = 0.0;
//		}
		for (auto i : outputNodesList) {
			nodes[i] = 0.0;
		}
	}

	inline void setInput(const int& nodeAddress, const double& value) {
		if (nodeAddress < nrInNodes) {
			//nodes[nodeAddress] = value;
			nodes[inputNodesList[nodeAddress]] = value;
		} else {
			cout << "in Brain::setInput() : Writing to invalid input node (" << nodeAddress << ") - this brain needs more input nodes!\nExiting" << endl;
			exit(1);
		}
	}

	inline void setNode(const int& nodeAddress, const double& value) {
		if (nodeAddress < nrInNodes + nrOutNodes + nrHiddenNodes) {
			nodes[nodeAddress] = value;
		} else {
			cout << "in Brain::setNode() :Writing to invalid input node (" << nodeAddress << ") - this brain needs more nodes!\nExiting" << endl;
			exit(1);
		}
	}

	inline double readInput(const int& nodeAddress) {
		if (nodeAddress < nrInNodes) {
			//return nodes[nodeAddress];
			return nodes[inputNodesList[nodeAddress]];
		} else {
			cout << "in Brain::readInput() : Reading from invalid input node (" << nodeAddress << ") - this brain needs more input nodes!\nExiting" << endl;
			exit(1);
		}
	}

	inline double readOutput(const int& nodeAddress) {
		if (nodeAddress < nrOutNodes) {
			//return nodes[nodeAddress + nrInNodes];
			return nodes[outputNodesList[nodeAddress]];
		} else {
			cout << "in Brain::readOutput() : Reading from invalid output node (" << nodeAddress << ") - this brain needs more output nodes!\nExiting" << endl;
			exit(1);
		}
	}

	inline double ReadNode(const int& nodeAddress) {
		if (nodeAddress < nrOfBrainNodes) {
			return nodes[nodeAddress];
		} else {
			cout << "in Brain::ReadNode() :Writing to invalid input node (" << nodeAddress << ") - this brain needs more nodes!\nExiting" << endl;
			exit(1);
		}
	}

	inline vector<double> ReadAllNodes() {
		return nodes;
	}

	// converts the value of each value in nodes[] to bit and converts the bits to an int
	// useful to generate values for lookups, useful for caching
	int allNodesToBitToInt() {
		int result = 0;
		for (int i = 0; i < nrOfBrainNodes; i++)
			result = (result) + Bit(nodes[i]);
		return result;

	}

	void setOutputNodesList(vector<int> nodesList) {
		if ((int) nodesList.size() == nrOutNodes) {
			outputNodesList = nodesList;
		} else {
			cout << "ERROR: In setOutputNodesList(). size of nodesList does not match nrOutNodes.\n\nExiting.\n" << endl;
			exit(1);
		}
	}

	void setInputNodesList(vector<int> nodesList) {
		if ((int) nodesList.size() == nrInNodes) {
			inputNodesList = nodesList;
		} else {
			cout << "ERROR: In setInputNodesList(). size of nodesList does not match nrInNodes.\n\nExiting.\n" << endl;
			exit(1);
		}
	}

	vector<int> getOutputNodesList() {
		return outputNodesList;
	}

	vector<int> getInputNodesList() {
		return inputNodesList;
	}
	virtual shared_ptr<AbstractBrain> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) {
		cout << "ERROR IN AbstractBrain::makeCopy() - You are using the abstract copy constructor for brains. You must define your own" << endl;
		exit(1);
	}
};

#endif /* defined(__BasicMarkovBrainTemplate__Brain__) */
