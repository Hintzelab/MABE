//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "../IPDBrain/IPDBrain.h"

shared_ptr<ParameterLink<string>> IPDBrain::availableStrategiesPL = Parameters::register_parameter("BRAIN_IPD-availableStrategies", (string)"[AllD,AllC,TFT,2TFT,TF2T,SIMP,Rand]", "list of strategies which this brain can use");
shared_ptr<ParameterLink<string>> IPDBrain::genomeNamePL = Parameters::register_parameter("BRAIN_IPD_NAMES-genomeName", (string)"root", "name of genome used to encode this brain\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");

IPDBrain::IPDBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT) :
		AbstractBrain(_nrInNodes, _nrOutNodes, _PT) {

	convertCSVListToVector((PT == nullptr) ? availableStrategiesPL->lookup() : PT->lookupString("BRAIN_IPD-availableStrategies"), availableStrategies);
	genomeName = (PT == nullptr) ? genomeNamePL->lookup() : PT->lookupString("BRAIN_IPD_NAMES-genomeName");

// columns to be added to ave file
	popFileColumns.clear();
	for (auto i:availableStrategies) {
		popFileColumns.push_back("IPD_" + i);
	}
}

shared_ptr<AbstractBrain> IPDBrain::makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) {

	shared_ptr<IPDBrain> newBrain = make_shared<IPDBrain>(nrInputValues, nrOutputValues);
	auto genomeHandler = _genomes[genomeName]->newHandler(_genomes[genomeName], true);

	newBrain->strategy = availableStrategies[genomeHandler->readInt(0, ((int)availableStrategies.size())-1, 77, 0)];

	return newBrain;
}

shared_ptr<AbstractBrain> IPDBrain::makeBrainFromValues(vector<double> values) {
	shared_ptr<IPDBrain> newBrain = make_shared<IPDBrain>(nrInputValues, nrOutputValues);

	newBrain->strategy = availableStrategies[int(values[0]) % (int)availableStrategies.size()];

	return newBrain;
}

void IPDBrain::resetBrain(){
	AbstractBrain::resetBrain();
	movesSelf.clear();
	movesOther.clear();
	internalValues.clear();
}

void IPDBrain::update() {
	//cout << "in IPDBrain::update()" << endl;
	int C = (PT == nullptr) ? Parameters::root->lookupBool("WORLD_IPD-C") : PT->lookupBool("WORLD_IPD-C");
	int D = 1 - C;

	movesOther.push_back(inputValues[1]); // last element is the other players last move
										// note that movesSelf has this players last move (assigned last turn)

	if (strategy == "Rand"){
		//cout << "Rand" << endl;
		outputValues[0] = Random::getInt(0,1);
	}

	if (strategy == "AllC"){
		//cout << "AllC" << endl;
		outputValues[0] = C;
	}

	if (strategy == "AllD"){
		//cout << "AllD" << endl;
		outputValues[0] = D;

	}

	if (strategy == "TFT"){
		//cout << "TFT" << endl;
		outputValues[0] = ((movesOther[((int)movesOther.size())-1]) == D)?D:C;
	}

	if (strategy == "2TFT") {
		//cout << "2TFT" << endl;
		if ((int)movesOther.size() == 1) {
			outputValues[0] = (movesOther[0] == D) ? D : C;
		}
		else {
			outputValues[0] = ((movesOther[((int)movesOther.size()) - 1]) == D || (movesOther[((int)movesOther.size()) - 2]) == D) ? D : C;
		}
	}

	if (strategy == "TF2T") {
		//cout << "TF2T" << endl;
		if ((int)movesOther.size() == 1) {
			outputValues[0] = C;
		}
		else {
			outputValues[0] = ((movesOther[((int)movesOther.size()) - 1]) == D && (movesOther[((int)movesOther.size()) - 2]) == D) ? D : C;
		}
	}

	if (strategy == "SIMP") {
		//cout << "SIMP" << endl;
		// cc -> c
		// dc -> d
		// cd -> d
		// dd -> c
		if (movesSelf.size() == 0) {
			outputValues[0] = C;
		}
		else {
			int lastMove = (int)movesSelf[((int)movesSelf.size()) - 1];
			int lastMoveOther = (int)movesOther[((int)movesOther.size()) - 1];
			if (lastMove == C && lastMoveOther == C) {
				outputValues[0] = C;
			}
			else if (lastMove == D && lastMoveOther == C) {
				outputValues[0] = D;
			}
			else if (lastMove == C && lastMoveOther == D) {
				outputValues[0] = D;
			}
			else { // if (lastMove == D && lastMoveOther == D) {
				outputValues[0] = C;
			}
		}
	}
	movesSelf.push_back(outputValues[0]);

	//cout << "done IPDBrain::update()" << endl;

}

string IPDBrain::description() {
	string S = "IPD Brain\n";
	return S;
}

DataMap IPDBrain::getStats(string& prefix) {
	DataMap dataMap;

	for (auto i : availableStrategies) {
		if (strategy == i){
			dataMap.Set(prefix + "IPD_" + i,1.0);
		} else {
			dataMap.Set(prefix + "IPD_" + i,0.0);
		}
	}
	return (dataMap);
}

void IPDBrain::initalizeGenome(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) {
	_genomes[genomeName]->fillRandom();
}

