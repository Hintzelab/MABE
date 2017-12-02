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

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

#include "../../Genome/AbstractGenome.h"

#include "../../Utilities/Random.h"

#include "../AbstractBrain.h"


using namespace std;

class LSTMBrain: public AbstractBrain {
public:

	static shared_ptr<ParameterLink<string>> genomeNamePL;

	string genomeName;

    vector<vector<double>> Wf,Wi,Wc,Wo;
    vector<double> ft,it,Ct,Ot,dt;
    vector<double> bt,bi,bC,bO;
    int _I,_O;
    vector<double> C,X,H;

    
	LSTMBrain() = delete;

	LSTMBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT);

	virtual ~LSTMBrain() = default;

	virtual void update() override;

	virtual shared_ptr<AbstractBrain> makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) override;

	virtual string description() override;
	virtual DataMap getStats(string& prefix) override;
	virtual string getType() override {
		return "LSTM";
	}

	virtual void resetBrain() override;
	virtual void resetOutputs() override;

	virtual void initializeGenomes(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) override;
    
    double fastSigmoid(double value){
        return  value / (1.0 + fabs(value));
    }
    void singleLayerUpdate(vector<double> &IN,vector<double> &out,vector<vector<double>> &W);
    void vectorMathSigmoid(vector<double> &V);
    void vectorMathTanh(vector<double> &V);
    void vectorMathElementalPlus(vector<double> &A,vector<double> &B,vector<double> &result);
    void vectorMathElementalMultiply(vector<double> &A,vector<double> &B,vector<double> &result);
    void showVector(vector<double> &V);
    
    virtual shared_ptr<AbstractBrain> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;

	virtual unordered_set<string> requiredGenomes() override {
		return { genomeNamePL->get(PT) };
	}

};

inline shared_ptr<AbstractBrain> LSTMBrain_brainFactory(int ins, int outs, shared_ptr<ParametersTable> PT) {
	return make_shared<LSTMBrain>(ins, outs, PT);
}

