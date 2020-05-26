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

#include <vector> //std::vector
#include <memory> //std::shared_ptr, std::make_shared
#include <unordered_map> //std::unordered_map
#include <string> //std::string
#include <unordered_set> //std::unordered_set

#include <Brain/AbstractBrain.h>

class BiLogBrain : public AbstractBrain {

public:
    BiLogBrain() = delete;

    BiLogBrain(int In, int Out, std::shared_ptr<ParametersTable> PT);

    virtual ~BiLogBrain() = default;

    virtual std::shared_ptr<AbstractBrain>
    makeBrain(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &genomes) override;

	// convert a brain into data map with data that can be saved to file
	DataMap serialize(std::string &name)  override;
	// given an unordered_map<string, string> and PT, load data into this brain
	void deserialize(std::shared_ptr<ParametersTable> PT, std::unordered_map<std::string, std::string> &orgData, std::string &name) override;

    virtual void resetBrain() override;

    inline void setInput(const int &inputAddress, const double &value) override;

    inline double readInput(const int &inputAddress) override;

    inline void setOutput(const int &outputAddress, const double &value) override;

    inline double readOutput(const int &outputAddress) override;

	inline virtual void setAllInputs(const std::vector<double>& values) {
		if (values.size() == nrInputValues) {
			for (int i = 0; i < nrInputValues; i++) {
				nodes[N_Ins][i] = Bit(values[i]);
			}
		}
		else {
			std::cout << "in Brain::setAllInputs() : Size of provided vector (" << values.size() << ") does not match number of brain inputs (" << nrInputValues << ").\nExiting"
				<< std::endl;
			exit(1);
		}
	}

	inline virtual std::vector<double> readAllOutputs() {
		std::vector<double> returnVector;
		for (auto v : nodes[N_Outs]) {
			returnVector.push_back(static_cast<double>(v));
		}
		return returnVector;
	}

    virtual void update() override;

	////////////////////////////////////
	//// What is this??
    //std::vector<int> getHiddenNodes();
	////////////////////////////////////

    virtual void resetOutputs() override;

    virtual std::string description() override;

    virtual DataMap getStats(std::string &prefix) override;

    virtual void initializeGenomes(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &genomes) override;

    virtual std::shared_ptr<AbstractBrain> makeCopy(std::shared_ptr<ParametersTable> PT_) override;

    void showBrain();

    virtual std::shared_ptr<AbstractBrain> makeBrainFrom(std::shared_ptr<AbstractBrain> parent,
                                                         std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &_genomes) override;

    virtual std::shared_ptr<AbstractBrain> makeBrainFromMany(std::vector<std::shared_ptr<AbstractBrain>> parents,
                                                             std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &_genomes) override;

    // ------------------
    virtual std::string getType() override { return "BIT2"; }

    virtual std::unordered_set<std::string> requiredGenomes() override { return {}; }

    //-------------------
    static std::shared_ptr<ParameterLink<int>> nrOfRecurrentNodesPL;
    static std::shared_ptr<ParameterLink<int>> nrOfHiddenLayersPL;
    static std::shared_ptr<ParameterLink<std::string>> hiddenLayerSizeListPL;
    static std::shared_ptr<ParameterLink<bool>> recurrentOutputPL;
    static std::shared_ptr<ParameterLink<bool>> inputAlwaysAvailablePL;
    static std::shared_ptr<ParameterLink<bool>> recurrentAlwaysAvailablePL;
    static std::shared_ptr<ParameterLink<bool>> outputAlwaysAvailablePL;
	static std::shared_ptr<ParameterLink<double>> mutOneBrainPL;
	static std::shared_ptr<ParameterLink<double>> mutOneGatePL;
	static std::shared_ptr<ParameterLink<double>> mutLogic1PL;
	static std::shared_ptr<ParameterLink<double>> mutLogic2PL;
	static std::shared_ptr<ParameterLink<double>> mutLogic3PL;
	static std::shared_ptr<ParameterLink<double>> mutLogic4PL;
	static std::shared_ptr<ParameterLink<double>> mutWires1PL;
	static std::shared_ptr<ParameterLink<double>> mutWires2PL;
	static std::shared_ptr<ParameterLink<bool>> recordMutationHistoryPL;

	static std::shared_ptr<ParameterLink<std::string>> mutationProgramFileNamePL;

	bool recOut, alwaysIn, alwaysRec, alwaysOut;
    double mut_logic1, mut_logic2, mut_logic3, mut_logic4, mut_wires1, mut_wires2;
	double mutOneBrain, mutOneGate;

	double mut_logic1_ratio = 0;
	double mut_logic2_ratio = 0;
	double mut_logic3_ratio = 0;
	double mut_logic4_ratio = 0;
	double mut_wires1_ratio = 0;
	double mut_wires2_ratio = 0;

	int R, Hnum; // recurent nodes and number of hidden layers
	std::vector<int> Hsizes; //how big is each hidden layer

	int mutCountLogic1 = 0;
	int mutCountLogic2 = 0;
	int mutCountLogic3 = 0;
	int mutCountLogic4 = 0;
	int mutCountWire1 = 0;
	int mutCountWire2 = 0;
	std::string mutationHistory = "";

	static int mutProgIndex;

	static std::vector<int> mutProg_updates;

	static std::vector<double> mutProg_onePerBrain;
	static std::vector<double> mutProg_onePerGate;

	static std::vector<double> mutProg_mutLogic1;
	static std::vector<double> mutProg_mutLogic2;
	static std::vector<double> mutProg_mutLogic3;
	static std::vector<double> mutProg_mutLogic4;
	static std::vector<double> mutProg_mutWire1;
	static std::vector<double> mutProg_mutWire2;
private:
    //these consts make using the data structure easier

	//there are indexes into nodes vector
    const int N_Ins = 0;
    const int N_Outs = 1;
    const int N_Outs_prev = 2;
    const int N_Recs = 3;
    const int N_Recs_prev = 4;
    const int N_Hidden_offset = 5;

	//there are indexes into gates vector
    const int G_Outs = 0;
    const int G_Recs = 1;
    const int G_Hidden_offset = 2;

    //class data
	int I, O; // number of inputs, number of outputs
    std::vector<std::vector<bool>> nodes; // stores internal state, one node for each input, recurent, (possibly output), and gate. also next output and next recurrent
    std::vector<std::vector<std::pair<int, int>>> connections; // for each gate, where do they get their input <node layer, node within that layer>
	
	std::vector<int> layersWithGates; // list of layers with gates in them (used with mutOneBrain)

	bool recordMutationHistory;

    //internal gate class
    class Gate {
    public:
		static int logic_tables[16][2][2]; // look up table will return anwser give logic id and 2 input bits

		static int logic_mutations1[16][4]; // mutations for all logic with one mutation
		static int logic_mutations2[16][6]; // mutations for all logic with two mutations
		static int logic_mutations3[16][4]; // mutations for all logic with three mutations
		static int logic_mutations4[16][1]; // mutations for all logic with four mutations

		// these values define a gate
        int logicID, L1, N1, L2, N2;

        //to get the output of a gate update: logic_tables[logicID][nodes[L1][N1]][nodes[L2][N2]]

		// connection1 and connection2 provide the indexes into the connections list for
		// the layer that contains this gate. This is used to check for unique mutations.
		int connection1;
		int connection2;
        bool getOutput(const std::vector<std::vector<bool>> &nodes) {
			bool debugFlag = false;
			if (debugFlag) {
				std::vector<std::string> logicNames = { "FALSE","a_NOR_b","!a_AND_b","!a","a_AND_!b","!b","a_XOR_b","a_NAND_b","a_AND_b","a_NXOR_b","b","!a_OR_b","a","a_OR_!b","a_OR_b","TRUE"
				};
				std::cout << " inputs: " << nodes[L1][N1] << "," << nodes[L2][N2] << "=" << logic_tables[logicID][nodes[L1][N1]][nodes[L2][N2]] << "\tlogic: " << logicNames[logicID] << "\tL1,N1: " << L1 << "," << N1 << " L2,N2: " << L2 << "," << N2 << std::endl;
			}
            return logic_tables[logicID][nodes[L1][N1]][nodes[L2][N2]];
        }
    };

    //layers of lists of gates
    std::vector<std::vector<Gate>> gates;

    //concatenate vector a by appending b to the end of a
    template<typename T>
    inline void vectorExtend(std::vector<T> &a, std::vector<T> &b) {
        a.insert(a.end(), b.begin(), b.end());
    }

	void mutateGate(std::shared_ptr<BiLogBrain> newBrain, int layerID, int gateID);
	void setMutationRates();

	public:



	// return number of mutations for number concurent mutations
	int mutationOptions(int number = 1) { // assume only one mutation
		int logicOptions = 0; // the number of options per gate
		if (mut_logic1 > 0) { // if single point logic mutations are allowed
			logicOptions += 4; // each single bit flip
		}
		if (mut_logic2 > 0) { // if single point logic mutations are allowed
			logicOptions += 6; // each double bit flip
		}
		if (mut_logic3 > 0) { // if single point logic mutations are allowed
			logicOptions += 4; // each triple bit flip
		}
		if (mut_logic4 > 0) { // if single point logic mutations are allowed
			logicOptions += 1; // flip all 4 bits
		}

		//std::cout << "logicOptions: " << logicOptions << std::endl;

		int options = 0;
		// logic1, 2, 3, 4, Wire1, 2
		for (int l = 0; l < gates.size();l++) {
			int wireOptions = 0; // the number of options per gate
			if (mut_wires1 > 0) { // if single wire mutations are allowed
				wireOptions += (connections[l].size() - 1) * 2; // one wire can move to an other connection
			}
			if (mut_wires2 > 0) { // if double wire mutations are allowed
				wireOptions += std::pow((connections[l].size() - 1),2); // each wire can move to a diffrent connection independently
			}
			//std::cout << "layer" << l << " wireOptions: " << wireOptions << std::endl;
			for (int g = 0; g < gates[l].size(); g++) {
				options += logicOptions;
				options += wireOptions;
			}
		}
		return(std::pow(options,number));
	}

	// return a brain with the mutations indicated by indexList
	// every mutation can be seen as an offset from the current brain
	// the index indicates a particular offset
	// given a brain with 5 inputs, 3 outputs and no hidden or recurrent layers
	// i.e. a brain with two gates, we can think of a brain with
	//     g1 : ins 1,2 logic 1, g2 : ins 2,2 logic 6
	// and lets assue that avalible mutations are logic1, logic2 and wire 1
	// if we think of point mutations then for we must consider each gate
	//     for each input there is number of inputs - 1 alternatives (in this case 4)
	//         i.e. input 1 on gate one could become 0, 2, 3 or 4
	//         and there are 2 inputs per gate...
	//     (note, there are no output mutations, outputs are fixed!)
	//     for each logic table, we must count the number of options (for 1 point, 4, for 2 point, 6) 10 total
	// so for each gate there are (4 * 2) + 10 options = 18
	// and this is 16 per gate, 2 gates will result in 36 possible options.
	//
	// the options will be indexed by gate then by mutation types (for mutations with probability > 0)
	//   so in the example, each gate has 18 options, so index 0 -> 17 will be mutations to gate 1
	//
	// Lets assume logic mutations (logic1, logic2, logic3, logic4)
	// are indexed before wire mutations (wire1, wire2) and that
	// logic mutations are indexed in the order provided by the logic types lookup table
	// and wire mutations are indexed by the order of the connection list for the gates layer
	// then an index will indicate the gate and the variation of a particular mutation of that gate

	std::shared_ptr<BiLogBrain> getMutantBrain(std::vector<int> indexList, std::vector<int>& mutationTypeList) {
		int index = indexList.back();
		int origIndex = index; // used in error message if no mutation is located for index
		indexList.pop_back();
		auto newBrain = std::dynamic_pointer_cast<BiLogBrain>(makeCopy(PT));
		// this is slow, but...
		// run nested for loops until the index is achived, and then cause that mutation.
		// then if number > 1, call on resulting brain with indexList with last element removed

		for (int l = 0; l < newBrain->gates.size() && index >= 0; l++) {
			//add shortcut to advance by gate index - (logics + connections) > 0 means we can skip to index - (logics + connections) 
			for (int g = 0; g < gates[l].size() && index >= 0; g++) {
				for (int logic = 0; logic < 4 && index >= 0 && mut_logic1 > 0; logic++) { // logic1
					if (index == 0) {
						newBrain->gates[l][g].logicID = newBrain->gates[l][g].logic_mutations1[gates[l][g].logicID][logic]; // 4 options
						mutationTypeList.push_back(0);
					}
					index--;
				}
				for (int logic = 0; logic < 6 && index >= 0 && mut_logic2 > 0; logic++) { // logic2
					if (index == 0) {
						newBrain->gates[l][g].logicID = newBrain->gates[l][g].logic_mutations2[gates[l][g].logicID][logic]; // 6 options
						mutationTypeList.push_back(1);
					}
					index--;
				}
				for (int logic = 0; logic < 4 && index >= 0 && mut_logic3 > 0; logic++) { // logic3
					if (index == 0) {
						newBrain->gates[l][g].logicID = newBrain->gates[l][g].logic_mutations3[gates[l][g].logicID][logic]; // 4 options
						mutationTypeList.push_back(2);
					}
					index--;
				}
				for (int logic = 0; logic < 1 && index >= 0 && mut_logic4 > 0; logic++) { // logic4
					if (index == 0) {
						newBrain->gates[l][g].logicID = newBrain->gates[l][g].logic_mutations4[gates[l][g].logicID][logic]; // 1 option
						mutationTypeList.push_back(3);
					}
					index--;
				}

				if (mut_wires1 > 0) {
					//mutate wire 1
					for (int c = 0; c < (newBrain->connections[l].size() - 1) && index >= 0; c++) {
						if (index == 0) {
							newBrain->gates[l][g].connection1 = (c < newBrain->gates[l][g].connection1) ? c : c + 1;
							// if connection is the same as current, get next one.

							newBrain->gates[l][g].L1 = connections[l][newBrain->gates[l][g].connection1].first;
							newBrain->gates[l][g].N1 = connections[l][newBrain->gates[l][g].connection1].second;
							mutationTypeList.push_back(4);
						}
						index--;
					}

					// mutate wire2
					for (int c = 0; c < (newBrain->connections[l].size() - 1) && index >= 0; c++) {
						if (index == 0) {
							newBrain->gates[l][g].connection2 = (c < newBrain->gates[l][g].connection2) ? c : c + 1;
							// if connection is the same as current, get next one.

							newBrain->gates[l][g].L2 = connections[l][newBrain->gates[l][g].connection2].first;
							newBrain->gates[l][g].N2 = connections[l][newBrain->gates[l][g].connection2].second;
							mutationTypeList.push_back(4);
						}
						index--;
					}
				}
				// mutate both wires
				if (mut_wires2 > 0) {
					for (int c1 = 0; c1 < (newBrain->connections[l].size() - 1) && index >= 0; c1++) { // wire1
						for (int c2 = 0; c2 < (newBrain->connections[l].size() - 1) && index >= 0; c2++) { // wire2
							if (index == 0) {
								newBrain->gates[l][g].connection1 = (c1 < newBrain->gates[l][g].connection1) ? c1 : c1 + 1;
								// if connection is the same as current, get next one.

								newBrain->gates[l][g].L1 = connections[l][newBrain->gates[l][g].connection1].first;
								newBrain->gates[l][g].N1 = connections[l][newBrain->gates[l][g].connection1].second;

								newBrain->gates[l][g].connection2 = (c2 < newBrain->gates[l][g].connection2) ? c2 : c2 + 1;
								// if connection is the same as current, get next one.

								newBrain->gates[l][g].L2 = connections[l][newBrain->gates[l][g].connection2].first;
								newBrain->gates[l][g].N2 = connections[l][newBrain->gates[l][g].connection2].second;
								mutationTypeList.push_back(5);
							}
							index--;
						}
					}
				}
			}
		}
		if (mutationTypeList.size() < 1) {
			std::cout << "In BiLogBrain::getMutantBrain - was given mutationIndex of " << origIndex << " but this matches no mutation (it's too large)."
				"\n  this brain only has " << mutationOptions(1) << " single point legal mutations.\n  exitting..." << std::endl;
			exit(1);
		}
		return((indexList.size() < 1) ? newBrain : newBrain->getMutantBrain(indexList,mutationTypeList));
	};

};

inline std::shared_ptr<AbstractBrain> BiLogBrain_brainFactory(int ins, int outs, std::shared_ptr<ParametersTable> PT) {
    return std::make_shared<BiLogBrain>(ins, outs, PT);
}
