//
//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

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
//#include "Utilities/WorldUtilities.h"
#include "Utilities/MTree.h"

#include "modules.h"
#if defined(__MINGW32__)
#include <windows.h> /// for getting PID, for proper RNG for MinGW
#endif

using namespace std;

int main(int argc, const char * argv[]) {


	
	const string logo =
R"raw(


	MM   MM      A       BBBBBB    EEEEEE
	MMM MMM     AAA      BB   BB   EE
	MMMMMMM    AA AA     BBBBBB    EEEEEE
	MM M MM   AAAAAAA    BB   BB   EE
	MM   MM  AA     AA   BBBBBB    EEEEEE

	Modular    Agent      Based    Evolver


	https://github.com/HintzeLab/MABE


	for help run MABE with the "-h" flag (i.e. ./MABE -h).

)raw";
	cout << logo;

	configureDefaultsAndDocumentation(); // sets up values from modules.h
	bool saveFiles = Parameters::initializeParameters(argc, argv);  // loads command line and configFile values into registered parameters

	// also writes out a settings files if requested
	if (saveFiles) { // if saveFiles (save settings files) is set
		int maxLineLength = Global::maxLineLengthPL->get();
		int commentIndent = Global::commentIndentPL->get();

		Parameters::saveSettingsFiles(maxLineLength, commentIndent, { "*" }, { { "settings_organism.cfg", { "GATE*", "GENOME*", "BRAIN*" } }, { "settings_world.cfg", { "WORLD*" } }, { "settings.cfg", { "" } } });
		cout << "Saving settings files and exiting." << endl;
		exit(0);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	/// IF YOU WANT TO HACK PARAMETERS, DO IT HERE! /////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////
	///
	/// Parameters::root->setExistingParameter("BRAIN-brainType", "LSTM");
	///
	/////////////////////////////////////////////////////////////////////////////////////////////

	// outputDirectory must exist. If outputDirectory does not exist, no error will occur, but no data will be written. THIS SHOULD BE ADDRESSED ONE DAY!
	FileManager::outputDirectory = Global::outputDirectoryPL->get();

	// set up random number generator
	if (Global::randomSeedPL->get() == -1) {
		random_device rd;
        /// random_device is not implemented for MinGW on windows (it's like the old rand())
        /// so we need to seed it with entropy
        #if defined(__MINGW32__)
        DWORD temp = GetCurrentProcessId();
        #else
        int temp = rd();
        #endif
		Random::getCommonGenerator().seed(temp);
		cout << "Generating Random Seed\n  " << temp << endl;
	}
	else {
		Random::getCommonGenerator().seed(Global::randomSeedPL->get());
		cout << "Using Random Seed: " << Global::randomSeedPL->get() << endl;
	}

	// make world uses WORLD-worldType to determine type of world
	auto world = makeWorld(Parameters::root);
	map<string, shared_ptr<Group>> groups;
	shared_ptr<ParametersTable> PT;

	string NS;

	unordered_map<string, unordered_set<string>> worldRequirements = world->requiredGroups();
	// for each name space in the GLOBAL-groups create a group. if GLOBAL-groups is empty, create "default" group.
	for (auto groupInfo : worldRequirements) {
		cout << endl;
		NS = groupInfo.first;
		if (NS == "root::") {
			PT = Parameters::root;
		}
		else {
			PT = Parameters::root->getTable(NS); // create (or get a pointer to) a new parameters table with this name
		}
		cout << "Building group with name space: " << groupInfo.first << endl;

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
			// for each group required by world, determine the brains and genomes required by the world. Also determine if any
			// of the required brains require genomes. If two requirements have the same name, this indicated that only a single
			// instance should be created and at any time that this name is accessed, this will refer to this single instance
			// (i.e. if the world required "bodyGenome" and the brain required "genome" and both were given the same name, then
			// when the world looked up "bodyGenome" and when the brain looked up "genome" they would be looking at the same object.
			if (s.size() <= 4) {
				cout << "\n\nwhile converting world requirements in \"" + NS + "\" group, found requirement \"" + s + "\".\n requirements must start with B: (brain) or G: (genome), followed with a name and end with \"::\".\nexiting." << endl;
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
			if (brainName == "root::") {
				This_PT = Parameters::root;
			}
			else {
				This_PT = Parameters::root->getTable(brainName);
			}

			cout << "    ... building a " << This_PT->lookupString("BRAIN-brainType") << " brain using " << brainName << " name space." << endl;
			templateBrains[brainName] = makeTemplateBrain(brainIns[brainName], brainOuts[brainName], This_PT);
			strSet = templateBrains[brainName]->requiredGenomes();
			if (strSet.size() > 0) {
				cout << "    ..... this brain requires genomes: ";
				for (string g : strSet) {
					cout << g << "  ";
				}
				cout << endl;
			}
			genomeNames.insert(strSet.begin(), strSet.end()); // add this brains required genomes to genome names
		}

		cout << endl << " building genomes..." << endl;

		for (string genomeName : genomeNames) {
			cout << "  found genome: " << genomeName << endl;
			shared_ptr<ParametersTable> This_PT;
			if (genomeName == "") {
				cout << "\n\nfound empty genome name, this is not allowed. Exiting..." << endl;
				exit(1);
			}
			if (genomeName == "root::") {
				This_PT = Parameters::root;
			}
			else { // this genome is not at root
				This_PT = Parameters::root->getTable(genomeName);
			}
			cout << "    ... building a " << This_PT->lookupString("GENOME-genomeType") << " genome using " << genomeName << " name space." << endl;
			templateGenomes[genomeName] = makeTemplateGenome(This_PT);
		}

		// make a organism with a templateGenomes and templateBrains - progenitor serves as an ancestor to all and a template organism
		shared_ptr<Organism> progenitor = make_shared<Organism>(templateGenomes, templateBrains, PT);

		int popSize = Global::popSizePL->get(PT);

		vector<shared_ptr<Organism>> population;

		if (Global::modePL->get(Parameters::root) == "run") {

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
					brain.second->initializeGenomes(newGenomes);
					newBrains[brain.first] = brain.second->makeBrain(newGenomes);
				}
				// create new organism using progenitor as template (i.e. to define brains) and the new genomes

				//// must address initalizGenome in case of multi brains!!
				newOrg = make_shared<Organism>(progenitor, newGenomes, newBrains, PT);

				// add new organism to population
				population.push_back(newOrg);
			}

		} else { 
			// we are in visualize or analyze mode so we don't need to make the population
		}
		// popFileColumns holds a list of data titles which various modules indicate are interesting/should be tracked and which are averageable
		// ** popFileColumns define what will appear in the pop.csv file **
		// the following code asks world, genomes and brains for ave file columns
		vector<string> popFileColumns;
		popFileColumns.clear();
		popFileColumns.push_back("update");
		popFileColumns.insert(popFileColumns.end(), world->popFileColumns.begin(), world->popFileColumns.end());
		popFileColumns.insert(popFileColumns.end(), optimizer->popFileColumns.begin(), optimizer->popFileColumns.end());
		for (auto genome : progenitor->genomes) {
			for (auto c : genome.second->popFileColumns) {
				(genome.first == "root::") ? popFileColumns.push_back(c) : popFileColumns.push_back(genome.first + c);
			}
		}
		for (auto brain : progenitor->brains) {
			for (auto c : brain.second->popFileColumns) {
				(brain.first == "root::") ? popFileColumns.push_back(c) : popFileColumns.push_back(brain.first + c);
			}
		}

		// create an archivist of type determined by ARCHIVIST-outputMethod
		shared_ptr<DefaultArchivist> archivist = makeArchivist(popFileColumns, optimizer->optimizeFormula, PT, (groupInfo.first == "root::") ? "" : groupInfo.first);

		// create a new group with the new population, optimizer and archivist and place this group in the map groups
		groups[groupInfo.first] = make_shared<Group>(population, optimizer, archivist);

		groups[groupInfo.first]->templateOrg = progenitor->makeCopy();
		// the progenitor has served it's purpose. Killing an organsim is important as it allows for cleanup.
		progenitor->kill();

		//report on what was just built
		if (Global::modePL->get() == "run") {

			cout << "\nFinished Building Group: " << groupInfo.first << "   Group name space: " << NS << "\n  population size: " << popSize << "     Optimizer: "
				<< PT->lookupString("OPTIMIZER-optimizer") << "     Archivist: " << PT->lookupString("ARCHIVIST-outputMethod") << endl;
			cout << endl;
		}
		else {
			cout << "\nFinished Building Group: " << groupInfo.first << "   Group name space: " << NS << "\n  population size: 0 (not in run mode)     Optimizer: "
				<< PT->lookupString("OPTIMIZER-optimizer") << "     Archivist: " << PT->lookupString("ARCHIVIST-outputMethod") << endl;
			cout << endl;
		}
		// end of report
	}

	Global::update = 0;  // the beginning of time - now we construct the first population

	// in run mode we evolve organsims
	bool done = false;
	
	if (Global::modePL->get() == "run") {
		////////////////////////////////////////////////////////////////////////////////////
		// run mode - evolution loop
		////////////////////////////////////////////////////////////////////////////////////

		while (!done){//!groups[defaultGroup]->archivist->finished) {
			world->evaluate(groups, false, false, AbstractWorld::debugPL->get());  // evaluate each organism in the population using a World
			cout << "update: " << Global::update << "   " << flush;
			done = true; // until we find out otherwise, assume we are done.
			for (auto group : groups) {
				if (!group.second->archivist->finished) {
					group.second->optimize(); // create the next updates population
					group.second->archive(); // save data, update memory and delete unneeded data;
					if (!group.second->archivist->finished) {
						done = false; // if any groups archivist says we are not done, then we are not done
					}
					group.second->optimizer->cleanup(group.second->population);
				}
			}
			cout << endl;
			Global::update++; // advance time to create new population(s)
		}

		// the run is finished... flush any data that has not been output yet
		for (auto group : groups) {
			group.second->archive(1);
		}
	}

	//// in visualize mode we load in organisms (usually genomes) and rerun them to collect addtional data
	//// this data may relate to the world, brain... or other things...
	//// this section of code is very rough...
	else if (Global::modePL->get() == "visualize") {
		////////////////////////////////////////////////////////////////////////////////////
		// visualize mode
		////////////////////////////////////////////////////////////////////////////////////
		cout << "\n  You are running MABE in visualize mode." << endl << endl;


		//which orgs do I need to make?
		// lets make all orgs required by world.
		unordered_map<int, unordered_map<string, string>> data;
		string fileName = Global::visualizePopulationFilePL->get();
		cout << "loading file " << fileName << endl;
		string indexName = "ID";
		vector<int> IDs;
		convertCSVListToVector(Global::visualizeOrgIDPL->get(), IDs);
		int orgID = IDs[0];

		loadIndexedCSVFile(fileName, data, indexName);
		unordered_map<string, string> orgData = data[orgID];

		for (auto group : groups) {
			shared_ptr<Organism> newOrg = group.second->templateOrg->makeCopy();
			for (auto genome : newOrg->genomes) {
				string name = genome.first;
				genome.second->deserialize(genome.second->PT, orgData, name);
			}
			for (auto brain : newOrg->brains) {
				//cout << brain.first << "  before brain: " << brain.second->description() << endl;
				newOrg->brains[brain.first] = brain.second->makeBrain(newOrg->genomes);
				//cout << brain.first << "  after brain: " << brain.second->description() << endl;
				string name = brain.first;
				brain.second->deserialize(brain.second->PT, orgData, name);
				//cout << brain.first << "  after brain: " << brain.second->description() << endl;
			}
			group.second->population.push_back(newOrg);
		}

		for (auto group : groups) {
			//cout << "GROUP: " << group.first << endl;
			for (auto genome : group.second->population[0]->genomes) {
				cout << genome.first << " -- " << endl;
				genome.second->printGenome();
				cout << endl;
			}
			for (auto brain : group.second->population[0]->brains) {
				//cout << group.second->population[0]->brains.size() << "  SIZE" << endl;
				//cout << "brain: " << brain.first << " -- " << brain.second->description() << endl;
			}
		}

		world->evaluate(groups, 0, 1, 0);
		cout << "org with ID: " << orgData["ID"] << "  generated score: " << groups["root::"]->population[0]->dataMap.getAverage("score") << endl;

	}

	return 0;
}

