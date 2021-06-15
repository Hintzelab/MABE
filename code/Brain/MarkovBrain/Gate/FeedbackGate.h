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

class FeedbackGate : public AbstractGate {
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
  FeedbackGate() = delete;
  FeedbackGate(std::shared_ptr<ParametersTable> _PT = nullptr) :
  	AbstractGate(_PT) {
  	table = {};
  }
  virtual ~FeedbackGate() = default;
  virtual std::string gateType() override{
  	return "Feedback";
  }
  virtual std::shared_ptr<AbstractGate> makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr) override;
  FeedbackGate(std::pair<std::vector<int>, std::vector<int>> addresses,
      std::vector<std::vector<int>> rawTable,
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
};
