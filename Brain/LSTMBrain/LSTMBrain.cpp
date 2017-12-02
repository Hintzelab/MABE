//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "../LSTMBrain/LSTMBrain.h"


shared_ptr<ParameterLink<string>> LSTMBrain::genomeNamePL = Parameters::register_parameter("BRAIN_LSTM_NAMES-genomeNameSpace", (string)"root::", "namespace used to set parameters for genome used to encode this brain");



LSTMBrain::LSTMBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT) :
		AbstractBrain(_nrInNodes, _nrOutNodes, _PT) {

	genomeName = genomeNamePL->get(PT);

            _I=_nrInNodes;
            _O=_nrOutNodes;
            /*
	valueMin = (PT == nullptr) ? valueMinPL->lookup() : PT->lookupDouble("BRAIN_CONSTANT-valueMin");
	valueMax = (PT == nullptr) ? valueMaxPL->lookup() : PT->lookupDouble("BRAIN_CONSTANT-valueMax");
	valueType = (PT == nullptr) ? valueTypePL->lookup() : PT->lookupInt("BRAIN_CONSTANT-valueType");
	samplesPerValue = (PT == nullptr) ? samplesPerValuePL->lookup() : PT->lookupInt("BRAIN_CONSTANT-samplesPerValue");

	initializeUniform = (PT == nullptr) ? initializeUniformPL->lookup() : PT->lookupBool("BRAIN_CONSTANT-initializeUniform");
	initializeConstant = (PT == nullptr) ? initializeConstantPL->lookup() : PT->lookupBool("BRAIN_CONSTANT-initializeConstant");
	initializeConstantValue = (PT == nullptr) ? initializeConstantValuePL->lookup() : PT->lookupInt("BRAIN_CONSTANT-initializeConstantValue");

// columns to be added to ave file
	popFileColumns.clear();
	for (int i = 0; i < nrOutputValues; i++) {
		popFileColumns.push_back("brainValue" + to_string(i));
	}
             */
}

shared_ptr<AbstractBrain> LSTMBrain::makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) {
	shared_ptr<LSTMBrain> newBrain = make_shared<LSTMBrain>(nrInputValues, nrOutputValues,PT);
	auto genomeHandler = _genomes[genomeName]->newHandler(_genomes[genomeName], true);
    
    newBrain->_I=_I;
    newBrain->_O=_O;
    newBrain->C.resize(_O);
    newBrain->H.resize(_O);
    newBrain->X.resize(_I+_O);
    newBrain->Wf.resize(_I+_O);
    newBrain->Wi.resize(_I+_O);
    newBrain->Wc.resize(_I+_O);
    newBrain->Wo.resize(_I+_O);
    newBrain->bt.resize(_O);
    newBrain->bi.resize(_O);
    newBrain->bC.resize(_O);
    newBrain->bO.resize(_O);
    
    newBrain->ft.resize(_O);
    newBrain->it.resize(_O);
    newBrain->Ct.resize(_O);
    newBrain->Ot.resize(_O);
    newBrain->dt.resize(_O);
    for(int i=0;i<_O;i++){
        newBrain->bt[i]=genomeHandler->readDouble(-1.0, 1.0);
        newBrain->bi[i]=genomeHandler->readDouble(-1.0, 1.0);
        newBrain->bC[i]=genomeHandler->readDouble(-1.0, 1.0);
        newBrain->bO[i]=genomeHandler->readDouble(-1.0, 1.0);
    }
    for(int i=0;i<_I+_O;i++){
        newBrain->Wf[i].clear();
        newBrain->Wi[i].clear();
        newBrain->Wc[i].clear();
        newBrain->Wo[i].clear();
        for(int j=0;j<_O;j++){
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
				tempValue += genomeHandler->readInt(valueMin, valueMax);
			} else if (valueType == 1) {
				tempValue += genomeHandler->readDouble(valueMin, valueMax);
			} else {
				cout << "  ERROR! BRAIN_CONSTANT-valueType is invalid. current value: " << valueType << endl;
				exit(1);
			}
		}
		if (valueType == 0) {
			newBrain->outputValues[i] = int(tempValue / samplesPerValue);
		}
		if (valueType == 1) {
			newBrain->outputValues[i] = tempValue / samplesPerValue;
		}
	}
 */
	return newBrain;
}

void LSTMBrain::resetBrain() {
    for(int i=0;i<_I+_O;i++){
        X[i]=0.0;
    }
    for(int o=0;o<_O;o++){
        H[o]=0.0;
        C[o]=0.0;
    }
}

void LSTMBrain::update() {
    for(int i=0;i<_I;i++)
        X[i]=inputValues[i];
    singleLayerUpdate(X, ft, Wf);
    vectorMathElementalPlus(ft,bt,ft);
    vectorMathSigmoid(ft);
    
    singleLayerUpdate(X, it, Wi);
    vectorMathElementalPlus(it,bi,it);
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
    for(int o=0;o<_O;o++)
        dt[o]=C[o];
    vectorMathTanh(dt);
    vectorMathElementalMultiply(Ot, dt, H);
    for(int o=0;o<_O;o++){
        X[o+_I]=H[o];
        outputValues[o]=H[o];
    }
}

void inline LSTMBrain::resetOutputs() {
    for(int o=0;o<_O;o++){
        H[o]=0.0;
    }
}

string LSTMBrain::description() {
	string S = "LSTM Brain";
	return S;
}

DataMap LSTMBrain::getStats(string& prefix) {
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

void LSTMBrain::initializeGenomes(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) {
    //int totalGenomeSizeNeeded=(_I*_O*4)+(_O*4);
    _genomes[genomeName]->fillRandom();
    //printf("%s\n",_genome->genomeToStr().c_str());
    //exit(0);
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
			cout << "ERROR: initializeConstantValue must be greater then or equal to valueMin" << endl;
			exit(1);
		}
		if (initializeConstantValue > valueMax) {
			cout << "ERROR: initializeConstantValue must be less then or equal to valueMax" << endl;
			exit(1);
		}
		auto handler = _genome->newHandler(_genome);
		while (!handler->atEOG()) {
			if (valueType == 1) {
				handler->writeInt(initializeConstantValue, valueMin, valueMax);
			} else if (valueType == 0) {
				handler->writeInt(initializeConstantValue, valueMin, valueMax);
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
//				randomValue = Random::getDouble(valueMin, valueMax);
//			} else if (valueType == 0) {
//				randomValue = Random::getInt(valueMin, valueMax);
//			}
//		}
		while (!handler->atEOG()) {
			if (count == samplesPerValue) {
				count = 0;
				if (valueType == 1) {
					randomValue = Random::getDouble(valueMin, valueMax);
				} else if (valueType == 0) {
					randomValue = Random::getInt(valueMin, valueMax);
				}
			}
			if (valueType == 1) {
				handler->writeInt(randomValue, valueMin, valueMax);
			} else if (valueType == 0) {
				handler->writeInt(randomValue, valueMin, valueMax);
			}
			count++;
		}
	} else {
		_genome->fillRandom();
	}
     */
}

void LSTMBrain::singleLayerUpdate(vector<double> &IN,vector<double> &OUT,vector<vector<double>> &W){
    int I=(int)IN.size();
    int O=(int)OUT.size();
    for(int o=0;o<O;o++)
        OUT[o]=0.0;
    for(int i=0;i<I;i++)
        for(int o=0;o<O;o++){
            OUT[o]+=IN[i]*W[i][o];
        }
}

void LSTMBrain::vectorMathSigmoid(vector<double> &V){
    for(auto&& v :V)
        v=fastSigmoid(v);
}

void LSTMBrain::vectorMathTanh(vector<double> &V){
    for(auto&& v :V)
        v=tanh(v);
    
}

void LSTMBrain::vectorMathElementalPlus(vector<double> &A,vector<double> &B,vector<double> &result){
    int L=(int)A.size();
    for(int i=0;i<L;i++)
        result[i]=A[i]+B[i];
}

void LSTMBrain::vectorMathElementalMultiply(vector<double> &A,vector<double> &B,vector<double> &result){
    int L=(int)A.size();
    for(int i=0;i<L;i++)
        result[i]=A[i]*B[i];
}

void LSTMBrain::showVector(vector<double> &V){
    for(size_t i=0;i<V.size();i++){
        printf("%f ",V[i]);
    }
    printf("\n");
}

shared_ptr<AbstractBrain> LSTMBrain::makeCopy(shared_ptr<ParametersTable> _PT)
{
    if (_PT == nullptr) {
        _PT = PT;
    }
    auto newBrain = make_shared<LSTMBrain>(nrInputValues, nrOutputValues, _PT);
    newBrain->_I=_I;
    newBrain->_O=_O;
    
    newBrain->Wf=Wf;
    newBrain->Wi=Wi;
    newBrain->Wc=Wc;
    newBrain->Wo=Wo;
    newBrain->ft=ft;
    newBrain->it=it;
    newBrain->Ct=Ct;
    newBrain->Ot=Ot;
    newBrain->dt=dt;
    newBrain->bt=bt;
    newBrain->bi=bi;
    newBrain->bC=bC;
    newBrain->bO=bO;
    newBrain->C=C;
    newBrain->X=X;
    newBrain->H=H;

    return newBrain;
}



