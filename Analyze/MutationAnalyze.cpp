//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "MutationAnalyze.h"

using namespace std;

///*
// * Run "testAgent" "n" times in world "world"
// */
//
//vector<double> MutationAnalyse::evalAgentNTimes(Agent* testAgent, World* world, int n) {
//	vector<double> Fits;
//	for (int run = 0; run < n; run++) {
//		Fits.push_back(world->testIndividual(testAgent, false));
//	}
//	return Fits;
//}
//
///*
// * searches "genome" for 1/2 definded start codons. That is places in the genome where either
// * a possible first code of a start codon is not followed by the corresponding second codon
// * or a possible second code of a start codeon is not proceeded but the corresponding first condon
// *
// * uses Global::inUseGateTypes to determine which values may code for gates.
// *
// * the return value is a vector of sets of int where:
// * each set lists the address of the identified element.
// * the first set in the vector is all of the first position codes
// * the second set is the the second position condons
// */
//vector<set<int>> MutationAnalyse::getPossibleStartCodons(Genome* genome) {
//	vector<set<int>> possStartCodons;
//	possStartCodons.resize(2); //vector[0] is the list of first-position start codons (e.g. 42), vector[1] is the second-position start codons
//	for (int genomeSearcher = 0; genomeSearcher < (int) genome->sites.size(); genomeSearcher++) { //for every spot in the genome
//		for (auto gateType : Global::inUseGateTypes) {
//			if (genome->sites[genomeSearcher] == gateType) { //if a nucleotide is one of these numbers
//				if (genome->sites[(genomeSearcher + 1) % genome->sites.size()]
//						!= (256 - genome->sites[genomeSearcher])) { //if this is not already a gate
//					possStartCodons[0].insert(genomeSearcher);
//				}
//			}
//			if (genome->sites[genomeSearcher] == 256 - gateType) { //FOR LOOKING FOR A CERTAIN GATE: CHANGE THIS NUMBER RIGHT HERE
//				if (genome->sites[(genomeSearcher - 1) % genome->sites.size()]
//						!= 256 - (genome->sites[genomeSearcher])) {
//					possStartCodons[1].insert(genomeSearcher);
//				}
//			}
//		}
//	}
//	return possStartCodons;
//}
//
///*
// * evaluates all single point variations for "genome" and updates a number of passed by reference variables
// *
// * - a list of all coding sites is collected from the gates (base data generated when the gates are created)
// * - getPossibleStartCodons is called to find all single point mutations which will result in the creation of a new gate
// * - all sites where a change will result in a new gate are compaired with the coding sites list. When a site exists in
// *   both list, it is removed from the start codon list.
// * - once all sites to be tested have been determined
// *
// * - a total is determined, which is the total number of mutations which will be tested for. The wild type is run this
// *   number of times - this is probably exsesive!
// *
// *   for active coding sites 7 tests are made for each possible site mutation (all 255 alternitives)
// *   for start codon sites 7 tests are made only for the single mutation that results in a new gate
// *   (all other mutations would result in the wild type fitness)
// *
// *   the following data is collected:
// *   rawWildTypeFitness - a list of results from running the wild type genome a disturbing number of times.
// *   avePerVar - a list of lists where each element is the average score of 7(n) tests of each possible point mutation
// *   allVar - raw data, all tests
// *   avePerSite - the average score per site
// *
// *
// *   note to self - when doing allVar for WT sites put in selection from WT distribution
// *   				when doing avePerVar for WT put in WTave.
// *   				fr avePerSite use WTave
// */
//void MutationAnalyse::evalPossibleAgents(Genome* genome, int brainStates, World *world, double &avgWildTypeFitness,
//		vector<double> &rawWildTypeFitness, map<int, vector<double>> &avePerVar, vector<double> &allVar,
//		vector<double> &avePerSite) { //evaluates all possible variations for all genomes in a population
//
//	int changedCount = 0; //failsafe to make sure we're changing the right number of nucleotides
//	int numVarRuns = 7;
//
//	Genome *testGenome = new Genome; //makes testGenome for testing
//	testGenome->sites = genome->sites; //assigns testGenome to the genome of the brain currently being tested
//	Genome *staticTestGenome = genome; //makes staticTestGenome for comparison - so that we don't need to keep recasting genome!
//	Agent *testAgent = new Agent(testGenome, brainStates); //assigns testAgent to be the brain with the genome of testGenome
//	set<int> brainCodingReg = testAgent->findCodingRegions(0); //finds coding regions (argument is mask) for the brain in question
//	vector<set<int>> possStartCodons = getPossibleStartCodons(testGenome);
//
//	//double avgFitnessChange=0.0; // avgFitnessChange is the average fitness change for all variations of all sites in the brain
//
//	//these loops check to make sure identified start codons are not already in brainCodingRegions
//	for (auto position : possStartCodons[0]) {
//		int testSite = position + 1;
//		if (testSite == (int) testGenome->sites.size()) {
//			testSite = 0;
//		}
//		if (brainCodingReg.find(testSite) != brainCodingReg.end()) { //if the site after this site is in the list of coding regions
//			possStartCodons[0].erase(position);
//		}
//	}
//
//	for (auto position : possStartCodons[1]) {
//		int testSite = position - 1;
//		if (testSite == -1) {
//			testSite = testGenome->sites.size() - 1;
//		}
//		if (brainCodingReg.find(testSite) != brainCodingReg.end()) { //if the site before this site is in the list of coding regions
//			possStartCodons[1].erase(position);
//		}
//	}
//
//	int numBaseFitnessRuns = (brainCodingReg.size() + possStartCodons[0].size() + possStartCodons[1].size()) * 256 * 7;
//	rawWildTypeFitness = evalAgentNTimes(testAgent, world, numBaseFitnessRuns);
//	for (size_t BFVars = 0; BFVars < rawWildTypeFitness.size(); BFVars++) {
//		avgWildTypeFitness += rawWildTypeFitness[BFVars];
//	}
//	avgWildTypeFitness = avgWildTypeFitness / double(numBaseFitnessRuns); //averages the wildtype fitnesses
//
//	delete (testAgent); //you have to delete testAgent because the coding regions can only be accessed by the gate constructor. You can't update the brain, only delete and recreate it.
//
//	for (int site : brainCodingReg) { //for every site in the brain's coding region
//		changedCount++; //this just increments the failsafe
//		double avgPerSiteFitness = 0.0; //creates new average for each coding site
//		avePerVar[site].resize(256); //there will be 256 entries in every vector assigned to a specific site
//		avePerVar[site][testGenome->sites[site]] = avgWildTypeFitness; //assign the index of the vector that is the original variation for this site to the wild type fitness
//		avgPerSiteFitness += avgWildTypeFitness;
//		for (int i = 0; i < numVarRuns; i++) {
//			allVar.push_back(avgWildTypeFitness);
//		}
//		for (int variation = (testGenome->sites[site] + 1) & 255; variation != staticTestGenome->sites[site];
//				variation = (variation + 1) & 255) { //
//			testGenome->sites[site] = variation; //changes the testAgent's site to a different variation
//			testAgent = new Agent(testGenome, brainStates);
//			double varFitnessTotal = 0.0;
//			vector<double> varFits = evalAgentNTimes(testAgent, world, numVarRuns);
//			for (double fits : varFits) {
//				varFitnessTotal += fits; //this is for avePerVar
//				allVar.push_back(fits); //this is for allVar
//			}
//			avePerVar[site][variation] = (varFitnessTotal / double(numVarRuns));
//			avgPerSiteFitness += avePerVar[site][variation]; //this is for avePerSite
//			//cout << "Fitness Change =" << popID << " " << site << " allVar" << variation << " " << popScores[popID][site][variation] << "\n";
//			delete (testAgent);
//		}
//
//		avgPerSiteFitness = avgPerSiteFitness / 256.0; //This could be either 265 or 255 depending on the implications you want but it's so small it barely matters
//		avePerSite.push_back(avgPerSiteFitness);
//
//		testGenome->sites[site] = staticTestGenome->sites[site]; //reset genome
//
//		//cout << "Before testing potential gates, the number of nucleotides changed is " << changedCount << "\n";
//	}
//	for (auto startCod1 : possStartCodons[0]) {
//		changedCount++;
//		double avgPerSiteFitness = 0.0; //creates new average for each coding site
//		avePerVar[startCod1 + 1].resize(256);
//		for (int filler = (testGenome->sites[startCod1 + 1] + 1) & 255;
//				filler != staticTestGenome->sites[startCod1 + 1]; filler = (filler + 1) & 255) {
//			avgPerSiteFitness += avgWildTypeFitness;
//			avePerVar[startCod1 + 1][filler] = avgWildTypeFitness;
//			for (int i = 0; i < numVarRuns; i++) {
//				allVar.push_back(avgWildTypeFitness);
//			}
//		}
//		testGenome->sites[startCod1 + 1] = 256 - (testGenome->sites[startCod1]); //sets the nucleotide after the first half of the start codon to 256-it
//		testAgent = new Agent(testGenome, brainStates); //make new brain
//		double varFitnessTotal = 0.0;
//		vector<double> varFits = evalAgentNTimes(testAgent, world, numVarRuns);
//		for (double fits : varFits) {
//			varFitnessTotal += fits;
//			allVar.push_back(fits); //this is for allVar
//		}
//		avePerVar[startCod1 + 1][256 - (staticTestGenome->sites[startCod1])] = (varFitnessTotal / double(numVarRuns));
//		avgPerSiteFitness += (varFitnessTotal / double(numVarRuns));
//		avePerSite.push_back(avgPerSiteFitness / (double) 256.0);
//
//		testGenome->sites[startCod1 + 1] = staticTestGenome->sites[startCod1 + 1];
//		delete (testAgent);
//		//avgFitnessChange+=siteFitness;
//	}
//	for (auto startCod2 : possStartCodons[1]) {
//		changedCount++;
//		double avgPerSiteFitness = 0.0; //creates new average for each coding site
//		avePerVar[startCod2 - 1].resize(256);
//		for (int filler = (testGenome->sites[startCod2 - 1] + 1) & 255;
//				filler != staticTestGenome->sites[startCod2 - 1]; filler = (filler + 1) & 255) {
//			avgPerSiteFitness += avgWildTypeFitness;
//			avePerVar[startCod2 - 1][filler] = avgWildTypeFitness;
//			for (int i = 0; i < numVarRuns; i++) {
//				allVar.push_back(avgWildTypeFitness);
//			}
//		}
//		testGenome->sites[startCod2 - 1] = 256 - (testGenome->sites[startCod2]);
//		testAgent = new Agent(testGenome, brainStates); //make new brain
//		double varFitnessTotal = 0.0;
//		vector<double> varFits = evalAgentNTimes(testAgent, world, numVarRuns);
//		for (double fits : varFits) {
//			varFitnessTotal += fits;
//			allVar.push_back(fits); //this is for allVar
//		}
//		avePerVar[startCod2 - 1][256 - (staticTestGenome->sites[startCod2])] = (varFitnessTotal / double(numVarRuns));
//		avgPerSiteFitness += (varFitnessTotal / double(numVarRuns));
//		avePerSite.push_back(avgPerSiteFitness / (double) 256.0);
//
//		testGenome->sites[startCod2 - 1] = staticTestGenome->sites[startCod2 - 1];
//		delete (testAgent);
//		//avgFitnessChange+=siteFitness;
//	}
//	//avgFitnessChange=avgFitnessChange/brainCodingReg.size();
//	//cout << "Agent ID =" << popID << " Coding Region Size = " << brainCodingReg.size() << "  Genome Size = " << testGenome->sites.size() <<"  Average Total Fitness Change =" << avgFitnessChange << "\n";
//
//}
//
//void MutationAnalyse::test_point_mutations_and_output(int gen, Genome* tester, bool avePerVar, bool allVar,
//		bool wildTypeDist, int nrOfBrainStates, World* world, string outputDirectory_Name) {
//	Genome* subject = tester; //make a test genome called subject
//	Agent* subAgent = new Agent((Genome*) subject, nrOfBrainStates); //make the corresponding brain
//
//	double avgWTF;
//	vector<double> rawWTF;
//	map<int, vector<double>> avePerVarStructure;
//	vector<double> allVarStructure;
//	vector<double> avePerSiteStructure;
//
//	evalPossibleAgents(subject, nrOfBrainStates, world, avgWTF, rawWTF, avePerVarStructure, allVarStructure,
//			avePerSiteStructure);
//
//	if (avePerVar == 1) { //this prints a file with the average wildtype fitness and avePerVarStructure
//		char buffer2[50];
//		sprintf(buffer2, "%i", gen);
//		string ave_File_Name = outputDirectory_Name + "/AvesPerVar" + "_g" + string(buffer2) + "_MED_dat.py";
//		ofstream AVE_FILE;
//		AVE_FILE.open(ave_File_Name.c_str());
//		AVE_FILE << "genomeSize = " << ((Genome*) subject)->sites.size() << "\n" << "wildTypeFit = " << avgWTF << "\n"
//				<< "numGates = " << subAgent->numGates() << "\n" << "data = [";
//		vector<double> varsPerSite;
//		for (auto site = avePerVarStructure.begin(); site != avePerVarStructure.end(); site++) {
//			varsPerSite = avePerVarStructure[site->first];
//			for (size_t var = 0; var < varsPerSite.size(); var++) {
//				AVE_FILE << varsPerSite[var] << ",";
//				if (var % 250 == 0) {
//					AVE_FILE << "\n";
//				}
//			}
//			AVE_FILE << "\n";
//		}
//		AVE_FILE << "]\n";
//		for (int i = 0; i < 2; i++) {
//			AVE_FILE << "\n";
//		}
//		AVE_FILE.close();
//	}
//
//	if (allVar == 1) { //this prints a file with the average wildtype fitness and allVarStructure
//		char buffer2[50];
//		sprintf(buffer2, "%i", gen);
//		string ave_File_Name = outputDirectory_Name + "/AllVars" + "_g" + string(buffer2) + "_MED_dat.py";
//		ofstream AVE_FILE;
//		AVE_FILE.open(ave_File_Name.c_str());
//		AVE_FILE << "genomeSize = " << ((Genome*) subject)->sites.size() << "\n" << "wildTypeFit = " << avgWTF << "\n"
//				<< "numGates = " << subAgent->numGates() << "\n" << "data = [";
//		for (size_t i = 0; i < allVarStructure.size(); i++) {
//			AVE_FILE << allVarStructure[i] << ",";
//			if (i % 250 == 0) {
//				AVE_FILE << "\n";
//			}
//		}
//		AVE_FILE << "]\n";
//		for (int i = 0; i < 2; i++) {
//			AVE_FILE << "\n";
//		}
//		AVE_FILE.close();
//	}
//
//	if (wildTypeDist == 1) { //this prints a file with the average wildtype fitness and wildtype variation
//		char buffer2[50];
//		sprintf(buffer2, "%i", gen);
//		string ave_File_Name = outputDirectory_Name + "/WildTypeDist" + "_g" + string(buffer2) + "_MED_dat.py";
//		ofstream AVE_FILE;
//		AVE_FILE.open(ave_File_Name.c_str());
//		AVE_FILE << "genomeSize = " << ((Genome*) subject)->sites.size() << "\n" << "wildTypeFit = " << avgWTF << "\n"
//				<< "numGates = " << subAgent->numGates() << "\n" << "data = [";
//		for (size_t i = 0; i < rawWTF.size(); i++) {
//			AVE_FILE << rawWTF[i] << ",";
//			if (i % 250 == 0) {
//				AVE_FILE << "\n";
//			}
//		}
//		AVE_FILE << "]";
//		for (int i = 0; i < 2; i++) {
//			AVE_FILE << "\n";
//		}
//		AVE_FILE.close();
//	}
//	delete subAgent;
//}
///*
// void printPopscores(vector<map<int,vector<double>>> popScores){ //not helpful
// for(int brainID=0;brainID<popScores.size();brainID++){
// cout << "Agent ID =" << brainID << "\n";
// for(int siteIndex=0;siteIndex<popScores[brainID].size();siteIndex++){ //for every int key of the map
// cout << "Site Index =" << siteIndex << "\n";
// for(int variation=0;variation<256;variation++){
// cout << "Variation =" << variation << "\n";
// }
// }
// }
// }
// */

