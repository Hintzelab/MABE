//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

// see bottom of this file for python scripts used to generated logic table mutations

#include "BiLogBrain.h"
#include <Utilities/Utilities.h> //convertCSVListToVector
#include <Utilities/CSV.h> //parseLine
#include <Utilities/Random.h> //random
#include <utility> //std::pair
#include <iostream> //std::cout


int BiLogBrain::mutProgIndex = 0;

std::vector<int> BiLogBrain::mutProg_updates;

std::vector<double> BiLogBrain::mutProg_onePerBrain;
std::vector<double> BiLogBrain::mutProg_onePerGate;

std::vector<double> BiLogBrain::mutProg_mutLogic1;
std::vector<double> BiLogBrain::mutProg_mutLogic2;
std::vector<double> BiLogBrain::mutProg_mutLogic3;
std::vector<double> BiLogBrain::mutProg_mutLogic4;
std::vector<double> BiLogBrain::mutProg_mutWire1;
std::vector<double> BiLogBrain::mutProg_mutWire2;

int BiLogBrain::Gate::logic_tables[16][2][2] = {
	{{0, 0}, {0, 0}}, //0      FALSE
	{{1, 0}, {0, 0}}, //1   A  NOR  B
	{{0, 1}, {0, 0}}, //2  !A  AND  B
	{{1, 1}, {0, 0}}, //3  !A
	{{0, 0}, {1, 0}}, //4   A  AND !B
	{{1, 0}, {1, 0}}, //5          !B
	{{0, 1}, {1, 0}}, //6   A  XOR  B
	{{1, 1}, {1, 0}}, //7   A  NAND B
	{{0, 0}, {0, 1}}, //8   A  AND  B
	{{1, 0}, {0, 1}}, //9   A  NXOR B
	{{0, 1}, {0, 1}}, //10          B
	{{1, 1}, {0, 1}}, //11 !A  OR   B
	{{0, 0}, {1, 1}}, //12  A
	{{1, 0}, {1, 1}}, //13  A  OR  !B
	{{0, 1}, {1, 1}}, //14  A  OR   B
	{{1, 1}, {1, 1}}  //15     TRUE
};

int BiLogBrain::Gate::logic_mutations1[16][4] = { // flip 1 bit
	{1, 2,  4,  8},   //0
	{0, 3,  5,  9},   //1
	{0, 3,  6,  10},  //2
	{1, 2,  7,  11},  //3
	{0, 5,  6,  12},  //4
	{1, 4,  7,  13},  //5
	{2, 4,  7,  14},  //6
	{3, 5,  6,  15},  //7
	{0, 9,  10, 12},  //8
	{1, 8,  11, 13},  //9
	{2, 8,  11, 14},  //10
	{3, 9,  10, 15},  //11
	{4, 8,  13, 14},  //12
	{5, 9,  12, 15},  //13
	{6, 10, 12, 15},  //14
	{7, 11, 13, 14}   //15
};

int BiLogBrain::Gate::logic_mutations2[16][6] = { // flip 2 bits
	{12,10,9,6,5,3},
	{13,11,8,7,4,2},
	{14,8,11,4,7,1},
	{15,9,10,5,6,0},
	{8,14,13,2,1,7},
	{9,15,12,3,0,6},
	{10,12,15,0,3,5},
	{11,13,14,1,2,4},
	{4,2,1,14,13,11},
	{5,3,0,15,12,10},
	{6,0,3,12,15,9},
	{7,1,2,13,14,8},
	{0,6,5,10,9,15},
	{1,7,4,11,8,14},
	{2,4,7,8,11,13},
	{3,5,6,9,10,12}
};

int BiLogBrain::Gate::logic_mutations3[16][4] = { // flip 3 bits
	{14,13,11,7},
	{15,12,10,6},
	{12,15,9,5},
	{13,14,8,4},
	{10,9,15,3},
	{11,8,14,2},
	{8,11,13,1},
	{9,10,12,0},
	{6,5,3,15},
	{7,4,2,14},
	{4,7,1,13},
	{5,6,0,12},
	{2,1,7,11},
	{3,0,6,10},
	{0,3,5,9},
	{1,2,4,8}
};

int BiLogBrain::Gate::logic_mutations4[16][1] = { // flip all bits
	{15},
	{14},
	{13},
	{12},
	{11},
	{10},
	{9},
	{8},
	{7},
	{6},
	{5},
	{4},
	{3},
	{2},
	{1},
	{0}
};

std::shared_ptr<ParameterLink<int>> BiLogBrain::nrOfRecurrentNodesPL = Parameters::register_parameter(
	"BRAIN_BiLog-nrOfRecurrentNodes", 0, "how many recurrent nodes does the brain have? default=0");
std::shared_ptr<ParameterLink<int>> BiLogBrain::nrOfHiddenLayersPL = Parameters::register_parameter(
	"BRAIN_BiLog-nrOfHiddenLayers", 0,
	"how many hidden layers does the brain have? default=0 (this number should match the length of the hidden layer size list)");
std::shared_ptr<ParameterLink<std::string>> BiLogBrain::hiddenLayerSizeListPL = Parameters::register_parameter(
	"BRAIN_BiLog-hiddenLayerSizeList", (std::string) "NONE",
	"what are the lengths of your hidden nodes? (if setting one size for all hidden nodes use a single integer i.e. 4; if you are setting the length of each layer individually use comma seperated integers with no spaces i.e. \"3,2,5\" ) default=NONE");
std::shared_ptr<ParameterLink<bool>> BiLogBrain::recurrentOutputPL = Parameters::register_parameter(
	"BRAIN_BiLog-recurrentOutput", false, "is the output fed back as input? default=false");
std::shared_ptr<ParameterLink<bool>> BiLogBrain::inputAlwaysAvailablePL = Parameters::register_parameter(
	"BRAIN_BiLog-inputAlwaysAvailable", false,
	"are the input nodes available as input to all hidden layers? default=false");
std::shared_ptr<ParameterLink<bool>> BiLogBrain::recurrentAlwaysAvailablePL = Parameters::register_parameter(
	"BRAIN_BiLog-recurrentAlwaysAvailable", false,
	"are the recurrent nodes available as input to all hidden layers? default=false");
std::shared_ptr<ParameterLink<bool>> BiLogBrain::outputAlwaysAvailablePL = Parameters::register_parameter(
	"BRAIN_BiLog-outputAlwaysAvailable", false,
	"are the output nodes available as input to all hidden layers? (only enable this if recurrentOutput = true) default=false");

std::shared_ptr<ParameterLink<double>> BiLogBrain::mutLogic1PL = Parameters::register_parameter(
	"BRAIN_BiLog_MUTATIONS-mutationRateLogic1", 0.01, "chance for a single point mutation to a gates logic per gate");
std::shared_ptr<ParameterLink<double>> BiLogBrain::mutLogic2PL = Parameters::register_parameter(
	"BRAIN_BiLog_MUTATIONS-mutationRateLogic2", 0.01, "chance for a double point mutation to a gates logic per gate");
std::shared_ptr<ParameterLink<double>> BiLogBrain::mutLogic3PL = Parameters::register_parameter(
	"BRAIN_BiLog_MUTATIONS-mutationRateLogic3", 0.01, "chance for a triple point mutation to a gates logic per gate");
std::shared_ptr<ParameterLink<double>> BiLogBrain::mutLogic4PL = Parameters::register_parameter(
	"BRAIN_BiLog_MUTATIONS-mutationRateLogic4", 0.01, "chance for a quadruple point mutation to a gates logic per gate");

std::shared_ptr<ParameterLink<double>> BiLogBrain::mutWires1PL = Parameters::register_parameter(
	"BRAIN_BiLog_MUTATIONS-mutationRateWires1", 0.01, "chance for a single wire mutation per gate");
std::shared_ptr<ParameterLink<double>> BiLogBrain::mutWires2PL = Parameters::register_parameter(
	"BRAIN_BiLog_MUTATIONS-mutationRateWires2", 0.01, "chance for a double wire mutation per gate");


std::shared_ptr<ParameterLink<bool>> BiLogBrain::recordMutationHistoryPL = Parameters::register_parameter(
	"BRAIN_BiLog_MUTATIONS-recordMutationHistory", false,
	"If true, mutations will be recorded into data and max files. This may generate large data files!");


std::shared_ptr<ParameterLink<double>> BiLogBrain::mutOneBrainPL = Parameters::register_parameter(
	"BRAIN_BiLog_MUTATIONS-onePerBrain", -1.0,
	"If value is positive, then mutations will be limited to one per brain at this rate per brain."
	"\nThe distribution of mutations will be determined by the ratios of the defined mutation rates.");

std::shared_ptr<ParameterLink<double>> BiLogBrain::mutOneGatePL = Parameters::register_parameter(
	"BRAIN_BiLog_MUTATIONS-onePerGate", -1.0,
	"If value is positive, then mutations will be limited to one per gate at this rate per gate."
	"\nThe distribution of mutations will be determined by the ratios of the defined mutation rates."
	"\nIf onePerBrain is set then onePerGate will be ignored");

std::shared_ptr<ParameterLink<std::string>> BiLogBrain::mutationProgramFileNamePL = Parameters::register_parameter(
	"BRAIN_BiLog_MUTATIONS-mutation_Program_Filename", (std::string)"",
	"if a file name is provided, mutation rates will be set using this file\n"
	"The file needs to be in CSV format and must contain the following columns:\n"
	"update,onePerBrain,onePerGate,wire1,wire2,logic1,logic2,logic3,logic4\n"
	"when Global::update = update for each row, each of the mutation parameters\n"
	"will be set to associated values. If the first update is not 0, then the\n"
	"paramters values will be used until the first defined update.\n"
	"An example file is provided: Brain/BiLogBrain/mutation_program.csv");

BiLogBrain::BiLogBrain(int In, int Out, std::shared_ptr<ParametersTable> PT) :
	AbstractBrain(In, Out, PT), I(In), O(Out) {
	//load parameters
	R = nrOfRecurrentNodesPL->get(PT);
	Hnum = nrOfHiddenLayersPL->get(PT);
	if (hiddenLayerSizeListPL->get(PT) == "NONE") {
		if (Hnum != 0) {
			std::cout
				<< "ERROR: (BiLogBrain) BRAIN_BiLog-nrOfHiddenLayers must be 0 if BRAIN_BiLog-hiddenLayerSizeList = NONE"
				<< std::endl;
			exit(1);
		}
	}
	else if (Hnum == 0) {
		if (hiddenLayerSizeListPL->get(PT) != "NONE") {
			std::cout
				<< "ERROR: (BiLogBrain) BRAIN_BiLog-hiddenLayerSizeList must be NONE if BRAIN_BiLog-nrOfHiddenLayers = 0"
				<< std::endl;
			exit(1);
		}
	}
	else {

		convertCSVListToVector(hiddenLayerSizeListPL->get(PT), Hsizes);

		if (Hsizes.size() > 1 && Hsizes.size() != Hnum) {
			std::cout
				<< "ERROR: (BiLogBrain) BRAIN_BiLog-nrOfHiddenLayers must match the count of items listed in BRAIN_BiLog-hiddenLayerSizeList when BRAIN_BiLog-hiddenLayerSizeList contains more than one integer"
				<< std::endl;
			exit(1);
		}
	}
	recOut = recurrentOutputPL->get(PT);
	alwaysIn = inputAlwaysAvailablePL->get(PT);
	alwaysRec = recurrentAlwaysAvailablePL->get(PT);
	alwaysOut = outputAlwaysAvailablePL->get(PT);

	//BiLogBrain input > 0 input or recurrent > 0 or (Out > 0 and recurent Out)
	int layer1_connections_count = In + R + ((recOut) ? Out : 0);
	if (layer1_connections_count == 0) {
		std::cout << "  In BiLogBrain construction: BiLogBrain needs the first layer to have atleast 1 possible connection,\n"
			"    but since the world provides no inputs, output is not recurrent (or there are 0 output)\n"
			"    and nrRecurrentNodes = 0, there are no connections for the first gate layer... this brain\n"
			"    can not be constructed!" << std::endl;
		exit(1);
	}

	//check for parameter conflict and throw error
	if (!recOut && alwaysOut) {
		std::cout
			<< "ERROR: (BiLogBrain) You can not enable BRAIN_BiLog-outputAlwaysAvailable without enabling BRAIN_BiLog-recurrentOutput!"
			<< std::endl;
		exit(1);
	}

	//nodes
	nodes.resize(N_Hidden_offset + Hnum);
	//the named node layers are addressed in a fixed manner
	nodes[N_Ins].resize(I); //input layer
	nodes[N_Outs].resize(O); //output layer
	if (recOut) {
		nodes[N_Outs_prev].resize(O); //output layer (t-1)
	}
	nodes[N_Recs].resize(R); //recurrent layer
	nodes[N_Recs_prev].resize(R); //recurrent layer (t-1)

	//hidden layers are addressed by nodes[5+] in order to dynamically add/remove them
	if (Hnum > 0) {
		if (Hsizes.size() == 1) {
			//make all hidden layers the same size
			for (int i = 0; i < Hnum; i++) {
				nodes[N_Hidden_offset + i].resize(Hsizes[0]);
			}
		}
		else {
			//make each hidden layer the size indicated in Hsizes
			for (int i = 0; i < Hnum; i++) {
				nodes[N_Hidden_offset + i].resize(Hsizes[i]);
			}
		}
	}

	//gates
	gates.resize(G_Hidden_offset + Hnum);
	//the named gate layers are addressed in a fixed manner
	gates[G_Outs].resize(O); //writes to output layer
	gates[G_Recs].resize(R); //writes to recurrent layer

	//hidden layer gates are addressed by gates[2+] in order to dynamically add/remove them
	if (Hnum > 0) {
		if (Hsizes.size() == 1) {
			//make all hidden layers the same number of gates
			for (int i = 0; i < Hnum; i++) {
				gates[G_Hidden_offset + i].resize(Hsizes[0]);
			}
		}
		else {
			//make each hidden layer the number of gates indicated in Hsizes
			for (int i = 0; i < Hnum; i++) {
				gates[G_Hidden_offset + i].resize(Hsizes[i]);
			}
		}
	}

	//build connection map
	//step 1 each layer creates a (Layer,Node) pair of indices for its own range
	std::vector<std::vector<std::pair<int, int>>> connections_selfGen;

	for (int i = 0; i < nodes.size(); i++) {
		std::vector<std::pair<int, int>> connectionSet_selfGen;

		for (int j = 0; j < nodes[i].size(); j++) {
			std::pair<int, int> connection_selfGen(i, j);
			connectionSet_selfGen.push_back(connection_selfGen);
		}

		connections_selfGen.push_back(connectionSet_selfGen);
	}

	//step 2, combine the pairs from above into sets, one for each gate layer's set of inputs
	connections.resize(gates.size());
	if (Hnum > 0) {
		// if hidden layers exist, link their inputs from top to bottom
		for (int i = 0; i < Hnum; i++) {
			//iterate through hidden layers
			if (i == 0) {
				//when linking the first hidden layer
				//connect it to the input layers
				vectorExtend(connections[G_Hidden_offset + i], connections_selfGen[N_Ins]);
				if (recOut) {
					vectorExtend(connections[G_Hidden_offset + i], connections_selfGen[N_Outs_prev]);
				}
				vectorExtend(connections[G_Hidden_offset + i], connections_selfGen[N_Recs_prev]);
			}
			else {
				//when not linking the first hidden layer
				//connect this hidden layer to the previous hidden layer
				vectorExtend(connections[G_Hidden_offset + i], connections_selfGen[N_Hidden_offset + i - 1]);
				if (alwaysIn) {
					vectorExtend(connections[G_Hidden_offset + i], connections_selfGen[N_Ins]);
				}
				if (alwaysOut) {
					vectorExtend(connections[G_Hidden_offset + i], connections_selfGen[N_Outs_prev]);
				}
				if (alwaysRec) {
					vectorExtend(connections[G_Hidden_offset + i], connections_selfGen[N_Recs_prev]);
				}
			}
		}
		//finally connect outputs to final hidden
		vectorExtend(connections[G_Outs], connections_selfGen[N_Hidden_offset + Hnum - 1]);
		vectorExtend(connections[G_Recs], connections_selfGen[N_Hidden_offset + Hnum - 1]);

		if (alwaysIn) {
			vectorExtend(connections[G_Outs], connections_selfGen[N_Ins]);
			vectorExtend(connections[G_Recs], connections_selfGen[N_Ins]);
		}
		if (alwaysOut) {
			vectorExtend(connections[G_Outs], connections_selfGen[N_Outs_prev]);
			vectorExtend(connections[G_Recs], connections_selfGen[N_Outs_prev]);
		}
		if (alwaysRec) {
			vectorExtend(connections[G_Outs], connections_selfGen[N_Recs_prev]);
			vectorExtend(connections[G_Recs], connections_selfGen[N_Recs_prev]);
		}
	}
	else {
		//when no hidden layers to connect
		//connect outputs to inputs directly
		vectorExtend(connections[G_Outs], connections_selfGen[N_Ins]);
		vectorExtend(connections[G_Recs], connections_selfGen[N_Ins]);
		if (recOut) {
			vectorExtend(connections[G_Outs], connections_selfGen[N_Outs_prev]);
			vectorExtend(connections[G_Recs], connections_selfGen[N_Outs_prev]);
		}
		vectorExtend(connections[G_Outs], connections_selfGen[N_Recs_prev]);
		vectorExtend(connections[G_Recs], connections_selfGen[N_Recs_prev]);
	}

	// get list of layers with gates
	for (int i = 0; i < gates.size(); i++) {
		if (gates[i].size() > 0) {
			layersWithGates.push_back(i);
		}
	}

	if (Global::update == 0 && mutationProgramFileNamePL->get(PT) != "" && mutProg_updates.size()==0) { // if this is the progenitor
	// load mutations csv file is there is one

		CSV mutationDataCSV = CSV(mutationProgramFileNamePL->get(PT));

		for (auto name : mutationDataCSV.column_names()) {
			std::cout << name << " ";
		}
		std::cout << std::endl;
		for (auto row : mutationDataCSV.rows()) {
			for (auto data : row) {
				std::cout << data << ",";
			}
			std::cout << std::endl;
		}

		convertVectorOfStringsToVector(mutationDataCSV.singleColumn("update"), mutProg_updates);
		if (mutProg_updates.size() > 0) {
			int checker = mutProg_updates[0];
			for (int i = 1; i < mutProg_updates.size(); i++) {
				if (mutProg_updates[i] <= checker) { // the updates list is out of order!
					std::cout << "  while reading a BiLog brain mutation program file, found non-acending values in updates list.\n  exiting..." << std::endl;
					exit(1);
				}
				checker = mutProg_updates[i];
			}
		}
		// now we have the updates list and update values are in order!

		convertVectorOfStringsToVector(mutationDataCSV.singleColumn("onePerBrain"), mutProg_onePerBrain);
		convertVectorOfStringsToVector(mutationDataCSV.singleColumn("onePerGate"), mutProg_onePerGate);

		convertVectorOfStringsToVector(mutationDataCSV.singleColumn("wire1"), mutProg_mutWire1);
		convertVectorOfStringsToVector(mutationDataCSV.singleColumn("wire2"), mutProg_mutWire2);
		convertVectorOfStringsToVector(mutationDataCSV.singleColumn("logic1"), mutProg_mutLogic1);
		convertVectorOfStringsToVector(mutationDataCSV.singleColumn("logic2"), mutProg_mutLogic2);
		convertVectorOfStringsToVector(mutationDataCSV.singleColumn("logic3"), mutProg_mutLogic3);
		convertVectorOfStringsToVector(mutationDataCSV.singleColumn("logic4"), mutProg_mutLogic4);
	}

	if (mutProg_updates.size() > 0 && mutProg_updates[mutProgIndex] == 0) {
		mut_logic1 = mutProg_mutLogic1[mutProgIndex];
		mut_logic2 = mutProg_mutLogic2[mutProgIndex];
		mut_logic3 = mutProg_mutLogic3[mutProgIndex];
		mut_logic4 = mutProg_mutLogic4[mutProgIndex];
		mut_wires1 = mutProg_mutWire1[mutProgIndex];
		mut_wires2 = mutProg_mutWire2[mutProgIndex];

		mutOneBrain = mutProg_onePerBrain[mutProgIndex];
		mutOneGate = mutProg_onePerGate[mutProgIndex];
		// do not increment mutProgIndex (that will happen later)
	}
	else { // use values in parameters file

		mut_logic1 = mutLogic1PL->get(PT);
		mut_logic2 = mutLogic2PL->get(PT);
		mut_logic3 = mutLogic3PL->get(PT);
		mut_logic4 = mutLogic4PL->get(PT);
		mut_wires1 = mutWires1PL->get(PT);
		mut_wires2 = mutWires2PL->get(PT);

		mutOneBrain = mutOneBrainPL->get(PT);
		mutOneGate = mutOneGatePL->get(PT);
	}

	if (mutOneBrain > 0 || mutOneGate > 0) {
		//double totMut = std::min(1.0, mut_logic1 + mut_logic2 + mut_logic3 + mut_logic4 + mut_wires1 + mut_wires2);
		double totMut = mut_logic1 + mut_logic2 + mut_logic3 + mut_logic4 + mut_wires1 + mut_wires2;
		mut_logic1_ratio = mut_logic1 / totMut;
		mut_logic2_ratio = mut_logic1_ratio + (mut_logic2 / totMut);
		mut_logic3_ratio = mut_logic2_ratio + (mut_logic3 / totMut);
		mut_logic4_ratio = mut_logic3_ratio + (mut_logic4 / totMut);
		mut_wires1_ratio = mut_logic4_ratio + (mut_wires1 / totMut);
		mut_wires2_ratio = mut_wires1_ratio + (mut_wires2 / totMut);
		//std::cout << mut_logic1_ratio << "  " << mut_logic2_ratio << "  " << mut_logic3_ratio << "  "
		//	<< mut_logic4_ratio << "  " << mut_wires1_ratio << "  " << mut_wires2_ratio << std::endl;
	}

	recordMutationHistory = recordMutationHistoryPL->get(PT);

	popFileColumns.clear();
	popFileColumns.push_back("mutCountWire1");
	popFileColumns.push_back("mutCountWire2");
	popFileColumns.push_back("mutCountLogic1");
	popFileColumns.push_back("mutCountLogic2");
	popFileColumns.push_back("mutCountLogic3");
	popFileColumns.push_back("mutCountLogic4");
}

std::shared_ptr<AbstractBrain>
BiLogBrain::makeBrain(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &genomes) {
	std::shared_ptr<BiLogBrain> newBrain = std::make_shared<BiLogBrain>(I, O, PT);

	//randomly configure each element of the brain (gates: wires, logic)
	for (int layer = 0; layer < newBrain->gates.size(); layer++) {
		for (auto &gate : newBrain->gates[layer]) {

			bool zeroStart = false;
			if (zeroStart) {
				gate.logicID = Random::getIndex(0);
				gate.connection1 = Random::getIndex(0);
				gate.connection2 = Random::getIndex(0);
			}
			else {
				gate.logicID = Random::getIndex(16);
				gate.connection1 = Random::getIndex(connections[layer].size());
				gate.connection2 = Random::getIndex(connections[layer].size());
			}
			gate.L1 = connections[layer][gate.connection1].first;
			gate.N1 = connections[layer][gate.connection1].second;
			gate.L2 = connections[layer][gate.connection2].first;
			gate.N2 = connections[layer][gate.connection2].second;
		}
	}

	/*
	for (int layer = 0; layer < gates.size(); layer++) {
		int gCount = 0;
		for (auto &gate : newBrain->gates[layer]) {
			std::cout << "L" << layer << " N" << gCount << ":" << gate.L1 << "," << gate.N1 <<
			"/" << gate.L2 << "," << gate.N2 << " L:" << gate.logicID << std::endl;

			gCount++;
		}
	}
	*/

	return newBrain;
}

// convert a brain into data map with data that can be saved to file
DataMap BiLogBrain::serialize(std::string &name) {
	DataMap dataMap;
	std::stringstream ss;

	for (auto gateLayer : gates) {
		for (auto gate : gateLayer) {
			ss << gate.logicID << "|" << gate.connection1 << "|" << gate.connection2 << "|";
		}
		ss.seekp(-1, std::ios_base::end); // set write head back one char
		ss << "_"; // char for break between layers
	}
	std::string gatesStr = ss.str();
	dataMap.set(name + "_BiLogBrainGates", gatesStr.substr(0, gatesStr.size()-1));
	return dataMap;
}

// given an unordered_map<string, string> and PT, load data into this brain
void BiLogBrain::deserialize(std::shared_ptr<ParametersTable> PT, std::unordered_map<std::string, std::string> &orgData, std::string &name) {
	
	auto brainData = orgData["BRAIN_" + name + "_BiLogBrainGates"];

	std::vector<std::string> brainLayersData;
	std::vector<int> thisLayersValues;

	gates.clear();

	//std::cout << orgData["BRAIN_" + name + "_BiLogBrainGates"] << std::endl;

	convertCSVListToVector(orgData["BRAIN_" + name + "_BiLogBrainGates"], brainLayersData, '_');
	int layerCount = 0;
	for (auto layerData : brainLayersData) {
		gates.push_back({}); // make room for this layer
		convertCSVListToVector(layerData, thisLayersValues, '|');
		int index = 0;
		while (index < thisLayersValues.size()) {
			gates[layerCount].push_back(Gate());

			gates[layerCount].back().logicID = thisLayersValues[index++];
			gates[layerCount].back().connection1 = thisLayersValues[index++];
			gates[layerCount].back().connection2 = thisLayersValues[index++];
		
			gates[layerCount].back().L1 = connections[layerCount][gates[layerCount].back().connection1].first;
			gates[layerCount].back().N1 = connections[layerCount][gates[layerCount].back().connection1].second;
			gates[layerCount].back().L2 = connections[layerCount][gates[layerCount].back().connection2].first;
			gates[layerCount].back().N2 = connections[layerCount][gates[layerCount].back().connection2].second;
		}
		layerCount++;
	}

	//std::cout << serialize((std::string)"fuckling").getStringVector("fuckling_BiLogBrainGates")[0] << std::endl;
	//exit(1);

}
void
BiLogBrain::resetBrain() {
	for (auto &layer : nodes) {
		for (auto &&node : layer) {
			node = false;
		}
	}
}

void
BiLogBrain::setInput(const int &inputAddress, const double &value) {
	nodes[N_Ins][inputAddress] = Bit(value);
}

double
BiLogBrain::readInput(const int &inputAddress) {
	return nodes[N_Ins][inputAddress];
}

void
BiLogBrain::setOutput(const int &outputAddress, const double &value) {
	nodes[N_Outs][outputAddress] = Bit(value);
}

double
BiLogBrain::readOutput(const int &outputAddress) {
	return nodes[N_Outs][outputAddress];
}

void
BiLogBrain::update() {
	bool debugFlag = false;
	if (debugFlag) {
		std::cout << "update with...\n  input: ";
		for (auto v : nodes[N_Ins]) {
			std::cout << v << " ";
		}
		std::cout << "\n" << "  recurrent nodes: ";
		for (auto v : nodes[N_Recs]) {
			std::cout << v << " ";
		}
		std::cout << std::endl;
	}
	//copy Recurrent
	for (int i = 0; i < nodes[N_Recs].size(); i++) {
		nodes[N_Recs_prev][i] = nodes[N_Recs][i];
	}

	//copy Output
	/*
	if (recOut){
	  for (int i = 0; i < nodes[N_Outs].size(); i++){
		nodes[N_Outs_prev][i] = nodes[N_Outs][i];
	  }
	}
	*/
	//execute all gates
	//execute hidden layers first, in ascending order of ID
	for (int i = 0; i < Hnum; i++) { // for each hidden gate layer
		if (debugFlag) {
			std::cout << "running hidden layer " << i << " gates..." << std::endl;
		}
		for (int j = 0; j < gates[G_Hidden_offset + i].size(); j++) { // for each gate in that layer
			if (debugFlag) {
				std::cout << "gate: " << i;
			}
			nodes[N_Hidden_offset + i][j] = gates[G_Hidden_offset + i][j].getOutput(nodes);
		}
	}
	//execute output and recurrent layers last
	if (debugFlag) {
		std::cout << "running output gates..." << std::endl;
	}
	for (int i = 0; i < gates[G_Outs].size(); i++) {
		if (debugFlag) {
			std::cout << "gate: " << i;
		}
		nodes[N_Outs][i] = gates[G_Outs][i].getOutput(nodes);
	}
	if (debugFlag) {
		std::cout << "running recurrent gates..." << std::endl;
	}
	for (int i = 0; i < gates[G_Recs].size(); i++) {
		if (debugFlag) {
			std::cout << "gate: " << i;
		}
		nodes[N_Recs][i] = gates[G_Recs][i].getOutput(nodes);
	}

	if (debugFlag) {
		std::cout << "\n" << "  output nodes: ";
		for (auto v : nodes[N_Outs]) {
			std::cout << v << " ";
		}
		std::cout << "\n" << "  recurrent nodes: ";
		for (auto v : nodes[N_Recs]) {
			std::cout << v << " ";
		}
		std::cout << std::endl;
		std::cout << std::endl;
	}
}

void inline BiLogBrain::resetOutputs() {
	for (auto &&node : nodes[N_Outs]) {
		node = 0;
	}
}

std::string
BiLogBrain::description() {
	std::string S = "BiLog Brain";
	return S;
}

DataMap
BiLogBrain::getStats(std::string &prefix) {
	DataMap dataMap;
	dataMap.append(prefix + "mutCountWire1", mutCountWire1);
	dataMap.append(prefix + "mutCountWire2", mutCountWire2);
	dataMap.append(prefix + "mutCountLogic1", mutCountLogic1);
	dataMap.append(prefix + "mutCountLogic2", mutCountLogic2);
	dataMap.append(prefix + "mutCountLogic3", mutCountLogic3);
	dataMap.append(prefix + "mutCountLogic4", mutCountLogic4);
	if (recordMutationHistory) {
		dataMap.set(prefix + "mutationHistory", mutationHistory);
		//dataMap.setOutputBehavior(prefix + "mutationHistory", DataMap::FIRST);
	}
	return dataMap;
}

void
BiLogBrain::initializeGenomes(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &genomes) {
	//EMPTY - This brain is direct encoded, so there is no genome!
}

std::shared_ptr<AbstractBrain>
BiLogBrain::makeCopy(std::shared_ptr<ParametersTable> PT_) {
	auto newBrain = std::make_shared<BiLogBrain>(I, O, PT_);
	newBrain->gates = gates;

	newBrain->mutCountWire1 = mutCountWire1;
	newBrain->mutCountWire2 = mutCountWire2;

	newBrain->mutCountLogic1 = mutCountLogic1;
	newBrain->mutCountLogic2 = mutCountLogic2;
	newBrain->mutCountLogic3 = mutCountLogic3;
	newBrain->mutCountLogic4 =  mutCountLogic4;

	if (recordMutationHistory) {
		newBrain->mutationHistory = mutationHistory;
	}

	return newBrain;
}

void
BiLogBrain::showBrain() {
	//EMPTY
}



void BiLogBrain::mutateGate(std::shared_ptr<BiLogBrain> newBrain, int layerID, int gateID) {

	Gate& gate = newBrain->gates[layerID][gateID];

	// First check for gate logic mutation
	if (Random::P(mut_logic1)) { // single point logic table mutation (flip 1 bit)
		auto pre = gate.logicID;
		gate.logicID = gate.logic_mutations1[gate.logicID][Random::getIndex(4)]; // 4 options
		if (recordMutationHistory) {
			newBrain->mutationHistory += "L1_" + std::to_string(newBrain->mutCountLogic1) + "U" + std::to_string(Global::update) + ":" + std::to_string(layerID) + "/" + std::to_string(gateID) + ":T" + std::to_string(pre) + ">" + std::to_string(gate.logicID) + ",";
		}
		newBrain->mutCountLogic1 += 1;
	}
	if (Random::P(mut_logic2)) {// two point logic table mutation (flip 2 bits)
		auto pre = gate.logicID;
		gate.logicID = gate.logic_mutations2[gate.logicID][Random::getIndex(6)]; // 6 options
		if (recordMutationHistory) {
			newBrain->mutationHistory += "L2_" + std::to_string(newBrain->mutCountLogic2) + "U" + std::to_string(Global::update) + ":" + std::to_string(layerID) + "/" + std::to_string(gateID) + ":T" + std::to_string(pre) + ">" + std::to_string(gate.logicID) + ",";
		}
		newBrain->mutCountLogic2 += 1;
	}
	if (Random::P(mut_logic3)) { // three point logic table mutation (flip 3 bits)
		auto pre = gate.logicID;
		gate.logicID = gate.logic_mutations3[gate.logicID][Random::getIndex(4)]; // 4 options
		if (recordMutationHistory) {
			newBrain->mutationHistory += "L3_" + std::to_string(newBrain->mutCountLogic3) + "U" + std::to_string(Global::update) + ":" + std::to_string(layerID) + "/" + std::to_string(gateID) + ":T" + std::to_string(pre) + ">" + std::to_string(gate.logicID) + ",";
		}
		newBrain->mutCountLogic3 += 1;
	}
	if (Random::P(mut_logic4)) { // four point logic table mutation (flip all bits)
		auto pre = gate.logicID;
		gate.logicID = gate.logic_mutations4[gate.logicID][0]; // 1 option (invert gate)
		if (recordMutationHistory) {
			newBrain->mutationHistory += "L4_" + std::to_string(newBrain->mutCountLogic4) + "U" + std::to_string(Global::update) + ":" + std::to_string(layerID) + "/" + std::to_string(gateID) + ":T" + std::to_string(pre) + ">" + std::to_string(gate.logicID) + ",";
		}
		newBrain->mutCountLogic4 += 1;
	}
	if (connections[layerID].size() > 1) { // if there is at least two elements in the connecitons list
		// for this layer, we can mutate wires. If there is only one, there is nowhere to mutate. if
		// 0, the brain is probably a mess...

		if (Random::P(mut_wires1)) { // mutate one wire to this gate
			newBrain->mutCountWire1 += 1;
			if (Random::P(.5)) { // 50% chance to mutate left wire

				auto randWireID = Random::getIndex(connections[layerID].size() - 1);
				// get 1 less then the number of options for this wire so that if the roll is the same or higher
				// 1 can be added to the roll and there is no need to get another random number.
				//ERROR WARRNING : this assumes at least two possible connections

				gate.connection1 = (randWireID < gate.connection1) ? randWireID : randWireID + 1;
				// if connection is the same as current, get next one.

				if (recordMutationHistory) {
					newBrain->mutationHistory += "W1_" + std::to_string(newBrain->mutCountWire1) + "U" + std::to_string(Global::update) + ":" + std::to_string(layerID) + "/" + std::to_string(gateID) + ":left" + std::to_string(gate.L1) + "/" + std::to_string(gate.N1) + ">";
				}
				gate.L1 = connections[layerID][gate.connection1].first;
				gate.N1 = connections[layerID][gate.connection1].second;
				if (recordMutationHistory) {
					newBrain->mutationHistory += std::to_string(gate.L1) + "/" + std::to_string(gate.N1) + ",";
				}
			}
			else { // right wire mutation
				auto randWireID = Random::getIndex(connections[layerID].size() - 1);
				// get 1 less then the number of options for this wire so that if the roll is the same or higher
				// 1 can be added to the roll and there is no need to get another random number.
				//ERROR WARRNING : this assumes at least two possible connections

				gate.connection2 = (randWireID < gate.connection2) ? randWireID : randWireID + 1; // if connection is the same as current, get next one.
				if (recordMutationHistory) {
					newBrain->mutationHistory += "W1_" + std::to_string(newBrain->mutCountWire1) + "U" + std::to_string(Global::update) + ":" + std::to_string(layerID) + "/" + std::to_string(gateID) + ":right" + std::to_string(gate.L2) + "/" + std::to_string(gate.N2) + ">";
				}
				gate.L2 = connections[layerID][gate.connection2].first;
				gate.N2 = connections[layerID][gate.connection2].second;
				if (recordMutationHistory) {
					newBrain->mutationHistory += std::to_string(gate.L2) + "/" + std::to_string(gate.N2) + ",";
				}
			}
		}

		if (Random::P(mut_wires2)) { // mutate both wires to this gate
			newBrain->mutCountWire2 += 1;
			// mutate the left wire
			auto randWireID = Random::getIndex(connections[layerID].size() - 1);
			// get 1 less then the number of options for this wire so that if the roll is the same or higher
			// 1 can be added to the roll and there is no need to get another random number.
			//ERROR WARRNING : this assumes at least two possible connections

			gate.connection1 = (randWireID < gate.connection1) ? randWireID : randWireID + 1;
			// if connection is the same as current, get next one.

			if (recordMutationHistory) {
				newBrain->mutationHistory += "W2_" + std::to_string(newBrain->mutCountWire2) + "U" + std::to_string(Global::update) + ":" + std::to_string(layerID) + "/" + std::to_string(gateID) + ":left" + std::to_string(gate.L1) + "/" + std::to_string(gate.N1) + ">";
			}
			gate.L1 = connections[layerID][gate.connection1].first;
			gate.N1 = connections[layerID][gate.connection1].second;
			if (recordMutationHistory) {
				newBrain->mutationHistory += std::to_string(gate.L1) + "/" + std::to_string(gate.N1);
			}

			// mutate the right wire
			randWireID = Random::getIndex(connections[layerID].size() - 1);
			// get 1 less then the number of options for this wire so that if the roll is the same or higher
			// 1 can be added to the roll and there is no need to get another random number.
			//ERROR WARRNING : this assumes at least two possible connections

			gate.connection2 = (randWireID < gate.connection2) ? randWireID : randWireID + 1; // if connection is the same as current, get next one.
			if (recordMutationHistory) {
				newBrain->mutationHistory += ":right" + std::to_string(gate.L2) + "/" + std::to_string(gate.N2) + ">";
			}
			gate.L2 = connections[layerID][gate.connection2].first;
			gate.N2 = connections[layerID][gate.connection2].second;
			if (recordMutationHistory) {
				newBrain->mutationHistory += std::to_string(gate.L2) + "/" + std::to_string(gate.N2) + ",";
			}
		}
	}
}

void BiLogBrain::setMutationRates() {
	// determine the type of mutation
	double mutPick = Random::getDouble(1.0);
	if (mutPick < mut_logic1_ratio) {
		mut_logic1 = 1.0;
		mut_logic2 = 0.0;
		mut_logic3 = 0.0;
		mut_logic4 = 0.0;
		mut_wires1 = 0.0;
		mut_wires2 = 0.0;
	}
	else if (mutPick < mut_logic2_ratio) {
		mut_logic1 = 0.0;
		mut_logic2 = 1.0;
		mut_logic3 = 0.0;
		mut_logic4 = 0.0;
		mut_wires1 = 0.0;
		mut_wires2 = 0.0;
	}
	else if (mutPick < mut_logic3_ratio) {
		mut_logic1 = 0.0;
		mut_logic2 = 0.0;
		mut_logic3 = 1.0;
		mut_logic4 = 0.0;
		mut_wires1 = 0.0;
		mut_wires2 = 0.0;
	}
	else if (mutPick < mut_logic4_ratio) {
		mut_logic1 = 0.0;
		mut_logic2 = 0.0;
		mut_logic3 = 0.0;
		mut_logic4 = 1.0;
		mut_wires1 = 0.0;
		mut_wires2 = 0.0;
	}
	else if (mutPick < mut_wires1_ratio) {
		mut_logic1 = 0.0;
		mut_logic2 = 0.0;
		mut_logic3 = 0.0;
		mut_logic4 = 0.0;
		mut_wires1 = 1.0;
		mut_wires2 = 0.0;
	}
	else if (mutPick < mut_wires2_ratio) {
		mut_logic1 = 0.0;
		mut_logic2 = 0.0;
		mut_logic3 = 0.0;
		mut_logic4 = 0.0;
		mut_wires1 = 0.0;
		mut_wires2 = 1.0;
	}
}

std::shared_ptr<AbstractBrain>
BiLogBrain::makeBrainFrom(std::shared_ptr<AbstractBrain> parent, std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &_genomes) {

	// if Global::updates catches up to mutProg_updates next entry then add 1 to
	// mutProgIndex so that we used the next set of mutation rates.
	if (mutProgIndex + 1 < mutProg_updates.size() && mutProg_updates[mutProgIndex + 1] == Global::update) {
		mutProgIndex++;
	}
	if (mutProg_updates.size() > 0 && Global::update >= mutProg_updates[mutProgIndex]) {
		mut_logic1 = mutProg_mutLogic1[mutProgIndex];
		mut_logic2 = mutProg_mutLogic2[mutProgIndex];
		mut_logic3 = mutProg_mutLogic3[mutProgIndex];
		mut_logic4 = mutProg_mutLogic4[mutProgIndex];
		mut_wires1 = mutProg_mutWire1[mutProgIndex];
		mut_wires2 = mutProg_mutWire2[mutProgIndex];

		mutOneBrain = mutProg_onePerBrain[mutProgIndex];
		mutOneGate = mutProg_onePerGate[mutProgIndex];
	}



	if (mutOneBrain > 0 || mutOneGate > 0) {
		//double totMut = std::min(1.0, mut_logic1 + mut_logic2 + mut_logic3 + mut_logic4 + mut_wires1 + mut_wires2);
		double totMut = mut_logic1 + mut_logic2 + mut_logic3 + mut_logic4 + mut_wires1 + mut_wires2;
		mut_logic1_ratio = mut_logic1 / totMut;
		mut_logic2_ratio = mut_logic1_ratio + (mut_logic2 / totMut);
		mut_logic3_ratio = mut_logic2_ratio + (mut_logic3 / totMut);
		mut_logic4_ratio = mut_logic3_ratio + (mut_logic4 / totMut);
		mut_wires1_ratio = mut_logic4_ratio + (mut_wires1 / totMut);
		mut_wires2_ratio = mut_wires1_ratio + (mut_wires2 / totMut);
		//std::cout << mut_logic1_ratio << "  " << mut_logic2_ratio << "  " << mut_logic3_ratio << "  "
		//	<< mut_logic4_ratio << "  " << mut_wires1_ratio << "  " << mut_wires2_ratio << std::endl;
	}

	auto parentBrain = std::dynamic_pointer_cast<BiLogBrain>(parent);
	auto newBrain = std::dynamic_pointer_cast<BiLogBrain>(parent->makeCopy(PT));

	//check for mutOneBrain
	if (mutOneBrain > 0) {
		if (Random::P(mutOneBrain)) {
			// if mutOneBrain is being used (>0) and we rolled for a mutation
			// determine the type of mutation
			setMutationRates();
			//What layer, what gate?
			auto layerIndex = layersWithGates[Random::getIndex(layersWithGates.size())];
			auto gateIndex = Random::getIndex(newBrain->gates[layerIndex].size());
			mutateGate(newBrain, layerIndex, gateIndex);
		}
	}
	else
	{
		for (int layerIndex = 0; layerIndex < gates.size(); layerIndex++) {
			for (int gateIndex = 0; gateIndex < gates[layerIndex].size(); gateIndex++) {
				//check for mutOneGate
				if (mutOneGate > 0) {
					if (Random::P(mutOneGate)) {
						// if mutOneBrain is being used (>0) and we rolled for a mutation
						setMutationRates();
						// mutate this gate
						mutateGate(newBrain, layerIndex, gateIndex);
					}
				}
				else { // mutate gate with mutation rates (possible for more then one mutation per gate)
					mutateGate(newBrain, layerIndex, gateIndex);
				}
			}
		}
	}
	// this will display all gates with layer, node indexes, wire info and logic
	// i.e. what we need to visualize and/or reconstruct the brain.
	/*for (int layer = 0; layer < gates.size(); layer++) {
		int gCount = 0;
		for (auto &gate : newBrain->gates[layer]) {
			std::cout << "L" << layer << " N" << gCount << ":" << gate.L1 << "," << gate.N1 <<
				"/" << gate.L2 << "," << gate.N2 << " L:" << gate.logicID << std::endl;

			gCount++;
		}
	}
	*/

	// this will display layer and gate counts and number of possible mutations
	/*std::cout << "layers: " << newBrain->gates.size() << "  gates per layer: ";
	for (auto l : gates) {
		std::cout << l.size() << ",";
	}
	std::cout << std::endl;
	std::cout << "mutation options: " << newBrain->mutationOptions(1) << std::endl;
	*/
	return newBrain;

}


std::shared_ptr<AbstractBrain>
BiLogBrain::makeBrainFromMany(std::vector<std::shared_ptr<AbstractBrain>> parents,
	std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &_genomes) {
	//return makeBrain(_genomes);
	return makeBrainFrom(parents[0],_genomes);
}


/*
#The following is the python code used to genereate the logic table mutations

# function to convert a bit string to int
def bits2int(bits) :
	value = 0
	for b in bits :
		value *= 2
		value += b
	return value

# create all 16 4 bit logic tables
allLogic = []
for a in range(2) :
	for b in range(2) :
		for c in range(2) :
			for d in range(2) :
				allLogic.append([a, b, c, d])

print('all logic (should be 0 -> 15')
for l in allLogic :
	print(bits2int(l),end=',')
print()

# SinglePoint Mutations
print('\nsinglePointMutations:')
allOnePoint = []
for l in allLogic:
	allOnePoint.append([])
	for i in [3,2,1,0]:
		nl = list(l)
		nl[i] = (nl[i]+1)%2
		allOnePoint[-1].append(nl)

for m in range(len(allOnePoint)):
	print('{',end='')
	for i in range(len(allOnePoint[m])):
		if (i < len(allOnePoint[m]) - 1):
			print(bits2int(allOnePoint[m][i]),end=',')
		else:
			print(bits2int(allOnePoint[m][i]),end='')
	if (m < len(allOnePoint) - 1):
		print('},')
	else:
		print('}')

# TwoPoint Mutations
print('\ntwoPointMutations:')
allTwoPoint = []
for l in allLogic:
	allTwoPoint.append([])
	for swapIndexes in [[1,0],[2,0],[3,0],[2,1],[3,1],[3,2]]:
		nl = list(l)
		for si in swapIndexes:
			nl[si] = (nl[si]+1)%2
		allTwoPoint[-1].append(nl)

for m in range(len(allTwoPoint)):
	print('{',end='')
	for i in range(len(allTwoPoint[m])):
		if (i < len(allTwoPoint[m]) - 1):
			print(bits2int(allTwoPoint[m][i]),end=',')
		else:
			print(bits2int(allTwoPoint[m][i]),end='')
	if (m < len(allTwoPoint) - 1):
		print('},')
	else:
		print('}')

# ThreePoint Mutations
print('\nthreePointMutations:')
allThreePoint = []
for l in allLogic:
	allThreePoint.append([])
	for swapIndexes in [[2,1,0],[3,1,0],[3,2,0],[3,2,1]]:
		nl = list(l)
		for si in swapIndexes:
			nl[si] = (nl[si]+1)%2
		allThreePoint[-1].append(nl)

for m in range(len(allThreePoint)):
	print('{',end='')
	for i in range(len(allThreePoint[m])):
		if (i < len(allThreePoint[m]) - 1):
			print(bits2int(allThreePoint[m][i]),end=',')
		else:
			print(bits2int(allThreePoint[m][i]),end='')
	if (m < len(allThreePoint) - 1):
		print('},')
	else:
		print('}')
*/
