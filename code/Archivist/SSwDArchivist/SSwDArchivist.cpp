//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "SSwDArchivist.h"

std::shared_ptr<ParameterLink<std::string>> SSwDArchivist::SSwD_Arch_dataSequenceStrPL =
    Parameters::register_parameter(
        "ARCHIVIST_SSWD-dataSequence", (std::string) ":100",
        "when to save a data file (format: x = single value, x-y = x to y, "
        "x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to "
        "'updates' on z) e.g. '1-100:10, 200, 300:100'");
std::shared_ptr<ParameterLink<std::string>>
    SSwDArchivist::SSwD_Arch_organismSequenceStrPL =
        Parameters::register_parameter(
            "ARCHIVIST_SSWD-organismsSequence", (std::string) ":1000",
            "when to save an organisms file (format: x = single value, x-y = x "
            "to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = "
            "from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
std::shared_ptr<ParameterLink<int>> SSwDArchivist::SSwD_Arch_dataDelayPL =
    Parameters::register_parameter("ARCHIVIST_SSWD-dataDelay", 10,
                                   "when using Snap Shot with Delay output "
                                   "Method, how long is the delay before "
                                   "saving data");
std::shared_ptr<ParameterLink<int>> SSwDArchivist::SSwD_Arch_organismDelayPL =
    Parameters::register_parameter("ARCHIVIST_SSWD-organismsDelay", 10,
                                   "when using Snap Shot with Delay output "
                                   "Method, how long is the delay before "
                                   "saving organisms ");
std::shared_ptr<ParameterLink<int>> SSwDArchivist::SSwD_Arch_cleanupIntervalPL =
    Parameters::register_parameter("ARCHIVIST_SSWD-cleanupInterval", 100,
                                   "How often to cleanup old checkpoints");
std::shared_ptr<ParameterLink<std::string>> SSwDArchivist::SSwD_Arch_FilePrefixPL =
    Parameters::register_parameter("ARCHIVIST_SSWD-filePrefix", (std::string) "NONE",
                                   "prefix for files saved by this archivst. "
                                   "\"NONE\" indicates no prefix.");
std::shared_ptr<ParameterLink<bool>> SSwDArchivist::SSwD_Arch_writeDataFilesPL =
    Parameters::register_parameter("ARCHIVIST_SSWD-writeDataFiles", true,
                                   "if true, data files will be written");
std::shared_ptr<ParameterLink<bool>> SSwDArchivist::SSwD_Arch_writeOrganismFilesPL =
    Parameters::register_parameter("ARCHIVIST_SSWD-writeOrganismsFiles", true,
                                   "if true, genome files will be written");

SSwDArchivist::SSwDArchivist(std::vector<std::string> popFileColumns,
                             std::shared_ptr<Abstract_MTree> _maxFormula,
                             std::shared_ptr<ParametersTable> PT_,
                             std::string group_prefix)
    : DefaultArchivist(popFileColumns, _maxFormula, PT_, group_prefix) {

  dataDelay = SSwD_Arch_dataDelayPL->get(PT);
  organismDelay = SSwD_Arch_organismDelayPL->get(PT);

  cleanupInterval = SSwD_Arch_cleanupIntervalPL->get(PT);

  DataFilePrefix = (group_prefix_ == "")
                       ? "SSwD_data"
                       : group_prefix_.substr(0, group_prefix_.size() - 2) +
                             "__" + "SSwD_data";
  DataFilePrefix = (SSwD_Arch_FilePrefixPL->get(PT) == "NONE")
                       ? DataFilePrefix
                       : SSwD_Arch_FilePrefixPL->get(PT) + DataFilePrefix;
  OrganismFilePrefix = (group_prefix_ == "")
                           ? "SSwD_organisms"
                           : group_prefix_.substr(0, group_prefix_.size() - 2) +
                                 "__" + "SSwD_organisms";
  OrganismFilePrefix =
      (SSwD_Arch_FilePrefixPL->get(PT) == "NONE")
          ? OrganismFilePrefix
          : SSwD_Arch_FilePrefixPL->get(PT) + OrganismFilePrefix;

  writeDataFiles = SSwD_Arch_writeDataFilesPL->get(PT);
  writeOrganismFiles = SSwD_Arch_writeOrganismFilesPL->get(PT);

  auto dataSequenceStr = SSwD_Arch_dataSequenceStrPL->get(PT);
  auto organismSequenceStr = SSwD_Arch_organismSequenceStrPL->get(PT);

  dataSequence.push_back(0);
  organismSequence.push_back(0);

  if (writeDataFiles != false) {
    dataSequence.clear();
    dataSequence = seq(dataSequenceStr, Global::updatesPL->get(), true);
    if (dataSequence.size() == 0) {
      std::cout << "unable to translate ARCHIVIST_SSWD-dataSequence \""
           << dataSequenceStr << "\".\nExiting." << std::endl;
      exit(1);
    }
  }

  if (writeOrganismFiles != false) {
    organismSequence = seq(organismSequenceStr, Global::updatesPL->get(), true);
    if (organismSequence.size() == 0) {
      std::cout << "unable to translate ARCHIVIST_SSWD-organismsSequence \""
           << organismSequenceStr << "\".\nExiting." << std::endl;
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
// if a checkpoint is from before Global::update - archivist::intervalDelay than
// delete the checkpoint
// and all of it's org parents (with clear) assuming org was dead at the time
// this will have the effect of a delayed pruning, but should do a good enough
// job keeping memory down.
void SSwDArchivist::cleanup() {

  {
    std::vector<int> expiredCheckPoints;
    bool checkpointEmpty;
    for (auto checkpoint : checkpoints) { // for every checkpoint
      if (checkpoint.first <
          (Global::update -
           std::max(dataDelay, organismDelay))) { // if that checkpoint is older then
                                             // the longest intervalDelay
        checkpointEmpty = true;
        for (auto weakPtrToOrg :
             checkpoints[checkpoint.first]) {   // than for each element in that
                                                // checkpoint
          if (auto org = weakPtrToOrg.lock()) { // if this ptr is still good
            if ((!org->alive) &&
                (org->timeOfDeath <
                 (Global::update -
                  std::max(dataDelay, organismDelay)))) { // and if the organism was
                                                     // dead before the current
                                                     // interesting data
              // org->parents.clear();  // clear this organisms parents :: NOTE
              // this was assuming that Default snapshot was not interested in
              // parents.
              // really we can only clear parents if org is older then oldest in
              // current populaiton This is not addressed later in this function
            } else {
              checkpointEmpty = false; // there is an organism in this
                                       // checkpoint that was alive later then
                                       // (Global::update - intervalDelay)
              // this could organism could still be active
            }
          }
        }
        if (checkpointEmpty) { // if all of the organisims in this checkpoint
                               // have been dead for a good long time...
          // if this checkpoint is not empty, we want to keep it so we can check
          // it again later.
          expiredCheckPoints.push_back(checkpoint.first); // add this checkpoint
                                                          // to a list of
                                                          // checkpoints to be
                                                          // deleted
        }
      }
    }
    for (auto checkPointTime :
         expiredCheckPoints) { // for each checkpoint in the too be deleted list
      checkpoints.erase(checkPointTime); // delete this checkpoint
    }
  }
}

bool SSwDArchivist::archive(std::vector<std::shared_ptr<Organism>> &population,
                            int flush) {

  if (finished_ && !flush) {
    return finished_;
  }

  if (flush != 1) {

    if ((Global::update == realtimeSequence[realtime_sequence_index_]) &&
        (flush == 0)) { // do not write files on flush - these organisms have
                        // not been evaluated!
      writeRealTimeFiles(population); // write to Max and average files
      if (realtime_sequence_index_ + 1 < (int)realtimeSequence.size()) {
        realtime_sequence_index_++;
      }
    }

    if ((Global::update == realtimeDataSequence[realtime_data_seq_index_]) &&
        (flush == 0) &&
        writeSnapshotDataFiles) { // do not write files on flush - these
                                  // organisms have not been evaluated!
      saveSnapshotData(population);
      if (realtime_data_seq_index_ + 1 < (int)realtimeDataSequence.size()) {
        realtime_data_seq_index_++;
      }
    }
    if ((Global::update ==
         realtimeOrganismSequence[realtime_organism_seq_index_]) &&
        (flush == 0) &&
        writeSnapshotGenomeFiles) { // do not write files on flush - these
                                    // organisms have not been evaluated!
      saveSnapshotOrganisms(population);
      if (realtime_organism_seq_index_ + 1 <
          (int)realtimeOrganismSequence.size()) {
        realtime_organism_seq_index_++;
      }
    }

    ///// Clean up the checkpoints
    if (Global::update % cleanupInterval == 0) {
      cleanup();
    }

    ///// ADDING TO THE ARCHIVE

    if ((Global::update == nextOrganismCheckPoint && writeOrganismFiles) ||
        (Global::update == nextDataCheckPoint &&
         writeDataFiles)) { // if we are at a data or genome interval...
      // we need to make a checkpoint of the current population

      // if this is a data snapshot update we need to collect some info (who
      // will be saved and oldest org to be saved)
      std::vector<std::shared_ptr<Organism>> saveList;
      int minBirthTime =
          population[0]->timeOfBirth; // time of birth of oldest org being saved
                                      // in this update (init with random value)
      if (Global::update == nextDataCheckPoint &&
          Global::update <= Global::updatesPL->get()) {

        if (save_new_orgs_) {
          saveList = population;
          for (auto org : population) {
            minBirthTime = std::min(org->timeOfBirth, minBirthTime);
          }
        } else {
          for (auto org : population) {
            if (org->timeOfBirth < Global::update) {
              saveList.push_back(org);
            }
            minBirthTime = std::min(org->timeOfBirth, minBirthTime);
          }
        }
      }

      for (auto org :
           population) { // add the current population to checkPointTracker
        if (save_new_orgs_ ||
            org->timeOfBirth < Global::update) { // if we are saving all orgs or
                                                 // this org is atleast 1 update
                                                 // old...
          // ... checkpoint org
          checkpoints[Global::update].push_back(org);
          org->snapShotDataMaps[Global::update] =
              std::make_shared<DataMap>(org->dataMap); // back up state of dataMap
        }
        if (Global::update == nextDataCheckPoint &&
            Global::update <= Global::updatesPL->get()) {
          // if this is a data interval, add ancestors to snapshot dataMap
          // first we need to make sure that ancestor lists are up to date

          if (org->ancestors.size() != 1 ||
              org->ancestors.find(org->ID) == org->ancestors.end()) {
            // if this org does not only contain only itself in
            // snapshotAncestors then it has not been saved before.
            // we must confirm that snapshotAncestors is correct because things
            // may have changed while we were not looking
            // this process does 2 things:
            // a) if this org is being saved then it makes sure it's up to date
            // b) it makes sure that it's ancestor list is correct so that it's
            // offspring will pass on the correct ancestor info.
            //
            // How does it work? (good question)
            // get a checklist of parents of the current org
            // for each parent, if they are going to be saved in this update,
            // yay, we can just assign their ID to the ancestor list
            // ... if they are not going to be saved then we need to check their
            // ancestors to see if they are going to be saved,
            // unless they are atleast as old as the oldest org being saved to
            // this file.
            // if they are at least as old as the oldest org being saved to this
            // file then we can simply append their ancestors

            org->ancestors.clear();
            std::vector<std::shared_ptr<Organism>> parentCheckList = org->parents;

            while (parentCheckList.size() > 0) {
              auto parent = parentCheckList.back(); // this is "this parent"
              parentCheckList.pop_back(); // remove this parent from checklist

              // cout << "\n org: " << org->ID << " parent: " << parent->ID <<
              // endl;
              if (find(saveList.begin(), saveList.end(), parent) !=
                  saveList.end()) { // if this parent is being saved, they will
                                    // serve as an ancestor
                org->ancestors.insert(parent->ID);
              } else { // this parent is not being saved
                if (parent->timeOfBirth < minBirthTime ||
                    (parent->ancestors.size() == 1 &&
                     parent->ancestors.find(parent->ID) !=
                         parent->ancestors.end())) {
                  // if this parent is old enough that it can not have a parent
                  // in the save list (and is not in save list),
                  // or this parent has self in it's ancestor list (i.e. it has
                  // already been saved to another file),
                  // copy ancestors from this parent
                  // cout << "getting ancestors for " << org->ID << " parent "
                  // << parent->ID << " is old enough or has self as
                  // ancestor..." << endl;
                  for (auto ancestorID : parent->ancestors) {
                    // cout << "adding from parent " << parent->ID << " ancestor
                    // " << ancestorID << endl;
                    org->ancestors.insert(ancestorID);
                  }
                } else { // this parent not old enough (see if above), add this
                         // parents parents to check list (we need to keep
                         // looking)
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
            if (org->timeOfBirth >= Global::update) { // if this is a new org...
              std::cout
                  << "  WARRNING :: in SSwD::archive(), while adding to a "
                     "snapshot data found new org (age < 1) with self as "
                     "ancestor (with ID: "
                  << org->ID
                  << "... this will result in a new root to the phylogony tree!"
                  << std::endl;
              if (save_new_orgs_) {
                std::cout << "    this org is being saved" << std::endl;
              } else {
                std::cout << "    this org is not being saved (this may be very "
                        "bad...)"
                     << std::endl;
              }
            }
          }
          // now that we know that ancestor list is good for this org...

          if (save_new_orgs_ ||
              org->timeOfBirth < Global::update) { // if this org is set up to
                                                   // be saved in this snapshot
            for (auto ancestor : org->ancestors) {
              org->snapShotDataMaps[Global::update].append("ancestors",
                                                           ancestor);
            }
            org->ancestors.clear(); // clear ancestors (this data is safe in the
                                    // checkPoint)
            org->ancestors.insert(org->ID); // now that we have saved the
                                            // ancestor data, set ancestors to
                                            // self (so that others will inherit
                                            // correctly)
            // also, if this survives over intervals, it'll be pointing to self
            // as ancestor in files (which is good)
          }
        }

        if (Global::update == nextOrganismCheckPoint && writeOrganismFiles) {
          org->trackOrganism = true; // since this update is in the genome
                                     // sequence, set the flag to track genome
        }
      }
      if (Global::update == nextOrganismCheckPoint &&
          Global::update <=
              Global::updatesPL->get()) { // we have now made a genome
                                          // checkpoint, advance
                                          // nextGenomeCheckPoint to get ready
                                          // for the next interval
        if ((int)organismSequence.size() > checkPointOrganismSeqIndex + 1) {
          checkPointOrganismSeqIndex++;
          nextOrganismCheckPoint = organismSequence[checkPointOrganismSeqIndex];
        } else {
          nextOrganismCheckPoint = Global::updatesPL->get() + 1;
        }
      }
      if (Global::update == nextDataCheckPoint &&
          Global::update <=
              Global::updatesPL->get()) { // we have now made a data checkpoint,
                                          // advance nextDataCheckPoint to get
                                          // ready for the next interval
        if ((int)dataSequence.size() > checkPointDataSeqIndex + 1) {
          checkPointDataSeqIndex++;
          nextDataCheckPoint = dataSequence[checkPointDataSeqIndex];
        } else {
          nextDataCheckPoint = Global::updatesPL->get() + 1;
        }
      }
    }

    ////// WRITING FROM THE ARCHIVE

    ////// WRITING ORGANISMS

    if ((Global::update == nextOrganismWrite + organismDelay) &&
        (nextOrganismWrite <= Global::updatesPL->get()) &&
        writeOrganismFiles) { // now it's time to write genomes in the
                              // checkpoint at time nextGenomeWrite
      auto organismFileName =
          OrganismFilePrefix + "_" + std::to_string(nextOrganismWrite) + ".csv";

      // string dataString;
      size_t index = 0;
      while (index < checkpoints[nextOrganismWrite].size()) {
        if (auto org = checkpoints[nextOrganismWrite][index]
                           .lock()) { // this ptr is still good

          DataMap OrgMap;
          OrgMap.set("ID", org->ID);
          std::string tempName;

          for (auto genome : org->genomes) {
            tempName = "GENOME_" + genome.first;
            OrgMap.merge(genome.second->serialize(tempName));
          }
          for (auto brain : org->brains) {
            tempName = "BRAIN_" + brain.first;
            OrgMap.merge(brain.second->serialize(tempName));
          }
          OrgMap.openAndWriteToFile(organismFileName); // append new data to the file
          index++;
        } else { // this ptr is expired - cut it out of the vector
          swap(checkpoints[nextOrganismWrite][index],
               checkpoints[nextOrganismWrite]
                   .back()); // swap expired ptr to back of vector
          checkpoints[nextOrganismWrite]
              .pop_back(); // pop expired ptr from back of vector
        }
      }

      FileManager::closeFile(organismFileName); // since this is a snapshot, we
                                                // will not be writting to this
                                                // file again.

      if ((int)organismSequence.size() > writeOrganismSeqIndex + 1) {
        writeOrganismSeqIndex++;
        nextOrganismWrite =
            organismSequence[writeOrganismSeqIndex]; // genomeInterval;
      } else {
        nextOrganismWrite = Global::updatesPL->get() + 1;
      }
    }

    ////// WRITING DATA

    if ((Global::update == nextDataWrite + dataDelay) &&
        (nextDataWrite <= Global::updatesPL->get()) &&
        writeDataFiles) { // now it's time to write data in the checkpoint at
                          // time nextDataWrite
      std::string dataFileName =
          DataFilePrefix + "_" + std::to_string(nextDataWrite) + ".csv";

      // if file info has not been initialized yet, find a valid org and extract
      // it's keys
      if (files_.find("data") == files_.end()) {
        bool found = false;
        std::shared_ptr<Organism> org;

        while (!found) { // check each org in checkPointTraker[nextDataWrite]
                         // until we find a valid org
          if (auto temp_org = checkpoints[nextDataWrite][0]
                                  .lock()) { // this ptr is still good
            org = temp_org;
            found = true;
          } else { // it' empty, swap to back and remove.
            swap(checkpoints[nextDataWrite][0],
                 checkpoints[nextDataWrite]
                     .back()); // swap expired ptr to back of vector
            checkpoints[nextDataWrite]
                .pop_back(); // pop expired ptr from back of vector
          }
        }
        // processAllLists(org->snapShotDataMaps[nextDataWrite]);
        std::vector<std::string> tempKeysList =
            org->snapShotDataMaps[nextDataWrite].getKeys(); // get all keys from
                                                            // the valid orgs
                                                            // dataMap (all orgs
                                                            // should have the
                                                            // same keys in
                                                            // their dataMaps)
        files_["data"].push_back("update");
        for (auto key : tempKeysList) { // for every key in dataMap...
          files_["data"].push_back(
              key); // add it to the list of keys associated with the data file.
        }
      }

      // write out data for all orgs in
      // checkPointTracker[Global::nextGenomeWrite] to "genome_" +
      // to_string(Global::nextGenomeWrite) + ".csv"

      size_t index = 0;
      while (index < checkpoints[nextDataWrite].size()) {
        if (auto org = checkpoints[nextDataWrite][index]
                           .lock()) { // this ptr is still good
          // processAllLists(org->snapShotDataMaps[nextDataWrite]);
          org->snapShotDataMaps[nextDataWrite].set("update", nextDataWrite);
          org->snapShotDataMaps[nextDataWrite].setOutputBehavior(
              "update", DataMap::FIRST);
          org->snapShotDataMaps[nextDataWrite].openAndWriteToFile(
              dataFileName, files_["data"]); // append new data to the file
          index++;                           // advance to nex element
        } else { // this ptr is expired - cut it out of the vector
          swap(checkpoints[nextDataWrite][index],
               checkpoints[nextDataWrite]
                   .back()); // swap expired ptr to back of vector
          checkpoints[nextDataWrite]
              .pop_back(); // pop expired ptr from back of vector
        }
      }
      if ((int)dataSequence.size() > writeDataSeqIndex + 1) {
        writeDataSeqIndex++;
        nextDataWrite = dataSequence[writeDataSeqIndex]; // genomeInterval;
      } else {
        nextDataWrite = Global::updatesPL->get() + 1;
      }
    }
  }
  // if enough time has passed to save all data and genomes, then we are done!
  finished_ =
      finished_ ||
      ((nextDataWrite > Global::updatesPL->get() || !(writeDataFiles)) &&
       (nextOrganismWrite > Global::updatesPL->get() ||
        !(writeOrganismFiles)) &&
       Global::update >= Global::updatesPL->get());

  ////////////////////////////////////////////////
  //
  ////////////////////////////////////////////////
  std::vector<std::shared_ptr<Organism>> toCheck;
  std::unordered_set<std::shared_ptr<Organism>> checked;
  int minBirthTime = population[0]->timeOfBirth; // time of birth of oldest org
                                                 // being saved in this update
                                                 // (init with random value)

  for (auto org :
       population) { // we don't need to worry about tracking parents or
                     // lineage, so we clear out this data every generation.
    if (!writeSnapshotDataFiles && !writeDataFiles && !writeOrganismFiles) {
      org->parents.clear();
      // cout << "HERE?" << endl;
    } else if (org->snapshotAncestors.find(org->ID) !=
                   org->snapshotAncestors.end() &&
               org->ancestors.find(org->ID) != org->ancestors.end() &&
               (org->timeOfDeath <
                (Global::update -
                 std::max(dataDelay,
                     organismDelay)))) { // if ancestors and snapshotAncestors
                                         // contains self, then this org has
                                         // been saved and it's ancestor list
                                         // has been collapsed
      org->parents.clear();
      checked.insert(org); // make a note, so we don't check this org later
      minBirthTime = std::min(org->timeOfBirth, minBirthTime);
    } else { // org has not ever been saved to either snapshot_Data or SSwD_Data
      toCheck.push_back(org); // we will need to check to see if we can do clean
                              // up related to this org
      checked.insert(org);    // make a note, so we don't check twice
      minBirthTime = std::min(org->timeOfBirth, minBirthTime);
    }
  }

  while (toCheck.size() > 0) {
    auto org = toCheck.back();
    toCheck.pop_back();
    if ((org->timeOfBirth < minBirthTime) &&
        (org->timeOfDeath <
         (Global::update -
          std::max(dataDelay, organismDelay)))) { // no living org can be this orgs
                                             // ancestor and this org died long
                                             // enough ago that they can not be
                                             // in a SSwD snapshot
      // cout << "minBirthTime: " << minBirthTime << " org->timeOfBirth: " <<
      // org->timeOfBirth << " org->timeOfDeath: " << org->timeOfDeath <<
      // "max(dataDelay, organismDelay): " << max(dataDelay, organismDelay) <<
      // endl;
      org->parents.clear(); // we can safely release parents
    } else {
      for (auto p : org->parents) { // we need to check parents (if any)
        if (checked.find(p) == checked.end()) { // if parent is not already in
                                                // checked list (i.e. either
                                                // checked or going to be)
          toCheck.push_back(p);
          checked.insert(org); // make a note, so we don't check twice
        }
      }
    }
  }
  ////////////////////////////////////////////////
  //
  ////////////////////////////////////////////////

  return finished_;
}

