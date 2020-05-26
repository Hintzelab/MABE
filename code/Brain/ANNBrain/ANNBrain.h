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
#include <string>
#include <algorithm>

#include <Genome/AbstractGenome.h>
#include <Utilities/Random.h>
#include <Brain/AbstractBrain.h>


//using namespace std;

class ANNBrain: public AbstractBrain {
public:

	static std::shared_ptr<ParameterLink<int>> nrOfRecurringNodesPL;
	static std::shared_ptr<ParameterLink<int>> nrOfHiddenLayersPL;
	static std::shared_ptr<ParameterLink<std::string>> hiddenLayerSizesPL;
	static std::shared_ptr<ParameterLink<std::string>> biasRangePL;
	static std::shared_ptr<ParameterLink<std::string>> weightRangePL;
	static std::shared_ptr<ParameterLink<std::string>> thresholdMethodPL;
	static std::shared_ptr<ParameterLink<std::string>> genomeNamePL;

	int nrOfRecurringNodes;
	int nrOfHiddenLayers;
	std::vector<double> weightRange;
	std::vector<double> biasRange;

	enum ThresholdMethods { NONE, Sigmoid, Tanh, ReLU, Binary};
	ThresholdMethods thresholdMethod;

	std::string genomeName;
	std::vector<int> hiddenLayerSizes;

    int _I,_O;
	std::vector<std::vector<double>> nodes, biases;
	std::vector<std::vector<std::vector<double>>> weights;
	ANNBrain() = delete;

	ANNBrain(int _nrInNodes, int _nrOutNodes, std::shared_ptr<ParametersTable> _PT = Parameters::root);

	virtual ~ANNBrain() = default;

	virtual void update() override;

	virtual std::shared_ptr<AbstractBrain> makeBrain(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>& _genomes) override;

	virtual std::string description() override;
	virtual DataMap getStats(std::string& prefix) override;
	virtual std::string getType() override { return "ANN"; }

	virtual void resetBrain() override;
	virtual void resetOutputs() override;

	virtual void initializeGenomes(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>& _genomes) override;
        
    void vectorMathSigmoid(std::vector<double> &V);
	void vectorMathTanh(std::vector<double> &V);
	void vectorMathReLU(std::vector<double> &V);
	void vectorMathBinary(std::vector<double> &V);

    virtual std::shared_ptr<AbstractBrain> makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr) override;

	virtual std::unordered_set<std::string> requiredGenomes() override {
		return {genomeNamePL->get(PT)};
	}
	inline void setInput(const int& inputAddress, const double& value);


	inline virtual void setAllInputs(const std::vector<double>& values) {
		if (values.size() == nrInputValues) {
			nodes[0] = values;
		}
		else {
			std::cout << "in ANNBrain::setAllInputs() : Size of provided vector (" << values.size() << ") does not match number of brain inputs (" << nrInputValues << ").\nExiting"
				<< std::endl;
			exit(1);
		}
	}

	inline virtual std::vector<double> readAllOutputs() {
		return nodes[(int)nodes.size() - 1];
	}

	inline double readInput(const int& inputAddress);
    inline void setOutput(const int& outputAddress, const double& value);
    inline double readOutput(const int& outputAddress);
    virtual void getAllBrainStates(std::vector<double> &I, std::vector<double> &O, std::vector<double> &H) {
        H.insert(H.begin(),nodes[1].begin()+nrOutputValues,nodes[1].end());
        O.insert(O.begin(),nodes[1].begin(),nodes[1].begin()+nrOutputValues);
        I.insert(I.begin(),nodes[0].begin(),nodes[0].begin()+nrInputValues);
    }

    void showBrain();
};

inline std::shared_ptr<AbstractBrain> ANNBrain_brainFactory(int ins, int outs, std::shared_ptr<ParametersTable> PT) {
	return std::make_shared<ANNBrain>(ins, outs, PT);
}



