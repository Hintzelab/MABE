//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include <algorithm>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "Global.h"

#include "Group/Group.h"

#include "Organism/Organism.h"

#include "Utilities/Parameters.h"
#include "Utilities/Random.h"
#include "Utilities/Data.h"
#include "Utilities/Utilities.h"

#include "modules.h"

using namespace std;

int main(int argc, const char * argv[]) {
	// also writes out a config file if requested
	cout << "\n\n" << "\tMM   MM      A       BBBBBB    EEEEEE\n" << "\tMMM MMM     AAA      BB   BB   EE\n" << "\tMMMMMMM    AA AA     BBBBBB    EEEEEE\n" << "\tMM M MM   AAAAAAA    BB   BB   EE\n" << "\tMM   MM  AA     AA   BBBBBB    EEEEEE\n" << "\n" << "\tModular    Agent      Based    Evolver\n\n\n\thintzelab.msu.eduMABE\n\n" << endl;

	cout << "\tfor help run MABE with the \"-h\" flag (i.e. ./MABE -h)." << endl << endl;
	configureDefaultsAndDocumentation();
	bool saveFiles = Parameters::initializeParameters(argc, argv);  // loads command line and configFile values into registered parameters
	// also writes out a config file if requested

	if (saveFiles) {
		int maxLineLength = Global::maxLineLengthPL->lookup();
		int commentIndent = Global::commentIndentPL->lookup();

		Parameters::saveSettingsFiles(maxLineLength, commentIndent, { "*" }, { { "settings_organism.cfg", { "GATE*", "GENOME*", "BRAIN*" } }, { "settings_world.cfg", { "WORLD*" } }, { "settings.cfg", { "" } } });
		cout << "Saving config Files and Exiting." << endl;
		exit(0);
	}

	// outputDirectory must exist. If outputDirectory does not exist, no error will occur, but no data will be writen.
	FileManager::outputDirectory = Global::outputDirectoryPL->lookup();

	if (Global::randomSeedPL->lookup() == -1) {
		random_device rd;
		int temp = rd();
		Random::getCommonGenerator().seed(temp);
		cout << "Generating Random Seed\n  " << temp << endl;
	} else {
		Random::getCommonGenerator().seed(Global::randomSeedPL->lookup());
		cout << "Using Random Seed: " << Global::randomSeedPL->lookup() << endl;
	}

	auto world = makeWorld();

	vector<string> groupNameSpaces;
	convertCSVListToVector(Global::groupNameSpacesPL->lookup(), groupNameSpaces);
	groupNameSpaces.push_back("");
	map<string, shared_ptr<Group>> groups;
	shared_ptr<ParametersTable> PT;

	for (auto NS : groupNameSpaces) {
		if (NS == "") {
			PT = nullptr;  //Parameters::root;
			NS = "default";
		} else {
			PT = Parameters::root->getTable(NS);
		}

		Global::update = -1;  // before there was time, there was a progenitor

		shared_ptr<AbstractGenome> templateGenome = makeTemplateGenome(PT);
		shared_ptr<AbstractBrain> templateBrain = makeTemplateBrain(world, PT);
		shared_ptr<Organism> progenitor = make_shared<Organism>(templateGenome, templateBrain);  // make a organism with a genome and brain - progenitor serves as an ancestor to all and a template organism

		Global::update = 0;  // the beginning of time - now we construct the first population
		int popSize = (PT == nullptr) ? Global::popSizePL->lookup() : PT->lookupInt("GLOBAL-popSize");
		vector<shared_ptr<Organism>> population;
		for (int i = 0; i < popSize; i++) {
			auto newGenome = templateGenome->makeLike();
			templateBrain->initalizeGenome(newGenome);  // use progenitors brain to prepare genome (add start codons, change ratio of site values, etc)
			auto newOrg = make_shared<Organism>(progenitor, newGenome);
			population.push_back(newOrg);  // add a new org to population using progenitors template and a new random genome
		}
		progenitor->kill();  // the progenitor has served it's purpose.

		shared_ptr<AbstractOptimizer> optimizer = makeOptimizer(PT);

		vector<string> aveFileColumns;
		aveFileColumns.clear();
		aveFileColumns.push_back("update");
		aveFileColumns.insert(aveFileColumns.end(), world->aveFileColumns.begin(), world->aveFileColumns.end());
		aveFileColumns.insert(aveFileColumns.end(), population[0]->genome->aveFileColumns.begin(), population[0]->genome->aveFileColumns.end());
		aveFileColumns.insert(aveFileColumns.end(), population[0]->brain->aveFileColumns.begin(), population[0]->brain->aveFileColumns.end());

		shared_ptr<DefaultArchivist> archivist = makeArchivist(aveFileColumns, PT);

		groups[NS] = make_shared<Group>(population, optimizer, archivist);

		if (PT == nullptr) {
			PT = Parameters::root;
		}
		cout << "Group name: " << NS << "\n  " << popSize << " organisms with " << PT->lookupString("GENOME-genomeType") << "<" << PT->lookupString("GENOME-sitesType") << "> genomes and " << PT->lookupString("BRAIN-brainType") << " brains.\n  Optimizer: " << PT->lookupString("OPTIMIZER-optimizer") << "\n  Archivist: " << PT->lookupString("ARCHIVIST-outputMethod") << endl;
		cout << endl;
	}

	string defaultGroup = "default";
	if (groups.find(defaultGroup) == groups.end()) {
		cout << "Group " << defaultGroup << " not found in groups.\nExiting." << endl;
		exit(1);
	}

	if (Global::modePL->lookup() == "run") {
		//////////////////
		// run mode - evolution loop
		//////////////////
		bool finished = false;  // when the archivist says we are done, we can stop!

		while (!finished) {
			world->evaluate(groups[defaultGroup], AbstractWorld::groupEvaluationPL->lookup(), false, false, AbstractWorld::debugPL->lookup());  // evaluate each organism in the population using a World
			//cout << "  evaluation done" << endl;
			finished = groups[defaultGroup]->archive();  // save data, update memory and delete any unneeded data;
														 //cout << "  archive done\n";
			//cout << "  archive done" << endl;

			Global::update++;
			groups[defaultGroup]->optimize();  // update the population (reproduction and death)
			//cout << "  optimize done\n";

			cout << "update: " << Global::update - 1 << "   maxFitness: " << groups[defaultGroup]->optimizer->maxFitness << "" << endl;
		}

		groups[defaultGroup]->archive(1);  // flush any data that has not been output yet

	}

	else if (Global::modePL->lookup() == "visualize") {
		//////////////////
		// visualize mode
		//////////////////
		cout << "  You are running MABE in visualize mode." << endl << endl;
		vector<shared_ptr<AbstractGenome>> testGenomes;
		groups[defaultGroup]->population[0]->genome->loadGenomeFile(Global::visualizePopulationFilePL->lookup(), testGenomes);

		int num_genomes = (int) testGenomes.size();

		vector<int> IDs;
		convertCSVListToVector(Global::visualizeOrgIDPL->lookup(), IDs);

		vector<shared_ptr<Organism>> testPopulation;

		bool padPopulation = false;

		if (IDs[0] == -1) { // visualize last
			shared_ptr<AbstractGenome> temp = testGenomes[(int) testGenomes.size() - 1];
			testGenomes.clear();
			testGenomes.push_back(temp);
		} else if (IDs[0] == -2 && IDs.size() == 1) { // visualize population
			padPopulation = true;
		} else { // visualize given ID(s)
			int foundCount = 0;
			vector<shared_ptr<AbstractGenome>> subsetGenomes;
			for (auto ID : IDs) {
				if (ID != -2) {
					bool found = false;
					for (auto g : testGenomes) {
						if (g->dataMap.Get("ID") == to_string(ID)) {
							subsetGenomes.push_back(g);
							foundCount++;
							found = true;
						}
					}
					if (!found) {
						cout << "ERROR: in visualize mode, can not find genome with ID " << ID << " in file: " << Global::visualizePopulationFilePL->lookup() << ".\n  Exiting." << endl;
						exit(1);
					}
				} else {
					padPopulation = true;
					num_genomes--;
					foundCount++;
				}
			}
			if (foundCount < (int) IDs.size()) {
				cout << "WARRNING: in visualize mode " << (int) IDs.size() - foundCount << " genomes specified in Global::visualizeOrgID could not be found." << endl;
			}
			testGenomes = subsetGenomes;
		}

		if (padPopulation) {
			int index = 0;
			if ((int) testGenomes.size() < Global::popSizePL->lookup()) {
				cout << "  Population size is larger then the number of genomes in the file. Padding population with " << Global::popSizePL->lookup() - (int) testGenomes.size() << " extra copies." << endl;
				while ((int) testGenomes.size() < Global::popSizePL->lookup()) {
					testGenomes.push_back(testGenomes[index]);
					index++;
					cout << index << endl;
					if (index >= num_genomes) {
						index = 0;
					}
				}
			}
			if ((int) testGenomes.size() > Global::popSizePL->lookup()) {
				cout << "  Population size is smaller then the number of genomes in the file. deleting genomes from head of population." << endl;
				testGenomes.erase(testGenomes.begin(), testGenomes.begin() + ((int) testGenomes.size() - Global::popSizePL->lookup()));
			}
		}

		for (auto g : testGenomes) {
			auto newOrg = make_shared<Organism>(groups[defaultGroup]->population[0], g);
			//newOrg->brain->setRecordActivity(true);
			//newOrg->brain->setRecordFileName("wireBrain.run");
			testPopulation.push_back(newOrg);  // add a new org to population using progenitors template and a new random genome
		}

		shared_ptr<Group> testGroup = make_shared<Group>(testPopulation, groups[defaultGroup]->optimizer, groups[defaultGroup]->archivist);
		world->runWorld(testGroup, false, true, false);
		for (auto o : testGroup->population) {
			cout << "  organism with ID: " << o->genome->dataMap.Get("ID") << " generated score: " << o->score << " " << endl;
		}
	} else {
		cout << "\n\nERROR: Unrecognized mode set in configuration!\n  \"" << Global::modePL->lookup() << "\" is not defined.\n\nExiting.\n" << endl;
		exit(1);
	}

	return 0;
}
