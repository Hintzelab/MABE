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

#include <set>
#include <vector>

#include <Genome/AbstractGenome.h>
#include <Utilities/Parameters.h>
#include <Global.h>

class AbstractBrain {
public:
    static std::shared_ptr<ParameterLink<std::string>> brainTypeStrPL;

    const std::shared_ptr<ParametersTable> PT;

    std::vector<std::string> popFileColumns;

    bool recordActivity;
    std::string recordActivityFileName;

    int nrInputValues;
    int nrOutputValues;
    std::vector<double> inputValues;
    std::vector<double> outputValues;

    AbstractBrain() = delete;

    AbstractBrain(int ins, int outs, std::shared_ptr<ParametersTable> PT_);

    virtual ~AbstractBrain() = default;

    virtual void update() = 0;

    // make a copy of the brain that called this
    virtual std::shared_ptr<AbstractBrain> makeCopy(std::shared_ptr<ParametersTable> PT_);

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

    // setRecordActivity and setRecordFileName provide a standard way to set up brain
    // activity recoding. How and when the brain records activity is up to the brain developer
    virtual void inline setRecordActivity(bool _recordActivity) {
        recordActivity = _recordActivity;
    }

    virtual void inline setRecordFileName(std::string _recordActivityFileName) {
        recordActivityFileName = _recordActivityFileName;
    }

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
