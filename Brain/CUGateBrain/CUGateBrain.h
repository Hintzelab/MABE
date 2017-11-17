//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__CUGateBrain__
#define __BasicMarkovBrainTemplate__CUGateBrain__

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

//#include "GateListBuilder/GateListBuilder.h"
#include "../../Genome/AbstractGenome.h"
#include "../../Utilities/Random.h"
#include "../AbstractBrain.h"

#include "hemi/hemi.h"
#include "hemi/array.h"
#include "hemi/launch.h"
#include "hemi/device_api.h"
#include "hemi/parallel_for.h"

using namespace std;

class CUGateBrain : public AbstractBrain {
 protected:
	string recordedList;
 public:


	static shared_ptr<ParameterLink<int>> hiddenNodesPL;
	static shared_ptr<ParameterLink<int>> fanInPL;
	static shared_ptr<ParameterLink<string>> genomeNamePL;
	int hiddenNodes;
	int fanInNr;

	//vector<double> nodes;
	//vector<double> nextNodes;
	int* inputStates;
	bool inputStatesDirty;
	bool outputStatesDirty;
	int* outputStates_h;
	int* hiddenStates_h;
	int* hiddenStates_d;
	int* nextHiddenStates_d;

	struct CUGate {
		int ins[4];
		int logic;
	};

	int nrNodes;
	int nrH,totalN;
	CUGate *CUGates_h;
	CUGate *CUGates_d;

	CUGateBrain() = delete;

	CUGateBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = Parameters::root);
	CUGateBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes, int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = Parameters::root);

	~CUGateBrain(){
#ifdef HEMI_CUDA_COMPILER
		cudaFree(CUGates_d);
		cudaFreeHost(CUGates_h);
		cudaFree(hiddenStates_d);
		cudaFree(nextHiddenStates_d);
		cudaFreeHost(hiddenStates_h);
		cudaFreeHost(outputStates_h);
#else
		delete[] CUGates_h;
		delete[] hiddenStates_h;
		delete[] nextHiddenStates_d;
		delete[] outputStates_h;
#endif
		delete[] inputStates;
	};

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
		inputStates[inputAddress] = floor(value);
		inputStatesDirty = true;
	}
	
	virtual double readInput(const int& inputAddress) override {
		return inputStates[inputAddress];//nodes[inputAddress];
	}
	
	virtual void setOutput(const int& outputAddress, const double& value) override {
		//nodes[nrInputValues+outputAddress]=value;
	}
	
	virtual double readOutput(const int& outputAddress) override  {
		if (outputStatesDirty) {
#ifdef HEMI_CUDA_COMPILER
			cudaMemcpy(outputStates_h, hiddenStates_d+nrInputValues, nrOutputValues*sizeof(int), cudaMemcpyDeviceToHost);
#else
			for (int i=0; i<nrOutputValues; i++) {
				outputStates_h[i] = hiddenStates_d[nrInputValues+i];
			}
#endif
		}
		return outputStates_h[outputAddress];
		//return Random::getDouble(-1,1);//nodes[nrInputValues+outputAddress];
	}


	string getStateString(){
		string S="";
		//for(auto n : nodes)
		//	S+=to_string((int)n);
		return S;
	}
};

inline shared_ptr<AbstractBrain> CUGateBrain_brainFactory(int ins, int outs, shared_ptr<ParametersTable> PT) {
	return make_shared<CUGateBrain>(ins, outs, PT);
}

#endif /* defined(__BasicMarkovBrainTemplate__CUGateBrain__) */
