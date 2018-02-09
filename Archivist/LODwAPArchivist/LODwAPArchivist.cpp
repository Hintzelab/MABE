//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "LODwAPArchivist.h"

shared_ptr<ParameterLink<string>> LODwAPArchivist::LODwAP_Arch_dataSequencePL = Parameters::register_parameter("ARCHIVIST_LODWAP-dataSequence", (string) ":100",
		"How often to write to data file. (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
shared_ptr<ParameterLink<string>> LODwAPArchivist::LODwAP_Arch_organismSequencePL = Parameters::register_parameter("ARCHIVIST_LODWAP-organismsSequence", (string) ":1000",
		"How often to write genome file. (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
shared_ptr<ParameterLink<int>> LODwAPArchivist::LODwAP_Arch_pruneIntervalPL = Parameters::register_parameter("ARCHIVIST_LODWAP-pruneInterval", 100, "How often to attempt to prune LOD and actually write out to files");
shared_ptr<ParameterLink<int>> LODwAPArchivist::LODwAP_Arch_terminateAfterPL = Parameters::register_parameter("ARCHIVIST_LODWAP-terminateAfter", 10, "how long to run after updates (to get allow time for coalescence)");
shared_ptr<ParameterLink<bool>> LODwAPArchivist::LODwAP_Arch_writeDataFilePL = Parameters::register_parameter("ARCHIVIST_LODWAP-writeDataFile", true, "if true, a data file will be written");
shared_ptr<ParameterLink<bool>> LODwAPArchivist::LODwAP_Arch_writeOrganismFilePL = Parameters::register_parameter("ARCHIVIST_LODWAP-writeOrganismsFile", true, "if true, an organisms file will be written");
shared_ptr<ParameterLink<string>> LODwAPArchivist::LODwAP_Arch_FilePrefixPL = Parameters::register_parameter("ARCHIVIST_LODWAP-filePrefix", (string) "NONE", "prefix for files saved by this archivst. \"NONE\" indicates no prefix.");



LODwAPArchivist::LODwAPArchivist(vector<string> popFileColumns, shared_ptr<Abstract_MTree> _maxFormula, shared_ptr<ParametersTable> _PT, string _groupPrefix) :
		DefaultArchivist(popFileColumns, _maxFormula, _PT, _groupPrefix) {

	pruneInterval = LODwAP_Arch_pruneIntervalPL->get(PT);
	terminateAfter = LODwAP_Arch_terminateAfterPL->get(PT);
	DataFileName = ((LODwAP_Arch_FilePrefixPL->get(PT) == "NONE")? "" : LODwAP_Arch_FilePrefixPL->get(PT)) + (((groupPrefix == "") ? "LOD_data.csv" : groupPrefix.substr(0, groupPrefix.size() - 2) + "__" + "LOD_data.csv"));
	OrganismFileName = ((LODwAP_Arch_FilePrefixPL->get(PT) == "NONE") ? "" : LODwAP_Arch_FilePrefixPL->get(PT)) + (((groupPrefix == "") ? "LOD_organisms.csv"  : groupPrefix.substr(0, groupPrefix.size() - 2) + "__" + "LOD_organisms.csv"));

	writeDataFile = LODwAP_Arch_writeDataFilePL->get(PT);
	writeOrganismFile = LODwAP_Arch_writeOrganismFilePL->get(PT);

	dataSequence.push_back(0);
	organismSequence.push_back(0);

	string dataSequenceStr = LODwAP_Arch_dataSequencePL->get(PT);
	string genomeSequenceStr = LODwAP_Arch_organismSequencePL->get(PT);

	dataSequence = seq(dataSequenceStr, Global::updatesPL->get(), true);
	if (dataSequence.size() == 0) {
		cout << "unable to translate ARCHIVIST_LODWAP-dataSequence \"" << dataSequenceStr << "\".\nExiting." << endl;
		exit(1);
	}
	organismSequence = seq(genomeSequenceStr, Global::updatesPL->get(), true);
	if (organismSequence.size() == 0) {
		cout << "unable to translate ARCHIVIST_LODWAP-organismsSequence \"" << genomeSequenceStr << "\".\nExiting." << endl;
		exit(1);
	}

	if (writeDataFile != false) {
		dataSequence.clear();
		dataSequence = seq(dataSequenceStr, Global::updatesPL->get(), true);
		if (dataSequence.size() == 0) {
			cout << "unable to translate ARCHIVIST_SSWD-dataSequence \"" << dataSequenceStr << "\".\nExiting." << endl;
			exit(1);
		}
	}

	if (writeOrganismFile != false) {
		organismSequence = seq(genomeSequenceStr, Global::updatesPL->get(), true);
		if (organismSequence.size() == 0) {
			cout << "unable to translate ARCHIVIST_SSWD-organismsSequence \"" << genomeSequenceStr << "\".\nExiting." << endl;
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
			files[DataFileName].push_back("timeToCoalescence");
			for (auto key : population[0]->dataMap.getKeys()) {  // store keys from data map associated with file name
				files[DataFileName].push_back(key);
			}
		}

		// get the MRCA
		vector<shared_ptr<Organism>> LOD = population[0]->getLOD(population[0]);  // get line of decent
		shared_ptr<Organism> effective_MRCA;
		shared_ptr<Organism> real_MRCA;
		if (flush) {  // if flush then we don't care about coalescence
			cout << "flushing LODwAP: using population[0] as Most Recent Common Ancestor (MRCA)" << endl;
			effective_MRCA = population[0]->parents[0];  // this assumes that a population was created, but not tested at the end of the evolution loop!
			real_MRCA = population[0]->getMostRecentCommonAncestor(LOD);  // find the convergance point in the LOD.
		} else {
			effective_MRCA = population[0]->getMostRecentCommonAncestor(LOD);  // find the convergance point in the LOD.
			real_MRCA = effective_MRCA;
		}

		// Save Data
		int TTC;
		if (writeDataFile) {
			while ((effective_MRCA->timeOfBirth >= nextDataWrite) && (nextDataWrite <= Global::updatesPL->get())) {  // if there is convergence before the next data interval
				shared_ptr<Organism> current = LOD[nextDataWrite - lastPrune];
				current->dataMap.set("update", nextDataWrite);
				current->dataMap.setOutputBehavior("update", DataMap::FIRST);
				TTC = max(0, current->timeOfBirth - real_MRCA->timeOfBirth);
				current->dataMap.set("timeToCoalescence", TTC);
				current->dataMap.setOutputBehavior("timeToCoalescence", DataMap::FIRST);
				current->dataMap.writeToFile(DataFileName, files[DataFileName]);  // append new data to the file
				current->dataMap.clear("update");
				current->dataMap.clear("timeToCoalescence");
				if ((int) dataSequence.size() > dataSeqIndex + 1) {
					dataSeqIndex++;
					nextDataWrite = dataSequence[dataSeqIndex];
				} else {
					nextDataWrite = Global::updatesPL->get() + terminateAfter + 1;
				}
			}
			if (flush) {
				cout << "Most Recent Common Ancestor/Time to Coalescence was " << TTC << " updates ago."<<endl;
			}
		}

		//Save Organisms
		if (writeOrganismFile) {

			while ((effective_MRCA->timeOfBirth >= nextOrganismWrite) && (nextOrganismWrite <= Global::updatesPL->get())) {  // if there is convergence before the next data interval

				shared_ptr<Organism> current = LOD[nextOrganismWrite - lastPrune];

				DataMap OrgMap;
				OrgMap.set("ID", current->ID);
				OrgMap.set("update", nextOrganismWrite);
				OrgMap.setOutputBehavior("update", DataMap::FIRST);
				string tempName;

				for (auto genome : current->genomes) {
					tempName = "GENOME_" + genome.first;
					OrgMap.merge(genome.second->serialize(tempName));
				}
				for (auto brain : current->brains) {
					tempName = "BRAIN_" + brain.first;
					OrgMap.merge(brain.second->serialize(tempName));
				}
				OrgMap.writeToFile(OrganismFileName); // append new data to the file

				if ((int) organismSequence.size() > organismSeqIndex + 1) {
					organismSeqIndex++;
					nextOrganismWrite = organismSequence[organismSeqIndex];
				} else {
					nextOrganismWrite = Global::updatesPL->get() + terminateAfter + 1;
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
	//cout << Global::update << " >= " << Global::updatesPL->get() << " + " << terminateAfter << endl;
	finished = finished || (Global::update >= Global::updatesPL->get() + terminateAfter || lastPrune >= Global::updatesPL->get());

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
