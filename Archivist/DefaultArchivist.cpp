//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "DefaultArchivist.h"
using namespace std;

////// ARCHIVIST-outputMethod is actually set by Modules.h //////
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_outputMethodStrPL = Parameters::register_parameter("ARCHIVIST-outputMethod", (string) "This_string_is_set_by_modules.h",
	"This_string_is_set_by_modules.h");  // string parameter for outputMethod;
////// ARCHIVIST-outputMethod is actually set by Modules.h //////

shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_realtimeSequencePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-realtimeSequence", (string) ":10",
	"How often to write to realtime data files. (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
shared_ptr<ParameterLink<string>> DefaultArchivist::SS_Arch_dataSequencePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-snapshotDataSequence", (string) ":100",
	"How often to save a realtime snapshot data file. (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
shared_ptr<ParameterLink<string>> DefaultArchivist::SS_Arch_organismSequencePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-snapshotOrganismSequence", (string) ":1000",
	"How often to save a realtime snapshot genome file. (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");

shared_ptr<ParameterLink<bool>> DefaultArchivist::Arch_writePopFilePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-writePopFile", true, "Save data to average file?");
shared_ptr<ParameterLink<bool>> DefaultArchivist::Arch_writeMaxFilePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-writeMaxFile", true, "Save data to Max file?");
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_PopFileNamePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-popFileName", (string) "pop.csv", "name of population data file (saves population averages)");
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_MaxFileNamePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-maxFileName", (string) "max.csv", "name of max file (saves data on organism with max \"score\" as determined by Optimizer)");
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_DefaultPopFileColumnNamesPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-popFileColumns", (string) "[]",
	"data to be saved into average file (must be values that can generate an average). If empty, MABE will try to figure it out");

shared_ptr<ParameterLink<string>> DefaultArchivist::SS_Arch_DataFilePrefixPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-snapshotDataFilePrefix", (string) "snapshotData", "prefix for name of snapshot data file");
shared_ptr<ParameterLink<string>> DefaultArchivist::SS_Arch_OrganismsFilePrefixPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-snapshotOrganismFilePrefix", (string) "snapshotOrganisms", "prefix for name of snapshot organism file");
shared_ptr<ParameterLink<bool>> DefaultArchivist::SS_Arch_writeDataFilesPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-writeSnapshotDataFiles", false,
	"if true, snapshot data files will be written (with all non genome data for entire population)");
shared_ptr<ParameterLink<bool>> DefaultArchivist::SS_Arch_writeOrganismsFilesPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-writeSnapshotOrganismsFiles", false, "if true, snapshot organism files will be written (with all organisms for entire population)");

DefaultArchivist::DefaultArchivist(shared_ptr<ParametersTable> _PT, string _groupPrefix) :
	PT(_PT), groupPrefix(_groupPrefix) {

	writePopFile = (PT == nullptr) ? Arch_writePopFilePL->lookup() : PT->lookupBool("ARCHIVIST_DEFAULT-writePopFile");
	writeMaxFile = (PT == nullptr) ? Arch_writeMaxFilePL->lookup() : PT->lookupBool("ARCHIVIST_DEFAULT-writeMaxFile");


	PopFileName = (PT == nullptr) ? Arch_PopFileNamePL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-popFileName");
	PopFileName = (groupPrefix == "") ? PopFileName : groupPrefix + "__" + PopFileName;

	MaxFileName = (PT == nullptr) ? Arch_MaxFileNamePL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-maxFileName");
	MaxFileName = (groupPrefix == "") ? MaxFileName : groupPrefix + "__" + MaxFileName;

	PopFileColumnNames = (PT == nullptr) ? Arch_DefaultPopFileColumnNamesPL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-popFileColumns");

	DataFilePrefix = (PT == nullptr) ? SS_Arch_DataFilePrefixPL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-snapshotDataFilePrefix");
	DataFilePrefix = (groupPrefix == "") ? DataFilePrefix : groupPrefix + "__" + DataFilePrefix;

	OrganismFilePrefix = (PT == nullptr) ? SS_Arch_OrganismsFilePrefixPL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-snapshotOrganismFilePrefix");
	OrganismFilePrefix = (groupPrefix == "") ? OrganismFilePrefix : groupPrefix + "__" + OrganismFilePrefix;

	writeSnapshotDataFiles = (PT == nullptr) ? SS_Arch_writeDataFilesPL->lookup() : PT->lookupBool("ARCHIVIST_DEFAULT-writeSnapshotDataFiles");
	writeSnapshotGenomeFiles = (PT == nullptr) ? SS_Arch_writeOrganismsFilesPL->lookup() : PT->lookupBool("ARCHIVIST_DEFAULT-writeSnapshotOrganismsFiles");

	realtimeSequence.push_back(0);
	realtimeDataSequence.push_back(0);
	realtimeOrganismSequence.push_back(0);

	if (writePopFile != false || writeMaxFile != false) {
		string realtimeSequenceStr = (PT == nullptr) ? Arch_realtimeSequencePL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-realtimeSequence");
		realtimeSequence.clear();
		realtimeSequence = seq(realtimeSequenceStr, Global::updatesPL->lookup(), true);
		if (realtimeSequence.size() == 0) {
			cout << "unable to translate ARCHIVIST_DEFAULT-realtimeSequence \"" << realtimeSequenceStr << "\".\nExiting." << endl;
			exit(1);
		}
	}

	if (writeSnapshotDataFiles != false) {
		string dataSequenceStr = (PT == nullptr) ? SS_Arch_dataSequencePL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-snapshotDataSequence");
		realtimeDataSequence.clear();
		realtimeDataSequence = seq(dataSequenceStr, Global::updatesPL->lookup(), true);
		if (realtimeDataSequence.size() == 0) {
			cout << "unable to translate ARCHIVIST_DEFAULT-snapshotDataSequence \"" << dataSequenceStr << "\".\nExiting." << endl;
			exit(1);
		}
	}

	if (writeSnapshotGenomeFiles != false) {
		string organismIntervalStr = (PT == nullptr) ? SS_Arch_organismSequencePL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-snapshotOrganismSequence");
		realtimeOrganismSequence.clear();
		realtimeOrganismSequence = seq(organismIntervalStr, Global::updatesPL->lookup(), true);
		if (realtimeOrganismSequence.size() == 0) {
			cout << "unable to translate ARCHIVIST_DEFAULT-snapshotOrganismSequence \"" << organismIntervalStr << "\".\nExiting." << endl;
			exit(1);
		}
	}

	realtimeSequenceIndex = 0;
	realtimeDataSeqIndex = 0;
	realtimeOrganismSeqIndex = 0;

	finished = false;
}

DefaultArchivist::DefaultArchivist(vector<string> popFileColumns, shared_ptr<Abstract_MTree> _maxFormula, shared_ptr<ParametersTable> _PT, string _groupPrefix) :
	DefaultArchivist(_PT, _groupPrefix) {
	convertCSVListToVector(PopFileColumnNames, DefaultPopFileColumns);
	maxFormula = _maxFormula;
	if (DefaultPopFileColumns.size() <= 0) {
		DefaultPopFileColumns = popFileColumns;
	}
	for (auto key : DefaultPopFileColumns) {
		if (key == "update") {
			uniqueColumnNameToOutputBehaviors[key] = 0;
			continue;
		}
		size_t downslashPos = key.find_last_of('_');
		if (downslashPos != string::npos) { // if there is a downslash
			if (DataMap::knownOutputBehaviors.find(key.substr(downslashPos + 1)) == DataMap::knownOutputBehaviors.end()) { // if word after downslash does not indicate a known mask
				cout << "In DefaultArchivist::writerealTimeFiles :: Error, key '" << key << "' specifies an unknown output behavior (part after underscore)";
			}
			else {
				if (uniqueColumnNameToOutputBehaviors.find(key.substr(0, downslashPos)) == uniqueColumnNameToOutputBehaviors.end()) { // if key not in map
					uniqueColumnNameToOutputBehaviors[key] = DataMap::knownOutputBehaviors[key];
				}
				else { // key already in map
					uniqueColumnNameToOutputBehaviors[key.substr(0, downslashPos)] |= DataMap::knownOutputBehaviors[key.substr(downslashPos + 1)];
				}
			}
		}
		else { // add key normally, because it has no special flags specified
			if (uniqueColumnNameToOutputBehaviors.find(key) == uniqueColumnNameToOutputBehaviors.end()) {
				uniqueColumnNameToOutputBehaviors[key] = DataMap::AVE;
			}
			else {
				uniqueColumnNameToOutputBehaviors[key] |= DataMap::AVE;
			}
		}
	}
}

//save Max and pop file data
//keys named all* will be converted to *. These should key for lists of values. These values will be averaged (used to average world repeats)
void DefaultArchivist::writeRealTimeFiles(vector<shared_ptr<Organism>> &population) {
	// write out population data

	if (writePopFile) {
		double aveValue;
		DataMap PopMap;


		//for (auto key : DefaultPopFileColumns) {
		//	if (key != "update") {
		for (auto kv : uniqueColumnNameToOutputBehaviors) {
			if (kv.first != "update") {
				aveValue = 0;
				for (auto org : population) {
					if (org->timeOfBirth < Global::update || saveNewOrgs) {
						PopMap.Append(kv.first, org->dataMap.GetAverage(kv.first));
					}
				}
			}
			PopMap.setOutputBehavior(kv.first, uniqueColumnNameToOutputBehaviors[kv.first]);
		}
		PopMap.Set("update", Global::update);
		PopMap.writeToFile(PopFileName, { }, true); // write the PopMap to file with empty list (save all) and aveOnly = true (only save ave values)

	}
	// write out Max data
	if (writeMaxFile && maxFormula != nullptr) {
		vector<double> Scores;
		for (auto org : population) {
			if (org->timeOfBirth < Global::update || saveNewOrgs) {
				Scores.push_back(maxFormula->eval(org->dataMap, org->PT)[0]);
			}
		}

		int best = findGreatestInVector(Scores);
		population[best]->dataMap.Set("update", Global::update);
		population[best]->dataMap.writeToFile(MaxFileName);
		population[best]->dataMap.Clear("update");
	}
}

void DefaultArchivist::saveSnapshotData(vector<shared_ptr<Organism>> population) {
	// write out data
	string dataFileName = DataFilePrefix + "_" + to_string(Global::update) + ".csv";
	if (files.find("snapshotData") == files.end()) {  // first make sure that the dataFile has been set up.
			//population[0]->dataMap.Set("ancestors", "placeHolder");  // add ancestors so it will be in files (holds columns to be output for each file)
		files["snapshotData"] = population[0]->dataMap.getKeys();		// get all keys from the valid orgs dataMap (all orgs should have the same keys in their dataMaps)
		files["snapshotData"].push_back("snapshotAncestors");
	}

	// first, determine which orgs in population need to be saved.
	vector<shared_ptr<Organism>> saveList;
	int minBrithTime = population[0]->timeOfBirth; // time of birth of oldest org being saved in this update (init with random value)

	if (saveNewOrgs) {
		saveList = population;
		for (auto org : population) {
			minBrithTime = min(org->timeOfBirth, minBrithTime);
		}
	}
	else {
		for (auto org : population) {
			if (org->timeOfBirth < Global::update) {
				saveList.push_back(org);
			}
			minBrithTime = min(org->timeOfBirth, minBrithTime);
		}
	}

	// now for each org, update ancestors and save if in saveList
	for (auto org : population) {
		
		//cout << "---------------\n now looking at: " << org->ID << endl;
		//cout << "  with parents List (" << org->parents.size() << "): ";
		//for (auto a : org->parents) {
		//	cout << a->ID << "  ";
		//}
		//cout << endl;
		

		if (org->snapshotAncestors.size() != 1 || org->snapshotAncestors.find(org->ID) == org->snapshotAncestors.end()) {
			// if this org does not only contain only itself in snapshotAncestors then it has not been saved before.
			// we must confirm that snapshotAncestors is correct because things may have changed while we were not looking
			// this process does 2 things:
			// a) if this org is being saved then it makes sure it's up to date
			// b) it makes sure that it's ancestor list is correct so that it's offspring will pass on the correct ancestor info.
			//
			// How does it work? (good question)
			// get a checklist of parents of the current org
			// for each parent, if they are going to be saved in this update, yay, we can just assign their ID to the ancestor list
			// ... if they are not going to be saved then we need to check their ancestors to see if they are going to be saved,
			// unless they are atleast as old as the oldest org being saved to this file.
			// if they are at least as old as the oldest org being saved to this file then we can simply append their ancestors

			org->snapshotAncestors.clear();
			vector<shared_ptr<Organism>> parentCheckList = org->parents;
			
			while (parentCheckList.size() > 0) {
				auto parent = parentCheckList.back(); // this is "this parent"
				parentCheckList.pop_back(); // remove this parent from checklist

				//cout << "\n org: " << org->ID << " parent: " << parent->ID << endl;
				if (find(saveList.begin(), saveList.end(), parent) != saveList.end()) { // if this parent is being saved, they will serve as an ancestor
					org->snapshotAncestors.insert(parent->ID);
				}
				else { // this parent is not being saved
					if (parent->timeOfBirth < minBrithTime || (parent->snapshotAncestors.size() == 1 && parent->snapshotAncestors.find(parent->ID)!=parent->snapshotAncestors.end())) {
						// if this parent is old enough that it can not have a parent in the save list (and is not in save list),
						// or this parent has self in it's ancestor list (i.e. it has already been saved to another file),
						// copy ancestors from this parent
						//cout << "getting ancestors for " << org->ID << " parent " << parent->ID << " is old enough or has self as ancestor..." << endl;
						for (auto ancestorID : parent->snapshotAncestors) {
							//cout << "adding from parent " << parent->ID << " ancestor " << ancestorID << endl;
							org->snapshotAncestors.insert(ancestorID);
						}
					}
					else { // this parent not old enough (see if above), add this parents parents to check list (we need to keep looking)
						for (auto p : parent->parents) {
							parentCheckList.push_back(p);
						}
					}
				}
			}
		
			/* // uncomment to see updated ancesstors list
			cout << "  new snapshotAncestors List: ";
			for (auto a : org->snapshotAncestors) {
				cout << a << "  ";
			}
			cout << endl;
			*/

		} else { // org has self for ancestor
			if (org->timeOfBirth >= Global::update){ // if this is a new org...
				cout << "  WARRNING :: in DefaultArchivist::saveSnapshotData(), found new org (age < 1) with self as ancestor (with ID: " << org->ID << "... this will result in a new root to the phylogony tree!" << endl;
				if (saveNewOrgs) {
					cout << "    this org is being saved" << endl;
				}
				else {
					cout << "    this org is not being saved (this may be very bad...)" << endl;
				}
			}
		}
		// now that we know that ancestor list is good for this org...
		if (org->timeOfBirth < Global::update || saveNewOrgs) {
			//cout << "  is being saved" << endl;
			for (auto ancestorID : org->snapshotAncestors) {
				//cout << org->ID << " adding ancestor " << ancestorID << " to dataMap" << endl;
				org->dataMap.Append("snapshotAncestors", ancestorID);
			}
			org->dataMap.setOutputBehavior("snapshotAncestors", DataMap::LIST);

			org->snapshotAncestors.clear();  // now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
			org->snapshotAncestors.insert(org->ID);
			org->dataMap.Set("update", Global::update);
			org->dataMap.setOutputBehavior("update", DataMap::FIRST);
			org->dataMap.writeToFile(dataFileName, files["snapshotData"]);  // append new data to the file
			org->dataMap.Clear("snapshotAncestors");
			org->dataMap.Clear("update");
		}
	}
	FileManager::closeFile(dataFileName); // since this is a snapshot, we will not be writting to this file again.
}

void DefaultArchivist::saveSnapshotOrganisms(vector<shared_ptr<Organism>> population) {
	// write out organims
	string organismFileName = OrganismFilePrefix + "_" + to_string(Global::update) + ".csv";

	for (auto org : population) {
		if (org->timeOfBirth < Global::update || saveNewOrgs) {
			DataMap OrgMap;
			OrgMap.Set("ID", org->ID);
			string tempName;

			for (auto genome : org->genomes) {
				tempName = "GENOME_" + genome.first;
				OrgMap.Merge(genome.second->serialize(tempName));
			}
			for (auto brain : org->brains) {
				tempName = "BRAIN_" + brain.first;
				OrgMap.Merge(brain.second->serialize(tempName));
			}
			OrgMap.writeToFile(organismFileName); // append new data to the file
		}
	}
	FileManager::closeFile(organismFileName); // since this is a snapshot, we will not be writting to this file again.
}

// save data and manage in memory data
// return true if next save will be > updates + terminate after
bool DefaultArchivist::archive(vector<shared_ptr<Organism>> population, int flush) {

	if (finished) {
		return finished;
	}
	if (flush != 1) {
		if ((Global::update == realtimeSequence[realtimeSequenceIndex]) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
			writeRealTimeFiles(population);  // write to Max and average files
			if (realtimeSequenceIndex + 1 < (int)realtimeSequence.size()) {
				realtimeSequenceIndex++;
			}
		}

		if ((Global::update == realtimeDataSequence[realtimeDataSeqIndex]) && (flush == 0) && writeSnapshotDataFiles) {  // do not write files on flush - these organisms have not been evaluated!
			saveSnapshotData(population);
			if (realtimeDataSeqIndex + 1 < (int)realtimeDataSequence.size()) {
				realtimeDataSeqIndex++;
			}
		}
		if ((Global::update == realtimeOrganismSequence[realtimeOrganismSeqIndex]) && (flush == 0) && writeSnapshotGenomeFiles) {  // do not write files on flush - these organisms have not been evaluated!
			saveSnapshotOrganisms(population);
			if (realtimeOrganismSeqIndex + 1 < (int)realtimeOrganismSequence.size()) {
				realtimeOrganismSeqIndex++;
			}
		}

		for (auto org : population) {  // we don't need to worry about tracking parents or lineage, so we clear out this data every generation.
			if (org->snapshotAncestors.find(org->ID) != org->snapshotAncestors.end()) { // if ancestors contains self, then this org has been saved and it's ancestor list has been collapsed
				org->parents.clear();
			}
		}

	}
	// if we are at the end of the run
	finished = Global::update >= Global::updatesPL->lookup();
	return finished;
}

bool DefaultArchivist::isDataUpdate(int checkUpdate) {
	if (checkUpdate == -1) {
		checkUpdate = Global::update;
	}
	bool check = find(realtimeSequence.begin(), realtimeSequence.end(), checkUpdate) != realtimeSequence.end();
	check = check || find(realtimeDataSequence.begin(), realtimeDataSequence.end(), checkUpdate) != realtimeDataSequence.end();
	return check;
}

bool DefaultArchivist::isOrganismUpdate(int checkUpdate) {
	if (checkUpdate == -1) {
		checkUpdate = Global::update;
	}
	bool check = find(realtimeOrganismSequence.begin(), realtimeOrganismSequence.end(), checkUpdate) != realtimeOrganismSequence.end();
	return check;
}
