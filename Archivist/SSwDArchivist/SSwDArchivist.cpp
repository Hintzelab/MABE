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

shared_ptr<ParameterLink<string>> SSwDArchivist::SSwD_Arch_dataSequenceStrPL = Parameters::register_parameter("ARCHIVIST_SSWD-dataSequence", (string) ":100",
	"when to save a data file (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
shared_ptr<ParameterLink<string>> SSwDArchivist::SSwD_Arch_organismSequenceStrPL = Parameters::register_parameter("ARCHIVIST_SSWD-organismSequence", (string) ":1000",
	"when to save a organism file (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
shared_ptr<ParameterLink<int>> SSwDArchivist::SSwD_Arch_dataDelayPL = Parameters::register_parameter("ARCHIVIST_SSWD-dataDelay", 10, "when using Snap Shot with Delay output Method, how long is the delay before saving data");
shared_ptr<ParameterLink<int>> SSwDArchivist::SSwD_Arch_organismDelayPL = Parameters::register_parameter("ARCHIVIST_SSWD-organismDelay", 10, "when using Snap Shot with Delay output Method, how long is the delay before saving organisms ");
shared_ptr<ParameterLink<int>> SSwDArchivist::SSwD_Arch_cleanupIntervalPL = Parameters::register_parameter("ARCHIVIST_SSWD-cleanupInterval", 100, "How often to cleanup old checkpoints");
shared_ptr<ParameterLink<string>> SSwDArchivist::SSwD_Arch_DataFilePrefixPL = Parameters::register_parameter("ARCHIVIST_SSWD-dataFilePrefix", (string) "SSwD_data", "name of genome file (stores genomes)");
shared_ptr<ParameterLink<string>> SSwDArchivist::SSwD_Arch_OrganismFilePrefixPL = Parameters::register_parameter("ARCHIVIST_SSWD-organismFilePrefix", (string) "SSwD_organism", "name of data file (stores everything but organism file data)");
shared_ptr<ParameterLink<bool>> SSwDArchivist::SSwD_Arch_writeDataFilesPL = Parameters::register_parameter("ARCHIVIST_SSWD-writeDataFiles", true, "if true, data files will be written");
shared_ptr<ParameterLink<bool>> SSwDArchivist::SSwD_Arch_writeOrganismFilesPL = Parameters::register_parameter("ARCHIVIST_SSWD-writeOrganismFiles", true, "if true, genome files will be written");

SSwDArchivist::SSwDArchivist(vector<string> popFileColumns, shared_ptr<Abstract_MTree> _maxFormula, shared_ptr<ParametersTable> _PT, string _groupPrefix) :
	DefaultArchivist(popFileColumns, _maxFormula, _PT, _groupPrefix) {

	dataDelay = (PT == nullptr) ? SSwD_Arch_dataDelayPL->lookup() : PT->lookupInt("ARCHIVIST_SSWD-dataDelay");
	organismDelay = (PT == nullptr) ? SSwD_Arch_organismDelayPL->lookup() : PT->lookupInt("ARCHIVIST_SSWD-organismDelay");

	cleanupInterval = (PT == nullptr) ? SSwD_Arch_cleanupIntervalPL->lookup() : PT->lookupInt("ARCHIVIST_SSWD-cleanupInterval");

	DataFilePrefix = (PT == nullptr) ? SSwD_Arch_DataFilePrefixPL->lookup() : PT->lookupString("ARCHIVIST_SSWD-dataFilePrefix");
	DataFilePrefix = (groupPrefix == "") ? DataFilePrefix : groupPrefix + "__" + DataFilePrefix;
	OrganismFilePrefix = (PT == nullptr) ? SSwD_Arch_OrganismFilePrefixPL->lookup() : PT->lookupString("ARCHIVIST_SSWD-organismFilePrefix");
	OrganismFilePrefix = (groupPrefix == "") ? OrganismFilePrefix : groupPrefix + "__" + OrganismFilePrefix;

	writeDataFiles = (PT == nullptr) ? SSwD_Arch_writeDataFilesPL->lookup() : PT->lookupBool("ARCHIVIST_SSWD-writeDataFiles");
	writeOrganismFiles = (PT == nullptr) ? SSwD_Arch_writeOrganismFilesPL->lookup() : PT->lookupBool("ARCHIVIST_SSWD-writeOrganismFiles");

	string dataSequenceStr = (PT == nullptr) ? SSwD_Arch_dataSequenceStrPL->lookup() : PT->lookupString("ARCHIVIST_SSWD-dataSequence");
	string organismSequenceStr = (PT == nullptr) ? SSwD_Arch_organismSequenceStrPL->lookup() : PT->lookupString("ARCHIVIST_SSWD-organismSequence");

	dataSequence.push_back(0);
	organismSequence.push_back(0);

	if (writeDataFiles != false) {
		dataSequence.clear();
		dataSequence = seq(dataSequenceStr, Global::updatesPL->lookup(), true);
		if (dataSequence.size() == 0) {
			cout << "unable to translate ARCHIVIST_SSWD-dataSequence \"" << dataSequenceStr << "\".\nExiting." << endl;
			exit(1);
		}
	}

	if (writeOrganismFiles != false) {
		organismSequence = seq(organismSequenceStr, Global::updatesPL->lookup(), true);
		if (organismSequence.size() == 0) {
			cout << "unable to translate ARCHIVIST_SSWD-organismSequence \"" << organismSequenceStr << "\".\nExiting." << endl;
			exit(1);
		}
	}
	nextDataWrite = dataSequence[0];
	nextOrganismWrite = organismSequence[0];
	nextDataCheckPoint = dataSequence[0];
	nextOrganismCheckPoint = organismSequence[0];

	writeDataSeqIndex = 0;
	checkPointDataSeqIndex = 0;
	writeOrganismSeqIndex = 0;
	checkPointOrganismSeqIndex = 0;

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
			if (checkpoint.first < (Global::update - max(dataDelay, organismDelay))) {  // if that checkpoint is older then the longest intervalDelay
				checkpointEmpty = true;
				for (auto weakPtrToOrg : checkpoints[checkpoint.first]) {  // than for each element in that checkpoint
					if (auto org = weakPtrToOrg.lock()) {  // if this ptr is still good
						if ((!org->alive) && (org->timeOfDeath < (Global::update - max(dataDelay, organismDelay)))) {  // and if the organism was dead before the current interesting data
							org->parents.clear();  // clear this organisms parents
						}
						else {
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

	if (finished && !flush) {
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

		///// Clean up the checkpoints
		if (Global::update % cleanupInterval == 0) {
			cleanup();
		}

		///// ADDING TO THE ARCHIVE

		if ((Global::update == nextOrganismCheckPoint && writeOrganismFiles) || (Global::update == nextDataCheckPoint && writeDataFiles)) {  // if we are at a data or genome interval...
			// we need to make a checkpoint of the current population
					
			
			// if this is a data snapshot update we need to collect some info (who will be saved and oldest org to be saved)
			vector<shared_ptr<Organism>> saveList;
			int minBrithTime = population[0]->timeOfBirth; // time of birth of oldest org being saved in this update (init with random value)
			if (Global::update == nextDataCheckPoint && Global::update <= Global::updatesPL->lookup()) {

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
			}

			for (auto org : population) {  // add the current population to checkPointTracker
				if (saveNewOrgs || org->timeOfBirth < Global::update) { // if we are saving all orgs or this org is atleast 1 update old...
					// ... checkpoint org
					checkpoints[Global::update].push_back(org);
					org->snapShotDataMaps[Global::update] = make_shared<DataMap>(org->dataMap);  // back up state of dataMap
				}
				if (Global::update == nextDataCheckPoint && Global::update <= Global::updatesPL->lookup()) {
					// if this is a data interval, add ancestors to snapshot dataMap
					// first we need to make sure that ancestor lists are up to date


					if (org->ancestors.size() != 1 || org->ancestors.find(org->ID) == org->ancestors.end()) {
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

						org->ancestors.clear();
						vector<shared_ptr<Organism>> parentCheckList = org->parents;

						while (parentCheckList.size() > 0) {
							auto parent = parentCheckList.back(); // this is "this parent"
							parentCheckList.pop_back(); // remove this parent from checklist

							//cout << "\n org: " << org->ID << " parent: " << parent->ID << endl;
							if (find(saveList.begin(), saveList.end(), parent) != saveList.end()) { // if this parent is being saved, they will serve as an ancestor
								org->ancestors.insert(parent->ID);
							}
							else { // this parent is not being saved
								if (parent->timeOfBirth < minBrithTime || (parent->ancestors.size() == 1 && parent->ancestors.find(parent->ID) != parent->ancestors.end())) {
									// if this parent is old enough that it can not have a parent in the save list (and is not in save list),
									// or this parent has self in it's ancestor list (i.e. it has already been saved to another file),
									// copy ancestors from this parent
									//cout << "getting ancestors for " << org->ID << " parent " << parent->ID << " is old enough or has self as ancestor..." << endl;
									for (auto ancestorID : parent->ancestors) {
										//cout << "adding from parent " << parent->ID << " ancestor " << ancestorID << endl;
										org->ancestors.insert(ancestorID);
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

					}
					else { // org has self for ancestor
						if (org->timeOfBirth >= Global::update) { // if this is a new org...
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





					if (saveNewOrgs || org->timeOfBirth < Global::update) { // if this org is set up to be saved in this snapshot
						for (auto ancestor : org->ancestors) {
							org->snapShotDataMaps[Global::update].Append("ancestors", ancestor);
						}
						org->ancestors.clear();  // clear ancestors (this data is safe in the checkPoint)
						org->ancestors.insert(org->ID);  // now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
														 // also, if this survives over intervals, it'll be pointing to self as ancestor in files (which is good)

					}
				}






				if (Global::update == nextOrganismCheckPoint && writeOrganismFiles) {
					org->trackOrganism = true; // since this update is in the genome sequence, set the flag to track genome
				}
			}
			if (Global::update == nextOrganismCheckPoint && Global::update <= Global::updatesPL->lookup()) {  // we have now made a genome checkpoint, advance nextGenomeCheckPoint to get ready for the next interval
				if ((int)organismSequence.size() > checkPointOrganismSeqIndex + 1) {
					checkPointOrganismSeqIndex++;
					nextOrganismCheckPoint = organismSequence[checkPointOrganismSeqIndex];
				}
				else {
					nextOrganismCheckPoint = Global::updatesPL->lookup() + 1;
				}
			}
			if (Global::update == nextDataCheckPoint && Global::update <= Global::updatesPL->lookup()) {  // we have now made a data checkpoint, advance nextDataCheckPoint to get ready for the next interval
				if ((int)dataSequence.size() > checkPointDataSeqIndex + 1) {
					checkPointDataSeqIndex++;
					nextDataCheckPoint = dataSequence[checkPointDataSeqIndex];
				}
				else {
					nextDataCheckPoint = Global::updatesPL->lookup() + 1;
				}
			}
		}

		////// WRITING FROM THE ARCHIVE

		////// WRITING ORGANISMS

		if ((Global::update == nextOrganismWrite + organismDelay) && (nextOrganismWrite <= Global::updatesPL->lookup()) && writeOrganismFiles) {  // now it's time to write genomes in the checkpoint at time nextGenomeWrite
			string organismFileName = OrganismFilePrefix + "_" + to_string(nextOrganismWrite) + ".csv";

			//string dataString;
			size_t index = 0;
			while (index < checkpoints[nextOrganismWrite].size()) {
				if (auto org = checkpoints[nextOrganismWrite][index].lock()) {  // this ptr is still good

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
					index++;
				}
				else {  // this ptr is expired - cut it out of the vector
					swap(checkpoints[nextOrganismWrite][index], checkpoints[nextOrganismWrite].back());  // swap expired ptr to back of vector
					checkpoints[nextOrganismWrite].pop_back();  // pop expired ptr from back of vector
				}
			}

			FileManager::closeFile(organismFileName); // since this is a snapshot, we will not be writting to this file again.


			if ((int)organismSequence.size() > writeOrganismSeqIndex + 1) {
				writeOrganismSeqIndex++;
				nextOrganismWrite = organismSequence[writeOrganismSeqIndex];  //genomeInterval;
			}
			else {
				nextOrganismWrite = Global::updatesPL->lookup() + 1;
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
					}
					else {  // it' empty, swap to back and remove.
						swap(checkpoints[nextDataWrite][0], checkpoints[nextDataWrite].back());  // swap expired ptr to back of vector
						checkpoints[nextDataWrite].pop_back();  // pop expired ptr from back of vector
					}
				}
				//processAllLists(org->snapShotDataMaps[nextDataWrite]);
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
					//processAllLists(org->snapShotDataMaps[nextDataWrite]);
					org->snapShotDataMaps[nextDataWrite].Set("update", nextDataWrite);
					org->snapShotDataMaps[nextDataWrite].setOutputBehavior("update", DataMap::FIRST);
					org->snapShotDataMaps[nextDataWrite].writeToFile(dataFileName, files["data"]);  // append new data to the file
					index++;  // advance to nex element
				}
				else {  // this ptr is expired - cut it out of the vector
					swap(checkpoints[nextDataWrite][index], checkpoints[nextDataWrite].back());  // swap expired ptr to back of vector
					checkpoints[nextDataWrite].pop_back();  // pop expired ptr from back of vector
				}
			}
			if ((int)dataSequence.size() > writeDataSeqIndex + 1) {
				writeDataSeqIndex++;
				nextDataWrite = dataSequence[writeDataSeqIndex];  //genomeInterval;
			}
			else {
				nextDataWrite = Global::updatesPL->lookup() + 1;
			}
		}
	}
	// if enough time has passed to save all data and genomes, then we are done!
	finished = finished || ((nextDataWrite > Global::updatesPL->lookup() || !(writeDataFiles)) && (nextOrganismWrite > Global::updatesPL->lookup() || !(writeOrganismFiles)) && Global::update >= Global::updatesPL->lookup());
	return finished;
}

bool SSwDArchivist::isDataUpdate(int checkUpdate) {
	if (checkUpdate == -1) {
		checkUpdate = Global::update;
	}
	bool check = DefaultArchivist::isDataUpdate(checkUpdate);
	check = check || find(dataSequence.begin(), dataSequence.end(), checkUpdate) != dataSequence.end();
	return check;
}

bool SSwDArchivist::isOrganismUpdate(int checkUpdate) {
	if (checkUpdate == -1) {
		checkUpdate = Global::update;
	}
	bool check = DefaultArchivist::isOrganismUpdate(checkUpdate);
	check = check || find(organismSequence.begin(), organismSequence.end(), checkUpdate) != organismSequence.end();
	return check;
}
