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

shared_ptr<ParameterLink<int>> CGPBrain::hiddenNodesPL = Parameters::register_parameter("BRAIN_CGP-hiddenNodes", 3, "number of hidden nodes");

shared_ptr<ParameterLink<string>> CGPBrain::availableOperatorsPL = Parameters::register_parameter("BRAIN_CGP-availableOperators", (string)"[all]", "which opperators are allowed? all indicates, allow all opperators or, choose from: SUM,MULT,SUBTRACT,DIVIDE,SIN,COS,THRESH,RAND,IF,INV");
shared_ptr<ParameterLink<double>> CGPBrain::magnitudeMaxPL = Parameters::register_parameter("BRAIN_CGP-magnitudeMax", 1000000000.0, "values generated which are larger then this will by clipped");
shared_ptr<ParameterLink<double>> CGPBrain::magnitudeMinPL = Parameters::register_parameter("BRAIN_CGP-magnitudeMin", -1000000000.0, "values generated which are smaller then this will by clipped");
shared_ptr<ParameterLink<int>> CGPBrain::numOpsPreVectorPL = Parameters::register_parameter("BRAIN_CGP-operatorsPreFormula", 8, "number of instructions per formula. Ignored if buildMode is \"genes\"");

shared_ptr<ParameterLink<string>> CGPBrain::genomeNamePL = Parameters::register_parameter("BRAIN_CGP_NAMES-genomeName", (string)"root", "name of genome used to encode this brain\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");

shared_ptr<ParameterLink<string>> CGPBrain::buildModePL = Parameters::register_parameter("BRAIN_CGP-buildMode", (string)"linear", "How is the genome converted, \"linear\" : linear conversion starting at begining of genome, \"codon\" : start codons locate operator+in1+in2 along with the formula/output index and a location in formula");
shared_ptr<ParameterLink<int>> CGPBrain::codonMaxPL = Parameters::register_parameter("BRAIN_CGP-codonMax", 100, "if using \"genes\" buildMode, values will be extracted from genome as integers [0..codonMax] and two sites that add to codonMax defines a start codon");

shared_ptr<ParameterLink<bool>> CGPBrain::readFromOutputsPL = Parameters::register_parameter("BRAIN_CGP-readFromOutputs", true, "if true, previous updates outputs will be available as inputs.");

CGPBrain::CGPBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT) :
	AbstractBrain(_nrInNodes, _nrOutNodes, _PT) {

	convertCSVListToVector((PT == nullptr) ? availableOperatorsPL->lookup() : PT->lookupString("BRAIN_CGP-availableOperators"), availableOperators);
	nrHiddenValues = (PT == nullptr) ? hiddenNodesPL->lookup() : PT->lookupInt("BRAIN_CGP-hiddenNodes");
	magnitudeMax = (PT == nullptr) ? magnitudeMaxPL->lookup() : PT->lookupDouble("BRAIN_CGP-magnitudeMax");
	magnitudeMin = (PT == nullptr) ? magnitudeMinPL->lookup() : PT->lookupDouble("BRAIN_CGP-magnitudeMin");
	numOpsPreVector = (PT == nullptr) ? numOpsPreVectorPL->lookup() : PT->lookupInt("BRAIN_CGP-operatorsPreFormula");

	buildMode = (PT == nullptr) ? buildModePL->lookup() : PT->lookupString("BRAIN_CGP-buildMode");
	codonMax = (PT == nullptr) ? codonMaxPL->lookup() : PT->lookupInt("BRAIN_CGP-codonMax");

	readFromOutputs = (PT == nullptr) ? readFromOutputsPL->lookup() : PT->lookupBool("BRAIN_CGP-readFromOutputs");
	//cout << "init" << endl;

	allOps = { { "SUM",0 },{ "MULT",1 },{ "SUBTRACT",2 },{ "DIVIDE",3 },{ "SIN",4 },{ "COS",5 },{"THRESH",6},{"RAND",7},{"IF",8},{ "INV",9} };

	if (availableOperators.size() == 0 || availableOperators[0] == "all") {
		availableOps = { 0,1,2,3,4,5,6,7,8,9 };
	}
	else {
		for (auto o : availableOperators) {
			if (allOps.count(o)) {
				availableOps.push_back(allOps[o]);
			}
			else {
				cout << "In CGP brain inititialization, found unknown operator \"" << o << "\" in list of allowed operators.\n  exiting." << endl;
				exit(1);
			}
		}
	}

	availableOpsCount = availableOps.size();

	nrInputTotal = nrInputValues + ((readFromOutputs) ? nrOutputValues : 0) + nrHiddenValues;
	nrOutputTotal = nrOutputValues + nrHiddenValues;

	readFromValues.resize(nrInputTotal, 0);
	writeToValues.resize(nrOutputTotal, 0);

	genomeName = (PT == nullptr) ? genomeNamePL->lookup() : PT->lookupString("BRAIN_CGP_NAMES-genomeName");

	brainVectors.clear();

	// columns to be added to ave file
	popFileColumns.clear();
	if (buildMode == "codon") {
		popFileColumns.push_back("aveFormulaLength");
	}

}

CGPBrain::CGPBrain(int _nrInNodes, int _nrOutNodes, unordered_map<string, shared_ptr<AbstractGenome>>& _genomes, shared_ptr<ParametersTable> _PT) :
	CGPBrain(_nrInNodes, _nrOutNodes, _PT) {

	brainVectors.resize(nrOutputTotal);
	auto handler = _genomes[genomeName]->newHandler(_genomes[genomeName]);
	
	if (buildMode == "linear") {
		for (int f = 0; f < (nrOutputTotal); f++) {
			for (int i = 0; i < numOpsPreVector; i++) {
				brainVectors[f].push_back(availableOps[handler->readInt(0, availableOpsCount - 1)]);
				brainVectors[f].push_back(handler->readInt(0, nrInputTotal + (i)-1)); // num inputs + num new values - 1 since first is 0 -1 more to correct for 0 indexing
				brainVectors[f].push_back(handler->readInt(0, nrInputTotal + (i)-1));
			}
		}
	}
	else if (buildMode == "codon") {
#if CGPBRAIN_DEBUG == 1
		cout << "building with genes" << endl;
#endif
		// maps locations of ops in that formulas (index1 is formula, index2 is op, double is location
		vector<vector<double>> locations;
		// size locations to the number of formulas needed
		locations.resize(nrOutputTotal);

		bool translation_Complete = false;

		if (_genomes[genomeName]->isEmpty()) {
			translation_Complete = true;
		}
		else {

			bool readForward = true;
			auto genomeHandler = _genomes[genomeName]->newHandler(_genomes[genomeName], readForward);
			auto placeHolderGenomeHandler = _genomes[genomeName]->newHandler(_genomes[genomeName], readForward);
			auto gateGenomeHandler = _genomes[genomeName]->newHandler(_genomes[genomeName], readForward);

			// save start of genome info
			int i = 0;

			int testSite1Value, testSite2Value;
			testSite1Value = genomeHandler->readInt(0, codonMax);
			testSite2Value = genomeHandler->readInt(0, codonMax);
			while (!translation_Complete) {
				if (genomeHandler->atEOC()) {  // if genomeIndex > testIndex, testIndex has wrapped and we are done translating
					if (genomeHandler->atEOG()) {
						translation_Complete = true;
					}
					genomeHandler->resetHandlerOnChromosome();  // reset to start of this chromosome
					genomeHandler->copyTo(placeHolderGenomeHandler);  // move placeholder to the next chromosome aswell so mustReadAll method works
					testSite2Value = genomeHandler->readInt(0, codonMax);  // place first value in new chromosome in testSite2 so !mustReadAll method works
				}
				else if (testSite1Value + testSite2Value == codonMax) {  // if we found a start codon
#if CGPBRAIN_DEBUG == 1
					cout << "\nstart codon found: " << testSite1Value << "   " << testSite2Value << endl;
#endif
					genomeHandler->copyTo(gateGenomeHandler);
					gateGenomeHandler->toggleReadDirection();
					gateGenomeHandler->readInt(0, codonMax);  // move back 2 start codon values
					gateGenomeHandler->readInt(0, codonMax);
					gateGenomeHandler->toggleReadDirection();  // reverse the read direction again
					gateGenomeHandler->readInt(0, codonMax, 1, 0);  // mark start codon in genomes coding region
					gateGenomeHandler->readInt(0, codonMax, 1, 0);

					// Which formula is this op in?

					int f = gateGenomeHandler->readInt(0, nrOutputTotal - 1);
					// Now read the opperator and inputs
					int op = availableOps[gateGenomeHandler->readInt(0, availableOpsCount - 1,2)];
					int in1 = gateGenomeHandler->readInt(0, _genomes[genomeName]->getAlphabetSize() - 1,2);
					int in2 = gateGenomeHandler->readInt(0, _genomes[genomeName]->getAlphabetSize() - 1,2);
#if CGPBRAIN_DEBUG == 1
					cout << "got values:   f: " << f << "  op : " << op << "  in1: " << in1 << "  in2: " << in2 << endl;
#endif					
					// Now find the location in the formula
					double location = gateGenomeHandler->readDouble(0.0,1.0);
#if CGPBRAIN_DEBUG == 1
					cout << "   @ location: " << location << endl;
					cout << "   formula " << f << "  size = " << locations[f].size() << endl;
#endif
					if (locations[f].size() == 0 || locations[f][locations[f].size()-1] <= location) {
#if CGPBRAIN_DEBUG == 1
						if (locations[f].size() == 0) {
							cout << "      first" << endl;
						}
						else {
							cout << "      Last in formula... last location: " << locations[f][locations[f].size() - 1] << endl;
						}
#endif
						// if this formula is empty, or the new op is last push back new op
						brainVectors[f].push_back(op);
						brainVectors[f].push_back(in1);
						brainVectors[f].push_back(in2);
						locations[f].push_back(location);
					}
					else {
#if CGPBRAIN_DEBUG == 1
						cout << "      Not Last in formula... last location: " << locations[f][locations[f].size() - 1] << endl;
#endif						
						// new op is not last
						int index = locations[f].size()-1;
#if CGPBRAIN_DEBUG == 1
						cout << "index: " << index << endl;
#endif
						// copy current last op to back of formula
						brainVectors[f].push_back(brainVectors[f][brainVectors[f].size() - 3]);
						brainVectors[f].push_back(brainVectors[f][brainVectors[f].size() - 2]);
						brainVectors[f].push_back(brainVectors[f][brainVectors[f].size() - 1]);
						locations[f].push_back(locations[f][locations[f].size()-1]);

						// starting at the back, find the location, push each op forward until we find it.
						while (index > 0 && locations[f][index] > location) {
							brainVectors[f][((index + 1) * 3) + 2] = brainVectors[f][(index * 3) + 2];
							brainVectors[f][((index + 1) * 3) + 1] = brainVectors[f][(index * 3) + 1];
							brainVectors[f][((index + 1) * 3) + 0] = brainVectors[f][(index * 3) + 0];
							locations[f][index+1] = locations[f][index];
#if CGPBRAIN_DEBUG == 1
							cout << index << " <<>> ";
#endif
							index--;
#if CGPBRAIN_DEBUG == 1
							cout << index << endl;
#endif
						}

						// now index is set to where we need to insert the new instruction
						brainVectors[f][(index * 3)] = op;
						brainVectors[f][(index * 3) + 1] = in1;
						brainVectors[f][(index * 3) + 2] = in2;
						locations[f][index] = location;
					}

				}
				if (0) {//mustReadAll) {  // if start codon values are bigger then the alphabetSize of the genome, we must step forward one genome site at a time (slow)
					placeHolderGenomeHandler->advanceIndex();
					placeHolderGenomeHandler->copyTo(genomeHandler);
					testSite1Value = genomeHandler->readInt(0, codonMax);
					testSite2Value = genomeHandler->readInt(0, codonMax);
				}
				else {  // we know that start codon values fit in a single site, so we can be clever
					testSite1Value = testSite2Value;
					testSite2Value = genomeHandler->readInt(0, codonMax);
				}
				//cout << testSite1Value << " + " << testSite2Value << " = " << testSite1Value + testSite2Value << endl;
			}
			// translation compete - now fix the input values
			for (int f = 0; f < (nrOutputTotal); f++) {
#if CGPBRAIN_DEBUG == 1
				cout << "f: " << f << "  :: ";
#endif
				for (int i = 0; i < brainVectors[f].size(); i+=3) {
#if CGPBRAIN_DEBUG == 1
					cout << brainVectors[f][i] << "  ";
					cout << " (" << brainVectors[f][i + 1] << "%" << nrInputTotal + (i / 3) << ") ";
#endif
					brainVectors[f][i + 1] = brainVectors[f][i + 1] % (nrInputTotal + (i/3)); // num inputs + num new values - 1 since first is 0 -1 more to correct for 0 indexing
#if CGPBRAIN_DEBUG == 1
					cout << brainVectors[f][i+1] << "  ";
					cout << " (" << brainVectors[f][i + 2] << "%" << nrInputTotal + (i / 3) << ") ";
#endif
					brainVectors[f][i + 2] = brainVectors[f][i + 2] % (nrInputTotal + (i/3)); // num inputs + num new values - 1 since first is 0 -1 more to correct for 0 indexing
#if CGPBRAIN_DEBUG == 1
					cout << brainVectors[f][i+2] << " , ";
#endif
				}
#if CGPBRAIN_DEBUG == 1
				cout << endl;
#endif
			}
			// input values are now correct
		}
	}
	else {
		cout << "\n\nIn CGP constructor, found unknown buildMode \"" << buildMode << "\".\n exiting." << endl;
		exit(1);
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

			case 0: // SUM
#if CGPBRAIN_DEBUG == 1
				cout << "SUM(" << brainVectors[vec][site + 1] << "=" << op1 << "," << brainVectors[vec][site + 2] << "=" << op2 << ")" << endl;
#endif
				values.push_back(min(magnitudeMax, max(magnitudeMin, op1 + op2)));
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

DataMap CGPBrain::getStats(string& prefix) {
	DataMap dataMap;

	double aveFormulaLength = 0.0;
	if (buildMode == "codon") {
		for (auto vec : brainVectors) {
			aveFormulaLength += (double)(vec.size());
		}
		aveFormulaLength /= double(brainVectors.size());
		dataMap.Set(prefix + "aveFormulaLength", aveFormulaLength);
	}
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

void CGPBrain::initalizeGenomes(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) {
	_genomes[genomeName]->fillRandom();
}
