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

class DecomposableGate: public AbstractGate {  //conventional probabilistic gate
public:

	static std::shared_ptr<ParameterLink<std::string>> IO_RangesPL;

    std::vector<std::vector<double>> table;
    std::vector<std::vector<double>> factorList;
	DecomposableGate() = delete;
	DecomposableGate(std::shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
		table = {};
	}
	DecomposableGate(std::pair<std::vector<int>, std::vector<int>> addresses, std::vector<std::vector<int>> _rawTable, int _ID, std::vector<std::vector<double>> factorsListHandover, std::shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~DecomposableGate() = default;
	virtual void update(std::vector<double> & states, std::vector<double> & nextStates) override;
	virtual std::string gateType() override{
		return "Decomposable";
	}
	virtual std::shared_ptr<AbstractGate> makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr) override;
        virtual std::string getTPMdescription() override{
          std::string S="";
          S+="\"ins\":[";
          for(int i=0;i<inputs.size();i++){
            if(i!=0)
              S+=",";
            S+= std::to_string(inputs[i]);
          }
          S+="],\"outs\":[";
          for(int o=0;o<outputs.size();o++){
            if(o!=0)
              S+=",";
            S+= std::to_string(outputs[o]);
          }
          S+="],\"logic\":[";
          for(int i=0;i<(int)table.size();i++){
            if(i!=0)
              S+=",";
            S+="[";
            for(int o=0;o<(int)table[i].size();o++){
              if(o!=0)
                S+=",";
              S+= std::to_string(table[i][o]);
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
              S+= std::to_string(factorList[i][o]);
            }
            S+="]";
          }

          return "{\"type\":\"decomposable\","+S+"]}";
        }
};
