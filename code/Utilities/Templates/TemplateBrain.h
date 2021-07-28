//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once					// directive to insure that this .h file is only included one time

// AbstractBrain defines all the basic function templates for brains
#include <Brain/AbstractBrain.h>

// If your brain is (or maybe) constructed using a genome, you must include AbstractGenome.h
#include <Genome/AbstractGenome.h>

class {{MODULE_NAME}}Brain : public AbstractBrain {

public:
    {{MODULE_NAME}}Brain() = delete;

    {{MODULE_NAME}}Brain(int ins, int outs, std::shared_ptr<ParametersTable> PT);

    virtual ~{{MODULE_NAME}}Brain() = default;

    virtual void update();

    // make a copy of the brain that called this
    virtual std::shared_ptr<AbstractBrain> makeCopy(std::shared_ptr<ParametersTable> PT);

    // Make a brain like the brain that called this function, using genomes and initalizing other elements.
    virtual std::shared_ptr<AbstractBrain> makeBrain(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>& _genomes);

    virtual std::string description(); // returns a desription of this brain in it's current state

    virtual DataMap getStats(std::string& prefix); // return a vector of DataMap of stats from this brain

    virtual std::string getType(); // return the type of this brain

    virtual void setInput(const int& inputAddress, const double& value);

    virtual double readInput(const int& inputAddress);

    virtual void setOutput(const int& outputAddress, const double& value);

    virtual double readOutput(const int& outputAddress);

    virtual void resetOutputs();

    virtual void resetInputs();

    virtual void resetBrain();

    ///////////////////////////////////////////////////////////////////////////////////////////
    // these functions need to be filled in if genomes are being used in this brain
    ///////////////////////////////////////////////////////////////////////////////////////////

    virtual std::unordered_set<std::string> requiredGenomes(); // does this brain use any genomes
    
    // initializeGenomes can be used to randomize the genome and/or insert start codons
    virtual void initializeGenomes(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>& _genomes);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // these functions need to be filled in if this brain is direct encoded (in part or whole)
    ///////////////////////////////////////////////////////////////////////////////////////////

    // Make a brain like the brain that called this function, using genomes and
    // inheriting other elements from parent.
    // in the default case, we assume geneticly encoded brains, so this just calls
    // the no parent version (i.e. makeBrain which builds from genomes)
    virtual std::shared_ptr<AbstractBrain> makeBrainFrom(
        std::shared_ptr<AbstractBrain> parent,
        std::unordered_map<std::string,
        std::shared_ptr<AbstractGenome>>& _genomes);

    // see makeBrainFrom, this can take more then one parent
    virtual std::shared_ptr<AbstractBrain> makeBrainFromMany(
        std::vector<std::shared_ptr<AbstractBrain>> parents,
        std::unordered_map<std::string,
        std::shared_ptr<AbstractGenome>>&_genomes);

    // apply direct mutations to this brain
    virtual void mutate();

    // convert a brain into data map with data that can be saved to file so this brain can be reconstructed
    // 'name' here contains the prefix that must be so that deserialize can identify relavent data
    virtual DataMap serialize(std::string& name);

    // given an unordered_map<string, string> of org data and PT, load data into this brain
    // 'name' here contains the prefix that was used when data was being saved
    virtual void deserialize(std::shared_ptr<ParametersTable> PT,
        std::unordered_map<std::string, std::string>& orgData,
        std::string& name);

};

inline std::shared_ptr<AbstractBrain> {{MODULE_NAME}}Brain_brainFactory(int ins, int outs, std::shared_ptr<ParametersTable> PT) {
    return std::make_shared<{{MODULE_NAME}}Brain>(ins, outs, PT);
}
