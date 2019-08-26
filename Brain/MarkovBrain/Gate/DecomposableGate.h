//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#pragma once

#include "AbstractGate.h"

using namespace std;


class DecomposableGate: public AbstractGate {  //conventional probabilistic gate
public:

	static shared_ptr<ParameterLink<string>> IO_RangesPL;

	vector<vector<double>> table;
        vector<vector<double>> factorList;
	DecomposableGate() = delete;
	DecomposableGate(shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
		table = {};
	}
	DecomposableGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _rawTable, int _ID, vector<vector<double>> factorsListHandover, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~DecomposableGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string gateType() override{
		return "Decomposable";
	}
	virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;
        virtual std::string getTPMdescription() override{
          std::string S="";
          S+="\"ins\":[";
          for(int i=0;i<inputs.size();i++){
            if(i!=0)
              S+=",";
            S+=to_string(inputs[i]);
          }
          S+="],\"outs\":[";
          for(int o=0;o<outputs.size();o++){
            if(o!=0)
              S+=",";
            S+=to_string(outputs[o]);
          }
          S+="],\"logic\":[";
          for(int i=0;i<(int)table.size();i++){
            if(i!=0)
              S+=",";
            S+="[";
            for(int o=0;o<(int)table[i].size();o++){
              if(o!=0)
                S+=",";
              S+=to_string(table[i][o]);
            }
            S+="]";
          }
          S+="],\"rawFactors\":[";
          for(int i=0;i<(int)factorList.size();i++){
            if(i!=0)
              S+=",";
            S+="[";
            for(int o=0;o<(int)factorList[i].size();o++){
              if(o!=0)
                S+=",";
              S+=to_string(factorList[i][o]);
            }
            S+="]";
          }

          return "{\"type\":\"decomposable\","+S+"]}";
        }
};

