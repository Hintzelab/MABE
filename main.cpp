///123
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
#include "Utilities/WorldUtilities.h"

#include "modules.h"

using namespace std;

//void senseTotals(Vector2d<int>& worldgrid,  int& orgx,  int& orgy,  int& orgf,  Sensor& sensor, vector<int>& values){
//
//	bool blocked = false;
//	int currentIndex = 0;
//
//	while (currentIndex != -1){
//		//cout << currentIndex << "  :  " << sensor.angles[orgf]->cX(currentIndex) << "," << sensor.angles[orgf]->cY(currentIndex) << "  " << sensor.angles[orgf]->cX(currentIndex) + orgx << "," << sensor.angles[orgf]->cY(currentIndex) + orgy << "  <>  " << worldgrid(sensor.angles[orgf]->cX(currentIndex) + orgx,sensor.angles[orgf]->cY(currentIndex) + orgy) << endl;
//		values[worldgrid(sensor.angles[orgf]->cX(currentIndex) + orgx, sensor.angles[orgf]->cY(currentIndex) + orgy)]++;
//		blocked = worldgrid(sensor.angles[orgf]->cX(currentIndex)+orgx, sensor.angles[orgf]->cY(currentIndex)+orgy) == 5;
//
//		worldgrid(sensor.angles[orgf]->cX(currentIndex) + orgx, sensor.angles[orgf]->cY(currentIndex) + orgy) += 10;
//
//		currentIndex = sensor.angles[orgf]->advanceIndex(currentIndex, blocked);
//	}
//
//}


int main(int argc, const char * argv[]) {

	cout << "\n\n" << "\tMM   MM      A       BBBBBB    EEEEEE\n" << "\tMMM MMM     AAA      BB   BB   EE\n" << "\tMMMMMMM    AA AA     BBBBBB    EEEEEE\n" << "\tMM M MM   AAAAAAA    BB   BB   EE\n" << "\tMM   MM  AA     AA   BBBBBB    EEEEEE\n" << "\n"
			<< "\tModular    Agent      Based    Evolver\n\n\n\thttps://github.com/ahnt/MABE\n\n" << endl;

	cout << "\tfor help run MABE with the \"-h\" flag (i.e. ./MABE -h)." << endl << endl;
	configureDefaultsAndDocumentation(); // load all parameters from command line and file
	bool saveFiles = Parameters::initializeParameters(argc, argv);  // loads command line and configFile values into registered parameters

	// also writes out a settings files if requested
	if (saveFiles) { // if saveFiles (save settings files) is set
		int maxLineLength = Global::maxLineLengthPL->lookup();
		int commentIndent = Global::commentIndentPL->lookup();

		Parameters::saveSettingsFiles(maxLineLength, commentIndent, { "*" }, { { "settings_organism.cfg", { "GATE*", "GENOME*", "BRAIN*" } }, { "settings_world.cfg", { "WORLD*" } }, { "settings.cfg", { "" } } });
		cout << "Saving settings files and exiting." << endl;
		exit(0);
	}

	// outputDirectory must exist. If outputDirectory does not exist, no error will occur, but no data will be written. THIS SHOULD BE ADDRESSED ONE DAY!
	FileManager::outputDirectory = Global::outputDirectoryPL->lookup();

	// set up random number generator
	if (Global::randomSeedPL->lookup() == -1) {
		random_device rd;
		int temp = rd();
		Random::getCommonGenerator().seed(temp);
		cout << "Generating Random Seed\n  " << temp << endl;
	} else {
		Random::getCommonGenerator().seed(Global::randomSeedPL->lookup());
		cout << "Using Random Seed: " << Global::randomSeedPL->lookup() << endl;
	}

	// make world uses WORLD-worldType to determine type of world
	auto world = makeWorld();

	vector<string> groupNameSpaces;
	convertCSVListToVector(Global::groupNameSpacesPL->lookup(), groupNameSpaces);
	groupNameSpaces.push_back("");
	map<string, shared_ptr<Group>> groups;
	shared_ptr<ParametersTable> PT;

	// for each name space in the GLOBAL-groups create a group. if GLOBAL-groups is empty, create "default" group.
	for (auto NS : groupNameSpaces) {
		if (NS == "") {
			PT = nullptr;  //nullptr is the same as Parameters::root;
			NS = "default";
		} else {
			PT = Parameters::root->getTable(NS);
		}

		Global::update = -1;  // before there was time, there was a progenitor - set time to -1 so progenitor (the root organism) will have birth time -1

		// template objects are used to build progenitor
		shared_ptr<AbstractGenome> templateGenome = makeTemplateGenome(PT);
		shared_ptr<AbstractBrain> templateBrain = makeTemplateBrain(world, PT);

		// make a organism with a templateGenome and templateBrain - progenitor serves as an ancestor to all and a template organism
		shared_ptr<Organism> progenitor = make_shared<Organism>(templateGenome, templateBrain);

		Global::update = 0;  // the beginning of time - now we construct the first population
		int popSize = (PT == nullptr) ? Global::popSizePL->lookup() : PT->lookupInt("GLOBAL-popSize");
		vector<shared_ptr<Organism>> population;
		// add popSize organisms which look like progenitor to population
		for (int i = 0; i < popSize; i++) {
			// make a new genome like the template genome
			auto newGenome = templateGenome->makeLike();
			// use progenitors brain to prepare genome (i.e. add start codons, change ratio of site values, etc)
			templateBrain->initalizeGenome(newGenome);
			// create new organism using progenitor as template (i.e. to define type of brain) and the new genome
			auto newOrg = make_shared<Organism>(progenitor, newGenome);
			// add new organism to population
			population.push_back(newOrg);
		}

		// the progenitor has served it's purpose. Killing an organsim is important as it allows for cleanup.
		progenitor->kill();

		// create an optimizer of type defined by OPTIMIZER-optimizer
		shared_ptr<AbstractOptimizer> optimizer = makeOptimizer(PT);

		// aveFileColumns holds a list of data titles which various modules indicate are interesting/should be tracked and which are averageable
		// ** aveFileColumns define what will appear in the ave.csv file **
		// the following code asks world, genomes and brains for ave file columns
		vector<string> aveFileColumns;
		aveFileColumns.clear();
		aveFileColumns.push_back("update");
		aveFileColumns.insert(aveFileColumns.end(), world->aveFileColumns.begin(), world->aveFileColumns.end());
		aveFileColumns.insert(aveFileColumns.end(), population[0]->genome->aveFileColumns.begin(), population[0]->genome->aveFileColumns.end());
		aveFileColumns.insert(aveFileColumns.end(), population[0]->brain->aveFileColumns.begin(), population[0]->brain->aveFileColumns.end());
//		for (int i = 1; i < aveFileColumns.size(); i++) {
//			aveFileColumns[i] = aveFileColumns[i] + "_AVE";
//			cout << aveFileColumns[i] << " ";
//		}
//		cout << endl;
		// create an archivist of type determined by ARCHIVIST-outputMethod
		shared_ptr<DefaultArchivist> archivist = makeArchivist(aveFileColumns, PT);

		// create a new group with the new population, optimizer and archivist and place this group in the map groups
		groups[NS] = make_shared<Group>(population, optimizer, archivist);

		// report on what was just built
		if (PT == nullptr) {
			PT = Parameters::root;
		}
		cout << "Group name: " << NS << "\n  " << popSize << " organisms with " << PT->lookupString("GENOME-genomeType") << "<" << PT->lookupString("GENOME-sitesType") << "> genomes and " << PT->lookupString("BRAIN-brainType") << " brains.\n  Optimizer: "
				<< PT->lookupString("OPTIMIZER-optimizer") << "\n  Archivist: " << PT->lookupString("ARCHIVIST-outputMethod") << endl;
		cout << endl;
		// end of report
	}

	// this versVector2d<int> worldgrid(10, 10);ion of the code requires a default group. This is change in the future.
	string defaultGroup = "default";
	if (groups.find(defaultGroup) == groups.end()) {
		cout << "Group " << defaultGroup << " not found in groups.\nExiting." << endl;
		exit(1);
	}

	//exit(1);
	// in run mode we evolve organsims
	if (Global::modePL->lookup() == "run") {
		//////////////////
		// run mode - evolution loop
		//////////////////

		while (!groups[defaultGroup]->archivist->finished) {
			// evaluate population in world.
			world->evaluate(groups, false, false, AbstractWorld::debugPL->lookup());  // evaluate each organism in the population using a World
			//cout << "  evaluation done" << endl;

			// save data, update memory and delete any unneeded data;
			if (!groups[defaultGroup]->archivist->finished) {
				groups[defaultGroup]->archive();
				//cout << "  archive done" << endl;

				// move forward in time!

				// update the population (reproduction and death)
				Global::update++;
				groups[defaultGroup]->optimize();
				//cout << "  optimize done\n";
			}

			cout << "update: " << Global::update - 1 << "   maxFitness: " << groups[defaultGroup]->optimizer->maxScore << "" << endl;
		}

		// the run is finished... flush any data that has not been output yet
		groups[defaultGroup]->archive(1);

	}

	// in visualize mode we load in organisms (usually genomes) and rerun them to collect addtional data
	// this data may relate to the world, brain... or other things...
	// this section of code is very rough...
	else if (Global::modePL->lookup() == "visualize") {
		//////////////////
		// visualize mode
		//////////////////
		cout << "  You are running MABE in visualize mode." << endl << endl;
		vector<shared_ptr<AbstractGenome>> testGenomes;
		cout << "loading Genomes... ";
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
						if (g->dataMap.GetIntVector("ID")[0] == ID) {
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

		int IDcounter = -1;

		set<int> inUseIDs;

		for (auto g : testGenomes) {
			auto newOrg = make_shared<Organism>(groups[defaultGroup]->population[0], g);
			if (inUseIDs.find(g->dataMap.GetIntVector("ID")[0]) != inUseIDs.end()) {
				newOrg->ID = IDcounter--; // assign a negative ID to indicate that this is a copy
			} else {
				newOrg->ID = g->dataMap.GetAverage("ID"); // this is the first copy of this genome, so it get's it's own value!
			}
			inUseIDs.insert(newOrg->ID);

			//newOrg->brain->setRecordActivity(true);
			//newOrg->brain->setRecordFileName("wireBrain.run");
			testPopulation.push_back(newOrg);  // add a new org to population using progenitors template and a new random genome
		}

		shared_ptr<Group> testGroup = make_shared<Group>(testPopulation, groups[defaultGroup]->optimizer, groups[defaultGroup]->archivist);

		groups.clear();
		groups["default"] = testGroup;
		world->evaluate(groups, false, true, false);

		for (auto o : testGroup->population) {
			cout << "  organism with ID: " << o->genome->dataMap.GetAverage("ID") << " generated score: " << o->dataMap.GetAverage("score") << " " << endl;
		}
	} else {
		cout << "\n\nERROR: Unrecognized mode set in configuration!\n  \"" << Global::modePL->lookup() << "\" is not defined.\n\nExiting.\n" << endl;
		exit(1);
	}

	return 0;
}
