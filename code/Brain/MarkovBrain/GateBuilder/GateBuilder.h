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

#include <bitset>
#include <Utilities/Parameters.h>
#include <Brain/MarkovBrain/Gate/DeterministicGate.h>
#include <Brain/MarkovBrain/Gate/EpsilonGate.h>
#include <Brain/MarkovBrain/Gate/FeedbackGate.h>
#include <Brain/MarkovBrain/Gate/GPGate.h>
#include <Brain/MarkovBrain/Gate/NeuronGate.h>
#include <Brain/MarkovBrain/Gate/ProbabilisticGate.h>
#include <Brain/MarkovBrain/Gate/TritDeterministicGate.h>
#include <Brain/MarkovBrain/Gate/VoidGate.h>
#include <Brain/MarkovBrain/Gate/DecomposableGate.h>
#include <Brain/MarkovBrain/Gate/DecomposableDirectGate.h>
#include <Brain/MarkovBrain/Gate/DecomposableFeedbackGate.h>
#include <Brain/MarkovBrain/Gate/ComparatorGate.h>
#include <Brain/MarkovBrain/Gate/PassthroughGate.h>
#include <Brain/MarkovBrain/Gate/IzhikevichGate.h>
#include <Brain/MarkovBrain/Gate/AnnGate.h>

class Gate_Builder {  // manages what kinds of gates can be built
public:

	static std::shared_ptr<ParameterLink<bool>> usingProbGatePL;
	static std::shared_ptr<ParameterLink<int>> probGateInitialCountPL;
	static std::shared_ptr<ParameterLink<bool>> usingDetGatePL;
	static std::shared_ptr<ParameterLink<int>> detGateInitialCountPL;
	static std::shared_ptr<ParameterLink<bool>> usingEpsiGatePL;
	static std::shared_ptr<ParameterLink<int>> epsiGateInitialCountPL;
	static std::shared_ptr<ParameterLink<bool>> usingVoidGatePL;
	static std::shared_ptr<ParameterLink<int>> voidGateInitialCountPL;

	static std::shared_ptr<ParameterLink<bool>> usingFeedbackGatePL;
	static std::shared_ptr<ParameterLink<int>> feedbackGateInitialCountPL;
	static std::shared_ptr<ParameterLink<bool>> usingDecomposableFeedbackGatePL;
	static std::shared_ptr<ParameterLink<int>> decomposableFeedbackGateInitialCountPL;
	static std::shared_ptr<ParameterLink<bool>> usingDecoGatePL;
	static std::shared_ptr<ParameterLink<bool>> decoUse2LevelPL;
	static std::shared_ptr<ParameterLink<bool>> deco2LevelRowFirstPL;
	static std::shared_ptr<ParameterLink<int>> decoGateInitialCountPL;
	static std::shared_ptr<ParameterLink<bool>> usingGPGatePL;
	static std::shared_ptr<ParameterLink<int>> gPGateInitialCountPL;
	static std::shared_ptr<ParameterLink<int>> thGateInitialCountPL;

	static std::shared_ptr<ParameterLink<bool>> usingDecoDirectGatePL;
	static std::shared_ptr<ParameterLink<int>> decoDirectGateInitialCountPL;

	static std::shared_ptr<ParameterLink<bool>> usingTritDeterministicGatePL;
	static std::shared_ptr<ParameterLink<int>> tritDeterministicGateInitialCountPL;

	static std::shared_ptr<ParameterLink<bool>> usingNeuronGatePL;
	static std::shared_ptr<ParameterLink<int>> neuronGateInitialCountPL;

	static std::shared_ptr<ParameterLink<bool>> usingIzhikevichGatePL;
	static std::shared_ptr<ParameterLink<int>> izhikevichGateInitialCountPL;

	static std::shared_ptr<ParameterLink<bool>> usingAnnGatePL;
	static std::shared_ptr<ParameterLink<int>> annGateInitialCountPL;

	static std::shared_ptr<ParameterLink<bool>> usingComparatorGatePL;
	static std::shared_ptr<ParameterLink<int>> comparatorGateInitialCountPL;

	static std::shared_ptr<ParameterLink<bool>> usingPassthroughGatePL;
	static std::shared_ptr<ParameterLink<int>> passthroughGateInitialCountPL;

	static std::shared_ptr<ParameterLink<int>> bitsPerBrainAddressPL;  // how many bits are evaluated to determine the brain addresses.
	static std::shared_ptr<ParameterLink<int>> bitsPerCodonPL;

	std::set<int> inUseGateTypes;
	std::set<std::string> inUseGateNames;
	std::vector<std::vector<int>> gateStartCodes;

	std::map<int, int> intialGateCounts;

	const std::shared_ptr<ParametersTable> PT;

	//Gate_Builder() = default;
	Gate_Builder(std::shared_ptr<ParametersTable> _PT = nullptr) : PT(_PT){

		setupGates();
	}

	~Gate_Builder() = default;

	void AddGate(int gateType, std::function<std::shared_ptr<AbstractGate>(std::shared_ptr<AbstractGenome::Handler>, int gateID, std::shared_ptr<ParametersTable> gatePT)> theFunction);
	void setupGates();
	std::vector<std::function<std::shared_ptr<AbstractGate>(std::shared_ptr<AbstractGenome::Handler>, int gateID, std::shared_ptr<ParametersTable> gatePT)>> makeGate;

	//int getIOAddress(shared_ptr<AbstractGenome::Handler> genomeHandler, shared_ptr<AbstractGenome> genome, int gateID);  // extracts one brain state value address from a genome
	static void getSomeBrainAddresses(const int& howMany, const int& howManyMax, std::vector<int>& addresses, std::shared_ptr<AbstractGenome::Handler> genomeHandler, int code, int gateID, std::shared_ptr<ParametersTable> _PT);  // extracts many brain state value addresses from a genome
	static std::pair<std::vector<int>, std::vector<int>> getInputsAndOutputs(const std::pair<int, int> insRange, const std::pair<int, int>, std::shared_ptr<AbstractGenome::Handler> genomeHandle, int gateID, std::shared_ptr<ParametersTable> _PT);  // extracts the input and output brain state value addresses for this gate
	static std::pair<std::vector<int>, std::vector<int>> getInputsAndOutputs(const std::string IO_ranges, int& inMax, int& outMax, std::shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, std::shared_ptr<ParametersTable> _PT, const std::string featureName = "undefined");

	/* *** some c++ 11 magic to speed up translation from genome to gates *** */
	//function<shared_ptr<Gate>(shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID)> Gate_Builder::makeGate[256];
};


