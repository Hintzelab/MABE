//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "ANNBrain.h"

std::shared_ptr<ParameterLink<std::string>> ANNBrain::genomeNamePL = Parameters::register_parameter("BRAIN_ANN-genomeName", (std::string)"root::", "namespace for parameters used to define this brain");
std::shared_ptr<ParameterLink<int>> ANNBrain::nrOfRecurringNodesPL = Parameters::register_parameter("BRAIN_ANN-nrOfRecurringNodes", 10, "number of recurring nodes");
std::shared_ptr<ParameterLink<int>> ANNBrain::nrOfHiddenLayersPL = Parameters::register_parameter("BRAIN_ANN-nrOfHiddenLayers", 0, "number of hidden layers. if 0, hidden layer have (number of outputs + number of recurrent) nodes.");
std::shared_ptr<ParameterLink<std::string>> ANNBrain::hiddenLayerSizesPL = Parameters::register_parameter("BRAIN_ANN-hiddenLayerSizes", (std::string)"", "comma seperated list of sizes for each hidden layer if there are hidden layers\n"
	"if a single number, all hidden layers will have this size\n"
	"if blank, hidden layers will all have (number of outputs + number of recurrent) nodes.");
std::shared_ptr<ParameterLink<std::string>> ANNBrain::weightRangePL = Parameters::register_parameter("BRAIN_ANN-weightRange", (std::string)"-1.0,1.0",
	"range for weight values (min,max)");
std::shared_ptr<ParameterLink<std::string>> ANNBrain::biasRangePL = Parameters::register_parameter("BRAIN_ANN-biasRange", (std::string)"-1.0,1.0",
	"When each nodes value is calculated a bias in this range (derived from the genome) will be added to the node value before thresholding.  (min,max)");
std::shared_ptr<ParameterLink<std::string>> ANNBrain::thresholdMethodPL = Parameters::register_parameter("BRAIN_ANN-thresholdMethod", (std::string)"Tanh",
	"Threshold method applied to node values after summation. Must be one of: NONE, Sigmoid, Tanh, ReLU, Binary");

// Layers... ANNBrain has 0 or more hidden layers
// if 0 hidden layers then brain will have (number of outputs + number of recurrent nodes) nodes on the last layer. each last layer node will
//   recive inputs from all input and recurrent nodes and write to one node (either output or recurrent)
// if more then 0 hidden then all layers after the first layer will have (number of outputs + number of recurrent) nodes. Each node
//   will recive inputs to all nodes in the prior layer.

// The weight matrix
// weights defines how every node will contribute to each node in the next layer
// The weight matrix is organized as [layers[nodes[next_layer_node]]]. So weights[2][4][7] would define contribution
//   of the fourth node in the second layer to the seventh node in the third layer.

ANNBrain::ANNBrain(int _nrInNodes, int _nrOutNodes, std::shared_ptr<ParametersTable> _PT) : AbstractBrain(_nrInNodes, _nrOutNodes, _PT) {
    genomeName = genomeNamePL->get(PT);

	if (thresholdMethodPL->get(PT) == "NONE") {
		thresholdMethod = NONE;
	}
	else if (thresholdMethodPL->get(PT) == "Sigmoid") {
		thresholdMethod = Sigmoid;
	}
	else if (thresholdMethodPL->get(PT) == "Tanh") {
		thresholdMethod = Tanh;
	}
	else if (thresholdMethodPL->get(PT) == "ReLU") {
		thresholdMethod = ReLU;
	}
	else if (thresholdMethodPL->get(PT) == "Binary") {
		thresholdMethod = Binary;
	}
	else {
		std::cout << "  In ANN Brain constructor :: found unknown thresholdMethod \"" << thresholdMethodPL->get(PT) << "\".\n  Please check config file and correct.\n  exiting..." << std::endl;
		exit(1);
	}
	convertCSVListToVector(biasRangePL->get(PT), biasRange);
	convertCSVListToVector(weightRangePL->get(PT), weightRange);

    nrOfRecurringNodes = nrOfRecurringNodesPL->get(PT);
    _I=_nrInNodes;
    _O=_nrOutNodes;

	// set up hidden layer stuff
	nrOfHiddenLayers = nrOfHiddenLayersPL->get(PT);
	convertCSVListToVector(hiddenLayerSizesPL->get(PT), hiddenLayerSizes);
	if (nrOfHiddenLayers > 0 && hiddenLayerSizes.size() != nrOfHiddenLayers) {
		if (hiddenLayerSizes.size() == 0) { // if hiddenLayerSizes is empty, add an element with the size of the output/recurrent layer
			hiddenLayerSizes.push_back(_O + nrOfRecurringNodes);
		}
		if (hiddenLayerSizes.size() == 1) { // if layersizes is one, inflate to number of layers
			for (int i = 1; i < nrOfHiddenLayers; i++) {
				hiddenLayerSizes.push_back(hiddenLayerSizes[0]);
			}
		}
		else { // we could not have gotten here if (hiddenLayerSizes.size() == nrOfHiddenLayers) so no need to check
			std::cout << "  While setting up ANN_Brain :: number of elements in hiddenLayerSizes parameter must be 0 or 1 or number of hidden layers,\n"
				"but nrOfHiddenLayers = " << nrOfHiddenLayers << " and hiddenLayerSizes has " << hiddenLayerSizes.size() << " elements!\n  exiting..." <<
				std::endl;
			exit(1);
		}
	}
}

std::shared_ptr<AbstractBrain> ANNBrain::makeBrain(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>& _genomes) {
	std::shared_ptr<ANNBrain> newBrain = std::make_shared<ANNBrain>(nrInputValues, nrOutputValues);
	  auto genomeHandler = _genomes[genomeName]->newHandler(_genomes[genomeName], true);
    
    newBrain->nrOfRecurringNodes=nrOfRecurringNodes;
    newBrain->_I=_I;
    newBrain->_O=_O;
	newBrain->nrOfHiddenLayers = nrOfHiddenLayers;

	// set up nodes
    newBrain->nodes.resize(nrOfHiddenLayers+2); 
    newBrain->nodes[0].resize(_I+nrOfRecurringNodes); // inputs w/ recurrent nodes
	for (int i = 1; i < newBrain->nodes.size()-1; i++) {
		newBrain->nodes[i].resize(hiddenLayerSizes[i-1]); // set hidden layer sizes
	}
	newBrain->nodes[newBrain->nodes.size() - 1].resize(_O + nrOfRecurringNodes); // outputs w/ recurrent nodes

	// set up wights
 	newBrain->weights.resize((int)newBrain->nodes.size()-1); // first dim of weights is numlayers-1 (not the output/recurrent layer)
	for(int i=0;i<(int)newBrain->weights.size();i++) {
        // for each weight layer (numlayers-1) resize to (ins+recurrent) or (outs+recurrent)
        newBrain->weights[i].resize((int)newBrain->nodes[i].size());
        // this is where we create the weight matrix (each node from the previous layer has a weight for each node in the next layer
        for(int j=0;j<(int)newBrain->weights[i].size();j++) {
            // resize weights to same size as next layer
            newBrain->weights[i][j].resize((int)newBrain->nodes[i+1].size());
        }
    }
	for (int i = 0; i < (int)newBrain->weights.size(); i++) { // for each weights layer
		for (int j = 0; j < (int)newBrain->weights[i].size(); j++) { // for each weight set (i.e. the output weights for each node) in that layer
			for (int k = 0; k < (int)newBrain->weights[i][j].size(); k++) { // for each weight (i.e. for each node in the next layer)
				//double value=doCPPN(x,y,weightsCPPN);
				double value = genomeHandler->readDouble(weightRange[0], weightRange[1]); // get a value in range [-1.0,1.0) from genome
				newBrain->weights[i][j][k] = value; // create a 'normal' distribution
			}
		}
	}

	
	// set up biases
	newBrain->biases.resize((int)newBrain->nodes.size() - 1); // first dim of weights is numlayers-1 (not the input layer)
	for (int i = 0; i < (int)newBrain->biases.size(); i++) {
		// create the bias values for each node (each node except for the input layer needs a bias)
		for (int j = 0; j < (int)newBrain->nodes[i+1].size(); j++) { // index is off by one because layer 0 (inputs) have not bias
			// add a bias for each node
			newBrain->biases[i].push_back(genomeHandler->readDouble(biasRange[0], biasRange[1]));
		}
	}

	return newBrain;
}

void ANNBrain::resetBrain() {
	//showBrain();
	for (auto &N : nodes) {
		for (int i = 0; i < (int)N.size(); i++) {
			N[i] = 0.0;
		}
	}
}

void ANNBrain::setInput(const int& inputAddress, const double& value){
    nodes[0][inputAddress]=value;
}

double ANNBrain::readInput(const int& inputAddress){
    return nodes[0][inputAddress];
}

void ANNBrain::setOutput(const int& outputAddress, const double& value){
    nodes[(int)nodes.size()-1][outputAddress]=value;
}

double ANNBrain::readOutput(const int& outputAddress){
	return nodes[(int)nodes.size() - 1][outputAddress];
}

void ANNBrain::update() {
    // starting with second layer, compute forward activations
    for(int layer=1;layer<(int)nodes.size();layer++) {
        // reset summation for current layer
		for (int i = 0; i < (int)nodes[layer].size(); i++) {
			nodes[layer][i] = biases[layer - 1][i]; // indexing from 1, and biases starts at layer 1 (hence the - 1)
		}
        // sum activations for current layer assuming well-connected to previous layer
        for(int i=0;i<(int)nodes[layer-1].size();i++) { // for each node in the previous layer
            for(int j=0;j<(int)nodes[layer].size();j++) { // for each output weight associated with that node
                nodes[layer][j] += weights[layer-1][i][j]*nodes[layer-1][i]; // add the nodes weighted value to each node in this layer
            }
        }
		switch (thresholdMethod) {
		case NONE:
			break; // do nothing
		case Sigmoid:
			vectorMathSigmoid(nodes[layer]);
			break;
		case Tanh:
			vectorMathTanh(nodes[layer]);
			break;
		case ReLU:
			vectorMathReLU(nodes[layer]);
			break;
		}
    }

	// copy recurrent values to nodes layer 0
    int lastLayer=(int)nodes.size()-1;
	for (int i = 0; i < nrOfRecurringNodes; i++) {
		nodes[0][_I + i] = nodes[lastLayer][_O + i];
	}
}

void inline ANNBrain::resetOutputs() {
	for (int o = 0; o < _O; o++) { // set the first nodes in the last layer (the ones used for output) to 0
		nodes[(int)nodes.size() - 1][o] = 0.0;
	}
}

std::string ANNBrain::description() {
	std::string S = "ANN Brain";
	return S;
}

DataMap ANNBrain::getStats(std::string& prefix) {
	DataMap dataMap;
	return (dataMap);
}

void ANNBrain::initializeGenomes(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>& _genomes) {
    _genomes[genomeName]->fillRandom(); // randomize the genome
}

void ANNBrain::vectorMathSigmoid(std::vector<double> &V){
	for (auto&& v : V) {
		v = 2.0*((1.0 / (1.0 + exp(-1.0 * v)))-.5); // Logistic function
	}
}

void ANNBrain::vectorMathTanh(std::vector<double> &V) {
	for (auto&& v : V) {
		v = tanh(v);
	}
}

void ANNBrain::vectorMathReLU(std::vector<double> &V) {
	for (auto&& v : V) {
		v = std::max(0.0,v);
	}
}

void ANNBrain::vectorMathBinary(std::vector<double> &V) {
	for (auto&& v : V) {
		v = Bit(v);
	}
}

std::shared_ptr<AbstractBrain> ANNBrain::makeCopy(std::shared_ptr<ParametersTable> _PT){
    if (_PT == nullptr) {
        _PT = PT;
    }
    auto newBrain = std::make_shared<ANNBrain>(nrInputValues, nrOutputValues, _PT);
    newBrain->nrOfRecurringNodes=nrOfRecurringNodes;
    newBrain->_I=_I;
    newBrain->_O=_O;
    newBrain->nodes=nodes;
	newBrain->weights = weights;
	newBrain->biases = biases;

    return newBrain;
}

void ANNBrain::showBrain(){
    printf("Inputs: %i Outputs:%i  Recurrent:%i\n",_I,_O,nrOfRecurringNodes);
    for(int l=0;l<(int)nodes.size();l++){
        printf("layer %i has %i nodes.\n",l,(int)nodes[l].size());
    }
	for (int i = 0; i < (int)weights.size(); i++) {
		printf("layer(%d)\n", i);
		for (int j = 0; j < (int)weights[i].size(); j++) {
			printf("  node(%d) weights [", j);
			for (int k = 0; k < (int)weights[i][j].size(); k++) {
				printf("  %f,", weights[i][j][k]);
			}
			printf("]\n");
		}
	}
	for (int i = 0; i < (int)biases.size(); i++) {
		printf("layer(%d) biases: [", i + 1);
		for (int j = 0; j < (int)biases[i].size(); j++) {
			printf("  %f,", biases[i][j]);
		}
		printf("]\n");
	}

}
