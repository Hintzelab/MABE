#include "GateBrain.h"

shared_ptr<ParameterLink<int>> GateBrain::hiddenNodesPL = Parameters::register_parameter("BRAIN_GATE-hiddenNodes", 100, "number of hidden nodes (observe: nr of gates = hidden+output)");
shared_ptr<ParameterLink<int>> GateBrain::fanInPL = Parameters::register_parameter("BRAIN_GATE-fanInNr", 4, "number of wires that go into each gate (max 4)");
shared_ptr<ParameterLink<string>> GateBrain::genomeNamePL = Parameters::register_parameter("BRAIN_GATE-genomeNameSpace", (string)"root::", "namespace used to set parameters for genome used to encode this brain");

GateBrain::GateBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT) : AbstractBrain(_nrInNodes, _nrOutNodes, _PT){
	hiddenNodes	=  hiddenNodesPL->get(_PT);
	fanInNr	=  fanInPL->get(_PT);
	nrInputValues=_nrInNodes;
	nrOutputValues=_nrOutNodes;
	nrH=hiddenNodes;
	totalN=nrInputValues+nrOutputValues+nrH;
	nodes.resize(totalN);
	nextNodes.resize(totalN);
}

GateBrain::GateBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes, int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT) : GateBrain(_nrInNodes, _nrOutNodes, _PT){
	auto genomeHandler = _genomes[genomeNamePL->get(_PT)]->newHandler(_genomes[genomeNamePL->get(_PT)], true);
	vector<int> inputNodeIDs;
	inputNodeIDs.resize(fanInNr);
	for(int statei=0;statei<nrOutputValues+nrH;statei++){
		for(int fanIni=0;fanIni<fanInNr;fanIni++) inputNodeIDs[fanIni]=genomeHandler->readInt(0, totalN-1);
		shared_ptr<SimpleLogicGate> SLG=make_shared<SimpleLogicGate>(inputNodeIDs, fanInNr, nrInputValues+statei,(int)genomeHandler->readInt(0, 65535));
		gates.push_back(SLG);
	}
}


shared_ptr<AbstractBrain> GateBrain::makeCopy(shared_ptr<ParametersTable> _PT){
	shared_ptr<GateBrain> G=make_shared<GateBrain>(nrInputValues,nrOutputValues,_PT);
	for(auto& gate : gates)
		G->gates.push_back(make_shared<SimpleLogicGate>(gate->I,gate->fanInNr,gate->O,gate->logic));
	return G;
}


void GateBrain::update(){
	for(int i=0;i<totalN;i++){
		nextNodes[i]=0.0;
	}
	for(auto& gate : gates){
		gate->update(nodes,nextNodes);
	}
    for (int i = 0; i < gates.size(); i++) {
        gates[i]->update(nodes,nextNodes);
    }
	for(int i=0;i<totalN;i++){
		nodes[i]=nextNodes[i];
	}
}


shared_ptr<AbstractBrain> GateBrain::makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) {
	shared_ptr<GateBrain> newBrain = make_shared<GateBrain>(_genomes, nrInputValues, nrOutputValues);
	return newBrain;
}

string GateBrain::description(){
	return recordedList;
}

DataMap GateBrain::getStats(string& prefix){
	DataMap D;
	return D;
}

void GateBrain::resetBrain(){
	for(int i=0;i<totalN;i++){
		nodes[i]=0.0;
		nextNodes[i]=0.0;
	}
}
string GateBrain::gateList(){
	return "gate list empty - not implemented";
}
vector<vector<int>> GateBrain::getConnectivityMatrix(){
	vector<vector<int>> V;
	return V;
}
int GateBrain::brainSize(){
	return (int)nodes.size();
}

void GateBrain::initializeGenomes(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) {
    for (auto& keyvalPair : _genomes) {
        keyvalPair.second->fillRandom();
    }
}
