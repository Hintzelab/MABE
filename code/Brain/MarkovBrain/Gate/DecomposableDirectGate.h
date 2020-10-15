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

class DecomposableDirectGate: public AbstractGate {  //conventional probabilistic gate
public:

	static std::shared_ptr<ParameterLink<std::string>> IO_RangesPL;

    std::vector<std::vector<double>> factorsList;
    DecomposableDirectGate() = delete;
	DecomposableDirectGate(std::shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
		factorsList = {};
	}
	DecomposableDirectGate(std::pair<std::vector<int>, std::vector<int>> addresses, std::vector<std::vector<double>> factorsList, int _ID, std::shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~DecomposableDirectGate() = default;
	virtual void update(std::vector<double> & states, std::vector<double> & nextStates) override;
	virtual std::string gateType() override{
		return "DecomposableDirect";
	}
	virtual std::shared_ptr<AbstractGate> makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr) override;
  virtual std::string getTPMdescription() {
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
    for(int i=0;i<(double)factorsList.size();i++){
      if(i!=0)
        S+=",";
      S+="[";
      for(int o=0;o<(double)factorsList[i].size();o++){
        if(o!=0)
          S+=",";
        S+= std::to_string(factorsList[i][o]);
      }
      S+="]";
    }
    return "{\"type\":\"decomposableDirect\","+S+"]}";
  }
};

