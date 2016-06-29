//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

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

//shared_ptr<ParameterLink<bool>> Gate_Builder::usingFBGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES-feedBackGate", false, "set to true to enable feedback gates");
//shared_ptr<ParameterLink<int>> Gate_Builder::fBGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES-feedBackGate_InitialCount", 3, "seed genome with this many start codons");
shared_ptr<ParameterLink<bool>> Gate_Builder::usingGPGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_GENETICPROGRAMING-allow", false, "set to true to enable GP (what?) gates");
shared_ptr<ParameterLink<int>> Gate_Builder::gPGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_GENETICPROGRAMING-initialCount", 3, "seed genome with this many start codons");
//shared_ptr<ParameterLink<bool>> Gate_Builder::usingThGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES-thresholdGate", false, "set to true to enable threshold gates");
//shared_ptr<ParameterLink<int>> Gate_Builder::thGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES-thresholdGate_InitialCount", 3, "seed genome with this many start codons");

shared_ptr<ParameterLink<bool>> Gate_Builder::usingTritDeterministicGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_TRIT-allow", false, "set to true to enable tritDeterministic gates");
shared_ptr<ParameterLink<int>> Gate_Builder::tritDeterministicGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_TRIT-initialCount", 3, "seed genome with this many start codons");

shared_ptr<ParameterLink<bool>> Gate_Builder::usingNeuronGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-allow", false, "set to true to enable Neuron gates");
shared_ptr<ParameterLink<int>> Gate_Builder::neuronGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-initialCount", 3, "seed genome with this many start codons");

// *** General tools for All Gates ***

// Gets "howMany" addresses, advances the genome_index buy "howManyMax" addresses and updates "codingRegions" with the addresses being used.
void Gate_Builder::getSomeBrainAddresses(const int& howMany, const int& howManyMax, vector<int>& addresses, shared_ptr<AbstractGenome::Handler> genomeHandler, int code, int gateID) {
	int i;
	for (i = 0; i < howMany; i++) {  // for the number of addresses we need
		addresses[i] = genomeHandler->readInt(0, (1 << Global::bitsPerBrainAddressPL->lookup()) - 1, code, gateID);  // get an address
	}
	while (i < howManyMax) { // leave room in the genome in case this gate gets more IO later
		genomeHandler->readInt(0, (1 << Global::bitsPerBrainAddressPL->lookup()) - 1);
		i++;
	}
}

// given a genome and a genomeIndex:
// pull out the number a number of inputs, number of outputs and then that many inputs and outputs
// if number of inputs or outputs is less then the max possible inputs or outputs skip the unused sites in the genome
pair<vector<int>, vector<int>> Gate_Builder::getInputsAndOutputs(const pair<int, int> insRange, const pair<int, int> outsRange, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) { // (max#in, max#out,currentIndexInGenome,genome,codingRegions)

	int numInputs = genomeHandler->readInt(insRange.first, insRange.second, AbstractGate::IN_COUNT_CODE, gateID);
	//cout << "num_Inputs: " << numInputs << "\n";
	int numOutputs = genomeHandler->readInt(outsRange.first, outsRange.second, AbstractGate::OUT_COUNT_CODE, gateID);
	//cout << "num_Outputs: " << numOutputs << "\n";
	vector<int> inputs;
	vector<int> outputs;

	inputs.resize(numInputs);
	outputs.resize(numOutputs);

	if (insRange.second > 0) {
		getSomeBrainAddresses(numInputs, insRange.second, inputs, genomeHandler, AbstractGate::IN_ADDRESS_CODE, gateID);
	}
	if (outsRange.second > 0) {
		getSomeBrainAddresses(numOutputs, outsRange.second, outputs, genomeHandler, AbstractGate::OUT_ADDRESS_CODE, gateID);
	}
	return {inputs,outputs};
}

// wrapper for getInputsAndOutputs - converts string with format MinIn-MaxIn/MinOut-MaxOut to two pairs and calls getInputsAndOutputs() with pairs
pair<vector<int>, vector<int>> Gate_Builder::getInputsAndOutputs(const string IO_Ranges, int& inMax, int& outMax, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, const string featureName) {
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
	return getInputsAndOutputs( { inMin, inMax }, { outMin, outMax }, genomeHandler, gateID);
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
	//	int FeedbackCode = 49;
	//	int ThresholdCode = 50;

	int bitsPerCodon = Global::bitsPerCodonPL->lookup();
	makeGate.resize(1 << bitsPerCodon);
	for (int i = 0; i < (1 << bitsPerCodon); i++) {
		AddGate(i, nullptr);
	}
	gateStartCodes.resize(1 << bitsPerCodon);

	if ((PT == nullptr) ? usingProbGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES_PROBABILISTIC-allow")) {
		inUseGateNames.insert("Probabilistic");
		int codonOne = ProbabilisticCode;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = (PT == nullptr) ? probGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES_PROBABILISTIC-initialCount");
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
			//pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			string IO_Ranges = (_PT == nullptr) ? DeterministicGate::IO_RangesPL->lookup() : _PT->lookupString("BRAIN_MARKOV_GATES_PROBABILISTIC-IO_Ranges");
			int maxIn, maxOut;
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID,"BRAIN_MARKOV_GATES_PROBABILISTIC");
			vector<vector<int>> rawTable = genomeHandler->readTable( {1 << addresses.first.size(), 1 << addresses.second.size()}, {pow(2,maxIn), pow(2,maxOut)}, {0, 255}, AbstractGate::DATA_CODE, gateID);
			if (genomeHandler->atEOC()) {
				shared_ptr<ProbabilisticGate> nullObj = nullptr;
				return nullObj;
			}
			return make_shared<ProbabilisticGate>(addresses,rawTable,gateID, _PT);
		});
	}
	if ((PT == nullptr) ? usingDetGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES_DETERMINISTIC-allow")) {
		inUseGateNames.insert("Deterministic");
		int codonOne = DeterministicCode;
		inUseGateTypes.insert(codonOne);
		{
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = (PT == nullptr) ? detGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES_DETERMINISTIC-initialCount");
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
			//pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			string IO_Ranges = (_PT == nullptr) ? DeterministicGate::IO_RangesPL->lookup() : _PT->lookupString("BRAIN_MARKOV_GATES_DETERMINISTIC-IO_Ranges");
			int maxIn, maxOut;
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID,"BRAIN_MARKOV_GATES_DETERMINISTIC");
			vector<vector<int>> table = genomeHandler->readTable( {1 << addresses.first.size(), addresses.second.size()}, {pow(2,maxIn), maxOut}, {0, 1}, AbstractGate::DATA_CODE, gateID);
			if (genomeHandler->atEOC()) {
				shared_ptr<DeterministicGate> nullObj = nullptr;
				return nullObj;
			}
			return make_shared<DeterministicGate>(addresses,table,gateID, _PT);
		});
	}
	if ((PT == nullptr) ? usingEpsiGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES_EPSILON-allow")) {
		inUseGateNames.insert("Epsilon");
		int codonOne = EpsilonCode;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = (PT == nullptr) ? epsiGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES_EPSILON-initialCount");
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {

			//pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			string IO_Ranges = (_PT == nullptr) ? DeterministicGate::IO_RangesPL->lookup() : _PT->lookupString("BRAIN_MARKOV_GATES_EPSILON-IO_Ranges");
			int maxIn, maxOut;
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID,"BRAIN_MARKOV_GATES_EPSILON");
			vector<vector<int>> table = genomeHandler->readTable( {1 << addresses.first.size(), addresses.second.size()}, {pow(2,maxIn), maxOut}, {0, 1}, AbstractGate::DATA_CODE, gateID);

			double epsilon = (_PT == nullptr) ? EpsilonGate::EpsilonSourcePL->lookup() : _PT->lookupDouble("BRAIN_MARKOV_GATES_EPSILON-epsilonSource");

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
	if ((PT == nullptr) ? usingVoidGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES_VOID-allow")) {
		inUseGateNames.insert("Void");
		int codonOne = VoidCode;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = (PT == nullptr) ? voidGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES_VOID-initialCount");
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {

			//pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			string IO_Ranges = (_PT == nullptr) ? DeterministicGate::IO_RangesPL->lookup() : _PT->lookupString("BRAIN_MARKOV_GATES_VOID-IO_Ranges");
			int maxIn, maxOut;
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID,"BRAIN_MARKOV_GATES_VOID");
			vector<vector<int>> table = genomeHandler->readTable( {1 << addresses.first.size(), addresses.second.size()}, {pow(2,maxIn), maxOut}, {0, 1}, AbstractGate::DATA_CODE, gateID);

			double epsilon = (_PT == nullptr) ? VoidGate::voidGate_ProbabilityPL->lookup() : _PT->lookupDouble("BRAIN_MARKOV_GATES_VOID-epsilonSource");

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
	//	if ((PT == nullptr) ? usingFBGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES-feedBackGate")) {
	//		inUseGateNames.insert("FeedBack");
	//		int codonOne = FeedbackCode;
	//		inUseGateTypes.insert(codonOne);
	//		{
	//			gateStartCodes[codonOne].push_back(codonOne);
	//			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
	//		}
	//		intialGateCounts[codonOne] = (PT == nullptr) ? fBGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES-feedBackGate_InitialCount");
	//
	//		cout << "in Gate_Builder::setupGates() : GATES-feedBackGate = true, but there is no current implementation! Exiting." << endl;
	//		exit(1);
	//	}
	//		AddGate(FeedbackCode, [](shared_ptr<AbstractGenome> genome,shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {return make_shared<FeedbackGate>(genome,genomeHandler,gateID);});
	//		Global::inUseGateTypes.insert(44);
	//	}
	if ((PT == nullptr) ? usingGPGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES_GENETICPROGRAMING-allow")) {
		inUseGateNames.insert("GeneticPrograming");
		int codonOne = GPCode;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = (PT == nullptr) ? gPGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES_GENETICPROGRAMING-initialCount");
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
			//pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			string IO_Ranges = (_PT == nullptr) ? DeterministicGate::IO_RangesPL->lookup() : _PT->lookupString("BRAIN_MARKOV_GATES_GENETICPROGRAMING-IO_Ranges");
			int maxIn, maxOut;
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID,"BRAIN_MARKOV_GATES_GENETICPROGRAMING");
			int operation = genomeHandler->readInt(0, 8, AbstractGate::DATA_CODE, gateID);
			vector<double> constValues;
			for (int i = 0; i < 4; i++) {
				double constValueMin = (_PT == nullptr) ? GPGate::constValueMinPL->lookup() : _PT->lookupInt("BRAIN_MARKOV_GATES_GENETICPROGRAMING-constValueMin");
				double constValueMax = (_PT == nullptr) ? GPGate::constValueMaxPL->lookup() : _PT->lookupInt("BRAIN_MARKOV_GATES_GENETICPROGRAMING-constValueMax");
				constValues.push_back(genomeHandler->readDouble(constValueMin, constValueMax,AbstractGate::DATA_CODE, gateID));
			}
			if (genomeHandler->atEOC()) {
				shared_ptr<GPGate> nullObj = nullptr;;
				return nullObj;
			}
			return make_shared<GPGate>(addresses,operation, constValues, gateID, _PT);
		});

		//GPGate(pair<vector<int>, vector<int>> _addresses, int _operation, vector<double> _constValues, int gateID);

	}
	//	if ((PT == nullptr) ? usingThGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES-thresholdGate")) {
	//		inUseGateNames.insert("Threshold");
	//		int codonOne = ThresholdCode;
	//		inUseGateTypes.insert(codonOne);
	//		{
	//			gateStartCodes[codonOne].push_back(codonOne);
	//			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
	//		}
	//		intialGateCounts[codonOne] = (PT == nullptr) ? thGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES-thresholdGate_InitialCount");
	//
	//		cout << "in Gate_Builder::setupGates() : GATES-thresholdGate = true, but there is no current implementation! Exiting." << endl;
	//		exit(1);
	//	}
	//
	//		AddGate(ThresholdCode, [](shared_ptr<AbstractGenome> genome,shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {return make_shared<Thresholdgate>(genome,genomeHandler,gateID);});
	//		Global::inUseGateTypes.insert(46);
	//	}
	if ((PT == nullptr) ? usingTritDeterministicGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES_TRIT-allow")) {
		inUseGateNames.insert("TritDeterministic");
		int codonOne = TritDeterministicCode;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = (PT == nullptr) ? tritDeterministicGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES_TRIT-initialCount");

		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
			//pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			string IO_Ranges = (_PT == nullptr) ? DeterministicGate::IO_RangesPL->lookup() : _PT->lookupString("BRAIN_MARKOV_GATES_TRIT-IO_Ranges");
			int maxIn, maxOut;
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID,"BRAIN_MARKOV_GATES_TRIT");
			vector<vector<int>> table = genomeHandler->readTable( {pow(3,addresses.first.size()), addresses.second.size()}, {pow(3,maxIn), maxOut}, {-1, 1}, AbstractGate::DATA_CODE, gateID);
			if (genomeHandler->atEOC()) {
				shared_ptr<TritDeterministicGate> nullObj = nullptr;;
				return nullObj;
			}
			return make_shared<TritDeterministicGate>(addresses,table,gateID, _PT);
		});
	}
	if ((PT == nullptr) ? usingNeuronGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES_NEURON-allow")) {
		inUseGateNames.insert("Neuron");
		int codonOne = NeuronCode;
		inUseGateTypes.insert(codonOne);
		{
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = (PT == nullptr) ? neuronGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES_NEURON-initialCount");
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
			int defaultNumInputsMin = (_PT == nullptr) ? NeuronGate::defaultNumInputsMinPL->lookup() : _PT->lookupInt("BRAIN_MARKOV_GATES_NEURON-numInputsMin");
			int defaultNumInputsMax = (_PT == nullptr) ? NeuronGate::defaultNumInputsMaxPL->lookup() : _PT->lookupInt("BRAIN_MARKOV_GATES_NEURON-numInputsMax");
			int numInputs = genomeHandler->readInt(defaultNumInputsMin, defaultNumInputsMax, AbstractGate::IN_COUNT_CODE, gateID);
			vector<int> inputs;
			inputs.resize(numInputs);

			getSomeBrainAddresses(numInputs, defaultNumInputsMax, inputs, genomeHandler, AbstractGate::IN_ADDRESS_CODE, gateID);

			int output = genomeHandler->readInt(0, (1 << Global::bitsPerBrainAddressPL->lookup()) - 1, AbstractGate::OUT_ADDRESS_CODE, gateID);

			int dischargeBehavior = (_PT == nullptr) ? NeuronGate::defaultDischargeBehaviorPL->lookup() : _PT->lookupInt("BRAIN_MARKOV_GATES_NEURON-dischargeBehavior");
			if (dischargeBehavior == -1) {
				dischargeBehavior = genomeHandler->readInt(0, 2, AbstractGate::DATA_CODE, gateID);
			}

			double defaultThresholdMin = (_PT == nullptr) ? NeuronGate::defaultThresholdMinPL->lookup() : _PT->lookupInt("BRAIN_MARKOV_GATES_NEURON-thresholdMin");
			double defaultThresholdMax = (_PT == nullptr) ? NeuronGate::defaultThresholdMaxPL->lookup() : _PT->lookupInt("BRAIN_MARKOV_GATES_NEURON-thresholdMax");
			double thresholdValue = genomeHandler->readDouble(defaultThresholdMin, defaultThresholdMax, AbstractGate::DATA_CODE, gateID);

			bool thresholdActivates = 1;
			bool defaultAllowRepression = (_PT == nullptr) ? NeuronGate::defaultAllowRepressionPL->lookup() : _PT->lookupInt("BRAIN_MARKOV_GATES_NEURON-allowRepression");
			if (defaultAllowRepression == 1) {
				thresholdActivates = genomeHandler->readInt(0, 1, AbstractGate::DATA_CODE, gateID);
			}

			double decayRate = genomeHandler->readDouble(NeuronGate::defaultDecayRateMinPL->lookup(), NeuronGate::defaultDecayRateMaxPL->lookup(), AbstractGate::DATA_CODE, gateID);
			double deliveryCharge = genomeHandler->readDouble(NeuronGate::defaultDeliveryChargeMinPL->lookup(), NeuronGate::defaultDeliveryChargeMaxPL->lookup(), AbstractGate::DATA_CODE, gateID);
			double deliveryError = (_PT == nullptr) ? NeuronGate::defaultDeliveryErrorPL->lookup() : _PT->lookupInt("BRAIN_MARKOV_GATES_NEURON-deliveryError");

			int ThresholdFromNode = -1;
			int DeliveryChargeFromNode = -1;
			bool defaultThresholdFromNode = (_PT == nullptr) ? NeuronGate::defaultThresholdFromNodePL->lookup() : _PT->lookupInt("BRAIN_MARKOV_GATES_NEURON-thresholdFromNode");
			if (defaultThresholdFromNode) {
				ThresholdFromNode = genomeHandler->readInt(0, (1 << Global::bitsPerBrainAddressPL->lookup()) - 1, AbstractGate::IN_ADDRESS_CODE, gateID);
			}
			bool defaultDeliveryChargeFromNode = (_PT == nullptr) ? NeuronGate::defaultDeliveryChargeFromNodePL->lookup() : _PT->lookupInt("BRAIN_MARKOV_GATES_NEURON-thresholdFromNode");
			if (defaultDeliveryChargeFromNode) {
				DeliveryChargeFromNode = genomeHandler->readInt(0, (1 << Global::bitsPerBrainAddressPL->lookup()) - 1, AbstractGate::IN_ADDRESS_CODE, gateID);
			}
			if (genomeHandler->atEOC()) {
				shared_ptr<NeuronGate> nullObj = nullptr;;
				return nullObj;
			}
			return make_shared<NeuronGate>(inputs, output, dischargeBehavior, thresholdValue, thresholdActivates, decayRate, deliveryCharge, deliveryError, ThresholdFromNode, DeliveryChargeFromNode, gateID, _PT);
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
