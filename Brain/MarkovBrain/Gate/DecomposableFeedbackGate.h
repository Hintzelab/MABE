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

class DecomposableFeedbackGate : public AbstractGate {
 public:
  unsigned int posFBNode, negFBNode;
  unsigned char nrPos, nrNeg;
  vector<double> posLevelOfFB, negLevelOfFB;
  deque<unsigned char> chosenInPos, chosenInNeg, chosenOutPos, chosenOutNeg;
  vector<double> appliedPosFB;
  vector<double> appliedNegFB;

  static bool feedbackON;
  static shared_ptr<ParameterLink<string>> IO_RangesPL;
  
  vector<vector<double>> table;
  vector<vector<double>> originalTable;
  vector<vector<double>> factors;
  int ins,outs;
  DecomposableFeedbackGate() = delete;
  DecomposableFeedbackGate(shared_ptr<ParametersTable> _PT = nullptr) :
  	AbstractGate(_PT) {
  	table = {};
  }
  virtual ~DecomposableFeedbackGate() = default;
  virtual string gateType() override{
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
  virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;
  DecomposableFeedbackGate(pair<vector<int>,vector<int>> addresses, 
               vector<vector<int>> rawTable, 
               vector<vector<double>> factors, 
               unsigned int _posFBNode, 
               unsigned int _negFBNode, 
               unsigned char _nrPos, 
               unsigned char _nrNeg, 
               vector<double> _posLevelOfFB, 
               vector<double> _negLevelOfFB, 
               int _ID, 
               shared_ptr<ParametersTable> _PT);
  virtual string description();
  virtual void update(vector<double> & states, vector<double> & nextStates) override;
  virtual void applyNodeMap(vector<int> nodeMap, int maxNodes);
  virtual void resetGate(void);
  virtual vector<int> getIns();
  //virtual double computeGateRMS();
  //virtual double computeMutualInfo();
  virtual string getAppliedPosFeedback();
  virtual string getAppliedNegFeedback();
};
