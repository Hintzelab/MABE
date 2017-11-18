#include "CUGateBrain.h"

shared_ptr<ParameterLink<int>> CUGateBrain::hiddenNodesPL = Parameters::register_parameter("BRAIN_CUGATE-hiddenNodes", 100, "number of hidden nodes (observe: nr of gates = hidden+output)");
shared_ptr<ParameterLink<int>> CUGateBrain::fanInPL = Parameters::register_parameter("BRAIN_CUGATE-fanInNr", 4, "number of wires that go into each gate (max 4)");
shared_ptr<ParameterLink<string>> CUGateBrain::genomeNamePL = Parameters::register_parameter("BRAIN_CUGATE-genomeNameSpace", (string)"root::", "namespace used to set parameters for genome used to encode this brain");

const int KERNEL_SIZE = 65536;

CUGateBrain::CUGateBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT) : AbstractBrain(_nrInNodes, _nrOutNodes, _PT){
	hiddenNodes	=  hiddenNodesPL->get(_PT);
	fanInNr	=  fanInPL->get(_PT);
	nrInputValues=_nrInNodes;
	nrOutputValues=_nrOutNodes;
	nrH=hiddenNodes;
	totalN=nrInputValues+nrOutputValues+nrH;
	totalN=KERNEL_SIZE;
	treatStatesReversed = false;
	firstRun = true;
	//nodes.resize(totalN);
	//nextNodes.resize(totalN);
	inputStatesDirty = false;
	outputStatesDirty = false;
	inputStates = new int[nrInputValues];
#ifdef HEMI_CUDA_COMPILER
	cudaMallocHost((void**)&outputStates_h,nrOutputValues*sizeof(int));
	cudaMallocHost((void**)&hiddenStates_h,totalN*sizeof(int));
	cudaMalloc((void**)&hiddenStates_d,totalN*sizeof(int));
	cudaMalloc((void**)&nextHiddenStates_d,totalN*sizeof(int));
	cudaMalloc((void**)&CUGates_d,totalN*sizeof(CUGate));
	cudaMallocHost((void**)&CUGates_h,totalN*sizeof(CUGate));
#else
	outputStates_h = new int[nrOutputValues];
	hiddenStates_h = new int[totalN];
	nextHiddenStates_d = new int[totalN];
	CUGates_h = new CUGate[totalN];
#endif
}

CUGateBrain::CUGateBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes, int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT) : CUGateBrain(_nrInNodes, _nrOutNodes, _PT){
	int id=0;
	auto genomeHandler = _genomes[genomeNamePL->get(_PT)]->newHandler(_genomes[genomeNamePL->get(_PT)], true);
	vector<int> inputNodeIDs;
	inputNodeIDs.resize(fanInNr);
	for(int statei=0;statei<totalN;statei++){
		CUGate g;
		for(int i=0; i<4; i++) {
			g.ins[i]=genomeHandler->readInt(0, totalN-1);
		}
		g.logic=genomeHandler->readInt(0, KERNEL_SIZE);
		CUGates_h[statei]=g;
	}
#ifdef HEMI_CUDA_COMPILER
	cudaMemcpy(CUGates_d, CUGates_h, totalN*sizeof(CUGate), cudaMemcpyHostToDevice);
#else
	CUGates_d = CUGates_h;
#endif
	hemi::deviceSynchronize();
	for (int i=0; i<totalN; i++) hiddenStates_h[i] = 0;
}

shared_ptr<AbstractBrain> CUGateBrain::makeCopy(shared_ptr<ParametersTable> _PT){
	shared_ptr<CUGateBrain> G=make_shared<CUGateBrain>(nrInputValues,nrOutputValues,_PT);
	return G;
}

#ifdef HEMI_CUDA_COMPILER
#define IDX hemi::globalThreadIndex()
#endif

HEMI_KERNEL_FUNCTION(KernelBrain, int* hiddenStates, int* nextHiddenStates, CUGateBrain::CUGate* CUGates) {
#ifndef HEMI_CUDA_COMPILER
	for (int IDX=0; IDX<KERNEL_SIZE; IDX++) { /// simulate the device
#endif
	if (IDX >= KERNEL_SIZE) return;
	unsigned int I = 0;
	for (int j=0; j<4; j++) {
		I=(I<<1)+(hiddenStates[CUGates[IDX].ins[j]]&(unsigned int)1);
	}
	nextHiddenStates[IDX] = (CUGates[IDX].logic)&(unsigned int)1;
#ifndef HEMI_CUDA_COMPILER
	}
#endif
}
KernelBrain update_kernel;

void CUGateBrain::update(){

	if (inputStatesDirty) {
		inputStatesDirty = false;
		for (int h=0; h<nrInputValues; h++) { hiddenStates_h[h] = inputStates[h]; } /// set inputs
#ifdef HEMI_CUDA_COMPILER
		if (treatStatesReversed)
			cudaMemcpy(nextHiddenStates_d, hiddenStates_h, totalN*sizeof(int), cudaMemcpyHostToDevice);
		else
			cudaMemcpy(hiddenStates_d, hiddenStates_h, totalN*sizeof(int), cudaMemcpyHostToDevice);
#else
		if (treatStatesReversed)
			nextHiddenStates_d = hiddenStates_h; /// host version of cudaMemcpy
		else
			hiddenStates_d = hiddenStates_h; /// host version of cudaMemcpy
#endif
		treatStatesReversed = !treatStatesReversed;
	}

	/// brain update
#ifdef HEMI_CUDA_COMPILER
	if (treatStatesReversed)
		hemi::launch(update_kernel,nextHiddenStates_d,hiddenStates_d,CUGates_d);
	else
		hemi::launch(update_kernel,hiddenStates_d,nextHiddenStates_d,CUGates_d);
#else
	if (treatStatesReversed)
		update_kernel(nextHiddenStates_d,hiddenStates_d,CUGates_d);
	else
		update_kernel(hiddenStates_d,nextHiddenStates_d,CUGates_d);
#endif
	outputStatesDirty = true;
	hemi::deviceSynchronize();
	firstRun = false;
}


shared_ptr<AbstractBrain> CUGateBrain::makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) {
	shared_ptr<CUGateBrain> newBrain = make_shared<CUGateBrain>(_genomes, nrInputValues, nrOutputValues);
	return newBrain;
}

string CUGateBrain::description(){
	return recordedList;
}

DataMap CUGateBrain::getStats(string& prefix){
	DataMap D;
	return D;
}

void CUGateBrain::resetBrain(){
	for (int h=0; h<totalN;        h++) { hiddenStates_h[h] = 0; } /// reset hidden states
	for (int h=0; h<nrInputValues; h++) { inputStates[h] = 0; } /// set inputs
}
string CUGateBrain::gateList(){
	return "gate list empty - not implemented";
}
vector<vector<int>> CUGateBrain::getConnectivityMatrix(){
	vector<vector<int>> V;
	return V;
}
int CUGateBrain::brainSize(){
	return (int)KERNEL_SIZE;
}

void CUGateBrain::initializeGenomes(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) {
    for (auto& keyvalPair : _genomes) {
        keyvalPair.second->fillRandom();
    }
}
