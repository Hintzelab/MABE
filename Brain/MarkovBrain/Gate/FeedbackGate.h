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

class FeedbackGate : public AbstractGate {
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
  FeedbackGate() = delete;
  FeedbackGate(shared_ptr<ParametersTable> _PT = nullptr) :
  	AbstractGate(_PT) {
  	table = {};
  }
  virtual ~FeedbackGate() = default;
  virtual string gateType() override{
  	return "Feedback";
  }
  virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;
  FeedbackGate(pair<vector<int>, vector<int>> addresses, 
               vector<vector<int>> rawTable, 
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
