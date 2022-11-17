//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#define CGPBRAIN_DEBUG 0

#include "CGPBrain.h"

std::shared_ptr<ParameterLink<int>> CGPBrain::nrRecurrentNodesPL =
Parameters::register_parameter("BRAIN_CGP-nrRecurrentNodes", 3,
	"number of recurrent nodes");

std::shared_ptr<ParameterLink<std::string>> CGPBrain::availableOperatorsPL =
Parameters::register_parameter(
	"BRAIN_CGP-availableOperators", (std::string) "all",
	"which opperators are allowed? all indicates, allow all opperators or, "
	"choose from:\n"
	"  ADD (a+b), MULT (a*b), SUBTRACT (a-b), DIVIDE(a/b)\n"
	"  SIN (sin(a)), COS (sin(a)), GREATER (1 if a > b, else 0), RAND (double[a..b))\n"
	"  IF (b is a > 0, else 0), INV (-a), CONST (const)\n" 
	"  RNN (8 inputs sumed -> tanh) DETGATE (4 in, 1 out, logic lookup)\n"
	"if a list (| seperated) is provided, then each column has it's own availableOperators");
std::shared_ptr<ParameterLink<double>> CGPBrain::magnitudeMaxPL =
Parameters::register_parameter(
	"BRAIN_CGP-magnitudeMax", 100.0,
	"values generated which are larger then this will by clipped");
std::shared_ptr<ParameterLink<double>> CGPBrain::magnitudeMinPL =
Parameters::register_parameter(
	"BRAIN_CGP-magnitudeMin", -100.0,
	"values generated which are smaller then this will by clipped");

std::shared_ptr<ParameterLink<int>> CGPBrain::opSpaceColumnsPL =
Parameters::register_parameter("BRAIN_CGP-opsMatixColumns", 4,
	"width of the operations matrtix");

std::shared_ptr<ParameterLink<int>> CGPBrain::opSpaceRowsPL =
Parameters::register_parameter("BRAIN_CGP-opsMatixRows", 4,
	"height of the operations matrtix");

std::shared_ptr<ParameterLink<bool>> CGPBrain::readFromOutputsPL =
Parameters::register_parameter(
	"BRAIN_CGP-readFromOutputs", true,
	"if true, previous updates outputs will be available as inputs.");


std::shared_ptr<ParameterLink<int>> CGPBrain::discretizeRecurrentPL = Parameters::register_parameter(
	"BRAIN_CGP-discretizeRecurrent", 0, "should recurrent nodes be discretized when being copied?\n"
	"if 0, no, leave them be.\n"
	"if 1 then map <= 0 to 0, and > 0 to 1\n"
	"if > then 1, values are mapped to new equally spaced values in range [discretizeRecurrentRange[0]..[1]] such that each bin has the same sized range\n"
	"    i.e. if 3 and discretizeRecurrentRange = [-1,1], bin bounderies will be (-1.0,-.333-,.333-,1.0) and resulting values will be (-1.0,0.0,1.0)\n"
	"Note that this process ends up in a skewed mapping. mappings will always include -1.0 and 1.0. even values > 1 will result in remappings that do not have 0");

std::shared_ptr<ParameterLink<std::string>> CGPBrain::discretizeRecurrentRangePL = Parameters::register_parameter(
	"BRAIN_CGP-discretizeRecurrentRange", (std::string)"-1,1", "value range for discretizeRecurrent if discretizeRecurrent > 1");

std::shared_ptr<ParameterLink<double>> CGPBrain::inputMutationRatePL = Parameters::register_parameter(
	"BRAIN_CGP-MutationRate_INPUTS", .01, "mutation rate for operation inputs (per input)");

std::shared_ptr<ParameterLink<double>> CGPBrain::constMutationRatePL = Parameters::register_parameter(
	"BRAIN_CGP-MutationRate_CONST_value", .01, "mutation rate const operator value (per op)");

std::shared_ptr<ParameterLink<double>> CGPBrain::operatorMutationRatePL = Parameters::register_parameter(
	"BRAIN_CGP-MutationRate_OPERATION", .002, "mutation rate (per gate) to change an operation (includes inputs)");

std::shared_ptr<ParameterLink<double>> CGPBrain::outputMutationRatePL = Parameters::register_parameter(
	"BRAIN_CGP-MutationRate_OUTPUT", .002, "mutation rate (per output) to change where an output (or memory) is wired");

std::shared_ptr<ParameterLink<double>> CGPBrain::RNN_weightsMutationRatePL = Parameters::register_parameter(
	"BRAIN_CGP-MutationRate_RNN_weights", .01, "mutation rate (per weight) for RNN operator weights");

std::shared_ptr<ParameterLink<double>> CGPBrain::RNN_biasMutationRatePL = Parameters::register_parameter(
	"BRAIN_CGP-MutationRate_RNN_bias", .01, "mutation rate (per RNN op) for RNN operator bias");

std::shared_ptr<ParameterLink<double>> CGPBrain::RNN_biasMinPL = Parameters::register_parameter(
	"BRAIN_CGP-RNN_biasMinValue", -1.0, "mutation rate for operation inputs");

std::shared_ptr<ParameterLink<double>> CGPBrain::RNN_biasMaxPL = Parameters::register_parameter(
	"BRAIN_CGP-RNN_biasMaxValue", 1.0, "mutation rate for operation inputs");

std::shared_ptr<ParameterLink<double>> CGPBrain::DETGATE_logicMutationRatePL = Parameters::register_parameter(
	"BRAIN_CGP-MutationRate_DETGATE_logic", .01, "mutation rate for operation inputs");

void CGPBrain::graphBrain() {
	std::vector<bool> checkMatrix(opsMatrixSize, false);
	std::vector<std::string> graphOutput;

	std::string source;
	std::string dest;

	for (int i = 0; i < nrOutputTotal; i++) { // where are the output layer nodes reading from?
		if (i < nrOutputValues) {
			dest = "o_" + std::to_string(i);
		}
		else {
			dest = "r_" + std::to_string(i-nrOutputValues);

		}
		if (outputLayerAddresses[i] >= nrInputTotal) { // if the matrix cell input is not an input
			checkMatrix[outputLayerAddresses[i] - nrInputTotal] = true;
			source = "c_" + std::to_string(outputLayerAddresses[i] - nrInputTotal);
		}
		else {
			if (outputLayerAddresses[i] < nrInputValues) {
				source = "i_" + std::to_string(outputLayerAddresses[i]);
			}
			else if (outputLayerAddresses[i] < nrInputValues + nrRecurrentValues) {
				source = "r_" + std::to_string(outputLayerAddresses[i] - nrInputValues);
			}
			else {
				source = "o_" + std::to_string(outputLayerAddresses[i] - (nrInputValues + nrRecurrentValues));
			}
		}
		graphOutput.push_back(source + "->" + dest);
	}
	for (int i = opsMatrixSize - 1; i >= 0; i--) { // for each cell in the matrix
		if (checkMatrix[i]) { // if this cell is being used ...
			dest = "c_" + std::to_string(i);
			for (auto v : opsMatrix[i]->inputs) { // for each input to this cell in the matrix
				if (v >= nrInputTotal) { // if the matrix cell input is not an input
					checkMatrix[v - nrInputTotal] = true;
					source = "c_" + std::to_string(v - nrInputTotal);
				}
				else {
					if (v < nrInputValues) {
						source = "i_" + std::to_string(v);
					}
					else if (v < nrInputValues + nrRecurrentValues) {
						source = "r_" + std::to_string(v - nrInputValues);
					}
					else {
						source = "o_" + std::to_string(v - (nrInputValues + nrRecurrentValues));
					}
				}
				graphOutput.push_back(source + "->" + dest);
			}
		}
	}

	std::cout << std::endl;
	for (auto s : graphOutput) {
		std::cout << s << std::endl;
	}
	std::cout << std::endl;

	// trace execution
	for (int i = 0; i < opsMatrixSize; i++) {
		std::string shortname = "c_" + std::to_string(i);
		std::string name = shortname + "\\n" + opsMatrix[i]->name;
		std::cout << shortname + "[label=\"" + name;
		std::cout << "(" << opsMatrix[i]->row << "," << opsMatrix[i]->column << ")";
		std::cout << opsMatrix[i]->inputs.size() << "(" << opsMatrix[i]->numInputs << ")";
		if (checkMatrix[i]) {
			std::cout << "*\" style = filled color = lightblue ]" << std::endl;
		}
		else {
			std::cout << "\"]" << std::endl;
		}
	}
}
void CGPBrain::updateExecuteOrder() {
	std::vector<bool> checkMatrix(opsMatrixSize, false);

	for (int i = 0; i < nrOutputTotal; i++) { // where are the output layer nodes reading from?
		if (outputLayerAddresses[i] >= nrInputTotal) { // if the matrix cell input is not an input
			//std::cout << i << " " << outputLayerAddresses[i] << " this is a gate?!" << std::endl;
			checkMatrix[outputLayerAddresses[i] - nrInputTotal] = true;
		}
	}
	for (int i = opsMatrixSize - 1; i >= 0; i--) { // for each cell in the matrix
		if (checkMatrix[i]) { // if this cell is being used ...
			for (auto v : opsMatrix[i]->inputs) { // for each input to this cell in the matrix
				if (v >= nrInputTotal) { // if the matrix cell input is not an input
					checkMatrix[v - nrInputTotal] = true;
				}
			}
		}
	}
	
	executeOrder.clear();
	for (int i = 0; i < opsMatrixSize; i++) {
		if (checkMatrix[i]) {
			executeOrder.push_back(i);
		}
	}
}

CGPBrain::CGPBrain(int _nrInNodes, int _nrOutNodes,
	std::shared_ptr<ParametersTable> PT_)
	: AbstractBrain(_nrInNodes, _nrOutNodes, PT_) {

	operatorMutationRate = operatorMutationRatePL->get(PT);
	outputMutationRate = outputMutationRatePL->get(PT);

	readFromOutputs = readFromOutputsPL->get(PT);
	nrRecurrentValues = nrRecurrentNodesPL->get(PT);

	nrInputTotal = nrInputValues + nrRecurrentValues + ((readFromOutputs) ? nrOutputValues : 0);
	nrOutputTotal = nrOutputValues + nrRecurrentValues;

	if (nrInputTotal == 0) {
		std::cout << "in CGP brain :: there must be atleast 1 input or recurrent! exiting..." << std::endl;
		exit(1);
	}

	magnitudeMax = magnitudeMaxPL->get(PT);
	magnitudeMin = magnitudeMinPL->get(PT);
	opSpaceColumns = opSpaceColumnsPL->get(PT);
	opSpaceRows = opSpaceRowsPL->get(PT);

	opsMatrixSize = opSpaceColumns * opSpaceRows;
	if (opsMatrixSize == 0) {
		opSpaceColumns = 0;
		opSpaceRows = 0;
	}
	opsMatrix.resize(opsMatrixSize);
	outputLayerAddresses.resize(nrOutputTotal);
	currentMatrixValuesSize = nrInputTotal + opsMatrixSize;
	currentMatrixValues.resize(currentMatrixValuesSize);
	nextValues.resize(nrOutputTotal);

	std::vector<std::string> tempOpLists;
	std::vector<std::string> tempOpList;
	convertCSVListToVector(availableOperatorsPL->get(PT), tempOpLists, '|');

	if (tempOpLists[0] == "all" || tempOpLists.size() == 0) {
		availableOperators = { {OPTYPES::ADD, OPTYPES::MULT, OPTYPES::SUBTRACT, OPTYPES::DIVIDE, OPTYPES::SIN, OPTYPES::COS, OPTYPES::GREATER, OPTYPES::RAND, OPTYPES::IF, OPTYPES::INV, OPTYPES::CONST, OPTYPES::RNN, OPTYPES::DETGATE} };
	}
	else {
		if (tempOpLists.size() != 1 && tempOpLists.size() != opSpaceColumns){
			std::cout << "while setting up CGP brain, availableOperators parameter is invalid." << std::endl;
			std::cout << "  must be all, one list or exatly columns lists. exiting..." << std::endl;
			exit(1);
		}
		for (auto& ol : tempOpLists) {
			availableOperators.push_back({});
			convertCSVListToVector(ol, tempOpList);
			for (auto& o : tempOpList) {
				availableOperators.back().push_back(OPTYPES_MAP[o]);
			}
		}
	}

	discretizeRecurrent = discretizeRecurrentPL->get(PT);
	convertCSVListToVector(discretizeRecurrentRangePL->get(PT), discretizeRecurrentRange);

	//std::cout << "discretizeRecurrent: " << discretizeRecurrent << std::endl;
	//for (auto d : discretizeRecurrentRange) {
	//	std::cout << d << " ";
	//}
	//std::cout << std::endl;

	// uncomment to test ops
	//testOps();

	// columns to be added to ave file
	popFileColumns.clear();

}

CGPBrain::CGPBrain(int _nrInNodes, int _nrOutNodes, std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>& _genomes,
	std::shared_ptr<ParametersTable> PT_) : CGPBrain(_nrInNodes, _nrOutNodes, PT_) {
	
	bool report = false;
	if (report) {
		std::cout << std::endl << std::endl;
		std::cout << "numInputs: " << nrInputValues << "  numRecurrent: " << nrRecurrentValues << "  total in: " << nrInputTotal << std::endl;
		std::cout << "numOutputs: " << nrOutputValues << "  total out: " << nrOutputTotal << std::endl;
		std::cout << "W x H: " << opSpaceColumns << " x " << opSpaceRows << std::endl;
		std::cout << "opsMatrixSize: " << opsMatrixSize << std::endl;
		std::cout << "currentMatrixValuesSize: " << currentMatrixValuesSize << std::endl;
	}
	// make a matrix
	int availableInputs = nrInputTotal;
	if (report) {
		std::cout << "\nstarting construction...    availableInputs : " << availableInputs << std::endl;
	}

	int row = 0;
	int column = 0;
	for (int i = 0; i < opsMatrixSize; i++) {
		if (i != 0 && (i % opSpaceRows) == 0) { // if we get to the end of a column, there are new inputs avalible
			row = 0;
			column += 1;
			availableInputs += opSpaceRows;
			if (report) {
				std::cout << i << "        new availableInputs : " << availableInputs << std::endl;
			}
		}
		opsMatrix[i] = getNewOp(availableInputs, row, column);
		if (report) {
			std::cout << "cell id: " << i << "(" << row << "," << column << ") " << opsMatrix[i]->name << std::endl;
		}
		row += 1;
	}
	availableInputs += opSpaceRows; // output column can read from last column in opsMatrix
	for (int i = 0; i < nrOutputTotal; i++) {
		outputLayerAddresses[i] = Random::getIndex(availableInputs);
	}

	updateExecuteOrder();
	//testOps();
	//exit(1);

	if (report) { // generate a report
		int count = 0;
		int h = 0;
		int w = 0;
		for (auto& cell : opsMatrix) {
			std::cout << count++ << "  " << cell->row << "," << cell->column << "  " << cell->name << "  available: " << cell->availableInputs << std::endl;
			std::cout << "  ins: " << std::flush;
			for (auto& in : cell->inputs) {
				std::cout << in << " ";
			}
			std::cout << std::endl;
			h += 1;
			if (h == opSpaceRows) {
				h = 0;
				w += 1;
			}
		}
		std::cout << "  outputLayerAddress: " << std::endl;
		for (auto& out : outputLayerAddresses) {
			std::cout << out << " ";
		}
		std::cout << std::endl;
		std::cout << "  execute order: " << std::endl;
		for (auto& e : executeOrder) {
			std::cout << e << " ";
		}
		std::cout << std::endl;
		std::cout << std::endl << std::endl;

	}

	//graphBrain();
	//mutate();
	//std::cout << "mutated ---------------------------" << std::endl;
	//graphBrain();
	//std::cout << "mutated ---------------------------" << std::endl;
	//graphBrain();
	//std::cout << "mutated ---------------------------" << std::endl;
	//graphBrain();

	//exit(1);

}

void CGPBrain::resetBrain() {
	//std::cout << "in reset Brain" << std::endl;
	fill(inputValues.begin(), inputValues.end(), 0);
	fill(outputValues.begin(), outputValues.end(), 0);
	fill(nextValues.begin(), nextValues.end(), 0);
}

void CGPBrain::update() {
	bool report = false;

	if (discretizeRecurrent > 0) {
		discretizeVector(nextValues, discretizeRecurrent, discretizeRecurrentRange);
	}

	//std::cout << "discretizeRecurrent: " << discretizeRecurrent << "   " << discretizeRecurrentRange[0] << " " << discretizeRecurrentRange[1] << std::endl;
	//for (int i = 0; i < nextValues.size(); i++) {
	//	std::cout << tnv[i] << " -> " << nextValues[i] << std::endl;
	//}
	//exit(1);


	if (report) {
		std::cout << "in update..." << std::endl;
		std::cout <<   "inputs: ";
		for (auto v : inputValues) {
			std::cout << v << " ";
		}
		std::cout << "\nhidden: ";
		for (int index = 0; index < nrRecurrentValues; index++) { // copy input values into currentValues
			std::cout << nextValues[index + nrOutputValues] << " ";
		}
		std::cout << std::endl;
		int count = 0;
		int h = 0;
		int w = 0;
		for (auto& cell : opsMatrix) {
			std::cout << count++ << "  " << w << "," << h << "  " << cell->name << "  available: " << cell->availableInputs << std::endl;
			std::cout << "  ins: " << std::flush;
			for (auto& in : cell->inputs) {
				std::cout << in << " ";
			}
			std::cout << std::endl;
			h += 1;
			if (h == opSpaceRows) {
				h = 0;
				w += 1;
			}
		}
		std::cout << "  outputLayerAddress: " << std::endl;
		for (auto& out : outputLayerAddresses) {
			std::cout << out << " ";
		}
		std::cout << std::endl;
		std::cout << "  execute order: " << std::endl;
		for (auto& e : executeOrder) {
			std::cout << e << " ";
		}
		std::cout << std::endl << std::endl;
	}
	// copy inputs into current Values
	for (int index = 0; index < nrInputValues; index++) { // copy input values into currentValues
		currentMatrixValues[index] = inputValues[index];
		if (report) {
			std::cout << "currentMatrixValues[" << index << "]<-" <<
				"inputValues[" << index << "] = " << currentMatrixValues[index] <<
				std::endl;
		}
	}

	if (report) {
		std::cout << "currValues(post copy inputs)   : ";
		for (auto cv : currentMatrixValues) {
			std::cout << cv << " ";
		}
		std::cout << std::endl;
	}
	// copy recurrent (from t+1) into current Values
	for (int index = 0; index < nrRecurrentValues; index++) { // copy input values into currentValues
		currentMatrixValues[index + nrInputValues] = nextValues[index + nrOutputValues];
		if (report) {
			std::cout << "currentMatrixValues[" << index + nrInputValues << "]<-" <<
				"nextValues[" << index + nrOutputValues << "] = " << currentMatrixValues[index + nrInputValues] <<
				std::endl;
		}
	}

	if (report) {
		std::cout << "currValues(post copy recurrent): ";
		for (auto cv : currentMatrixValues) {
			std::cout << cv << " ";
		}
		std::cout << std::endl;
	}

	// copy outputs (from t+1) into current Values
	if (readFromOutputs) { // if readFromOutputs, then add last outputs
		for (int index = 0; index < nrOutputValues; index++) {
			currentMatrixValues[index + nrInputValues + nrRecurrentValues] = nextValues[index];
			if (report) {
				std::cout << "currentMatrixValues[" << index + nrInputValues + nrRecurrentValues << "] <-" <<
					"nextValues[" << index << "] = " << currentMatrixValues[index + nrInputValues + nrRecurrentValues] <<
					std::endl;
			}
		}
	}

	if (report) {
		std::cout << "currValues(post copy outputs)  : ";
		for (auto cv : currentMatrixValues) {
			std::cout << cv << " ";
		}
		std::cout << std::endl;
	}

	for (auto cell : executeOrder) {
		currentMatrixValues[cell + nrInputTotal] = 
			std::min(magnitudeMax, std::max(magnitudeMin, opsMatrix[cell]->update(currentMatrixValues)));
		
		if (report) {
			std::cout << "running cell: " << cell << " : " << opsMatrix[cell]->row << "," << opsMatrix[cell]->column << " = " << currentMatrixValues[cell + nrInputTotal] << std::endl;
			std::cout << "   currValues(new)     : ";
			for (auto cv : currentMatrixValues) {
				std::cout << cv << " ";
			}
			std::cout << std::endl;
		}
	}

	if (report) {
		std::cout << "cvs(post)     : ";
		for (auto cv : currentMatrixValues) {
			std::cout << cv << " ";
		}
		std::cout << std::endl;
	}

	for (int i = 0; i < nrOutputTotal; i++) {
		nextValues[i] = currentMatrixValues[outputLayerAddresses[i]];
	}
	if (report) {
		std::cout << "cvs(t+1)      : ";
		for (auto nv : nextValues) {
			std::cout << nv << " ";
		}
		std::cout << std::endl;
	}

	for (int i = 0; i < nrOutputValues; i++) {
		outputValues[i] = nextValues[i];
	}


}

std::string CGPBrain::description() {
	std::string S = "CGPBrain\n";
	return S;
}

DataMap CGPBrain::getStats(std::string& prefix) {
	DataMap dataMap;
	// get stats
	// cout << "warning:: getStats for CGPBrain needs to be written." << endl;
	return (dataMap);
}

void CGPBrain::initializeGenomes(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>& _genomes) {}

std::shared_ptr<AbstractBrain> CGPBrain::makeCopy(std::shared_ptr<ParametersTable> PT_) {
	//std::cout << "in makeCopy..." << std::endl;
	if (PT_ == nullptr) {
		PT_ = PT;
	}
	auto newBrain = std::make_shared<CGPBrain>(nrInputValues, nrOutputValues, PT_);

	newBrain->availableOperators = availableOperators;

	newBrain->nrInputTotal = nrInputTotal;
	newBrain->nrOutputTotal = nrOutputTotal;

	newBrain->opsMatrix = copyOpMatrix();
	newBrain->outputLayerAddresses = outputLayerAddresses;

	newBrain->executeOrder = executeOrder;

	return newBrain;
}
