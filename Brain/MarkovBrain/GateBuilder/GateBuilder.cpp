//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license



#include "GateBuilder.h"
#include <cmath>

shared_ptr<ParameterLink<bool>> Gate_Builder::usingProbGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES-probabilisticGate", false, "set to true to enable probabilistic gates");
shared_ptr<ParameterLink<int>> Gate_Builder::probGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES-probabilisticGate_InitialCount", 3, "seed genome with this many start codons");
shared_ptr<ParameterLink<bool>> Gate_Builder::usingDetGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES-deterministicGate", true, "set to true to enable deterministic gates?");
shared_ptr<ParameterLink<int>> Gate_Builder::detGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES-deterministicGate_InitialCount", 6, "seed genome with this many start codons");
shared_ptr<ParameterLink<bool>> Gate_Builder::usingEpsiGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES-fixedEpsilonGate", false, "set to true to enable epsilon gates");
shared_ptr<ParameterLink<int>> Gate_Builder::epsiGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES-fixedEpsilonGate_InitialCount", 3, "seed genome with this many start codons");
shared_ptr<ParameterLink<bool>> Gate_Builder::usingVoidGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES-voidGate", false, "set to true to enable void gates");
shared_ptr<ParameterLink<int>> Gate_Builder::voidGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES-voidGate_InitialCount", 3, "seed genome with this many start codons");

//shared_ptr<ParameterLink<bool>> Gate_Builder::usingFBGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES-feedBackGate", false, "set to true to enable feedback gates");
//shared_ptr<ParameterLink<int>> Gate_Builder::fBGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES-feedBackGate_InitialCount", 3, "seed genome with this many start codons");
shared_ptr<ParameterLink<bool>> Gate_Builder::usingGPGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES-geneticProgramingGate", false, "set to true to enable GP (what?) gates");
shared_ptr<ParameterLink<int>> Gate_Builder::gPGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES-geneticProgramingGate_InitialCount", 3, "seed genome with this many start codons");
//shared_ptr<ParameterLink<bool>> Gate_Builder::usingThGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES-thresholdGate", false, "set to true to enable threshold gates");
//shared_ptr<ParameterLink<int>> Gate_Builder::thGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES-thresholdGate_InitialCount", 3, "seed genome with this many start codons");

shared_ptr<ParameterLink<bool>> Gate_Builder::usingTritDeterministicGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES-tritGate", false, "set to true to enable tritDeterministic gates");
shared_ptr<ParameterLink<int>> Gate_Builder::tritDeterministicGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES-tritGate_InitialCount", 3, "seed genome with this many start codons");

shared_ptr<ParameterLink<bool>> Gate_Builder::usingNeuronGatePL = Parameters::register_parameter("BRAIN_MARKOV_GATES-neuronGate", false, "set to true to enable Neuron gates");
shared_ptr<ParameterLink<int>> Gate_Builder::neuronGateInitialCountPL = Parameters::register_parameter("BRAIN_MARKOV_GATES-neuronGate_InitialCount", 3, "seed genome with this many start codons");

// *** General tools for All Gates ***

// Gets "howMany" addresses, advances the genome_index buy "howManyMax" addresses and updates "codingRegions" with the addresses being used.
void Gate_Builder::getSomeBrainAddresses(const int& howMany, const int& howManyMax, vector<int>& addresses, shared_ptr<AbstractGenome::Handler> genomeHandler, int code, int gateID) {
	int i;
	for (i = 0; i < howMany; i++) {  // for the number of addresses we need
		addresses[i] = genomeHandler->readInt(0, (1 << Global::bitsPerBrainAddressPL->lookup()) - 1, code, gateID);  // get an address
		//cout << addresses[i] << "\n";
	}
	while (i < howManyMax) {  // leave room in the genome in case this gate gets more IO later
		//cout << "skipping: " << i << "\n";
		genomeHandler->readInt(0, (1 << Global::bitsPerBrainAddressPL->lookup()) - 1);
		i++;
	}
}

// given a genome and a genomeIndex:
// pull out the number a number of inputs, number of outputs and then that many inputs and outputs
// if number of inputs or outputs is less then the max possible inputs or outputs skip the unused sites in the genome
pair<vector<int>, vector<int>> Gate_Builder::getInputsAndOutputs(const pair<int, int> insRange, const pair<int, int> outsRange, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {  // (max#in, max#out,currentIndexInGenome,genome,codingRegions)

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

//setupGates() populates Gate::makeGate (a structure containing functions) with the constructors for various types of gates.
//there are 256 possible gates identified each by a pair of codons (n followed by 256-n)
//after initializing Gate::MakeGate, Gate::AddGate() adds values and the associated constructor function to Gate::MakeGate
void Gate_Builder::setupGates() {
	// the following "Codes" are idetifiers for diffrent gate types. These number are used to look up constructors and also as the first 1/2 of the start codeon
	// if codons are being used to generate gates.
	// more may be added, but the numbers should not change (if they do, then genomes will generate diffrent brains!)
	int ProbabilisticCode = 42;
	int DeterministicCode = 43;
	int FixedEpsilonCode = 44;
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

	if ((PT == nullptr) ? usingProbGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES-probabilisticGate")) {
		inUseGateNames.insert("Probabilistic");
		int codonOne = ProbabilisticCode;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = (PT == nullptr) ? probGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES-probabilisticGate_InitialCount");
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			vector<vector<int>> rawTable = genomeHandler->readTable( {1 << addresses.first.size(), 1 << addresses.second.size()}, {16, 16}, {0, 255}, AbstractGate::DATA_CODE, gateID);
			if (genomeHandler->atEOC()) {
				shared_ptr<ProbabilisticGate> nullObj = nullptr;
				return nullObj;
			}
			return make_shared<ProbabilisticGate>(addresses,rawTable,gateID, _PT);
		});
	}
	if ((PT == nullptr) ? usingDetGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES-deterministicGate")) {
		inUseGateNames.insert("Deterministic");
		int codonOne = DeterministicCode;
		inUseGateTypes.insert(codonOne);
		{
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = (PT == nullptr) ? detGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES-deterministicGate_InitialCount");
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			vector<vector<int>> table = genomeHandler->readTable( {1 << addresses.first.size(), addresses.second.size()}, {16, 4}, {0, 1}, AbstractGate::DATA_CODE, gateID);
				if (genomeHandler->atEOC()) {
					shared_ptr<DeterministicGate> nullObj = nullptr;
					return nullObj;
				}
				return make_shared<DeterministicGate>(addresses,table,gateID, _PT);
			});
	}
	if ((PT == nullptr) ? usingEpsiGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES-fixedEpsilonGate")) {
		inUseGateNames.insert("FixedEpsilon");
		int codonOne = FixedEpsilonCode;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = (PT == nullptr) ? epsiGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES-fixedEpsilonGate_InitialCount");
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
			
			//DELETE WHEN FINISHED
			string ns = (_PT == nullptr) ? "default" : _PT->getTableNameSpace();
			cout << ns << endl;

			
			//TODO: Continue working from here 
			//Change to add function options for epsilon
			
			
			bool check; 

			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			vector<vector<int>> table = genomeHandler->readTable( {1 << addresses.first.size(), addresses.second.size()}, {16, 4}, {0, 1}, AbstractGate::DATA_CODE, gateID);
			
			
			double epsilon = (_PT == nullptr) ? FixedEpsilonGate::EpsilonSourcePL->lookup() : _PT->lookupInt("BRAIN_MARKOV_GATES_FIXED_EPSILON-epsilonSource");

			check = genomeHandler->atEOC();

			if (epsilon > 1) {
				genomeHandler->advanceIndex((int)epsilon);
				epsilon = genomeHandler->readDouble(0, 1, AbstractGate::DATA_CODE, gateID);
				check = check || genomeHandler->atEOC();
			}
			else if (epsilon < 0) {
				genomeHandler->resetHandler();
				genomeHandler->advanceIndex((int)abs(epsilon));
				epsilon = genomeHandler->readDouble(0, 1, AbstractGate::DATA_CODE, gateID);
				check = check || genomeHandler->atEOC();
			}

			if (check) {
				shared_ptr<FixedEpsilonGate> nullObj = nullptr;;
				return nullObj;
			}
			return make_shared<FixedEpsilonGate>(addresses,table,gateID, epsilon, _PT);
		});
	}
	if ((PT == nullptr) ? usingVoidGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES-voidGate")) {
		inUseGateNames.insert("Void");
		int codonOne = VoidCode;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = (PT == nullptr) ? voidGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES-voidGate_InitialCount");
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {


			bool check; 

			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			vector<vector<int>> table = genomeHandler->readTable( {1 << addresses.first.size(), addresses.second.size()}, {16, 4}, {0, 1}, AbstractGate::DATA_CODE, gateID);
			
			
			double epsilon = (_PT == nullptr) ? VoidGate::voidGate_ProbabilityPL->lookup() : _PT->lookupInt("BRAIN_MARKOV_GATES_VOID-epsilonSource");

			check = genomeHandler->atEOC(); 

			if (epsilon > 1) {
				genomeHandler->advanceIndex((int)epsilon);
				epsilon = genomeHandler->readDouble(0, 1, AbstractGate::DATA_CODE, gateID);
				check = check || genomeHandler->atEOC();
			}
			else if (epsilon < 0) {
				genomeHandler->resetHandler();
				genomeHandler->advanceIndex((int)abs(epsilon));
				epsilon = genomeHandler->readDouble(0, 1, AbstractGate::DATA_CODE, gateID);
				check = check || genomeHandler->atEOC();
			}

			if (check) {
				shared_ptr<VoidGate> nullObj = nullptr;;
				return nullObj;
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
	if ((PT == nullptr) ? usingGPGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES-geneticProgramingGate")) {
		inUseGateNames.insert("GeneticPrograming");
		int codonOne = GPCode;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = (PT == nullptr) ? gPGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES-geneticProgramingGate_InitialCount");
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			int operation = genomeHandler->readInt(0, 8, AbstractGate::DATA_CODE, gateID);
			vector<double> constValues;
			for (int i = 0; i < 4; i++) {
				double constValueMin = (_PT == nullptr) ? GPGate::constValueMinPL->lookup() : _PT->lookupInt("BRAIN_MARKOV_GATES_GP-constValueMin");
				double constValueMax = (_PT == nullptr) ? GPGate::constValueMaxPL->lookup() : _PT->lookupInt("BRAIN_MARKOV_GATES_GP-constValueMax");
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
	if ((PT == nullptr) ? usingTritDeterministicGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES-tritGate")) {
		inUseGateNames.insert("TritDeterministic");
		int codonOne = TritDeterministicCode;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = (PT == nullptr) ? tritDeterministicGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES-tritGate_InitialCount");

		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT) {
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			vector<vector<int>> table = genomeHandler->readTable( {pow(3,addresses.first.size()), addresses.second.size()}, {81, 4}, {-1, 1}, AbstractGate::DATA_CODE, gateID);
			if (genomeHandler->atEOC()) {
				shared_ptr<TritDeterministicGate> nullObj = nullptr;;
				return nullObj;
			}
			return make_shared<TritDeterministicGate>(addresses,table,gateID, _PT);
		});
	}
	if ((PT == nullptr) ? usingNeuronGatePL->lookup() : PT->lookupBool("BRAIN_MARKOV_GATES-neuronGate")) {
		inUseGateNames.insert("Neuron");
		int codonOne = NeuronCode;
		inUseGateTypes.insert(codonOne);
		{
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = (PT == nullptr) ? neuronGateInitialCountPL->lookup() : PT->lookupInt("BRAIN_MARKOV_GATES-neuronGate_InitialCount");
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
