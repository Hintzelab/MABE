//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#define CGPBRAIN_DEBUG 0

#include "../CGPBrain/CGPBrain.h"

//shared_ptr<ParameterLink<double>> GeneticProgramingBrain::valueMinPL = Parameters::register_parameter("BRAIN_CONSTANT-valueMin", 0.0, "Minmum value that brain will deliver");
//shared_ptr<ParameterLink<double>> GeneticProgramingBrain::valueMaxPL = Parameters::register_parameter("BRAIN_CONSTANT-valueMax", 100.0, "Maximum value that brain will deliver");
//shared_ptr<ParameterLink<int>> GeneticProgramingBrain::valueTypePL = Parameters::register_parameter("BRAIN_CONSTANT-valueType", 0, "0 = int, 1 = double");
//shared_ptr<ParameterLink<int>> GeneticProgramingBrain::samplesPerValuePL = Parameters::register_parameter("BRAIN_CONSTANT-samplesPerValue", 1, "for each brain value, this many samples will be taken from genome and averaged");

//shared_ptr<ParameterLink<bool>> GeneticProgramingBrain::initializeUniformPL = Parameters::register_parameter("BRAIN_CONSTANT-initializeUniform", false, "Initialize genome randomly, with all samples having same value");
//shared_ptr<ParameterLink<bool>> GeneticProgramingBrain::initializeConstantPL = Parameters::register_parameter("BRAIN_CONSTANT-initializeConstant", false, "If true, all values in genome will be initialized to initial constant value.");
//shared_ptr<ParameterLink<int>> GeneticProgramingBrain::initializeConstantValuePL = Parameters::register_parameter("BRAIN_CONSTANT-initializeConstantValue", 0, "If initialized constant, this value is used to initialize entire genome.");

CGPBrain::CGPBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT) :
	AbstractBrain(_nrInNodes, _nrOutNodes, _PT) {

	//valueMin = (PT == nullptr) ? valueMinPL->lookup() : PT->lookupDouble("BRAIN_CONSTANT-valueMin");
	//valueMax = (PT == nullptr) ? valueMaxPL->lookup() : PT->lookupDouble("BRAIN_CONSTANT-valueMax");
	//valueType = (PT == nullptr) ? valueTypePL->lookup() : PT->lookupInt("BRAIN_CONSTANT-valueType");
	//samplesPerValue = (PT == nullptr) ? samplesPerValuePL->lookup() : PT->lookupInt("BRAIN_CONSTANT-samplesPerValue");

	//initializeUniform = (PT == nullptr) ? initializeUniformPL->lookup() : PT->lookupBool("BRAIN_CONSTANT-initializeUniform");
	//initializeConstant = (PT == nullptr) ? initializeConstantPL->lookup() : PT->lookupBool("BRAIN_CONSTANT-initializeConstant");
	//initializeConstantValue = (PT == nullptr) ? initializeConstantValuePL->lookup() : PT->lookupInt("BRAIN_CONSTANT-initializeConstantValue");

	//cout << "init" << endl;

	buildFromGenome = true;

	allOps = { { "SUM",0 },{ "MULT",1 },{ "SUBTRACT",2 },{ "DIVIDE",3 },{ "SIN",4 },{ "COS",5 },{"THRESH",6},{"RAND",7},{"IF",8},{ "INV",9} };
	// ops to add
	// THRESH(x,y) - if abs(x)>y then y, else x
	// RAND(x,y) - return double [x,y]
	// IF(x,y) - if x > 0, then y else 0

	availableOps = { 0,1,2,3,4,5,6,7,8,9 };
	availableOpsCount = availableOps.size();

	nrHiddenValues = 3;
	readFromOutputs = true;

	magnitudeMax = 1000000000.0; // values with abs > are set to this value (but keep sign)
	magnitudeMin = -1000000000.0; // values with abs < are set to this value (but keep sign)

	nrInputTotal = nrInputValues + ((readFromOutputs) ? nrOutputValues : 0) + nrHiddenValues;
	nrOutputTotal = nrOutputValues + nrHiddenValues;

	readFromValues.resize(nrInputTotal, 0);
	writeToValues.resize(nrOutputTotal, 0);


	brainVectors.clear();

	// columns to be added to ave file
	aveFileColumns.clear();

}

CGPBrain::CGPBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<AbstractGenome> _genome, shared_ptr<ParametersTable> _PT) :
	CGPBrain(_nrInNodes, _nrOutNodes, _PT) {

	brainVectors.resize(nrOutputValues + nrHiddenValues);
	auto handler = _genome->newHandler(_genome);

	for (int f = 0; f < (nrOutputTotal); f++) {
		for (int i = 0; i < numOpsPreVector; i++) {
			brainVectors[f].push_back(availableOps[handler->readInt(0, availableOpsCount - 1)]);
			brainVectors[f].push_back(handler->readInt(0, nrInputTotal + (i - 1) - 1)); // num inputs + num new values - 1 since first is 0 -1 more to correct for 0 indexing
			brainVectors[f].push_back(handler->readInt(0, nrInputTotal + (i - 1) - 1));
		}
	}
}


void CGPBrain::resetBrain() {
	//cout << "in reset Brain" << endl;
	fill(inputValues.begin(), inputValues.end(), 0);
	fill(outputValues.begin(), outputValues.end(), 0);
	fill(readFromValues.begin(), readFromValues.end(), 0);
	fill(writeToValues.begin(), writeToValues.end(), 0);
}

void CGPBrain::update() {
	for (int index = 0; index < nrInputValues; index++) { // copy input values into readFromValues
		readFromValues[index] = inputValues[index];
	}
	if (readFromOutputs) { // if readFromOutputs, then add last outputs
		for (int index = 0; index < nrOutputValues; index++) {
			readFromValues[index + nrInputValues] = writeToValues[index];
		}
	}
	for (int index = nrOutputValues; index < nrOutputValues + nrHiddenValues; index++) { // add hidden values from writeToValues
		readFromValues[index + nrInputValues - ((!readFromOutputs)? nrOutputValues:0)] = writeToValues[(index)];
	}

	DataMap dataMap;
	vector<double> values;

#if CGPBRAIN_DEBUG == 1
	cout << "***********************************\nSTART" << endl;
#endif

	for (int vec = 0; vec < brainVectors.size(); vec++) {
#if CGPBRAIN_DEBUG == 1
		cout << "vec: " << vec << endl;
#endif
		values.clear();
		values = readFromValues;
		for (int site = 0; site < brainVectors[vec].size(); site += 3) {
			double op1 = values[brainVectors[vec][site + 1]];
			double op2 = values[brainVectors[vec][site + 2]];
			switch (brainVectors[vec][site]) {
				//allOps = { { "SUM",0 },{ "MULT",1 },{ "SUBTRACT",2 },{ "DIVIDE",3 },{ "SIN",4 },{ "COS",5 } };
			case 0: // SUM
#if CGPBRAIN_DEBUG == 1
				cout << "SUM(" << brainVectors[vec][site + 1] << "=" << op1 << "," << brainVectors[vec][site + 2] << "=" << op2 << ")" << endl;
#endif
				values.push_back(min(magnitudeMax,max(magnitudeMin,op1 + op2)));
				break;
			case 1: // MULT
#if CGPBRAIN_DEBUG == 1
				cout << "MULT(" << brainVectors[vec][site + 1] << "=" << op1 << "," << brainVectors[vec][site + 2] << "=" << op2 << ")" << endl;
#endif
				values.push_back(min(magnitudeMax, max(magnitudeMin, op1 * op2)));
				break;
			case 2: // SUBTRACT
#if CGPBRAIN_DEBUG == 1
				cout << "SUBTRACT(" << brainVectors[vec][site + 1] << "=" << op1 << "," << brainVectors[vec][site + 2] << "=" << op2 << ")" << endl;
#endif
				values.push_back(min(magnitudeMax, max(magnitudeMin, op1 - op2)));
				break;
			case 3: // DIVIDE
				if (op2 == 0) {
#if CGPBRAIN_DEBUG == 1
					cout << "DIVIDE**0**(" << brainVectors[vec][site + 1] << "=" << op1 << "," << brainVectors[vec][site + 2] << "=" << op2 << ")" << endl;
#endif
					values.push_back(0);
				}
				else {
#if CGPBRAIN_DEBUG == 1
					cout << "DIVIDE(" << brainVectors[vec][site + 1] << "=" << op1 << "," << brainVectors[vec][site + 2] << "=" << op2 << ")" << endl;
#endif
					values.push_back(min(magnitudeMax, max(magnitudeMin, op1 / op2)));
				}break;
			case 4: // SIN
#if CGPBRAIN_DEBUG == 1
				cout << "SIN(" << brainVectors[vec][site + 1] << "=" << op1 << "," << brainVectors[vec][site + 2] << "=" << op2 << ")" << endl;
#endif
				values.push_back(min(magnitudeMax, max(magnitudeMin, sin(op1))));
				break;
			case 5: // COS
#if CGPBRAIN_DEBUG == 1
				cout << "COS(" << brainVectors[vec][site + 1] << "=" << op1 << "," << brainVectors[vec][site + 2] << "=" << op2 << ")" << endl;
#endif
				values.push_back(min(magnitudeMax, max(magnitudeMin, cos(op1))));
				break;
			
			case 6: // THRESH
#if CGPBRAIN_DEBUG == 1
				cout << "THRESH(" << brainVectors[vec][site + 1] << "=" << op1 << "," << brainVectors[vec][site + 2] << "=" << op2 << ")" << endl;
#endif
				values.push_back(min(magnitudeMax, max(magnitudeMin, (op1>op2)?op2:op1)));
				break;
			case 7: // RAND
#if CGPBRAIN_DEBUG == 1
				cout << "RAND(" << brainVectors[vec][site + 1] << "=" << op1 << "," << brainVectors[vec][site + 2] << "=" << op2 << ")" << endl;
#endif
				values.push_back(min(magnitudeMax, max(magnitudeMin, Random::getDouble(op1,op2))));
				break;
			case 8: // IF
#if CGPBRAIN_DEBUG == 1
				cout << "IF(" << brainVectors[vec][site + 1] << "=" << op1 << "," << brainVectors[vec][site + 2] << "=" << op2 << ")" << endl;
#endif
				values.push_back(min(magnitudeMax, max(magnitudeMin, (op1>0)?op2:0)));
				break;
			case 9: // INV
#if CGPBRAIN_DEBUG == 1
				cout << "INV(" << brainVectors[vec][site + 1] << "=" << op1 << "," << brainVectors[vec][site + 2] << "=" << op2 << ")" << endl;
#endif
				values.push_back(min(magnitudeMax, max(magnitudeMin, -1.0 * op1)));
				break;
			}
#if CGPBRAIN_DEBUG == 1
			cout << "values: ";
			for (auto v : values) {
				cout << v << " ";
			}
			cout << endl;
			cout << "outputValues: ";
			for (auto v : outputValues) {
				cout << v << " ";
			}
			cout << endl;
#endif
		}
		writeToValues[vec] = values.back();
		if (vec < nrOutputValues) {
			outputValues[vec] = values.back();
		}
	}
}

string CGPBrain::description() {
	string S = "CGPBrain\n";
	return S;
}

DataMap CGPBrain::getStats() {
	DataMap dataMap;
	// get stats
	//cout << "warning:: getStats for CGPBrain needs to be written." << endl;
	return (dataMap);
}

shared_ptr<AbstractBrain> CGPBrain::makeCopy(shared_ptr<ParametersTable> _PT) {
	//cout << "   start Make Copy" << endl;
	shared_ptr<CGPBrain> newBrain = make_shared<CGPBrain>(nrInputValues, nrOutputValues, _PT);

	cout << "CGPBrain::makeCopy needs to be written" << endl;
	exit(1);

	//cout << "   done Make Copy" << endl;
	return newBrain;
}

void CGPBrain::initalizeGenome(shared_ptr<AbstractGenome> _genome) {
	_genome->fillRandom();
}
