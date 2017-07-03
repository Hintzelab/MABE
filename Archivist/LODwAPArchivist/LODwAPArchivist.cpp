//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "LODwAPArchivist.h"

shared_ptr<ParameterLink<string>> LODwAPArchivist::LODwAP_Arch_dataSequencePL = Parameters::register_parameter("ARCHIVIST_LODWAP-dataSequence", (string) ":100",
		"How often to write to data file. (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
shared_ptr<ParameterLink<string>> LODwAPArchivist::LODwAP_Arch_organismSequencePL = Parameters::register_parameter("ARCHIVIST_LODWAP-organismSequence", (string) ":1000",
		"How often to write genome file. (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
shared_ptr<ParameterLink<int>> LODwAPArchivist::LODwAP_Arch_pruneIntervalPL = Parameters::register_parameter("ARCHIVIST_LODWAP-pruneInterval", 100, "How often to attempt to prune LOD and actually write out to files");
shared_ptr<ParameterLink<int>> LODwAPArchivist::LODwAP_Arch_terminateAfterPL = Parameters::register_parameter("ARCHIVIST_LODWAP-terminateAfter", 100, "how long to run after updates (to get allow time for coalescence)");
shared_ptr<ParameterLink<string>> LODwAPArchivist::LODwAP_Arch_DataFileNamePL = Parameters::register_parameter("ARCHIVIST_LODWAP-dataFileName", (string) "LOD_data.csv", "name of genome file (stores genomes for line of decent)");
shared_ptr<ParameterLink<string>> LODwAPArchivist::LODwAP_Arch_OrganismFileNamePL = Parameters::register_parameter("ARCHIVIST_LODWAP-genomeFileName", (string) "LOD_organism.csv", "name of data file (stores everything but genomes)");
shared_ptr<ParameterLink<bool>> LODwAPArchivist::LODwAP_Arch_writeDataFilePL = Parameters::register_parameter("ARCHIVIST_LODWAP-writeDataFile", true, "if true, a data file will be written");
shared_ptr<ParameterLink<bool>> LODwAPArchivist::LODwAP_Arch_writeOrganismFilePL = Parameters::register_parameter("ARCHIVIST_LODWAP-writeOrganismFile", true, "if true, a organism file will be written");

LODwAPArchivist::LODwAPArchivist(vector<string> popFileColumns, shared_ptr<Abstract_MTree> _maxFormula, shared_ptr<ParametersTable> _PT, string _groupPrefix) :
		DefaultArchivist(popFileColumns, _maxFormula, _PT, _groupPrefix) {

	pruneInterval = (PT == nullptr) ? LODwAP_Arch_pruneIntervalPL->lookup() : PT->lookupInt("ARCHIVIST_LODWAP-pruneInterval");
	terminateAfter = (PT == nullptr) ? LODwAP_Arch_terminateAfterPL->lookup() : PT->lookupInt("ARCHIVIST_LODWAP-terminateAfter");
	DataFileName = (PT == nullptr) ? LODwAP_Arch_DataFileNamePL->lookup() : PT->lookupString("ARCHIVIST_LODWAP-dataFileName");
	DataFileName = (groupPrefix == "") ? DataFileName : groupPrefix + "__" + DataFileName;
	OrganismFileName = (PT == nullptr) ? LODwAP_Arch_OrganismFileNamePL->lookup() : PT->lookupString("ARCHIVIST_LODWAP-genomeFileName");
	OrganismFileName = (groupPrefix == "") ? OrganismFileName : groupPrefix + "__" + OrganismFileName;

	writeDataFile = (PT == nullptr) ? LODwAP_Arch_writeDataFilePL->lookup() : PT->lookupBool("ARCHIVIST_LODWAP-writeDataFile");
	writeOrganismFile = (PT == nullptr) ? LODwAP_Arch_writeOrganismFilePL->lookup() : PT->lookupBool("ARCHIVIST_LODWAP-writeOrganismFile");

	dataSequence.push_back(0);
	organismSequence.push_back(0);

	string dataSequenceStr = (PT == nullptr) ? LODwAP_Arch_dataSequencePL->lookup() : PT->lookupString("ARCHIVIST_LODWAP-dataSequence");
	string genomeSequenceStr = (PT == nullptr) ? LODwAP_Arch_organismSequencePL->lookup() : PT->lookupString("ARCHIVIST_LODWAP-organismSequence");

	dataSequence = seq(dataSequenceStr, Global::updatesPL->lookup(), true);
	if (dataSequence.size() == 0) {
		cout << "unable to translate ARCHIVIST_LODWAP-dataSequence \"" << dataSequenceStr << "\".\nExiting." << endl;
		exit(1);
	}
	organismSequence = seq(genomeSequenceStr, Global::updatesPL->lookup(), true);
	if (organismSequence.size() == 0) {
		cout << "unable to translate ARCHIVIST_LODWAP-organismSequence \"" << genomeSequenceStr << "\".\nExiting." << endl;
		exit(1);
	}

	if (writeDataFile != false) {
		dataSequence.clear();
		dataSequence = seq(dataSequenceStr, Global::updatesPL->lookup(), true);
		if (dataSequence.size() == 0) {
			cout << "unable to translate ARCHIVIST_SSWD-dataSequence \"" << dataSequenceStr << "\".\nExiting." << endl;
			exit(1);
		}
	}

	if (writeOrganismFile != false) {
		organismSequence = seq(genomeSequenceStr, Global::updatesPL->lookup(), true);
		if (organismSequence.size() == 0) {
			cout << "unable to translate ARCHIVIST_SSWD-organismSequence \"" << genomeSequenceStr << "\".\nExiting." << endl;
			exit(1);
		}
	}

	dataSeqIndex = 0;
	organismSeqIndex = 0;
	nextDataWrite = dataSequence[dataSeqIndex];
	nextOrganismWrite = organismSequence[organismSeqIndex];

	lastPrune = 0;

}

bool LODwAPArchivist::archive(vector<shared_ptr<Organism>> population, int flush) {
	if (finished && !flush) {
		return finished;
	}

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
	if ((Global::update == realtimeOrganismSequence[realtimeOrganismSeqIndex]) && (flush == 0) && writeSnapshotGenomeFiles) {  // do not write files on flush - these organisms have not been evaluated!
		saveSnapshotOrganisms(population);
		if (realtimeOrganismSeqIndex + 1 < (int) realtimeOrganismSequence.size()) {
			realtimeOrganismSeqIndex++;
		}
	}

	if (writeOrganismFile && find(organismSequence.begin(), organismSequence.end(), Global::update) != organismSequence.end()) {
		for (auto org : population) {  // if this update is in the genome sequence, turn on genome tracking.
			org->trackOrganism = true;
		}

	}

	if ((Global::update % pruneInterval == 0) || (flush == 1)) {

		if (files.find(DataFileName) == files.end()) {  // if file has not be initialized yet
			files[DataFileName].push_back("update");
			for (auto key : population[0]->dataMap.getKeys()) {  // store keys from data map associated with file name
				files[DataFileName].push_back(key);
			}
		}

		// get the MRCA
		vector<shared_ptr<Organism>> LOD = population[0]->getLOD(population[0]);  // get line of decent
		shared_ptr<Organism> effective_MRCA;
		if (flush) {  // if flush then we don't care about coalescence
			cout << "flushing LODwAP: using population[0] as Most Recent Common Ancestor (MRCA)" << endl;
			effective_MRCA = population[0]->parents[0];  // this assumes that a population was created, but not tested at the end of the evolution loop!
		} else {
			effective_MRCA = population[0]->getMostRecentCommonAncestor(LOD);  // find the convergance point in the LOD.
		}

		// Save Data
		if (writeDataFile) {
			while ((effective_MRCA->timeOfBirth >= nextDataWrite) && (nextDataWrite <= Global::updatesPL->lookup())) {  // if there is convergence before the next data interval
				shared_ptr<Organism> current = LOD[nextDataWrite - lastPrune];
				current->dataMap.Set("update", nextDataWrite);
				current->dataMap.setOutputBehavior("update", DataMap::FIRST);
				current->dataMap.writeToFile(DataFileName, files[DataFileName]);  // append new data to the file
				current->dataMap.Clear("update");
				if ((int) dataSequence.size() > dataSeqIndex + 1) {
					dataSeqIndex++;
					nextDataWrite = dataSequence[dataSeqIndex];
				} else {
					nextDataWrite = Global::updatesPL->lookup() + terminateAfter + 1;
				}

			}
		}

		//Save Organisms
		if (writeOrganismFile) {

			while ((effective_MRCA->timeOfBirth >= nextOrganismWrite) && (nextOrganismWrite <= Global::updatesPL->lookup())) {  // if there is convergence before the next data interval

				shared_ptr<Organism> current = LOD[nextOrganismWrite - lastPrune];

				DataMap OrgMap;
				OrgMap.Set("ID", current->ID);
				OrgMap.Set("update", nextOrganismWrite);
				OrgMap.setOutputBehavior("update", DataMap::FIRST);
				string tempName;

				for (auto genome : current->genomes) {
					tempName = "GENOME_" + genome.first;
					OrgMap.Merge(genome.second->serialize(tempName));
				}
				for (auto brain : current->brains) {
					tempName = "BRAIN_" + brain.first;
					OrgMap.Merge(brain.second->serialize(tempName));
				}
				OrgMap.writeToFile(OrganismFileName); // append new data to the file

				if ((int) organismSequence.size() > organismSeqIndex + 1) {
					organismSeqIndex++;
					nextOrganismWrite = organismSequence[organismSeqIndex];
				} else {
					nextOrganismWrite = Global::updatesPL->lookup() + terminateAfter + 1;
				}
			}
		}
		// data and genomes have now been written out up till the MRCA
		// so all data and genomes from before the MRCA can be deleted
		effective_MRCA->parents.clear();
		lastPrune = effective_MRCA->timeOfBirth;  // this will hold the time of the oldest genome in RAM
	}

	// if we have reached the end of time OR we have pruned past updates (i.e. written out all data up to updates), then we ae done!
	//cout << "\nHERE" << endl;
	//cout << Global::update << " >= " << Global::updatesPL->lookup() << " + " << terminateAfter << endl;
	finished = finished || (Global::update >= Global::updatesPL->lookup() + terminateAfter || lastPrune >= Global::updatesPL->lookup());

	/*
	////////////////////////////////////////////////////////
	// code to support defualt archivist snapshotData
	////////////////////////////////////////////////////////
	vector<shared_ptr<Organism>> toCheck;
	unordered_set<shared_ptr<Organism>> checked;
	int minBirthTime = population[0]->timeOfBirth; // time of birth of oldest org being saved in this update (init with random value)

	for (auto org : population) {  // we don't need to worry about tracking parents or lineage, so we clear out this data every generation.
		if (!writeSnapshotDataFiles) {
			org->parents.clear();
		}
		else if (org->snapshotAncestors.find(org->ID) != org->snapshotAncestors.end()) { // if ancestors contains self, then this org has been saved and it's ancestor list has been collapsed
			org->parents.clear();
			checked.insert(org); // make a note, so we don't check this org later
			minBirthTime = min(org->timeOfBirth, minBirthTime);
		}
		else { // org has not ever been saved to file...
			toCheck.push_back(org); // we will need to check to see if we can do clean up related to this org
			checked.insert(org); // make a note, so we don't check twice
			minBirthTime = min(org->timeOfBirth, minBirthTime);
		}
	}
	////////////////////////////////////////////////////////
	// end code to support defualt archivist snapshotData
	////////////////////////////////////////////////////////
	*/

	return finished;
}

bool LODwAPArchivist::isDataUpdate(int checkUpdate) {
	if (checkUpdate == -1) {
		checkUpdate = Global::update;
	}
	bool check = DefaultArchivist::isDataUpdate(checkUpdate);
	check = check || find(dataSequence.begin(), dataSequence.end(), checkUpdate) != dataSequence.end();
	return check;
}

bool LODwAPArchivist::isOrganismUpdate(int checkUpdate) {
	if (checkUpdate == -1) {
		checkUpdate = Global::update;
	}
	bool check = DefaultArchivist::isOrganismUpdate(checkUpdate);
	check = check || find(organismSequence.begin(), organismSequence.end(), checkUpdate) != organismSequence.end();
	return check;
}
