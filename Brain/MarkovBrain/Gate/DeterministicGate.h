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

#include "AbstractGate.h"

using namespace std;


class DeterministicGate: public AbstractGate {
public:
	
	static shared_ptr<ParameterLink<string>> IO_RangesPL;

	vector<vector<int>> table;
	DeterministicGate() = delete;
	DeterministicGate(shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
		table = {}; 
	}
	DeterministicGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~DeterministicGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;
	//void setupForBits(int* Ins, int nrOfIns, int Out, int logic);
	virtual string gateType() override{
		return "Deterministic";
	}
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
          return "{\"type\":\"deterministic\","+S+"]}";
        }
};


