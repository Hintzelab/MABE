//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License


#include "SSwDArchivist.h"

shared_ptr<ParameterLink<string>> SSwDArchivist::SSwD_Arch_dataSequenceStrPL = Parameters::register_parameter("ARCHIVIST_SSWD-dataSequence", (string)":100", "when to save a data file (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
shared_ptr<ParameterLink<string>> SSwDArchivist::SSwD_Arch_genomeSequenceStrPL = Parameters::register_parameter("ARCHIVIST_SSWD-genomeSequence", (string)":1000", "when to save a genome file (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
shared_ptr<ParameterLink<int>> SSwDArchivist::SSwD_Arch_dataDelayPL = Parameters::register_parameter("ARCHIVIST_SSWD-dataDelay", 10, "when using Snap Shot with Delay output Method, how long is the delay before saving data");
shared_ptr<ParameterLink<int>> SSwDArchivist::SSwD_Arch_genomeDelayPL = Parameters::register_parameter("ARCHIVIST_SSWD-genomeDelay", 10, "when using Snap Shot with Delay output Method, how long is the delay before saving genomes ");
shared_ptr<ParameterLink<int>> SSwDArchivist::SSwD_Arch_cleanupIntervalPL = Parameters::register_parameter("ARCHIVIST_SSWD-cleanupInterval", 100, "How often to cleanup old checkpoints");
shared_ptr<ParameterLink<string>> SSwDArchivist::SSwD_Arch_DataFilePrefixPL = Parameters::register_parameter("ARCHIVIST_SSWD-dataFilePrefix", (string) "data", "name of genome file (stores genomes)");
shared_ptr<ParameterLink<string>> SSwDArchivist::SSwD_Arch_GenomeFilePrefixPL = Parameters::register_parameter("ARCHIVIST_SSWD-genomeFilePrefix", (string) "genome", "name of data file (stores everything but genomes)");
shared_ptr<ParameterLink<bool>> SSwDArchivist::SSwD_Arch_writeDataFilesPL = Parameters::register_parameter("ARCHIVIST_SSWD-writeDataFiles", true, "if true, data files will be written");
shared_ptr<ParameterLink<bool>> SSwDArchivist::SSwD_Arch_writeGenomeFilesPL = Parameters::register_parameter("ARCHIVIST_SSWD-writeGenomeFiles", true, "if true, genome files will be written");

SSwDArchivist::SSwDArchivist(vector<string> aveFileColumns, shared_ptr<ParametersTable> _PT) :
		DefaultArchivist(aveFileColumns, _PT) {

	string dataSequenceStr = (PT == nullptr) ? SSwD_Arch_dataSequenceStrPL->lookup() : PT->lookupString("ARCHIVIST_SSWD-dataSequence");
	string genomeSequenceStr = (PT == nullptr) ? SSwD_Arch_genomeSequenceStrPL->lookup() : PT->lookupString("ARCHIVIST_SSWD-genomeSequence");

	dataSequence = seq(dataSequenceStr,Global::updatesPL->lookup(),true);
	if (dataSequence.size() == 0) {
		cout << "unable to translate ARCHIVIST_SSWD-dataSequence \"" << dataSequenceStr << "\".\nExiting."<<endl;
		exit(1);
	}
	genomeSequence = seq(genomeSequenceStr,Global::updatesPL->lookup(),true);
	if (genomeSequence.size() == 0) {
		cout << "unable to translate ARCHIVIST_SSWD-genomeSequence \"" << genomeSequenceStr << "\".\nExiting."<<endl;
		exit(1);
	}

	dataDelay = (PT == nullptr) ? SSwD_Arch_dataDelayPL->lookup() : PT->lookupInt("ARCHIVIST_SSWD-dataDelay");
	genomeDelay = (PT == nullptr) ? SSwD_Arch_genomeDelayPL->lookup() : PT->lookupInt("ARCHIVIST_SSWD-genomeDelay");

	cleanupInterval = (PT == nullptr) ? SSwD_Arch_cleanupIntervalPL->lookup() : PT->lookupInt("ARCHIVIST_SSWD-cleanupInterval");

	DataFilePrefix = (PT == nullptr) ? SSwD_Arch_DataFilePrefixPL->lookup() : PT->lookupString("ARCHIVIST_SSWD-dataFilePrefix");
	GenomeFilePrefix = (PT == nullptr) ? SSwD_Arch_GenomeFilePrefixPL->lookup() : PT->lookupString("ARCHIVIST_SSWD-genomeFilePrefix");

	writeDataFiles = (PT == nullptr) ? SSwD_Arch_writeDataFilesPL->lookup() : PT->lookupBool("ARCHIVIST_SSWD-writeDataFiles");
	writeGenomeFiles = (PT == nullptr) ? SSwD_Arch_writeGenomeFilesPL->lookup() : PT->lookupBool("ARCHIVIST_SSWD-writeGenomeFiles");

	nextDataWrite = dataSequence[0];
	nextGenomeWrite = genomeSequence[0];
	nextDataCheckPoint = dataSequence[0];
	nextGenomeCheckPoint = genomeSequence[0];

	writeDataSeqIndex = 0;
	checkPointDataSeqIndex = 0;
	writeGenomeSeqIndex = 0;
	checkPointGenomeSeqIndex = 0;

}

///// CLEANUP / DELETE STALE CHECKPOINTS
// if a checkpoint is from before Global::update - archivist::intervalDelay than delete the checkpoint
// and all of it's org parents (with clear) assuming org was dead at the time
// this will have the effect of a delayed pruning, but should do a good enough job keeping memory down.
void SSwDArchivist::cleanup() {

	{
		vector<int> expiredCheckPoints;
		bool checkpointEmpty;
		for (auto checkpoint : checkpoints) {  // for every checkpoint
			if (checkpoint.first < (Global::update - max(dataDelay, genomeDelay))) {  // if that checkpoint is older then the longest intervalDelay
				checkpointEmpty = true;
				for (auto weakPtrToOrg : checkpoints[checkpoint.first]) {  // than for each element in that checkpoint
					if (auto org = weakPtrToOrg.lock()) {  // if this ptr is still good
						if ((!org->alive) && (org->timeOfDeath < (Global::update - max(dataDelay, genomeDelay)))) {  // and if the organism was dead before the current interesting data
							org->parents.clear();  // clear this organisms parents
						} else {
							checkpointEmpty = false;  // there is an organism in this checkpoint that was alive later then (Global::update - intervalDelay)
													  // this could organism could still be active
						}
					}
				}
				if (checkpointEmpty) {  // if all of the organisims in this checkpoint have been dead for a good long time...
										// if this checkpoint is not empty, we want to keep it so we can check it again later.
					expiredCheckPoints.push_back(checkpoint.first);  // add this checkpoint to a list of checkpoints to be deleted
				}
			}
		}
		for (auto checkPointTime : expiredCheckPoints) {  // for each checkpoint in the too be deleted list
			checkpoints.erase(checkPointTime);  // delete this checkpoint
		}
	}
}

bool SSwDArchivist::archive(vector<shared_ptr<Organism>> population, int flush) {

	if (flush != 1) {

		if ((Global::update == realtimeSequence[realtimeSequenceIndex]) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
			writeRealTimeFiles(population);  // write to dominant and average files
			if (realtimeSequenceIndex + 1 < (int)realtimeSequence.size()){
				realtimeSequenceIndex++;
			}
		}

		if ((Global::update == realtimeDataSequence[realtimeDataSeqIndex]) && (flush == 0) && writeSnapshotDataFiles) {  // do not write files on flush - these organisms have not been evaluated!
			saveSnapshotData(population);
			if (realtimeDataSeqIndex + 1 < (int)realtimeDataSequence.size()){
				realtimeDataSeqIndex++;
			}
		}
		if ((Global::update == realtimeGenomeSequence[realtimeGenomeSeqIndex]) && (flush == 0) && writeSnapshotGenomeFiles) {  // do not write files on flush - these organisms have not been evaluated!
			saveSnapshotGenomes(population);
			if (realtimeGenomeSeqIndex + 1 < (int)realtimeGenomeSequence.size()){
				realtimeGenomeSeqIndex++;
			}
		}

		///// Clean up the checkpoints
		if (Global::update % cleanupInterval == 0) {
			cleanup();
		}

		///// ADDING TO THE ARCHIVE

		if ((Global::update == nextGenomeCheckPoint && writeGenomeFiles) || (Global::update == nextDataCheckPoint && writeDataFiles)) {  // if we are at a data or genome interval...
			// we need to make a checkpoint of the current population
			for (auto org : population) {  // add the current population to checkPointTracker
				checkpoints[Global::update].push_back(org);
				org->snapShotDataMaps[Global::update] = make_shared<DataMap>(org->dataMap);  // back up state of dataMap

				if (Global::update == nextDataCheckPoint && Global::update <= Global::updatesPL->lookup()) {  // if this is a data interval, add ancestors to snapshot dataMap
					for (auto ancestor : org->ancestors) {
						org->snapShotDataMaps[Global::update].Append("ancestors", ancestor);
					}
					org->ancestors.clear();  // clear ancestors (this data is safe in the checkPoint)
					org->ancestors.insert(org->ID);  // now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
													 // also, if this survives over intervals, it'll be pointing to self as ancestor in files (which is good)
				}
			}
			if (Global::update == nextGenomeCheckPoint && Global::update <= Global::updatesPL->lookup()) {  // we have now made a genome checkpoint, advance nextGenomeCheckPoint to get ready for the next interval
				if ((int)genomeSequence.size() > checkPointGenomeSeqIndex + 1){
					checkPointGenomeSeqIndex++;
					nextGenomeCheckPoint = genomeSequence[checkPointGenomeSeqIndex];
				} else {
					nextGenomeCheckPoint = Global::updatesPL->lookup() + 1;
				}
			}
			if (Global::update == nextDataCheckPoint && Global::update <= Global::updatesPL->lookup()) {  // we have now made a data checkpoint, advance nextDataCheckPoint to get ready for the next interval
				if ((int)dataSequence.size() > checkPointDataSeqIndex + 1){
					checkPointDataSeqIndex++;
					nextDataCheckPoint = dataSequence[checkPointDataSeqIndex];
				} else {
					nextDataCheckPoint = Global::updatesPL->lookup() + 1;
				}
			}
		}

		////// WRITING FROM THE ARCHIVE

		////// WRITING GENOMES

		if ((Global::update == nextGenomeWrite + genomeDelay) && (nextGenomeWrite <= Global::updatesPL->lookup()) && writeGenomeFiles) {  // now it's time to write genomes in the checkpoint at time nextGenomeWrite
			string genomeFileName = GenomeFilePrefix + "_" + to_string(nextGenomeWrite) + ".csv";

			//string dataString;
			size_t index = 0;
			while (index < checkpoints[nextGenomeWrite].size()) {
				if (auto org = checkpoints[nextGenomeWrite][index].lock()) {  // this ptr is still good

					org->genome->dataMap.Set("ID", org->dataMap.Get("ID"));
					org->genome->dataMap.Set("update", to_string(nextGenomeWrite));

					org->genome->dataMap.Set("sites", org->genome->genomeToStr());

					org->genome->dataMap.writeToFile(genomeFileName, org->genome->genomeFileColumns);  // append new data to the file
					org->genome->dataMap.Clear("sites");  // this is large, clean it up now!
					index++;
				} else {  // this ptr is expired - cut it out of the vector
					swap(checkpoints[nextGenomeWrite][index], checkpoints[nextGenomeWrite].back());  // swap expired ptr to back of vector
					checkpoints[nextGenomeWrite].pop_back();  // pop expired ptr from back of vector
				}
			}
			if ((int)genomeSequence.size() > writeGenomeSeqIndex + 1){
				writeGenomeSeqIndex++;
				nextGenomeWrite = genomeSequence[writeGenomeSeqIndex];//genomeInterval;
			} else {
				nextGenomeWrite = Global::updatesPL->lookup() + 1;
			}
		}

		////// WRITING DATA

		if ((Global::update == nextDataWrite + dataDelay) && (nextDataWrite <= Global::updatesPL->lookup()) && writeDataFiles) {  // now it's time to write data in the checkpoint at time nextDataWrite
			string dataFileName = DataFilePrefix + "_" + to_string(nextDataWrite) + ".csv";

			// if file info has not been initialized yet, find a valid org and extract it's keys
			if (files.find("data") == files.end()) {
				bool found = false;
				shared_ptr<Organism> org;

				while (!found) {  // check each org in checkPointTraker[nextDataWrite] until we find a valid org
					if (auto temp_org = checkpoints[nextDataWrite][0].lock()) {  // this ptr is still good
						org = temp_org;
						found = true;
					} else {  // it' empty, swap to back and remove.
						swap(checkpoints[nextDataWrite][0], checkpoints[nextDataWrite].back());  // swap expired ptr to back of vector
						checkpoints[nextDataWrite].pop_back();  // pop expired ptr from back of vector
					}
				}
				processAllLists(org->snapShotDataMaps[nextDataWrite]);
				vector<string> tempKeysList = org->snapShotDataMaps[nextDataWrite].getKeys();  // get all keys from the valid orgs dataMap (all orgs should have the same keys in their dataMaps)
				files["data"].push_back("update");
				for (auto key : tempKeysList) {  // for every key in dataMap...
					files["data"].push_back(key);  // add it to the list of keys associated with the data file.
				}
			}

			// write out data for all orgs in checkPointTracker[Global::nextGenomeWrite] to "genome_" + to_string(Global::nextGenomeWrite) + ".csv"

			size_t index = 0;
			while (index < checkpoints[nextDataWrite].size()) {
				if (auto org = checkpoints[nextDataWrite][index].lock()) {  // this ptr is still good
					processAllLists(org->snapShotDataMaps[nextDataWrite]);
					org->snapShotDataMaps[nextDataWrite].Set("update",nextDataWrite);
					org->snapShotDataMaps[nextDataWrite].writeToFile(dataFileName, files["data"]);  // append new data to the file
					index++;  // advance to nex element
				} else {  // this ptr is expired - cut it out of the vector
					swap(checkpoints[nextDataWrite][index], checkpoints[nextDataWrite].back());  // swap expired ptr to back of vector
					checkpoints[nextDataWrite].pop_back();  // pop expired ptr from back of vector
				}
			}
			if ((int)dataSequence.size() > writeDataSeqIndex + 1){
				writeDataSeqIndex++;
				nextDataWrite = dataSequence[writeDataSeqIndex];//genomeInterval;
			} else {
				nextDataWrite = Global::updatesPL->lookup() + 1;
			}
		}
	}
	// if enough time has passed to save all data and genomes, then we are done!
	return ((nextDataWrite > Global::updatesPL->lookup() || !(writeDataFiles)) && (nextGenomeWrite > Global::updatesPL->lookup() || !(writeGenomeFiles)) && Global::update >= Global::updatesPL->lookup());
}
