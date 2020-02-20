//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "LSTMBrain.h"

std::shared_ptr<ParameterLink<std::string>> LSTMBrain::genomeNamePL =
    Parameters::register_parameter("BRAIN_LSTM_NAMES-genomeNameSpace",
                                   (std::string) "root::",
                                   "namespace used to set parameters for "
                                   "genome used to encode this brain");

LSTMBrain::LSTMBrain(int _nrInNodes, int _nrOutNodes,
                     std::shared_ptr<ParametersTable> PT_)
    : AbstractBrain(_nrInNodes, _nrOutNodes, PT_) {

  genomeName = genomeNamePL->get(PT);

  I_ = _nrInNodes;
  O_ = _nrOutNodes;
  /*
valueMin = (PT == nullptr) ? valueMinPL->lookup() :
PT->lookupDouble("BRAIN_CONSTANT-valueMin");
valueMax = (PT == nullptr) ? valueMaxPL->lookup() :
PT->lookupDouble("BRAIN_CONSTANT-valueMax");
valueType = (PT == nullptr) ? valueTypePL->lookup() :
PT->lookupInt("BRAIN_CONSTANT-valueType");
samplesPerValue = (PT == nullptr) ? samplesPerValuePL->lookup() :
PT->lookupInt("BRAIN_CONSTANT-samplesPerValue");

initializeUniform = (PT == nullptr) ? initializeUniformPL->lookup() :
PT->lookupBool("BRAIN_CONSTANT-initializeUniform");
initializeConstant = (PT == nullptr) ? initializeConstantPL->lookup() :
PT->lookupBool("BRAIN_CONSTANT-initializeConstant");
initializeConstantValue = (PT == nullptr) ? initializeConstantValuePL->lookup()
: PT->lookupInt("BRAIN_CONSTANT-initializeConstantValue");

// columns to be added to ave file
popFileColumns.clear();
for (int i = 0; i < nrOutputValues; i++) {
      popFileColumns.push_back("brainValue" + to_string(i));
}
   */
}

std::shared_ptr<AbstractBrain> LSTMBrain::makeBrain(
    std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
        &_genomes) {
  std::shared_ptr<LSTMBrain> newBrain =
      std::make_shared<LSTMBrain>(nrInputValues, nrOutputValues, PT);
  auto genomeHandler =
      _genomes[genomeName]->newHandler(_genomes[genomeName], true);

  newBrain->I_ = I_;
  newBrain->O_ = O_;
  newBrain->C.resize(O_);
  newBrain->H.resize(O_);
  newBrain->X.resize(I_ + O_);
  newBrain->Wf.resize(I_ + O_);
  newBrain->Wi.resize(I_ + O_);
  newBrain->Wc.resize(I_ + O_);
  newBrain->Wo.resize(I_ + O_);
  newBrain->bt.resize(O_);
  newBrain->bi.resize(O_);
  newBrain->bC.resize(O_);
  newBrain->bO.resize(O_);

  newBrain->ft.resize(O_);
  newBrain->it.resize(O_);
  newBrain->Ct.resize(O_);
  newBrain->Ot.resize(O_);
  newBrain->dt.resize(O_);
  for (int i = 0; i < O_; i++) {
    newBrain->bt[i] = genomeHandler->readDouble(-1.0, 1.0);
    newBrain->bi[i] = genomeHandler->readDouble(-1.0, 1.0);
    newBrain->bC[i] = genomeHandler->readDouble(-1.0, 1.0);
    newBrain->bO[i] = genomeHandler->readDouble(-1.0, 1.0);
  }
  for (int i = 0; i < I_ + O_; i++) {
    newBrain->Wf[i].clear();
    newBrain->Wi[i].clear();
    newBrain->Wc[i].clear();
    newBrain->Wo[i].clear();
    for (int j = 0; j < O_; j++) {
      newBrain->Wf[i].push_back(genomeHandler->readDouble(-1.0, 1.0));
      newBrain->Wi[i].push_back(genomeHandler->readDouble(-1.0, 1.0));
      newBrain->Wc[i].push_back(genomeHandler->readDouble(-1.0, 1.0));
      newBrain->Wo[i].push_back(genomeHandler->readDouble(-1.0, 1.0));
    }
  }

  /*
          for (int i = 0; i < nrOutputValues; i++) {
                  tempValue = 0;
                  for (int j = 0; j < samplesPerValue; j++) {
                          if (valueType == 0) {
                                  tempValue += genomeHandler->readInt(valueMin,
     valueMax);
                          } else if (valueType == 1) {
                                  tempValue +=
     genomeHandler->readDouble(valueMin, valueMax);
                          } else {
                                  cout << "  ERROR! BRAIN_CONSTANT-valueType is
     invalid. current value: " << valueType << endl;
                                  exit(1);
                          }
                  }
                  if (valueType == 0) {
                          newBrain->outputValues[i] = int(tempValue /
     samplesPerValue);
                  }
                  if (valueType == 1) {
                          newBrain->outputValues[i] = tempValue /
     samplesPerValue;
                  }
          }
   */
  return newBrain;
}

void LSTMBrain::resetBrain() {
  for (int i = 0; i < I_ + O_; i++) {
    X[i] = 0.0;
  }
  for (int o = 0; o < O_; o++) {
    H[o] = 0.0;
    C[o] = 0.0;
  }
}

void LSTMBrain::update() {
  for (int i = 0; i < I_; i++)
    X[i] = inputValues[i];
  singleLayerUpdate(X, ft, Wf);
  vectorMathElementalPlus(ft, bt, ft);
  vectorMathSigmoid(ft);

  singleLayerUpdate(X, it, Wi);
  vectorMathElementalPlus(it, bi, it);
  vectorMathSigmoid(it);

  singleLayerUpdate(X, Ct, Wc);
  vectorMathElementalPlus(Ct, bC, Ct);
  vectorMathTanh(Ct);

  singleLayerUpdate(X, Ot, Wo);
  vectorMathElementalPlus(Ot, bO, Ot);
  vectorMathSigmoid(Ot);
  vectorMathElementalMultiply(C, ft, C);
  vectorMathElementalMultiply(it, Ct, Ct);
  vectorMathElementalPlus(C, Ct, C);
  for (int o = 0; o < O_; o++)
    dt[o] = C[o];
  vectorMathTanh(dt);
  vectorMathElementalMultiply(Ot, dt, H);
  for (int o = 0; o < O_; o++) {
    X[o + I_] = H[o];
    outputValues[o] = H[o];
  }
}

void inline LSTMBrain::resetOutputs() {
  for (int o = 0; o < O_; o++) {
    H[o] = 0.0;
  }
}

std::string LSTMBrain::description() { return "LSTM Brain"; }

DataMap LSTMBrain::getStats(std::string &prefix) {
  DataMap dataMap;

  //	dataPairs.push_back("outputValues");
  //	string valuesList = "\"[";
  //	for (int i = 0; i < nrOutNodes; i++) {
  //		valuesList += to_string(nextNodes[outputNodesList[i]]) + ",";
  //	}
  //	valuesList.pop_back();
  //	valuesList += "]\"";
  //
  //	dataPairs.push_back(to_string(valuesList));
  /*
          for (int i = 0; i < nrOutputValues; i++) {
                  dataMap.Set("brainValue" + to_string(i),outputValues[i]);
          }
   */
  return (dataMap);
}

void LSTMBrain::initializeGenomes(
    std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
        &_genomes) {
  // int totalGenomeSizeNeeded=(I_*O_*4)+(O_*4);
  _genomes[genomeName]->fillRandom();
  // printf("%s\n",_genome->genomeToStr().c_str());
  // exit(0);
  /*
  auto handler = _genome->newHandler(_genome);
  while (!handler->atEOG()) {
      handler->writeInt(Random::getInt(-1024, 1024), -1, 1);
  }
  handler->resetHandler();
  */
  /*
      if (initializeConstant) {
              if (initializeConstantValue < valueMin) {
                      cout << "ERROR: initializeConstantValue must be greater
then or equal to valueMin" << endl;
                      exit(1);
              }
              if (initializeConstantValue > valueMax) {
                      cout << "ERROR: initializeConstantValue must be less then
or equal to valueMax" << endl;
                      exit(1);
              }
              auto handler = _genome->newHandler(_genome);
              while (!handler->atEOG()) {
                      if (valueType == 1) {
                              handler->writeInt(initializeConstantValue,
valueMin, valueMax);
                      } else if (valueType == 0) {
                              handler->writeInt(initializeConstantValue,
valueMin, valueMax);
                      }
              }
              handler->resetHandler();
              handler->writeInt(initializeConstantValue, valueMin, valueMax);
      } else if (initializeUniform) {
              auto handler = _genome->newHandler(_genome);
              int count = 0;
              double randomValue = 0;
              if (valueType == 1) {
                      randomValue = Random::getDouble(valueMin, valueMax);
              } else if (valueType == 0) {
                      randomValue = Random::getInt(valueMin, valueMax);
              }
//		if (count == samplesPerValue) {
//			if (valueType == 1) {
//				randomValue = Random::getDouble(valueMin,
valueMax);
//			} else if (valueType == 0) {
//				randomValue = Random::getInt(valueMin,
valueMax);
//			}
//		}
              while (!handler->atEOG()) {
                      if (count == samplesPerValue) {
                              count = 0;
                              if (valueType == 1) {
                                      randomValue = Random::getDouble(valueMin,
valueMax);
                              } else if (valueType == 0) {
                                      randomValue = Random::getInt(valueMin,
valueMax);
                              }
                      }
                      if (valueType == 1) {
                              handler->writeInt(randomValue, valueMin,
valueMax);
                      } else if (valueType == 0) {
                              handler->writeInt(randomValue, valueMin,
valueMax);
                      }
                      count++;
              }
      } else {
              _genome->fillRandom();
      }
   */
}

void LSTMBrain::singleLayerUpdate(std::vector<double> &IN,
                                  std::vector<double> &OUT,
                                  std::vector<std::vector<double>> &W) {
  int I = (int)IN.size();
  int O = (int)OUT.size();
  for (int o = 0; o < O; o++)
    OUT[o] = 0.0;
  for (int i = 0; i < I; i++)
    for (int o = 0; o < O; o++) {
      OUT[o] += IN[i] * W[i][o];
    }
}

void LSTMBrain::vectorMathSigmoid(std::vector<double> &V) {
  for (auto &&v : V)
    v = fastSigmoid(v);
}

void LSTMBrain::vectorMathTanh(std::vector<double> &V) {
  for (auto &&v : V)
    v = tanh(v);
}

void LSTMBrain::vectorMathElementalPlus(std::vector<double> &A,
                                        std::vector<double> &B,
                                        std::vector<double> &result) {
  int L = (int)A.size();
  for (int i = 0; i < L; i++)
    result[i] = A[i] + B[i];
}

void LSTMBrain::vectorMathElementalMultiply(std::vector<double> &A,
                                            std::vector<double> &B,
                                            std::vector<double> &result) {
  int L = (int)A.size();
  for (int i = 0; i < L; i++)
    result[i] = A[i] * B[i];
}

void LSTMBrain::showVector(std::vector<double> &V) {
  for (size_t i = 0; i < V.size(); i++) {
    printf("%f ", V[i]);
  }
  printf("\n");
}

std::shared_ptr<AbstractBrain>
LSTMBrain::makeCopy(std::shared_ptr<ParametersTable> PT_) {
  if (PT_ == nullptr) {
    PT_ = PT;
  }
  auto newBrain =
      std::make_shared<LSTMBrain>(nrInputValues, nrOutputValues, PT_);
  newBrain->I_ = I_;
  newBrain->O_ = O_;

  newBrain->Wf = Wf;
  newBrain->Wi = Wi;
  newBrain->Wc = Wc;
  newBrain->Wo = Wo;
  newBrain->ft = ft;
  newBrain->it = it;
  newBrain->Ct = Ct;
  newBrain->Ot = Ot;
  newBrain->dt = dt;
  newBrain->bt = bt;
  newBrain->bi = bi;
  newBrain->bC = bC;
  newBrain->bO = bO;
  newBrain->C = C;
  newBrain->X = X;
  newBrain->H = H;

  return newBrain;
}

