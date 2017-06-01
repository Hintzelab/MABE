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
shared_ptr<ParameterLink<string>> DefaultArchivist::SS_Arch_genomeSequencePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-snapshotGenomeSequence", (string) ":1000",
		"How often to save a realtime snapshot genome file. (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");

shared_ptr<ParameterLink<bool>> DefaultArchivist::Arch_writeMaxFilePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-writeMaxFile", true, "Save data to Max file?");
shared_ptr<ParameterLink<bool>> DefaultArchivist::Arch_writePopFilePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-writePopFile", true, "Save data to population file?");
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_PopFileNamePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-popFileName", (string) "pop.csv", "name of average file (saves population averages)");
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_MaxFileNamePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-maxFileName", (string) "max.csv", "name of max file (saves data on organism with max \"score\" as determined by Optimizer)");
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_DefaultPopFileColumnNamesPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-popFileColumns", (string) "[]",
		"data to be saved into average file (must be values that can generate an average). If empty, MABE will try to figure it out");

shared_ptr<ParameterLink<string>> DefaultArchivist::SS_Arch_DataFilePrefixPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-snapshotDataFilePrefix", (string) "snapshotData", "prefix for name of snapshot genome file (stores genomes)");
shared_ptr<ParameterLink<string>> DefaultArchivist::SS_Arch_GenomeFilePrefixPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-snapshotGenomeFilePrefix", (string) "snapshotGenome",
		"prefix for name of snapshot data file (stores everything but genomes)");
shared_ptr<ParameterLink<bool>> DefaultArchivist::SS_Arch_writeDataFilesPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-writeSnapshotDataFiles", false,
		"if true, snapshot data files will be written (with all non genome data for entire population)");
shared_ptr<ParameterLink<bool>> DefaultArchivist::SS_Arch_writeGenomeFilesPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-writeSnapshotGenomeFiles", false, "if true, snapshot genome files will be written (with all genomes for entire population)");

DefaultArchivist::DefaultArchivist(shared_ptr<ParametersTable> _PT) :
		PT(_PT) {
	
	writePopFile = (PT == nullptr) ? Arch_writePopFilePL->lookup() : PT->lookupBool("ARCHIVIST_DEFAULT-writePopFile");
	writeMaxFile = (PT == nullptr) ? Arch_writeMaxFilePL->lookup() : PT->lookupBool("ARCHIVIST_DEFAULT-writeMaxFile");
	PopFileName = (PT == nullptr) ? Arch_PopFileNamePL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-popFileName");
	MaxFileName = (PT == nullptr) ? Arch_MaxFileNamePL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-maxFileName");
	PopFileColumnNames = (PT == nullptr) ? Arch_DefaultPopFileColumnNamesPL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-popFileColumns");

	DataFilePrefix = (PT == nullptr) ? SS_Arch_DataFilePrefixPL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-snapshotDataFilePrefix");
	GenomeFilePrefix = (PT == nullptr) ? SS_Arch_GenomeFilePrefixPL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-snapshotGenomeFilePrefix");
	writeSnapshotDataFiles = (PT == nullptr) ? SS_Arch_writeDataFilesPL->lookup() : PT->lookupBool("ARCHIVIST_DEFAULT-writeSnapshotDataFiles");
	writeSnapshotGenomeFiles = (PT == nullptr) ? SS_Arch_writeGenomeFilesPL->lookup() : PT->lookupBool("ARCHIVIST_DEFAULT-writeSnapshotGenomeFiles");

	realtimeSequence.push_back(0);
	realtimeDataSequence.push_back(0);
	realtimeGenomeSequence.push_back(0);

	if (writePopFile != false || writeMaxFile != false || writePopFile != false) {
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
		string genomeIntervalStr = (PT == nullptr) ? SS_Arch_genomeSequencePL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-snapshotGenomeSequence");
		realtimeGenomeSequence.clear();
		realtimeGenomeSequence = seq(genomeIntervalStr, Global::updatesPL->lookup(), true);
		if (realtimeGenomeSequence.size() == 0) {
			cout << "unable to translate ARCHIVIST_DEFAULT-snapshotGenomeSequence \"" << genomeIntervalStr << "\".\nExiting." << endl;
			exit(1);
		}
	}

	realtimeSequenceIndex = 0;
	realtimeDataSeqIndex = 0;
	realtimeGenomeSeqIndex = 0;

	finished = false;
}

DefaultArchivist::DefaultArchivist(vector<string> popFileColumns, shared_ptr<Abstract_MTree> _maxFormula, shared_ptr<ParametersTable> _PT) :
		DefaultArchivist(_PT) {
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
		if ((key.length() > 3) && (key.at(key.length()-4)=='_')) {
			if (DataMap::knownOutputBehaviors.find(key.substr(key.length()-3)) == DataMap::knownOutputBehaviors.end()) { // if mask not known
				cout << "In DefaultArchivist::writerealTimeFiles :: Error, key '" << key << "' specifies an unknown output behavior (part after underscore)";
			}
			else {
				if (uniqueColumnNameToOutputBehaviors.find(key.substr(0,key.length()-4)) == uniqueColumnNameToOutputBehaviors.end()) { // if key not in map
					uniqueColumnNameToOutputBehaviors[key] = DataMap::knownOutputBehaviors[key];
				} else { // key already in map
					uniqueColumnNameToOutputBehaviors[key.substr(0,key.length()-4)] |= DataMap::knownOutputBehaviors[key.substr(key.length()-3)];
				}
			}
		} else { // add key normally, because it has no special flags specified
			if (uniqueColumnNameToOutputBehaviors.find(key) == uniqueColumnNameToOutputBehaviors.end()) {
				uniqueColumnNameToOutputBehaviors[key] = DataMap::AVE;
			} else {
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


		for (auto kv : uniqueColumnNameToOutputBehaviors) {
			if (kv.first != "update") {
				aveValue = 0;
				for (auto org : population) {
					PopMap.Append(kv.first, org->dataMap.GetAverage(kv.first));
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
			Scores.push_back(maxFormula->eval(org->dataMap, org->PT)[0]);
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
		//population[0]->dataMap.Clear("ancestors");
	}
	for (auto org : population) {
		for (auto ancestor : org->snapshotAncestors) {
			org->dataMap.Append("snapshotAncestors", ancestor);
		}
		org->dataMap.setOutputBehavior("snapshotAncestors", DataMap::LIST);

		org->snapshotAncestors.clear();
		org->snapshotAncestors.insert(org->ID);  // now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)

		org->dataMap.Set("update", Global::update);
		org->dataMap.setOutputBehavior("update", DataMap::AVE);

		org->dataMap.writeToFile(dataFileName, files["snapshotData"]);  // append new data to the file
		org->dataMap.Clear("snapshotAncestors");
		org->dataMap.Clear("update");
	}
	FileManager::closeFile(dataFileName); // since this is a snapshot, we will not be writting to this file again.
}

void DefaultArchivist::saveSnapshotGenomes(vector<shared_ptr<Organism>> population) {

	// write out genomes
	string genomeFileName = GenomeFilePrefix + "_" + to_string(Global::update) + ".csv";

	string dataString;
	bool savedGenomes = false;
	for (auto org : population) {

		if (org->hasGenome()) { // if org has a genome
			savedGenomes = true;
			org->genome->dataMap.Set("sites", org->genome->genomeToStr());
			org->genome->dataMap.Set("ID", org->dataMap.GetIntVector("ID")[0]);
			org->genome->dataMap.Set("update", Global::update);

			org->genome->dataMap.writeToFile(genomeFileName, org->genome->genomeFileColumns);		// append new data to the file
			org->genome->dataMap.Clear("update");		// we dont' need this anymore.
		}

	}
	if (savedGenomes) {
		FileManager::closeFile(genomeFileName); // since this is a snapshot, we will not be writting to this file again.
	}
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
			if (realtimeSequenceIndex + 1 < (int) realtimeSequence.size()) {
				realtimeSequenceIndex++;
			}
		}

		if ((Global::update == realtimeDataSequence[realtimeDataSeqIndex]) && (flush == 0) && writeSnapshotDataFiles) {  // do not write files on flush - these organisms have not been evaluated!
			saveSnapshotData(population);
			if (realtimeDataSeqIndex + 1 < (int) realtimeDataSequence.size()) {
				realtimeDataSeqIndex++;
			}
		}
		if ((Global::update == realtimeGenomeSequence[realtimeGenomeSeqIndex]) && (flush == 0) && writeSnapshotGenomeFiles) {  // do not write files on flush - these organisms have not been evaluated!
			saveSnapshotGenomes(population);
			if (realtimeGenomeSeqIndex + 1 < (int) realtimeGenomeSequence.size()) {
				realtimeGenomeSeqIndex++;
			}
		}

		for (auto org : population) {  // we don't need to worry about tracking parents or lineage, so we clear out this data every generation.
			org->clearHistory();
		}
	}
	// if we are at the end of the run
	finished = Global::update >= Global::updatesPL->lookup();
	return finished;
}

//void DefaultArchivist::processAllLists(OldDataMap &dm) {
//	vector<string> allKeys = dm.getKeys();
//	for (auto key : allKeys) {
//		if (key.substr(0, 3) == "all") {
//			if (find(allKeys.begin(), allKeys.end(), key.substr(3, key.size() - 1)) == allKeys.end()) {
//				double temp = 0;
//				vector<double> values;
//				convertCSVListToVector(dm.Get(key), values);
//				for (auto v : values) {
//					temp += v;
//					//cout << key << " " << allKey << " " << v << " " << temp << endl;
//				}
//				temp /= (double) values.size();
//				dm.Set(key.substr(3, key.size() - 1), temp);
//			}
//		}
//	}
//}

bool DefaultArchivist::isDataUpdate(int checkUpdate) {
	if (checkUpdate == -1) {
		checkUpdate = Global::update;
	}
	bool check = find(realtimeSequence.begin(), realtimeSequence.end(), checkUpdate) != realtimeSequence.end();
	check = check || find(realtimeDataSequence.begin(), realtimeDataSequence.end(), checkUpdate) != realtimeDataSequence.end();
	return check;
}

bool DefaultArchivist::isGenomeUpdate(int checkUpdate) {
	if (checkUpdate == -1) {
		checkUpdate = Global::update;
	}
	bool check = find(realtimeGenomeSequence.begin(), realtimeGenomeSequence.end(), checkUpdate) != realtimeGenomeSequence.end();
	return check;
}
