//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "{{MODULE_NAME}}Brain.h"


{{MODULE_NAME}}Brain::{{MODULE_NAME}}Brain(int ins, int outs, std::shared_ptr<ParametersTable> PT)
    : AbstractBrain(ins, outs, PT) {
    nrInputValues = ins;
    nrOutputValues = outs;

    inputValues.resize(nrInputValues);
    outputValues.resize(nrOutputValues);
}

void {{MODULE_NAME}}Brain::update(){
    // code here to update brain
    // inputs are in inputValues[inputAddress]
    // outputs are in outputValues[outputAddress]
    
    // example: a brain that outputs 1 if input 0 == input 1 else 0
    //if (inputValues[0] == inputValues[1]) {
    //    outputValues[0] = 1;
    //}
    //else {
    //    outputValues[0] = 0;
    //}
}

// make a copy of the brain that called this
std::shared_ptr<AbstractBrain> {{MODULE_NAME}}Brain::makeCopy(std::shared_ptr<ParametersTable> PT) {
    
    // You need to define this function. It needs to return a cop of the brain that called it
    
    return(std::make_shared<{{MODULE_NAME}}Brain>(nrInputValues, nrOutputValues, PT));
}

// Make a brain like the brain that called this function, using genomes and initalizing other elements.
std::shared_ptr<AbstractBrain> {{MODULE_NAME}}Brain::makeBrain(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> & _genomes) {
    
    // You need to define this function. It needs to return a brain, it can use the brain that called this, and _genomes

    return(std::make_shared<{{MODULE_NAME}}Brain>(nrInputValues, nrOutputValues, PT));
}

std::string {{MODULE_NAME}}Brain::description() {
    // returns a desription of this brain in it's current state
    return "no description provided...";
}

DataMap {{MODULE_NAME}}Brain::getStats(std::string& prefix) {
    // return a vector of DataMap of stats from this brain, this is called just after the brain is constructed
    // values in this datamap are added to the datamap on the organism that "owns" this brain
    // all data names must have prefix prepended (i.e. connections would be prefix + "connections"
    
    DataMap dataMap;
    // datamap example:
    //dataMap.append(prefix + "someStatName", someStat);
    return dataMap;
}

std::string {{MODULE_NAME}}Brain::getType() {
    // return the type of this brain
    return "Undefined";
}

void {{MODULE_NAME}}Brain::setInput(const int& inputAddress, const double& value) {
    if (inputAddress < nrInputValues) {
        inputValues[inputAddress] = value;
    }
    else {
        std::cout << "in Brain::setInput() : Writing to invalid input ("
            << inputAddress << ") - this brain needs more inputs!\nExiting"
            << std::endl;
        exit(1);
    }
}

double {{MODULE_NAME}}Brain::readInput(const int& inputAddress) {
    if (inputAddress < nrInputValues) {
        return inputValues[inputAddress];
    }
    else {
        std::cout << "in Brain::readInput() : Reading from invalid input ("
            << inputAddress << ") - this brain needs more inputs!\nExiting"
            << std::endl;
        exit(1);
    }
}

void {{MODULE_NAME}}Brain::setOutput(const int& outputAddress, const double& value) {
    if (outputAddress < nrOutputValues) {
        outputValues[outputAddress] = value;
    }
    else {
        std::cout << "in Brain::setOutput() : Writing to invalid output ("
            << outputAddress << ") - this brain needs more outputs!\nExiting"
            << std::endl;
        exit(1);
    }
}

double {{MODULE_NAME}}Brain::readOutput(const int& outputAddress) {
    if (outputAddress < nrOutputValues) {
        return outputValues[outputAddress];
    }
    else {
        std::cout << "in Brain::readOutput() : Reading from invalid output ("
            << outputAddress << ") - this brain needs more outputs!\nExiting"
            << std::endl;
        exit(1);
    }
}

void {{MODULE_NAME}}Brain::resetOutputs() {
    for (int i = 0; i < nrOutputValues; i++) {
        outputValues[i] = 0.0;
    }
}

void {{MODULE_NAME}}Brain::resetInputs() {
    for (int i = 0; i < nrInputValues; i++) {
        inputValues[i] = 0.0;
    }
}

void {{MODULE_NAME}}Brain::resetBrain() {
    resetInputs();
    resetOutputs();
}

///////////////////////////////////////////////////////////////////////////////////////////
// these functions need to be filled in if genomes are being used in this brain
///////////////////////////////////////////////////////////////////////////////////////////

// return a set of namespaces, MABE will insure that genomes with these names are created
// on organisms with these brains.
std::unordered_set<std::string> {{MODULE_NAME}}Brain::requiredGenomes() {
    // note namespace must end with ::, also, if you wish to use default values, then 
    //    return {"root::"};
    return {};
}

void {{MODULE_NAME}}Brain::initializeGenomes(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> & _genomes) {
    // do nothing by default... if this is a direct encoded brain, then no action is needed.
    // This can be used to randomize the genome and/or insert start codons
    // genomes will be found in _genomes[name] where name is the string used in required genomes
    // example: _genomes["root::"]

}

///////////////////////////////////////////////////////////////////////////////////////////
// these functions need to be filled in if this brain is direct encoded (in part or whole)
///////////////////////////////////////////////////////////////////////////////////////////

// Make a brain like the brain that called this function, using genomes and
// inheriting other elements from parent.
std::shared_ptr<AbstractBrain> {{MODULE_NAME}}Brain::makeBrainFrom(
    std::shared_ptr<AbstractBrain> parent,
    std::unordered_map<std::string, 
    std::shared_ptr<AbstractGenome>> & _genomes) {
   
    // in the default case, we assume geneticly encoded brains, so this just calls
    // the no parent version (i.e. makeBrain which builds from genomes)

    return makeBrain(_genomes);
}

// see makeBrainFrom, same thing, but for more then one parent
std::shared_ptr<AbstractBrain> {{MODULE_NAME}}Brain::makeBrainFromMany(
    std::vector<std::shared_ptr<AbstractBrain>> parents,
    std::unordered_map<std::string,
    std::shared_ptr<AbstractGenome>> & _genomes) {

    return makeBrain(_genomes);
}

// apply direct mutations to this brain
void {{MODULE_NAME}}Brain::mutate() {
    // do nothing by default...
    // if this is a direct encoded brain, then this function needs to be filled in to allow for mutations.
}

// convert a brain into data map with data that can be saved to file so this brain can be reconstructed
// 'name' here contains the prefix that must be so that deserialize can identify relavent data
DataMap {{MODULE_NAME}}Brain::serialize(std::string & name) {
    DataMap dataMap;
    return dataMap;
}

// given an unordered_map<string, string> of org data and PT, load data into this brain
// 'name' here contains the prefix that was used when data was being saved
void {{MODULE_NAME}}Brain::deserialize(std::shared_ptr<ParametersTable> PT,
    std::unordered_map<std::string, std::string> & orgData,
    std::string & name) {
    // note that the process by which deserialization (including text formatting) depends on
    // the corisponding serialize process
}
