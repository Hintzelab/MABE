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

#include <deque>
#include <functional>
#include <math.h>
#include <memory>
#include <stdlib.h>
#include <vector>
#include <iostream>

#include <Genome/AbstractGenome.h>
#include <Global.h>

#include <Utilities/Parameters.h>
#include <Utilities/Random.h>

class AbstractGate {
public:
	int ID;
	const std::shared_ptr<ParametersTable> PT;

	static const int START_CODE = 0;
	static const int IN_COUNT_CODE = 10;
	static const int IN_ADDRESS_CODE = 11;
	static const int OUT_COUNT_CODE = 20;
	static const int OUT_ADDRESS_CODE = 21;
	static const int DATA_CODE = 30;
	AbstractGate() = delete;
	AbstractGate(std::shared_ptr<ParametersTable> _PT = nullptr) : PT(_PT) {
		ID = 0;
	}
	virtual ~AbstractGate() = default;

	virtual std::shared_ptr<AbstractGate> makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr);
	std::vector<int> inputs;
	std::vector<int> outputs;

	virtual void applyNodeMap(std::vector<int> nodeMap, int maxNodes);  // converts genome values into brain state value addresses
	virtual void resetGate(void);  // this is empty here. Some gates so not need to reset, they can use this method.
	virtual std::vector<int> getIns();  // returns a vector of int with the adress for this gates input brain state value addresses
	virtual std::vector<int> getOuts();  // returns a vector of int with the adress for this gates onput brain state value addresses
	virtual void update(std::vector<double> & states, std::vector<double> & nextStates) = 0;  // the function is empty, and must be provided in any derived gates
	// returns a description of the gate. This method can be used in derived gates description method to return ins and outs and coding regions
	virtual std::string descriptionIO();  // returns a description of the gate. This method can be used in derived gates description method to return ins and outs and coding regions
	virtual std::string description();  // returns a description of the gate. This method can be used in derived gates description method to return ins and outs and coding regions
	virtual std::string gateType() {
		return "undefined";
	}
	virtual std::pair<std::vector<int>, std::vector<int>> getConnectionsLists(){
		std::pair<std::vector<int>, std::vector<int>> connectionsLists;
		connectionsLists.first = inputs;
		connectionsLists.second = outputs;
		return connectionsLists;
	}
        virtual std::string getTPMdescription(){
            return "{\"type\":\"undescribed\"}";
        }
};
