//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "DecomposableFeedbackGate.h"

#include <numeric> // for accumulate
#include <bitset>

#include <Utilities/Random.h>
#include <Utilities/Utilities.h>

bool DecomposableFeedbackGate::feedbackON = true;
std::shared_ptr<ParameterLink<std::string>> DecomposableFeedbackGate::IO_RangesPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_DECOMPOSABLE_FEEDBACK-IO_Ranges", (std::string)"1-4,1-4", "range of number of inputs and outputs (min inputs-max inputs,min outputs-max outputs)");

DecomposableFeedbackGate::DecomposableFeedbackGate(std::pair<std::vector<int>, std::vector<int>> addresses,
    std::vector<std::vector<int>> rawTable,
    std::vector<std::vector<double>> factors,
        unsigned int _posFBNode, 
        unsigned int _negFBNode, 
        unsigned char _nrPos, 
        unsigned char _nrNeg, 
    std::vector<double> _posLevelOfFB,
    std::vector<double> _negLevelOfFB,
        int _ID, 
    std::shared_ptr<ParametersTable> _PT) :
	AbstractGate(_PT), factors(factors), ins(addresses.first.size()), outs(addresses.second.size()) {

  ID = _ID;
  posLevelOfFB = _posLevelOfFB;
  negLevelOfFB = _negLevelOfFB;
  posFBNode = _posFBNode;
  negFBNode = _negFBNode;
  nrPos = _nrPos;
  nrNeg = _nrNeg;

  int i, j;
  inputs = addresses.first;
  outputs = addresses.second;

  int numInputs = inputs.size();
  int numOutputs = outputs.size();

  table.resize(1 << numInputs);
  //normalize each row
  for (i = 0; i < (1 << numInputs); i++) {  //for each row (each possible input bit string)
      table[i].resize(1 << numOutputs);
      // first sum the row
      double S = 0;
      for (j = 0; j < (1 << numOutputs); j++) {
          S += rawTable[i][j];
      }
      // now normalize the row
      if (S == 0.0) {  //if all the inputs on this row are 0, then give them all a probability of 1/(2^(number of outputs))
          for (j = 0; j < (1 << numOutputs); j++)
              table[i][j] = 1.0 / (double) (1 << numOutputs);
      } else {  //otherwise divide all values in a row by the sum of the row
          for (j = 0; j < (1 << numOutputs); j++)
              table[i][j] = rawTable[i][j] / S;
      }
  }
  originalTable = table; // initial copy

  chosenInPos.clear();
  chosenInNeg.clear();
  chosenOutPos.clear();
  chosenOutNeg.clear();
}

void DecomposableFeedbackGate::update(std::vector<double> & states, std::vector<double> & nextStates) {
  size_t i;
  double mod;
  std::bitset<64> bs(0);

  int numFactors(factors[0].size());
  int randomFactori(0);
  /// positive feedback
  /// NOTES: where is the chosen rowi,coli?
  if ((feedbackON) && (nrPos != 0) && (states[posFBNode] > 0.0)) {
      for (i = 0; i < chosenInPos.size(); i++) {
          randomFactori = rand()%numFactors;
          mod = Random::getDouble(1) * posLevelOfFB[i];
          appliedPosFB.push_back(mod);
          if (((chosenOutPos[i]>>randomFactori)&1) == 1) {
              factors[chosenInPos[i]][randomFactori] += mod;
          } else {
              factors[chosenInPos[i]][randomFactori] -= mod;
              factors[chosenInPos[i]][randomFactori] = std::max(factors[chosenInPos[i]][randomFactori],0.);
          }
          int rowi(chosenInPos[i]);
          for (int outputi=0; outputi<table[rowi].size(); outputi++) {
              double p(1.0);
              bs=outputi;
              /// loop through bits in each output and multiply
              /// the appropriate factors together in the right way (1-p) for bit=0 vs p for bit=1
              for (int biti=0; biti<outs; biti++) {
                  p *= (bs[biti] ? factors[rowi][biti] : 1-factors[rowi][biti]);
              }
              table[rowi][outputi] = p;
          }
          double rowSum( accumulate(begin(table[chosenInPos[i]]), end(table[chosenInPos[i]]), 0.) ); /// sum row
          for (auto& number : table[chosenInPos[i]]) number /= rowSum; /// divide row by sum
          //table[chosenInPos[i]][chosenOutPos[i]] += mod;
          //double s = 0.0;
          //for (size_t k = 0; k < table[chosenInPos[i]].size(); k++)
          //  s += table[chosenInPos[i]][k];
          //for (size_t k = 0; k < table[chosenInPos[i]].size(); k++)
          //  table[chosenInPos[i]][k] /= s;
      }
  }
    /// negative feedback
  if ((feedbackON) && (nrNeg != 0) && (states[negFBNode] > 0.0)) {
      for (i = 0; i < chosenInNeg.size(); i++) {
          randomFactori = rand()%numFactors;
          mod = Random::getDouble(1) * negLevelOfFB[i];
          appliedNegFB.push_back(mod);
          if (((chosenOutNeg[i]>>randomFactori)&1) == 1) {
              factors[chosenInNeg[i]][randomFactori] -= mod;
              factors[chosenInNeg[i]][randomFactori] = std::max(factors[chosenInNeg[i]][randomFactori],0.);
          } else {
              factors[chosenInNeg[i]][randomFactori] += mod;
          }
          int rowi(chosenInNeg[i]);
          for (int outputi=0; outputi<table[rowi].size(); outputi++) {
              double p(1.0);
              bs=outputi;
              /// loop through bits in each output and multiply
              /// the appropriate factors together in the right way (1-p) for bit=0 vs p for bit=1
              for (int biti=0; biti<outs; biti++) {
                  p *= (bs[biti] ? factors[rowi][biti] : 1-factors[rowi][biti]);
              }
              table[rowi][outputi] = p;
          }
          double rowSum( accumulate(begin(table[chosenInNeg[i]]), end(table[chosenInNeg[i]]), 0.) ); /// sum row
          for (auto& number : table[chosenInNeg[i]]) number /= rowSum; /// divide row by sum
          //transform( begin(table[chosenInNeg[i]]), end(table[chosenInNeg[i]]), begin(table[chosenInNeg[i]]), bind1st(divides<T>(), rowSum) ); /// divide row by sum
          //table[chosenInNeg[i]][chosenOutNeg[i]] -= mod;
          //if (table[chosenInNeg[i]][chosenOutNeg[i]] < 0.001)
          //    table[chosenInNeg[i]][chosenOutNeg[i]] = 0.001;
          //double s = 0.0;
          //for (size_t k = 0; k < table[chosenInNeg[i]].size(); k++)
          //    s += table[chosenInNeg[i]][k];
          //for (size_t k = 0; k < table[chosenInNeg[i]].size(); k++)
          //    table[chosenInNeg[i]][k] /= s;
      }
  }

  //do the logic of the gate
  int input = 0;
  int output = 0;
  double r = Random::getDouble(1);
  for (size_t i = 0; i < inputs.size(); i++)
      input = (input << 1) + Bit(states[inputs[i]]);
  while (r > table[input][output]) {
      r -= table[input][output];
      output++;
  }
  for (size_t i = 0; i < outputs.size(); i++)
      nextStates[outputs[i]] += 1.0 * ((output >> i) & 1);

  //remember the last actions for future feedback
  if (feedbackON) {
      chosenInPos.push_back(input);
      chosenInNeg.push_back(input);
      chosenOutPos.push_back(output);
      chosenOutNeg.push_back(output);
      while (chosenInPos.size() > nrPos) {
          chosenInPos.pop_front();
      }
      while (chosenOutPos.size() > nrPos) {
          chosenOutPos.pop_front();
      }
      while (chosenInNeg.size() > nrNeg) {
          chosenInNeg.pop_front();
      }
      while (chosenOutNeg.size() > nrNeg) {
          chosenOutNeg.pop_front();
      }
  }
}

std::string DecomposableFeedbackGate::description() {
    std::string S = "pos node:" + std::to_string((int) posFBNode) + "\n neg node:" + std::to_string((int) negFBNode);
    S=S+"\n#\nI:\t";
    for(int i=0;i<inputs.size();i++)
        S=S+" "+ std::to_string(inputs[i]);
    S=S+"\nO:\t";
    for(int i=0;i<outputs.size();i++)
        S=S+" "+ std::to_string(outputs[i]);
    S=S+"\n";
    for(int i=0;i<table.size();i++){
        for(int j=0;j<table[i].size();j++)
            S=S+"\t"+ std::to_string(table[i][j]);
        S=S+"\n";
    }
    S=S+"#\n";
    return "Decomposable Feedback Gate\n " + S + "\n";
}

void DecomposableFeedbackGate::applyNodeMap(std::vector<int> nodeMap, int maxNodes) {
  AbstractGate::applyNodeMap(nodeMap, maxNodes);
  posFBNode = nodeMap[posFBNode % maxNodes];
  negFBNode = nodeMap[negFBNode % maxNodes];
}

void DecomposableFeedbackGate::resetGate() {
  chosenInPos.clear();
  chosenInNeg.clear();
  chosenOutPos.clear();
  chosenOutNeg.clear();
    appliedNegFB.clear();
    appliedPosFB.clear();
  for (size_t i = 0; i < table.size(); i++)
    for (size_t j = 0; j < table[i].size(); j++)
      table[i][j] = originalTable[i][j];
  std::string temp;
}

std::vector<int> DecomposableFeedbackGate::getIns() {
    std::vector<int> R;
  R.insert(R.begin(), inputs.begin(), inputs.end());
  R.push_back(posFBNode);
  R.push_back(negFBNode);
  return R;
}

//double DecomposableFeedbackGate::computeGateRMS(){
//    double sumDiff=0;
//    double numPoints=0;
//    for(int i=0;i<table.size();i++)
//        for(int j=0;j<table[i].size();j++){
//            sumDiff=sumDiff+pow((double)table[i][j]-(double)originalTable[i][j],2.0);
//            numPoints=numPoints+1.0;
//        }
//    return sqrt(sumDiff*(1.0/(double)numPoints));
//}

//double DecomposableFeedbackGate::computeMutualInfo(){
//    vector<vector<double>> normalizedTable;
//    vector<double>columns;
//    vector<double> rows;
//    double mutualInfo=0.0;
//    
//    normalizedTable.resize(table.size());
//    rows.resize(table.size());
//    columns.resize(table[0].size());
//    for(int i=0;i<table.size();i++){
//        rows[i]=0.0;
//        normalizedTable[i].resize(table[i].size());
//        for(int j=0;j<table[i].size();j++){
//            if(j==0 &&i==0)
//                columns[j]=0.0;
//            normalizedTable[i][j]=(double)table[i][j]/table.size();
//            columns[j]+=normalizedTable[i][j];
//            rows[i]+=(normalizedTable[i][j]);
//        }
//    }
//    
//    for(int i=0;i<table.size();i++){
//        for(int j=0;j<table[i].size();j++){
//            if(normalizedTable[i][j]!=0){
//                mutualInfo+=normalizedTable[i][j]*log2(normalizedTable[i][j]/(columns[j]*rows[i]));
//            }
//        }
//    }
//    return mutualInfo;
//}

std::string DecomposableFeedbackGate::getAppliedPosFeedback(){

    //save all positive feedback the gate has used
    std::string temp="";
    for(int i = 0; i<appliedPosFB.size(); i++)
        temp+=','+ std::to_string(appliedPosFB[i]);
    appliedPosFB.clear();
    return temp;
}

std::string DecomposableFeedbackGate::getAppliedNegFeedback(){
    
    //save all negative feedback the gate has used
    std::string temp="";
    for(int i = 0; i<appliedNegFB.size(); i++)
        temp+=','+ std::to_string(appliedNegFB[i]);
    appliedNegFB.clear();
    return temp;
}

std::shared_ptr<AbstractGate> DecomposableFeedbackGate::makeCopy(std::shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newGate = std::make_shared<DecomposableFeedbackGate>(_PT);
	newGate->table = originalTable; // non-Lamarkian
    originalTable = originalTable;
    feedbackON = feedbackON;
    posFBNode = posFBNode;
    negFBNode = negFBNode;
    posLevelOfFB = posLevelOfFB;
    negLevelOfFB = negLevelOfFB;
	newGate->ID = ID;
	newGate->inputs = inputs;
	newGate->outputs = outputs;
	return newGate;
}
