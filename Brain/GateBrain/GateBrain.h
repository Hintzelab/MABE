//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__GateBrain__
#define __BasicMarkovBrainTemplate__GateBrain__

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

//#include "GateListBuilder/GateListBuilder.h"
#include "../../Genome/AbstractGenome.h"

#include "../../Utilities/Random.h"

#include "../AbstractBrain.h"

using namespace std;

class SimpleLogicGate{
public:
	vector<int> I;
	int fanInNr,O;
	int logic;
	
		SimpleLogicGate(vector<int> &_I,int _fanIn,int _O,int _logic){
		fanInNr=_fanIn;
		I.resize(fanInNr);
		for(int i=0;i<fanInNr;i++)
			I[i]=_I[i];
		O=_O;
		logic=_logic;
	}
	
	void update(vector<double> &nodes,vector<double> &nextNodes){
        /// Turn bit input from fan-in nodes into a single number
		int theInput=0;
		for(int i=0;i<fanInNr;i++){
			theInput=(theInput<<1)+Bit(nodes[I[i]]);
		}
        /// provide input to the next gate, as
        /// (a random number) shr theInputNumber
		nextNodes[O]=(double)((logic>>theInput)&1);
	}

};

class GateBrain : public AbstractBrain {
 protected:
	vector<shared_ptr<SimpleLogicGate>> gates;
	string recordedList;
 public:


	static shared_ptr<ParameterLink<int>> hiddenNodesPL;
	static shared_ptr<ParameterLink<int>> fanInPL;
	static shared_ptr<ParameterLink<string>> genomeNamePL;
	int hiddenNodes;
	int fanInNr;

	vector<double> nodes;
	vector<double> nextNodes;

	int nrNodes;
	int nrH,totalN;


	GateBrain() = delete;

	GateBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = Parameters::root);
	GateBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes, int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = Parameters::root);

	~GateBrain() = default;

	virtual shared_ptr<AbstractBrain> makeCopy(shared_ptr<ParametersTable> _PT = Parameters::root) override;


	virtual void update() override;


	virtual shared_ptr<AbstractBrain> makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) override;

	virtual string description() override;
	virtual DataMap getStats(string& prefix) override;

	virtual void resetBrain() override;
	virtual string gateList();
	virtual vector<vector<int>> getConnectivityMatrix();
	virtual int brainSize();

	virtual void initializeGenomes(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes);
	
	virtual void setInput(const int& inputAddress, const double& value) override  {
		nodes[inputAddress]=value;
	}
	
	virtual double readInput(const int& inputAddress) override {
		return nodes[inputAddress];
	}
	
	virtual void setOutput(const int& outputAddress, const double& value) override {
		nodes[nrInputValues+outputAddress]=value;
	}
	
	virtual double readOutput(const int& outputAddress) override  {
		return nodes[nrInputValues+outputAddress];
	}


	string getStateString(){
		string S="";
		for(auto n : nodes)
			S+=to_string((int)n);
		return S;
	}
};

inline shared_ptr<AbstractBrain> GateBrain_brainFactory(int ins, int outs, shared_ptr<ParametersTable> PT) {
	return make_shared<GateBrain>(ins, outs, PT);
}

#endif /* defined(__BasicMarkovBrainTemplate__GateBrain__) */
