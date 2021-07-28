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

#include "../AbstractWorld.h"

#include "../../Utilities/Utilities.h"
#include "../../Analyze/neurocorrelates.h"
#include "../../Analyze/fragmentation.h"
//#include "../../Brain/MarkovBrain/MarkovBrain.h"
//#include "../../Brain/RNNBrain/RNNBrain.h"

#include "../../Analyze/brainTools.h"
#include "../../Analyze/fragmentation.h"
#include "../../Analyze/smearedness.h"

#include <cstdlib>
#include <thread>

class NBackWorld : public AbstractWorld {

public:
    static std::shared_ptr<ParameterLink<int>> testMutantsPL;
    //static std::shared_ptr<ParameterLink<std::string>> analyzeWhatPL;
    static std::shared_ptr<ParameterLink<std::string>> NsListsPL;
    static std::shared_ptr<ParameterLink<int>> testsPerEvaluationPL;
    static std::shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;
    static std::shared_ptr<ParameterLink<int>> delayOutputEvalPL;
    static std::shared_ptr<ParameterLink<int>> scoreMultPL;
    static std::shared_ptr<ParameterLink<int>> RMultPL;
    static std::shared_ptr<ParameterLink<bool>> tritInputsPL;

    static std::shared_ptr<ParameterLink<bool>> saveFragOverTimePL;
    static std::shared_ptr<ParameterLink<bool>> saveBrainStructureAndConnectomePL;
    static std::shared_ptr<ParameterLink<bool>> saveStateToStatePL;
    static std::shared_ptr<ParameterLink<bool>> save_R_FragMatrixPL;
    static std::shared_ptr<ParameterLink<bool>> saveFlowMatrixPL;
    static std::shared_ptr<ParameterLink<bool>> saveStatesPL;
    
    bool saveFragOverTime;
    bool saveBrainStructureAndConnectome;
    bool saveStateToState;
    bool save_R_FragMatrix;
    bool saveFlowMatrix;
    bool saveStates;


    bool tritInputs = false;
    int testMutants = 0;
    std::vector<int> NListSwitchTimes; // when to switch from one List to the next
    std::vector<std::vector<int>> NListLists; // What outputs are being scored at the current time

    int testsPerEvaluation; // each agent is reset and evaluated this number of times
    int evaluationsPerGeneration; // each agent sees this number of inputs (+largest N) and is scored this number of times each evaluation

    static std::shared_ptr<ParameterLink<std::string>> groupNamePL;
    static std::shared_ptr<ParameterLink<std::string>> brainNamePL;
    std::string groupName;
    std::string brainName;

    int largestN = 0;
    int currentLargestN = 0; // largest N in the current list
    std::map<int, int> N2OutMap;
    int currentNList = 0;

    NBackWorld(std::shared_ptr<ParametersTable> PT_ = nullptr);
    virtual ~NBackWorld() = default;

    void evaluateSolo(std::shared_ptr<Organism> org, int analyze,
        int visualize, int debug);
    void evaluate(std::map<std::string, std::shared_ptr<Group>>& groups,
        int analyze, int visualize, int debug);

    virtual std::unordered_map<std::string, std::unordered_set<std::string>>
        requiredGroups() override;



    /* std::vector<int> getHiddenBrainStates(std::shared_ptr<AbstractBrain> brain) {
         std::vector<int> tempHidden;
         if (brain->getType() == "Markov") {
             std::shared_ptr<MarkovBrain> specificBrain = std::dynamic_pointer_cast<MarkovBrain>(brain);
             tempHidden = specificBrain->getHiddenNodes();
         }
         else if (brain->getType() == "RNN") {
             std::shared_ptr<RNNBrain> specificBrain = std::dynamic_pointer_cast<RNNBrain>(brain);
             tempHidden = specificBrain->getHiddenNodes();
         }
         else {
             std::cout << "Error. Not using a brain with the correct type. Markov, LSTM, RNN, VectorTree, and Forest are currently supported. Exiting..." << std::endl;
             exit(1);
         }
         return tempHidden;
     }
     */

};

