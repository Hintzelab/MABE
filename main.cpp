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
#include "Utilities/MTree.h"

#include "modules.h"

using namespace std;

int main(int argc, const char * argv[]) {
	cout << "\n\n" << "\tMM   MM      A       BBBBBB    EEEEEE\n" << "\tMMM MMM     AAA      BB   BB   EE\n" << "\tMMMMMMM    AA AA     BBBBBB    EEEEEE\n" << "\tMM M MM   AAAAAAA    BB   BB   EE\n" << "\tMM   MM  AA     AA   BBBBBB    EEEEEE\n" << "\n"
		<< "\tModular    Agent      Based    Evolver\n\n\n\thttps://github.com/ahnt/MABE\n\n" << endl;
	cout << "\tfor help run MABE with the \"-h\" flag (i.e. ./MABE -h)." << endl << endl;

	configureDefaultsAndDocumentation(); // sets up values from modules.h
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
	}
	else {
		Random::getCommonGenerator().seed(Global::randomSeedPL->lookup());
		cout << "Using Random Seed: " << Global::randomSeedPL->lookup() << endl;
	}

	// make world uses WORLD-worldType to determine type of world
	auto world = makeWorld();
	map<string, shared_ptr<Group>> groups;
	shared_ptr<ParametersTable> PT;

	string NS;

	unordered_map<string, unordered_set<string>> worldRequirements = world->requiredGroups();

	//for (auto r : worldRequirements) {
	//	cout << r.first << "   ";
	//	for (auto e : r.second) {
	//		cout << e << "   " << endl;
	//	}
	//	cout << endl;
	//}
	//cout << endl;

	// for each name space in the GLOBAL-groups create a group. if GLOBAL-groups is empty, create "default" group.
	for (auto groupInfo : worldRequirements) {
		cout << endl;
		NS = groupInfo.first;
		if (NS == "root") {
			PT = nullptr;  //nullptr is the same as Parameters::root;
		}
		else {
			NS = NS + "::";
			PT = Parameters::root->getTable(NS); // create (or get a pointer to) a new parameters table with this name
		}
		if (PT != nullptr) {
			cout << "Building group with name space: " << PT->getTableNameSpace() << endl;
		}
		else {
			cout << "Building group with \"root\" name space" << endl;
		}

		Global::update = -1;  // before there was time, there was a progenitor - set time to -1 so progenitor (the root organism) will have birth time -1

		// create an optimizer of type defined by OPTIMIZER-optimizer
		shared_ptr<AbstractOptimizer> optimizer = makeOptimizer(PT);

		unordered_set <string> brainNames;
		unordered_set <string> genomeNames;

		unordered_map<string, shared_ptr<AbstractBrain>> templateBrains; // templates for brains in organisms in this group
		unordered_map<string, shared_ptr<AbstractGenome>> templateGenomes; // templates for genomes in organisms in this group

		unordered_set<string> strSet; // temporary holder

		genomeNames = optimizer->requiredGenomes(); // get genome names from optimizer
		map<string, int> brainIns;
		map<string, int> brainOuts;

		for (auto s : groupInfo.second) {
			if (s.size() <= 2) {
				cout << "\n\nwhile converting world requirements in \"" + NS + "\" group, found requirement \"" + s + "\".\n requirements must start with B: (brain) or G: (genome) and follow with a name!\nexiting." << endl;
				exit(1);
			}
			if (s[0] == 'G' && s[1] == ':') {
				genomeNames.insert(s.substr(2));
			}
			else if (s[0] == 'B' && s[1] == ':') {
				string workingString = s.substr(2);
				string brainName = workingString.substr(0, workingString.find(','));
				brainNames.insert(brainName);
				workingString = workingString.substr(workingString.find(',')+1);
				int ins, outs;
				stringToValue(workingString.substr(0, workingString.find(',')), ins);
				stringToValue(workingString.substr(workingString.find(',')+1), outs);
				brainIns[brainName] = ins;
				brainOuts[brainName] = outs;
			}
			else {
				cout << "\n\nwhile converting world requirements in \"" + NS + "\" group, found requirement \"" + s + "\".\n requirements must start with B: (brain) or G: (genome)!\nexiting." << endl;
				exit(1);
			}
		}

		cout << endl<< " building brains..." << endl;

		for (string brainName : brainNames) {
			cout << "  found brain: " << brainName << endl;
			shared_ptr<ParametersTable> This_PT;
			if (brainName == "") {
				cout << "\n\nfound empty brain name, this is not allowed. Exiting..." << endl;
				exit(1);
			}
			if (brainName == "root") { // if brain name is "root" then this brain's namespace will be at the root level (i.e. ignore group name)
				This_PT = nullptr;
			}
			else { // this brain is not at root
				if (brainName.size() > 6 && brainName.substr(0, 7) == "GROUP::") {
					// This brain's will be in the name space of this group (which may be empty),
					// this brain can exist in more then one group and will then have a unique local name space for each group
					string shortBrainName = brainName.substr(7);
					if (shortBrainName == "") { // brain does not have it's own name space
						This_PT = PT; // ... and will use the group name space
					}
					else { // this brain has it's own name space and will be a child of the group name space
						if (NS == "root") { // this group is in root namespace, so name space for brain is just shortBrainName::
							This_PT = Parameters::root->getTable(shortBrainName + "::");
						}
						else { // this group has a name space (NS) and this brain will have a name space in that name space
							This_PT = Parameters::root->getTable(NS + shortBrainName + "::");
						}
					}
				} // this brain name is not root and does not start with "GROUP::" so it will have it's own name space, at the root level 
				else {
					This_PT = Parameters::root->getTable(brainName + "::");
				}
			}
			if (This_PT != nullptr) {
				cout << "    ... building a " << This_PT->lookupString("BRAIN-brainType") << " brain using " << This_PT->getTableNameSpace() << " name space." << endl;
			}
			else {
				cout << "    ... building a " << Parameters::root->lookupString("BRAIN-brainType") << " brain using \"root\" name space" << endl;
			}
			//templateBrains[brainName] = makeTemplateBrain(world->requiredInputs(brainName), world->requiredOutputs(brainName), This_PT);
			templateBrains[brainName] = makeTemplateBrain(brainIns[brainName], brainOuts[brainName], This_PT);
			strSet = templateBrains[brainName]->requiredGenomes();
			if (strSet.size() > 0) {
				cout << "    ..... this brain requires genomes: ";
				for (string g : strSet) {
					cout << g << "  ";
				}
				cout << endl;
			}
			genomeNames.insert(strSet.begin(), strSet.end()); // add optimizers required genomes
		}

		cout << endl << " building genomes..." << endl;

		for (string genomeName : genomeNames) {
			cout << "  found genome: " << genomeName << endl;
			shared_ptr<ParametersTable> This_PT;
			if (genomeName == "") {
				cout << "\n\nfound empty genome name, this is not allowed. Exiting..." << endl;
				exit(1);
			}
			if (genomeName == "root") { // if brain name is "root" then this brain's namespace will be at the root level (i.e. ignore group name)
				This_PT = nullptr;
			}
			else { // this brain is not at root
				if (genomeName.size() > 6 && genomeName.substr(0, 7) == "GROUP::") {
					// This brain's will be in the name space of this group (which may be empty),
					// this brain can exist in more then one group and will then have a unique local name space for each group
					string shortGenomeName = genomeName.substr(7);
					if (shortGenomeName == "") { // brain does not have it's own name space
						This_PT = PT; // ... and will use the group name space
					}
					else { // this brain has it's own name space and will be a child of the group name space
						if (NS == "root") { // this group is in root namespace, so name space for brain is just shortBrainName::
							This_PT = Parameters::root->getTable(shortGenomeName + "::");
						}
						else { // this group has a name space (NS) and this brain will have a name space in that name space
							This_PT = Parameters::root->getTable(NS + shortGenomeName + "::");
						}
					}
				} // this brain name is not root and does not start with "GROUP::" so it will have it's own name space, at the root level 
				else {
					This_PT = Parameters::root->getTable(genomeName + "::");
				}
			}
			if (This_PT != nullptr) {
				cout << "    ... building a " << This_PT->lookupString("GENOME-genomeType") << " genome using " << This_PT->getTableNameSpace() << " name space." << endl;
			}
			else {
				cout << "    ... building a " << Parameters::root->lookupString("GENOME-genomeType") << " genome using \"root\" name space" << endl;
			}

			templateGenomes[genomeName] = makeTemplateGenome(This_PT);
		}



		//cout << "\n  Required Brains in this group: ";
		//for (auto b : templateBrains) {
		//	cout << b.first << "  ";
		//}
		//cout << endl;

		//cout << "\n  Required Genomes in this group: ";
		//for (auto g : templateGenomes) {
		//	cout << g.first << "  ";
		//}
		//cout << endl;

		// make a organism with a templateGenomes and templateBrains - progenitor serves as an ancestor to all and a template organism
		shared_ptr<Organism> progenitor = make_shared<Organism>(templateGenomes, templateBrains, PT);
		
		Global::update = 0;  // the beginning of time - now we construct the first population

		int popSize = (PT == nullptr) ? Global::popSizePL->lookup() : PT->lookupInt("GLOBAL-popSize");
		vector<shared_ptr<Organism>> population;
		// add popSize organisms which look like progenitor to population
		shared_ptr<Organism> newOrg;
		for (int i = 0; i < popSize; i++) {
			// make a new genome like the template genome
			unordered_map<string, shared_ptr<AbstractGenome>>  newGenomes;
			unordered_map<string, shared_ptr<AbstractBrain>>  newBrains;
			for (auto genome : templateGenomes) {
				newGenomes[genome.first] = genome.second->makeLike();
			}
			for (auto brain : templateBrains) {
				brain.second->initalizeGenomes(newGenomes);
				newBrains[brain.first] = brain.second->makeBrain(newGenomes);
			}
			// create new organism using progenitor as template (i.e. to define brains) and the new genomes

			//// must address initalizGenome in case of multi brains!!
			
			newOrg = make_shared<Organism>(progenitor,newGenomes,newBrains,PT);

			// add new organism to population
			population.push_back(newOrg);
		}

		// the progenitor has served it's purpose. Killing an organsim is important as it allows for cleanup.
		progenitor->kill();

		// popFileColumns holds a list of data titles which various modules indicate are interesting/should be tracked and which are averageable
		// ** popFileColumns define what will appear in the pop.csv file **
		// the following code asks world, genomes and brains for ave file columns
		vector<string> popFileColumns;
		popFileColumns.clear();
		popFileColumns.push_back("update");
		popFileColumns.insert(popFileColumns.end(), world->popFileColumns.begin(), world->popFileColumns.end());
		popFileColumns.insert(popFileColumns.end(), optimizer->popFileColumns.begin(), optimizer->popFileColumns.end());

		for (auto genome : population[0]->genomes) {
			for (auto c : genome.second->popFileColumns) {
				(genome.first == "root") ? popFileColumns.push_back(c) : popFileColumns.push_back(genome.first + "_" + c);
				//(genome.first == "root") ? popFileColumns.push_back(c) : popFileColumns.push_back(genome.second->PT->getTableNameSpace() + "_" + c);
			}
		}
		for (auto brain : population[0]->brains) {
			for (auto c : brain.second->popFileColumns) {
				(brain.first == "root") ? popFileColumns.push_back(c) : popFileColumns.push_back(brain.first + "_" + c);
				//(brain.first == "root") ? popFileColumns.push_back(c) : popFileColumns.push_back(brain.second->PT->getTableNameSpace() + "_" + c);
				//cout << brain.first << "   " << c << endl;
			}
		}

		// create an archivist of type determined by ARCHIVIST-outputMethod
		shared_ptr<DefaultArchivist> archivist = makeArchivist(popFileColumns, optimizer->optimizeFormula, PT);

		// create a new group with the new population, optimizer and archivist and place this group in the map groups
		groups[groupInfo.first] = make_shared<Group>(population, optimizer, archivist);

		//report on what was just built
		if (PT == nullptr) {
			PT = Parameters::root;
		}
		cout << "\nFinished Building Group: " << groupInfo.first << "   Group name space: " << NS << "\n  population size: " << popSize << "     Optimizer: "
				<< PT->lookupString("OPTIMIZER-optimizer") << "     Archivist: " << PT->lookupString("ARCHIVIST-outputMethod") << endl;
		cout << endl;
		// end of report
	}

	// this versVector2d<int> worldgrid(10, 10);ion of the code requires a default group. This is change in the future.
	//string defaultGroup = "cat";// "root";
	//if (groups.find(defaultGroup) == groups.end()) {
	//	cout << "Group " << defaultGroup << " not found in groups.\nExiting." << endl;
	//	exit(1);
	//}

	// exit(1);
	// in run mode we evolve organsims
	bool done = false;
	if (Global::modePL->lookup() == "run") {
		//////////////////
		// run mode - evolution loop
		//////////////////

		// first archive initial populations
		//world->evaluate(groups, false, false, AbstractWorld::debugPL->lookup());  // evaluate each organism in the population using a World
		//for (auto group : groups) {
		//	if (!group.second->archivist->finished) {
		//		group.second->archive();
		//		if (!group.second->archivist->finished) {
		//			done = false;
		//		}
		//	}
		//}

		while (!done){//!groups[defaultGroup]->archivist->finished) {
			
			

			// evaluate population in world.
			//cout << "  start evaluation" << endl;
			world->evaluate(groups, false, false, AbstractWorld::debugPL->lookup());  // evaluate each organism in the population using a World
			//cout << "  evaluation done" << endl;

			// save data, update memory and delete any unneeded data;
			cout << "update: " << Global::update << "   ";
			
			// move forward in time!
			Global::update++; // advance time to create new population
			done = true;
			for (auto group : groups) {
				if (!group.second->archivist->finished) {

					// update the population (reproduction and death)
					vector<shared_ptr<Organism>> nextPopulation = group.second->optimize();

					Global::update--; // back up time to archive last generation
					group.second->archive();
					Global::update++; // jump forward
					//cout << "  archive done" << endl;
					if (!group.second->archivist->finished) {
						done = false;
					}

					for (auto org : group.second->population) {
						if (find(nextPopulation.begin(),nextPopulation.end(),org) == nextPopulation.end()) {  // if this org did not survive to the next generation, it must be killed (sad.)
							org->kill();
						}
					}
					group.second->population = nextPopulation;
					//cout << "  optimize done" << endl;
				}
			}
			cout << endl;

		}

		// the run is finished... flush any data that has not been output yet
		for (auto group : groups) {
			group.second->archive(1);
		}

	}

	//// in visualize mode we load in organisms (usually genomes) and rerun them to collect addtional data
	//// this data may relate to the world, brain... or other things...
	//// this section of code is very rough...
	//else if (Global::modePL->lookup() == "visualize") {
	//	//////////////////
	//	// visualize mode
	//	//////////////////
	//	cout << "  You are running MABE in visualize mode." << endl << endl;
	//	vector<shared_ptr<AbstractGenome>> testGenomes;
	//	cout << "loading Genomes... ";
	//	groups[defaultGroup]->population[0]->genome->loadGenomeFile(Global::visualizePopulationFilePL->lookup(), testGenomes);

	//	int num_genomes = (int) testGenomes.size();

	//	vector<int> IDs;
	//	convertCSVListToVector(Global::visualizeOrgIDPL->lookup(), IDs);

	//	vector<shared_ptr<Organism>> testPopulation;

	//	bool padPopulation = false;

	//	if (IDs[0] == -1) { // visualize last
	//		shared_ptr<AbstractGenome> temp = testGenomes[(int) testGenomes.size() - 1];
	//		testGenomes.clear();
	//		testGenomes.push_back(temp);
	//	} else if (IDs[0] == -2 && IDs.size() == 1) { // visualize population
	//		padPopulation = true;
	//	} else { // visualize given ID(s)
	//		int foundCount = 0;
	//		vector<shared_ptr<AbstractGenome>> subsetGenomes;
	//		for (auto ID : IDs) {
	//			if (ID != -2) {
	//				bool found = false;
	//				for (auto g : testGenomes) {
	//					if (g->dataMap.GetIntVector("ID")[0] == ID) {
	//						subsetGenomes.push_back(g);
	//						foundCount++;
	//						found = true;
	//					}
	//				}
	//				if (!found) {
	//					cout << "ERROR: in visualize mode, can not find genome with ID " << ID << " in file: " << Global::visualizePopulationFilePL->lookup() << ".\n  Exiting." << endl;
	//					exit(1);
	//				}
	//			} else {
	//				padPopulation = true;
	//				num_genomes--;
	//				foundCount++;
	//			}
	//		}
	//		if (foundCount < (int) IDs.size()) {
	//			cout << "WARRNING: in visualize mode " << (int) IDs.size() - foundCount << " genomes specified in Global::visualizeOrgID could not be found." << endl;
	//		}
	//		testGenomes = subsetGenomes;
	//	}

	//	if (padPopulation) {
	//		int index = 0;
	//		if ((int) testGenomes.size() < Global::popSizePL->lookup()) {
	//			cout << "  Population size is larger then the number of genomes in the file. Padding population with " << Global::popSizePL->lookup() - (int) testGenomes.size() << " extra copies." << endl;
	//			while ((int) testGenomes.size() < Global::popSizePL->lookup()) {
	//				testGenomes.push_back(testGenomes[index]);
	//				index++;
	//				cout << index << endl;
	//				if (index >= num_genomes) {
	//					index = 0;
	//				}
	//			}
	//		}
	//		if ((int) testGenomes.size() > Global::popSizePL->lookup()) {
	//			cout << "  Population size is smaller then the number of genomes in the file. deleting genomes from head of population." << endl;
	//			testGenomes.erase(testGenomes.begin(), testGenomes.begin() + ((int) testGenomes.size() - Global::popSizePL->lookup()));
	//		}
	//	}

	//	int IDcounter = -1;

	//	set<int> inUseIDs;

	//	for (auto g : testGenomes) {
	//		auto newOrg = make_shared<Organism>(groups[defaultGroup]->population[0], g,nullptr);
	//		if (inUseIDs.find(g->dataMap.GetIntVector("ID")[0]) != inUseIDs.end()) {
	//			newOrg->ID = IDcounter--; // assign a negative ID to indicate that this is a copy
	//		} else {
	//			newOrg->ID = (int)g->dataMap.GetAverage("ID"); // this is the first copy of this genome, so it get's it's own value!
	//		}
	//		inUseIDs.insert(newOrg->ID);

	//		//newOrg->brain->setRecordActivity(true);
	//		//newOrg->brain->setRecordFileName("wireBrain.run");
	//		testPopulation.push_back(newOrg);  // add a new org to population using progenitors template and a new random genome
	//	}

	//	shared_ptr<Group> testGroup = make_shared<Group>(testPopulation, groups[defaultGroup]->optimizer, groups[defaultGroup]->archivist);

	//	groups.clear();
	//	groups["root"] = testGroup;
	//	world->evaluate(groups, false, true, false);

	//	for (auto o : testGroup->population) {
	//		cout << "  organism with ID: " << o->genome->dataMap.GetAverage("ID") << " generated score: " << o->dataMap.GetAverage("score") << " " << endl;
	//	}
	//} else {
	//	cout << "\n\nERROR: Unrecognized mode set in configuration!\n  \"" << Global::modePL->lookup() << "\" is not defined.\n\nExiting.\n" << endl;
	//	exit(1);
	//}

	return 0;
}
