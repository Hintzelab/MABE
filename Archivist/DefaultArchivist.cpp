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

shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_outputMethodStrPL = Parameters::register_parameter("ARCHIVIST-outputMethod", (string) "Default", "output method, [default, LODwAP (Line of Decent with Aggressive Pruning), SSwD (SnapShot with Delay)]");  // string parameter for outputMethod;

shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_realtimeSequencePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-realtimeSequence", (string) ":10", "How often to write to realtime data files. (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
shared_ptr<ParameterLink<string>> DefaultArchivist::SS_Arch_dataSequencePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-snapshotDataSequence", (string) ":100", "How often to save a realtime snapshot data file. (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
shared_ptr<ParameterLink<string>> DefaultArchivist::SS_Arch_genomeSequencePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-snapshotGenomeSequence", (string) ":1000", "How often to save a realtime snapshot genome file. (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");

shared_ptr<ParameterLink<bool>> DefaultArchivist::Arch_writeAveFilePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-writeAveFile", true, "Save data to average file?");
shared_ptr<ParameterLink<bool>> DefaultArchivist::Arch_writeDominantFilePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-writeDominantFile", true, "Save data to dominant file?");
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_AveFileNamePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-aveFileName", (string) "ave.csv", "name of average file (saves population averages)");
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_DominantFileNamePL = Parameters::register_parameter("ARCHIVIST_DEFAULT-dominantFileName", (string) "dominant.csv", "name of dominant file (saves data on dominant organism)");
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_DefaultAveFileColumnNamesPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-aveFileColumns", (string) "[]", "data to be saved into average file (must be values that can generate an average). If empty, MABE will try to figure it out");
shared_ptr<ParameterLink<bool>> DefaultArchivist::Arch_DominantFileShowAllListsPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-dominantFileShowAllLists", true, "lists named 'all'* in data map will be averaged and added to file. if true, raw 'all'* lists will also be added to the file");

shared_ptr<ParameterLink<string>> DefaultArchivist::SS_Arch_DataFilePrefixPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-snapshotDataFilePrefix", (string) "snapshotData", "prefix for name of snapshot genome file (stores genomes)");
shared_ptr<ParameterLink<string>> DefaultArchivist::SS_Arch_GenomeFilePrefixPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-snapshotGenomeFilePrefix", (string) "snapshotGenome", "prefix for name of snapshot data file (stores everything but genomes)");
shared_ptr<ParameterLink<bool>> DefaultArchivist::SS_Arch_writeDataFilesPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-writeSnapshotDataFiles", false, "if true, snapshot data files will be written (with all non genome data for entire population)");
shared_ptr<ParameterLink<bool>> DefaultArchivist::SS_Arch_writeGenomeFilesPL = Parameters::register_parameter("ARCHIVIST_DEFAULT-writeSnapshotGenomeFiles", false, "if true, snapshot genome files will be written (with all genomes for entire population)");

DefaultArchivist::DefaultArchivist(shared_ptr<ParametersTable> _PT) :
		PT(_PT) {

	writeAveFile = (PT == nullptr) ? Arch_writeAveFilePL->lookup() : PT->lookupBool("ARCHIVIST_DEFAULT-writeAveFile");
	writeDominantFile = (PT == nullptr) ? Arch_writeDominantFilePL->lookup() : PT->lookupBool("ARCHIVIST_DEFAULT-writeDominantFile");
	AveFileName = (PT == nullptr) ? Arch_AveFileNamePL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-aveFileName");
	DominantFileName = (PT == nullptr) ? Arch_DominantFileNamePL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-dominantFileName");
	AveFileColumnNames = (PT == nullptr) ? Arch_DefaultAveFileColumnNamesPL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-aveFileColumns");
	DominantFileShowAllLists = (PT == nullptr) ? Arch_DominantFileShowAllListsPL->lookup() : PT->lookupBool("ARCHIVIST_DEFAULT-dominantFileShowAllLists");

	DataFilePrefix = (PT == nullptr) ? SS_Arch_DataFilePrefixPL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-snapshotDataFilePrefix");
	GenomeFilePrefix = (PT == nullptr) ? SS_Arch_GenomeFilePrefixPL->lookup() : PT->lookupString("ARCHIVIST_DEFAULT-snapshotGenomeFilePrefix");
	writeSnapshotDataFiles = (PT == nullptr) ? SS_Arch_writeDataFilesPL->lookup() : PT->lookupBool("ARCHIVIST_DEFAULT-writeSnapshotDataFiles");
	writeSnapshotGenomeFiles = (PT == nullptr) ? SS_Arch_writeGenomeFilesPL->lookup() : PT->lookupBool("ARCHIVIST_DEFAULT-writeSnapshotGenomeFiles");

	realtimeSequence.push_back(0);
	realtimeDataSequence.push_back(0);
	realtimeGenomeSequence.push_back(0);

	if (writeAveFile != false || writeDominantFile != false) {
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

DefaultArchivist::DefaultArchivist(vector<string> aveFileColumns, shared_ptr<ParametersTable> _PT) :
		DefaultArchivist(_PT) {
	convertCSVListToVector(AveFileColumnNames, DefaultAveFileColumns);
	if (DefaultAveFileColumns.size() <= 0) {
		DefaultAveFileColumns = aveFileColumns;
	}
}

//save dominant and average file data
//keys named all* will be converted to *. These should key for lists of values. These values will be averaged (used to average world repeats)
void DefaultArchivist::writeRealTimeFiles(vector<shared_ptr<Organism>> &population) {
	// write out Average data
	if (writeAveFile) {
		double aveValue;
		DataMap AveMap;

		for (auto key : DefaultAveFileColumns) {
			if (key != "update") {
				aveValue = 0;
				for (auto org : population) {
					aveValue += org->dataMap.GetAverage(key);
				}
				aveValue /= population.size();
				if(population[0]->dataMap.outputBehavior[key] & DataMap::AVE){ // if the value in question has it's AVE flag set...
					AveMap.Set(key + "_AVE", aveValue);
				} else {
					AveMap.Set(key, aveValue);
				}
			}
		}
		for (auto key : DefaultAveFileColumns) {
			AveMap.outputBehavior[key] = population[0]->dataMap.outputBehavior[key];
		}
		AveMap.Set("update", Global::update);
		AveMap.setOutputBehavior("update", DataMap::FIRST);
		AveMap.writeToFile(AveFileName); // write the AveMap to file with aveOnly = true (only save ave values)

	}
	// write out Dominant data
	if (writeDominantFile) {
		vector<double> Scores;
		for (auto org : population) {
			Scores.push_back(org->score);
		}

		int best = findGreatestInVector(Scores);
		population[best]->dataMap.Set("update", Global::update);
		population[best]->dataMap.writeToFile(DominantFileName);
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
}

void DefaultArchivist::saveSnapshotGenomes(vector<shared_ptr<Organism>> population) {

	// write out genomes
	string genomeFileName = GenomeFilePrefix + "_" + to_string(Global::update) + ".csv";

	string dataString;
	for (auto org : population) {
		org->genome->dataMap.Set("sites", org->genome->genomeToStr());
		org->genome->dataMap.Set("ID", org->dataMap.GetIntVector("ID")[0]);
		org->genome->dataMap.Set("update", Global::update);

		org->genome->dataMap.writeToFile(genomeFileName, org->genome->genomeFileColumns);		// append new data to the file
		org->genome->dataMap.Clear("update");		// we dont' need this anymore.
	}
}
// save data and manage in memory data
// return true if next save will be > updates + terminate after
bool DefaultArchivist::archive(vector<shared_ptr<Organism>> population, int flush) {
	if (flush != 1) {
		if ((Global::update == realtimeSequence[realtimeSequenceIndex]) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
			writeRealTimeFiles(population);  // write to dominant and average files
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

void DefaultArchivist::processAllLists(OldDataMap &dm) {
	vector<string> allKeys = dm.getKeys();
	for (auto key : allKeys) {
		if (key.substr(0, 3) == "all") {
			if (find(allKeys.begin(), allKeys.end(), key.substr(3, key.size() - 1)) == allKeys.end()) {
				double temp = 0;
				vector<double> values;
				convertCSVListToVector(dm.Get(key), values);
				for (auto v : values) {
					temp += v;
					//cout << key << " " << allKey << " " << v << " " << temp << endl;
				}
				temp /= (double) values.size();
				dm.Set(key.substr(3, key.size() - 1), temp);
			}
		}
	}
}

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
