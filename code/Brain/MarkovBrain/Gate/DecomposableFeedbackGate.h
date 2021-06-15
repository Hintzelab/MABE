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


class DecomposableFeedbackGate : public AbstractGate {
 public:
  unsigned int posFBNode, negFBNode;
  unsigned char nrPos, nrNeg;
  std::vector<double> posLevelOfFB, negLevelOfFB;
  std::deque<unsigned char> chosenInPos, chosenInNeg, chosenOutPos, chosenOutNeg;
  std::vector<double> appliedPosFB;
  std::vector<double> appliedNegFB;

  static bool feedbackON;
  static std::shared_ptr<ParameterLink<std::string>> IO_RangesPL;
  
  std::vector<std::vector<double>> table;
  std::vector<std::vector<double>> originalTable;
  std::vector<std::vector<double>> factors;
  int ins,outs;
  DecomposableFeedbackGate() = delete;
  DecomposableFeedbackGate(std::shared_ptr<ParametersTable> _PT = nullptr) :
  	AbstractGate(_PT) {
  	table = {};
  }
  virtual ~DecomposableFeedbackGate() = default;
  virtual std::string gateType() override{
  	return "DecomposableFeedback";
  }
  /*
   * convert rawTable to double
   * construct rawTable of decomposables in gateBuilder as usual
   * pass factors in with constructor to DecomposableFeedbackGate
   * on update w/feedback, choose which factor to modify randomly
   * * modify the factor up or down depending if its contribution is (a*x) or ((1-a)*x)
   * for the affected output column determined by feedback
  */
  virtual std::shared_ptr<AbstractGate> makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr) override;
  DecomposableFeedbackGate(std::pair<std::vector<int>, std::vector<int>> addresses,
      std::vector<std::vector<int>> rawTable,
      std::vector<std::vector<double>> factors,
               unsigned int _posFBNode, 
               unsigned int _negFBNode, 
               unsigned char _nrPos, 
               unsigned char _nrNeg, 
      std::vector<double> _posLevelOfFB,
      std::vector<double> _negLevelOfFB,
      int _ID, 
      std::shared_ptr<ParametersTable> _PT);
  virtual std::string description();
  virtual void update(std::vector<double> & states, std::vector<double> & nextStates) override;
  virtual void applyNodeMap(std::vector<int> nodeMap, int maxNodes);
  virtual void resetGate(void);
  virtual std::vector<int> getIns();
  //virtual double computeGateRMS();
  //virtual double computeMutualInfo();
  virtual std::string getAppliedPosFeedback();
  virtual std::string getAppliedNegFeedback();
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
    return "{\"type\":\"decomposableFeedback\",\"posFBnode\":"+ std::to_string(posFBNode)+",\"negFBnode\":"+ std::to_string(negFBNode)+","+S+"]}";
  }
};
