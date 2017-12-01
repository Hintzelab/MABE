//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "GateBuilder.h"
#include <cmath>

shared_ptr<ParameterLink<bool>> Gate_Builder::usingProbGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_PROBABILISTIC-allow", false, "set to true to enable probabilistic gates");
shared_ptr<ParameterLink<int>> Gate_Builder::probGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_PROBABILISTIC-initialCount", 3, "seed genome with this many start codons");
shared_ptr<ParameterLink<bool>> Gate_Builder::usingDetGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_DETERMINISTIC-allow", true, "set to true to enable deterministic gates?");
shared_ptr<ParameterLink<int>> Gate_Builder::detGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_DETERMINISTIC-initialCount", 6, "seed genome with this many start codons");
shared_ptr<ParameterLink<bool>> Gate_Builder::usingEpsiGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_EPSILON-allow", false, "set to true to enable epsilon gates");
shared_ptr<ParameterLink<int>> Gate_Builder::epsiGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_EPSILON-initialCount", 3, "seed genome with this many start codons");
shared_ptr<ParameterLink<bool>> Gate_Builder::usingVoidGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_VOID-allow", false, "set to true to enable void gates");
shared_ptr<ParameterLink<int>> Gate_Builder::voidGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_VOID-initialCount", 3, "seed genome with this many start codons");

shared_ptr<ParameterLink<bool>> Gate_Builder::usingGPGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_GENETICPROGRAMING-allow", false, "set to true to enable GP (what?) gates");
shared_ptr<ParameterLink<int>> Gate_Builder::gPGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_GENETICPROGRAMING-initialCount", 3, "seed genome with this many start codons");
//shared_ptr<ParameterLink<bool>> Gate_Builder::usingThGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES-thresholdGate", false, "set to true to enable threshold gates");
//shared_ptr<ParameterLink<int>> Gate_Builder::thGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES-thresholdGate_InitialCount", 3, "seed genome with this many start codons");

shared_ptr<ParameterLink<bool>> Gate_Builder::usingTritDeterministicGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_TRIT-allow", false, "set to true to enable tritDeterministic gates");
shared_ptr<ParameterLink<int>> Gate_Builder::tritDeterministicGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_TRIT-initialCount", 3, "seed genome with this many start codons");

shared_ptr<ParameterLink<bool>> Gate_Builder::usingNeuronGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-allow", false, "set to true to enable Neuron gates");
shared_ptr<ParameterLink<int>> Gate_Builder::neuronGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-initialCount", 3, "seed genome with this many start codons");

shared_ptr<ParameterLink<bool>> Gate_Builder::usingFeedbackGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_FEEDBACK-allow", false, "set to true to enable feedback gates");
shared_ptr<ParameterLink<int>> Gate_Builder::feedbackGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_FEEDBACK-initialCount", 3, "seed genome with this many start codons");

shared_ptr<ParameterLink<int>> Gate_Builder::bitsPerBrainAddressPL = Parameters::register_parameter("BRAIN_MARKOV_ADVANCED-bitsPerBrainAddress", 8, "how many bits are evaluated to determine the brain addresses");
shared_ptr<ParameterLink<int>> Gate_Builder::bitsPerCodonPL = Parameters::register_parameter("BRAIN_MARKOV_ADVANCED-bitsPerCodon", 8, "how many bits are evaluated to determine the codon addresses");

// *** General tools for All Gates ***

// Gets "howMany" addresses, advances the genome_index buy "howManyMax" addresses and updates "codingRegions" with the addresses being used.
void Gate_Builder::getSomeBrainAddresses(const int& howMany, const int& howManyMax, vector<int>& addresses, shared_ptr<AbstractGenome::Handler> genomeHandler, int code, int gateID, shared_ptr<ParametersTable> _PT) {
	int i;
	for (i = 0; i < howMany; i++) {  // for the number of addresses we need
		addresses[i] = genomeHandler->readInt(0, (1 << bitsPerBrainAddressPL->get(_PT)) - 1, code, gateID);  // get an address
	}
	while (i < howManyMax) { // leave room in the genome in case this gate gets more IO later
		genomeHandler->readInt(0, (1 << bitsPerBrainAddressPL->get(_PT)) - 1);
		i++;
	}
}

// given a genome and a genomeIndex:
// pull out the number a number of inputs, number of outputs and then that many inputs and outputs
// if number of inputs or outputs is less then the max possible inputs or outputs skip the unused sites in the genome
pair<vector<int>, vector<int>> Gate_Builder::getInputsAndOutputs(const pair<int, int> insRange, const pair<int, int> outsRange, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) { // (max#in, max#out,currentIndexInGenome,genome,codingRegions)

	int numInputs = genomeHandler->readInt(insRange.first, insRange.second, AbstractGate::IN_COUNT_CODE, gateID);
	//cout << "num_Inputs: " << numInputs << "\n";
	int numOutputs = genomeHandler->readInt(outsRange.first, outsRange.second, AbstractGate::OUT_COUNT_CODE, gateID);
	//cout << "num_Outputs: " << numOutputs << "\n";
	vector<int> inputs;
	vector<int> outputs;

	inputs.resize(numInputs);
	outputs.resize(numOutputs);

	if (insRange.second > 0) {
		getSomeBrainAddresses(numInputs, insRange.second, inputs, genomeHandler, AbstractGate::IN_ADDRESS_CODE, gateID, _PT);
	}
	if (outsRange.second > 0) {
		getSomeBrainAddresses(numOutputs, outsRange.second, outputs, genomeHandler, AbstractGate::OUT_ADDRESS_CODE, gateID, _PT);
	}
	return {inputs,outputs};
}

// wrapper for getInputsAndOutputs - converts string with format MinIn-MaxIn/MinOut-MaxOut to two pairs and calls getInputsAndOutputs() with pairs
pair<vector<int>, vector<int>> Gate_Builder::getInputsAndOutputs(const string IO_Ranges, int& inMax, int& outMax, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT, const string featureName) {
	stringstream ss(IO_Ranges);
	int inMin, outMin;
	char c;

	ss >> inMin;
	if (ss.fail()) {
		cout << "  SYNTAX ERROR in IO_range: \"" << IO_Ranges << "\" for \"" << featureName << "\".\n  Exiting." << endl;
		exit(1);
	}
	ss >> c;
	if (c != '-') {
		cout << "  SYNTAX ERROR in IO_range: \"" << IO_Ranges << "\" for \"" << featureName << "\".\n  Exiting." << endl;
		exit(1);
	}
	ss >> inMax;
	if (ss.fail()) {
		cout << "  SYNTAX ERROR in IO_range: \"" << IO_Ranges << "\" for \"" << featureName << "\".\n  Exiting." << endl;
		exit(1);
	}
	ss >> c;
	if (c != ',') {
		cout << "  SYNTAX ERROR in IO_range: \"" << IO_Ranges << "\" for \"" << featureName << "\".\n  Exiting." << endl;
		exit(1);
	}
	ss >> outMin;
	if (ss.fail()) {
		cout << "  SYNTAX ERROR in IO_range: \"" << IO_Ranges << "\" for \"" << featureName << "\".\n  Exiting." << endl;
		exit(1);
	}
	ss >> c;
	if (c != '-') {
		cout << "  SYNTAX ERROR in IO_range: \"" << IO_Ranges << "\" for \"" << featureName << "\".\n  Exiting." << endl;
		exit(1);
	}
	ss >> outMax;
	if (ss.fail()) {
		cout << "  SYNTAX ERROR in IO_range: \"" << IO_Ranges << "\" for \"" << featureName << "\".\n  Exiting." << endl;
		exit(1);
	}
	return getInputsAndOutputs( { inMin, inMax }, { outMin, outMax }, genomeHandler, gateID, _PT);
}

//setupGates() populates Gate::makeGate (a structure containing functions) with the constructors for various types of gates.
//there are 256 possible gates identified each by a pair of codons (n followed by 256-n)
//after initializing Gate::MakeGate, Gate::AddGate() adds values and the associated constructor function to Gate::MakeGate
void Gate_Builder::setupGates() {
	// the following "Codes" are identifiers for different gate types. These number are used to look up constructors and also as the first 1/2 of the start codeon
	// if codons are being used to generate gates.
	// more may be added, but the numbers should not change (if they do, then genomes will generate different brains!)
	int ProbabilisticCode = 42;
	int DeterministicCode = 43;
	int EpsilonCode = 44;
	int VoidCode = 45;
	int GPCode = 46;
	int TritDeterministicCode = 47;
	int NeuronCode = 48;
	int FeedbackCode = 49;
	//	int ThresholdCode = 50;

	int bitsPerCodon = bitsPerCodonPL->get(PT);
	makeGate.resize(1 << bitsPerCodon);
	for (int i = 0; i < (1 << bitsPerCodon); i++) {
		AddGate(i, nullptr);
	}
	gateStartCodes.resize(1 << bitsPerCodon);

	if (usingProbGatePL->get(PT)) {
		inUseGateNames.insert("Probabilistic");
		int codonOne = ProbabilisticCode;
		inUseGateTypes.insert(codonOne);
		{
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = probGateInitialCountPL->get(PT);
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
			string IO_Ranges = ProbabilisticGate::IO_RangesPL->get(_PT);
			int maxIn, maxOut;
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID, _PT, "BRAIN_MARKOV_GATES_PROBABILISTIC");
			vector<vector<int>> rawTable = genomeHandler->readTable( {1 << addresses.first.size(), 1 << addresses.second.size()}, {(int)pow(2,maxIn), (int)pow(2,maxOut)}, {0, 255}, AbstractGate::DATA_CODE, gateID);
			if (genomeHandler->atEOC()) {
				shared_ptr<ProbabilisticGate> nullObj = nullptr;
				return nullObj;
			}
			return make_shared<ProbabilisticGate>(addresses,rawTable,gateID, _PT);
		});
	}
	if (usingDetGatePL->get(PT)) {
		inUseGateNames.insert("Deterministic");
		int codonOne = DeterministicCode;
		inUseGateTypes.insert(codonOne);
		{
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = detGateInitialCountPL->get(PT);
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
			string IO_Ranges = DeterministicGate::IO_RangesPL->get(_PT);
			int maxIn, maxOut;
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID, _PT, "BRAIN_MARKOV_GATES_DETERMINISTIC");
			vector<vector<int>> table = genomeHandler->readTable( {1 << (int)addresses.first.size(), (int)addresses.second.size()}, {(int)pow(2,maxIn), maxOut}, {0, 1}, AbstractGate::DATA_CODE, gateID);
			if (genomeHandler->atEOC()) {
				shared_ptr<DeterministicGate> nullObj = nullptr;
				return nullObj;
			}
			return make_shared<DeterministicGate>(addresses,table,gateID, _PT);
		});
	}
	if (usingEpsiGatePL->get(PT)) {
		inUseGateNames.insert("Epsilon");
		int codonOne = EpsilonCode;
		inUseGateTypes.insert(codonOne);
		{
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = epsiGateInitialCountPL->get(PT);
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {

			string IO_Ranges = EpsilonGate::IO_RangesPL->get(_PT);
			int maxIn, maxOut;
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID, _PT, "BRAIN_MARKOV_GATES_EPSILON");
			vector<vector<int>> table = genomeHandler->readTable( {1 << (int)addresses.first.size(), (int)addresses.second.size()}, {(int)pow(2,maxIn), maxOut}, {0, 1}, AbstractGate::DATA_CODE, gateID);

			double epsilon = EpsilonGate::EpsilonSourcePL->get(_PT);

			if (epsilon > 1) {
				genomeHandler->advanceIndex((int)epsilon);
				epsilon = genomeHandler->readDouble(0, 1, AbstractGate::DATA_CODE, gateID);
			}
			else if (epsilon < 0) {
				// if we are reading from head of genome, the we are not worried about EOC, so we get a new handler so as not to reset the EOC/EOG staus.
				auto cleanGenomeHandler = genomeHandler->makeCopy();
				cleanGenomeHandler->resetHandler();
				cleanGenomeHandler->advanceIndex((int)abs(epsilon));
				epsilon = cleanGenomeHandler->readDouble(0, 1, AbstractGate::DATA_CODE, gateID);
			}

			return make_shared<EpsilonGate>(addresses,table,gateID, epsilon, _PT);
		});
	}
	if (usingVoidGatePL->get(PT)) {
		inUseGateNames.insert("Void");
		int codonOne = VoidCode;
		inUseGateTypes.insert(codonOne);
		{
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = voidGateInitialCountPL->get(PT);
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {

			string IO_Ranges = VoidGate::IO_RangesPL->get(_PT);
			int maxIn, maxOut;
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID, _PT, "BRAIN_MARKOV_GATES_VOID");
			vector<vector<int>> table = genomeHandler->readTable( {1 << (int)addresses.first.size(), (int)addresses.second.size()}, {(int)pow(2,maxIn), maxOut}, {0, 1}, AbstractGate::DATA_CODE, gateID);

			double epsilon = VoidGate::voidGate_ProbabilityPL->get(_PT);

			if (epsilon > 1) {
				genomeHandler->advanceIndex((int)epsilon);
				epsilon = genomeHandler->readDouble(0, 1, AbstractGate::DATA_CODE, gateID);
			}
			else if (epsilon < 0) {
				// if we are reading from head of genome, the we are not worried about EOC, so we get a new handler so as not to reset the EOC/EOG staus.
				auto cleanGenomeHandler = genomeHandler->makeCopy();
				cleanGenomeHandler->resetHandler();
				cleanGenomeHandler->advanceIndex((int)abs(epsilon));
				epsilon = cleanGenomeHandler->readDouble(0, 1, AbstractGate::DATA_CODE, gateID);
			}

			return make_shared<VoidGate>(addresses,table,gateID, epsilon, _PT);
		});
	}
	if (usingGPGatePL->get(PT)) {
		inUseGateNames.insert("GeneticPrograming");
		int codonOne = GPCode;
		inUseGateTypes.insert(codonOne);
		{
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = gPGateInitialCountPL->get(PT);
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
			string IO_Ranges = GPGate::IO_RangesPL->get(_PT);
			int maxIn, maxOut;
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID, _PT, "BRAIN_MARKOV_GATES_GENETICPROGRAMING");
			int operation = genomeHandler->readInt(0, 8, AbstractGate::DATA_CODE, gateID);
			vector<double> constValues;
			for (int i = 0; i < 4; i++) {
				double constValueMin = GPGate::constValueMinPL->get(_PT);
				double constValueMax = GPGate::constValueMaxPL->get(_PT);
				constValues.push_back(genomeHandler->readDouble(constValueMin, constValueMax,AbstractGate::DATA_CODE, gateID));
			}
			if (genomeHandler->atEOC()) {
				shared_ptr<GPGate> nullObj = nullptr;;
				return nullObj;
			}
			return make_shared<GPGate>(addresses,operation, constValues, gateID, _PT);
		});
	}
	if (usingTritDeterministicGatePL->get(PT)) {
		inUseGateNames.insert("TritDeterministic");
		int codonOne = TritDeterministicCode;
		inUseGateTypes.insert(codonOne);
		{
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = tritDeterministicGateInitialCountPL->get(PT);

		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
			string IO_Ranges = TritDeterministicGate::IO_RangesPL->get(_PT);
			int maxIn, maxOut;
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID, _PT, "BRAIN_MARKOV_GATES_TRIT");
			vector<vector<int>> table = genomeHandler->readTable( {(int)pow(3,(int)addresses.first.size()), (int)addresses.second.size()}, {(int)pow(3,maxIn), maxOut}, {-1, 1}, AbstractGate::DATA_CODE, gateID);
			if (genomeHandler->atEOC()) {
				shared_ptr<TritDeterministicGate> nullObj = nullptr;;
				return nullObj;
			}
			return make_shared<TritDeterministicGate>(addresses,table,gateID, _PT);
		});
	}
	if (usingNeuronGatePL->get(PT)) {
		inUseGateNames.insert("Neuron");
		int codonOne = NeuronCode;
		inUseGateTypes.insert(codonOne);
		{
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = neuronGateInitialCountPL->get(PT);
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
			int defaultNumInputsMin = NeuronGate::defaultNumInputsMinPL->get(_PT);
			int defaultNumInputsMax = NeuronGate::defaultNumInputsMaxPL->get(_PT);
			int numInputs = genomeHandler->readInt(defaultNumInputsMin, defaultNumInputsMax, AbstractGate::IN_COUNT_CODE, gateID);
			vector<int> inputs;
			inputs.resize(numInputs);

			getSomeBrainAddresses(numInputs, defaultNumInputsMax, inputs, genomeHandler, AbstractGate::IN_ADDRESS_CODE, gateID, _PT);

			int output = genomeHandler->readInt(0, (1 << bitsPerBrainAddressPL->get(_PT)) - 1, AbstractGate::OUT_ADDRESS_CODE, gateID);

			int dischargeBehavior = NeuronGate::defaultDischargeBehaviorPL->get(_PT);
			if (dischargeBehavior == -1) {
				dischargeBehavior = genomeHandler->readInt(0, 2, AbstractGate::DATA_CODE, gateID);
			}

			double defaultThresholdMin = NeuronGate::defaultThresholdMinPL->get(_PT);
			double defaultThresholdMax = NeuronGate::defaultThresholdMaxPL->get(_PT);
			double thresholdValue = genomeHandler->readDouble(defaultThresholdMin, defaultThresholdMax, AbstractGate::DATA_CODE, gateID);

			bool thresholdActivates = 1;
			bool defaultAllowRepression = NeuronGate::defaultAllowRepressionPL->get(_PT);
			if (defaultAllowRepression == 1) {
				thresholdActivates = genomeHandler->readInt(0, 1, AbstractGate::DATA_CODE, gateID);
			}

			double decayRate = genomeHandler->readDouble(NeuronGate::defaultDecayRateMinPL->get(_PT), NeuronGate::defaultDecayRateMaxPL->get(_PT), AbstractGate::DATA_CODE, gateID);
			double deliveryCharge = genomeHandler->readDouble(NeuronGate::defaultDeliveryChargeMinPL->get(_PT), NeuronGate::defaultDeliveryChargeMaxPL->get(_PT), AbstractGate::DATA_CODE, gateID);
			double deliveryError = NeuronGate::defaultDeliveryErrorPL->get(_PT);

			int ThresholdFromNode = -1;
			int DeliveryChargeFromNode = -1;
			bool defaultThresholdFromNode = NeuronGate::defaultThresholdFromNodePL->get(_PT);
			if (defaultThresholdFromNode) {
				ThresholdFromNode = genomeHandler->readInt(0, (1 << bitsPerBrainAddressPL->get(_PT)) - 1, AbstractGate::IN_ADDRESS_CODE, gateID);
			}
			bool defaultDeliveryChargeFromNode = NeuronGate::defaultDeliveryChargeFromNodePL->get(_PT);
			if (defaultDeliveryChargeFromNode) {
				DeliveryChargeFromNode = genomeHandler->readInt(0, (1 << bitsPerBrainAddressPL->get(_PT)) - 1, AbstractGate::IN_ADDRESS_CODE, gateID);
			}
			if (genomeHandler->atEOC()) {
				shared_ptr<NeuronGate> nullObj = nullptr;;
				return nullObj;
			}
			return make_shared<NeuronGate>(inputs, output, dischargeBehavior, thresholdValue, thresholdActivates, decayRate, deliveryCharge, deliveryError, ThresholdFromNode, DeliveryChargeFromNode, gateID, _PT);
		});
	}
	if (usingFeedbackGatePL->get(PT)) {
		inUseGateNames.insert("Feedback");
		int codonOne = FeedbackCode;
		inUseGateTypes.insert(codonOne);
		{
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = feedbackGateInitialCountPL->get(PT);
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
            unsigned int posFBNode, negFBNode;
            unsigned char nrPos, nrNeg;
            vector<double> posLevelOfFB, negLevelOfFB;
			string IO_Ranges = FeedbackGate::IO_RangesPL->get(_PT);
			int maxIn, maxOut;
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID, _PT, "BRAIN_MARKOV_GATES_FEEDBACK");
			vector<vector<int>> rawTable = genomeHandler->readTable( {1 << addresses.first.size(), 1 << addresses.second.size()}, {(int)pow(2,maxIn), (int)pow(2,maxOut)}, {0, 255}, AbstractGate::DATA_CODE, gateID);
            posFBNode = genomeHandler->readInt(0,255); // we will scale to, say, 256 and floor later.
            negFBNode = genomeHandler->readInt(0,255);
            nrPos = genomeHandler->readInt(0,3);
            nrNeg = genomeHandler->readInt(0,3);
            posLevelOfFB.resize(nrPos);
            negLevelOfFB.resize(nrNeg);
            for (int i=0; i<nrPos; i++) posLevelOfFB[i] = genomeHandler->readDouble(0,256);
            for (int i=0; i<nrNeg; i++) negLevelOfFB[i] = genomeHandler->readDouble(0,256);
			if (genomeHandler->atEOC()) {
				shared_ptr<FeedbackGate> nullObj = nullptr;
				return nullObj;
			}
			return make_shared<FeedbackGate>(addresses,rawTable,posFBNode,negFBNode,nrPos,nrNeg,posLevelOfFB,negLevelOfFB,gateID, _PT);
            //std::pair<vector<int>,vector<int>> thepair = std::make_pair(std::vector<int>(),std::vector<int>());
            //std::vector<std::vector<int>> dvec;
            //unsigned int uint = 0;
            //unsigned char uchar = '\0';
            //std::vector<double> vdouble;
			//return make_shared<FeedbackGate>(thepair,dvec,uint,uint,uchar,uchar,vdouble,vdouble,gateID, _PT);
		});
	}
}

/* *** some c++ 11 magic to speed up translation from genome to gates *** */
//function<shared_ptr<Gate>(shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID)> Gate_Builder::makeGate[256];
//vector<function<shared_ptr<AbstractGate>(shared_ptr<AbstractGenome::Handler>, int gateID)>> Gate_Builder::makeGate;
void Gate_Builder::AddGate(int gateType, function<shared_ptr<AbstractGate>(shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> gatePT)> theFunction) {
	makeGate[gateType] = theFunction;
}
/* *** end - some c++ 11 magic to speed up translation from genome to gates *** */
