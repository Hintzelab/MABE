//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__MTree__
#define __BasicMarkovBrainTemplate__MTree__

#include <cwctype>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <unordered_set>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

#include "Utilities.h"
#include "Parameters.h"
#include "Data.h"

using namespace std;

class Abstract_MTree {
public:

	shared_ptr<ParametersTable> PT;
	shared_ptr<DataMap> dataMap;

	enum OPERATIONS {
		MULTI, CONST, fromDataMap, fromParameterTable, SUM, ADD, MULT, SUBTRACT, DIVIDE, SIN, COS
	};

	Abstract_MTree(shared_ptr<ParametersTable> _PT, shared_ptr<DataMap> _dataMap):PT(_PT),dataMap(_dataMap){


	};
	virtual ~Abstract_MTree() = default;

	virtual vector<double> eval() = 0;
};

class MULTI_MTree: public Abstract_MTree {
public:
	vector<shared_ptr<Abstract_MTree>> branches;

	MULTI_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<ParametersTable> _PT = nullptr, shared_ptr<DataMap> _dataMap = nullptr) :
		Abstract_MTree(_PT,_dataMap), branches (_branches){
	}
	virtual ~MULTI_MTree() = default;

	virtual vector<double> eval() override {
		vector<double> output;
		for (auto b : branches){
			output.push_back(b->eval()[0]);
		}
		return output;
	}
};

class CONST_MTree: public Abstract_MTree {
public:
	double value;

	CONST_MTree(double _value, shared_ptr<ParametersTable> _PT = nullptr, shared_ptr<DataMap> _dataMap = nullptr) :
		Abstract_MTree(_PT,_dataMap), value(_value) {
	}
	virtual ~CONST_MTree() = default;

	virtual vector<double> eval() override{
		vector<double> output;
		output.push_back(value);
		return output;
	}
};

class fromDataMap_MTree: public Abstract_MTree {
public:

	string key;

	fromDataMap_MTree(string _key, shared_ptr<ParametersTable> _PT = nullptr, shared_ptr<DataMap> _dataMap = nullptr) :
		Abstract_MTree(_PT,_dataMap), key(_key) {
	}
	virtual ~fromDataMap_MTree() = default;

	virtual vector<double> eval() override{
		vector<double> output;
		output.push_back(dataMap->GetAverage(key));
		return output;
	}
};

class SUM_MTree: public Abstract_MTree {
public:
	vector<shared_ptr<Abstract_MTree>> branches;

	SUM_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<ParametersTable> _PT = nullptr, shared_ptr<DataMap> _dataMap = nullptr) :
		Abstract_MTree(_PT,_dataMap), branches (_branches){
	}
	virtual ~SUM_MTree() = default;

	virtual vector<double> eval() override{
		vector<double> output;
		output.push_back(0);
		for (auto b : branches){
			output[0]+=b->eval()[0];
		}
		return output;
	}
};




#endif /* defined(__BasicMarkovBrainTemplate__MTree__) */
