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

#include "../Analyze/timeSeries.h"
#include "../Analyze/stateToState.h"

class AbstractBrain {
public:
    static std::shared_ptr<ParameterLink<std::string>> brainTypeStrPL;

    const std::shared_ptr<ParametersTable> PT;

    std::vector<std::string> popFileColumns;

    int nrInputValues;
    int nrOutputValues;
    std::vector<double> inputValues;
    std::vector<double> outputValues;

    // brainConnectome - a square map of input,output,hidden x input,output,hidden where each cell is the count of actual wires from T -> T+1
    // input and output should likely by input and output, hidden needs to be determined by the brain designer
    virtual std::vector<std::vector<int>> getConnectome() {
        return(std::vector<std::vector<int>>()); // if not defined by brain, return empty
    }
    virtual void saveConnectome(std::string fileName = "brainConnectome.py") {
        FileManager::writeToFile(fileName, "saveConnectome() not written for " + getType());
    }

    // saveBrainStructure - save a file that somehow shows the structure of the brain - how this works is absolutly upto the brain designer.
    virtual void saveStructure(std::string fileName = "brainStructure.dot") {
        FileManager::writeToFile(fileName, "saveBrainStructure() not written for " + getType());
    }

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


    // I dont this this is being used anywhere....
                //// setRecordActivity and setRecordFileName provide a standard way to set up brain
                //// activity recoding. How and when the brain records activity is up to the brain developer
                //virtual void inline setRecordActivity(bool _recordActivity) {
                //    saveActivity = _recordActivity;
                //}

                //virtual void inline setRecordFileName(std::string _recordActivityFileName) {
                //    saveActivityFileName = _recordActivityFileName;
                //}

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

    virtual std::vector<double> getInputVector() {
        return (inputValues);
    }
    virtual std::vector<double> getOutputVector() {
        return (outputValues);
    }

    virtual void setInputVector(std::vector<double> newInputValues) {
        if (newInputValues.size() == inputValues.size()) {
            inputValues = newInputValues;
        }
        else {
            std::cout << "in AbstractBrain::setInputVector :: newInputValues is not the same size as InputValues... exitting." << std::endl;
            exit(1);
        }
    }
    virtual void setOutputVector(std::vector<double> newOutputValues) {
        if (newOutputValues.size() == outputValues.size()) {
            outputValues = newOutputValues;
        }
        else {
            std::cout << "in AbstractBrain::setOutputVector :: newOutputValues is not the same size as OutputValues... exitting." << std::endl;
            exit(1);
        }
    }



    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    // information theory stuff
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // recurrentHidden and recurrentOutput are provided for use in individual brains.
    // these are needed for intTimeSeries analysis
    bool recurrentOutput = false;

    // If recordActivity = true then on update (and possibly also brain reset) brain should update InputStates, OutputStates, and HiddenStates

    bool recordActivity = false; // record activity will store input, output, and hidden for future access
    void setRecordActivity(bool setting) {
        recordActivity = setting;
    };

    TS::TimeSeries InputStates;
    TS::TimeSeries OutputStates;
    TS::TimeSeries HiddenStates;
    std::vector<int> lifeTimes = { 0 }; // a vector of the durration of each lifetime

    TS::TimeSeries getInputStates() {
        return InputStates;
    }
    TS::TimeSeries getOutputStates() {
        return OutputStates;
    }
    TS::TimeSeries getHiddenStates() {
        return HiddenStates;
    }
    std::vector<int> getLifeTimes() {
        return lifeTimes;
    }

    void resetStatesAndLifetimes() {
        InputStates.clear();
        OutputStates.clear();
        HiddenStates.clear();
        lifeTimes = { 0 };
    }

    // allows users to provide a state vector (ideally in the same format as the hidden state vectors) that the brain will use to set it's internal state.
    virtual void setHiddenState(std::vector<double> newstate) {
        std::cout << "ERROR IN AbstractBrain::setHiddenState - You are attepting using this function with a " << getType() << " brain. But no specialized version of has been written for this brain type." << std::endl;
        exit(1);
    }
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    // end information theory stuff
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
};
