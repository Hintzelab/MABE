//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "DefaultArchivist.h"

#include<limits>

////// ARCHIVIST-outputMethod is actually set by Modules.h //////
std::shared_ptr<ParameterLink<std::string>>
    DefaultArchivist::Arch_outputMethodStrPL = Parameters::register_parameter(
        "ARCHIVIST-outputMethod",
        std::string("This_string_is_set_by_modules.h"),
        "This_string_is_set_by_modules.h"); // string parameter for
                                            // outputMethod;
////// ARCHIVIST-outputMethod is actually set by Modules.h //////

std::shared_ptr<ParameterLink<std::string>>
    DefaultArchivist::Arch_realtimeSequencePL = Parameters::register_parameter(
        "ARCHIVIST_DEFAULT-realtimeSequence", std::string(":10"),
        "How often to write to realtime data files. (format: x = single value, "
        "x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = "
        "from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
std::shared_ptr<ParameterLink<std::string>>
    DefaultArchivist::SS_Arch_dataSequencePL = Parameters::register_parameter(
        "ARCHIVIST_DEFAULT-snapshotDataSequence", std::string(":100"),
        "How often to save a realtime snapshot data file. (format: x = single "
        "value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on "
        "z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
std::shared_ptr<ParameterLink<std::string>> DefaultArchivist::
    SS_Arch_organismSequencePL = Parameters::register_parameter(
        "ARCHIVIST_DEFAULT-snapshotOrganismsSequence", std::string(":1000"),
        "How often to save a realtime snapshot genome file. (format: x = "
        "single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to "
        "updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, "
        "300:100'");

std::shared_ptr<ParameterLink<bool>> DefaultArchivist::Arch_writePopFilePL =
    Parameters::register_parameter("ARCHIVIST_DEFAULT-writePopFile", true,
                                   "Save data to average file?");
std::shared_ptr<ParameterLink<bool>> DefaultArchivist::Arch_writeMaxFilePL =
    Parameters::register_parameter("ARCHIVIST_DEFAULT-writeMaxFile", true,
                                   "Save data to Max file?");
std::shared_ptr<ParameterLink<std::string>>
    DefaultArchivist::Arch_DefaultPopFileColumnNamesPL =
        Parameters::register_parameter(
            "ARCHIVIST_DEFAULT-popFileColumns", std::string(""),
            "data to be saved into average file (must be values that can "
            "generate an average). If empty, MABE will try to figure it out");

std::shared_ptr<ParameterLink<std::string>>
    DefaultArchivist::Arch_FilePrefixPL =
        Parameters::register_parameter("ARCHIVIST_DEFAULT-filePrefix",
                                       std::string("NONE"),
                                       "prefix for files saved by "
                                       "this archivst. \"NONE\" "
                                       "indicates no prefix.");
std::shared_ptr<ParameterLink<bool>>
    DefaultArchivist::SS_Arch_writeDataFilesPL = Parameters::register_parameter(
        "ARCHIVIST_DEFAULT-writeSnapshotDataFiles", false,
        "if true, snapshot data files will "
        "be written (with all non genome "
        "data for entire population)");
std::shared_ptr<ParameterLink<bool>>
    DefaultArchivist::SS_Arch_writeOrganismsFilesPL =
        Parameters::register_parameter(
            "ARCHIVIST_DEFAULT-writeSnapshotOrganismsFiles", false,
            "if true, snapshot organisms files will be written (with all "
            "organisms for entire population)");

DefaultArchivist::DefaultArchivist(std::shared_ptr<ParametersTable> PT_,
                                   const std::string & group_prefix)
    : PT(std::move(PT_)), group_prefix_(group_prefix) {

  writePopFile = Arch_writePopFilePL->get(PT);
  writeMaxFile = Arch_writeMaxFilePL->get(PT);

  PopFileName =
      (group_prefix_.empty())
          ? "pop.csv"
          : group_prefix_.substr(0, group_prefix_.size() - 2) + "__pop.csv";
  PopFileName = (Arch_FilePrefixPL->get(PT) == "NONE")
                    ? PopFileName
                    : Arch_FilePrefixPL->get(PT) + PopFileName;

  MaxFileName =
      (group_prefix_.empty())
          ? "max.csv"
          : group_prefix_.substr(0, group_prefix_.size() - 2) + "__max.csv";
  MaxFileName = (Arch_FilePrefixPL->get(PT) == "NONE")
                    ? MaxFileName
                    : Arch_FilePrefixPL->get(PT) + MaxFileName;

  PopFileColumnNames = Arch_DefaultPopFileColumnNamesPL->get(PT);

  DataFilePrefix = (group_prefix_.empty())
                       ? "snapshot_data"
                       : group_prefix_.substr(0, group_prefix_.size() - 2) +
                             "__" + "snapshot_data";
  DataFilePrefix = (Arch_FilePrefixPL->get(PT) == "NONE")
                       ? DataFilePrefix
                       : Arch_FilePrefixPL->get(PT) + DataFilePrefix;

  OrganismFilePrefix = (group_prefix_.empty())
                           ? "snapshot_organisms"
                           : group_prefix_.substr(0, group_prefix_.size() - 2) +
                                 "__" + "snapshot_organisms";
  OrganismFilePrefix = (Arch_FilePrefixPL->get(PT) == "NONE")
                           ? OrganismFilePrefix
                           : Arch_FilePrefixPL->get(PT) + OrganismFilePrefix;

  writeSnapshotDataFiles = SS_Arch_writeDataFilesPL->get(PT);
  writeSnapshotGenomeFiles = SS_Arch_writeOrganismsFilesPL->get(PT);


  if (writePopFile || writeMaxFile ) 
    realtimeSequence =
        seq(Arch_realtimeSequencePL->get(PT), Global::updatesPL->get(), true);
  

  if (writeSnapshotDataFiles) 
    realtimeDataSequence =
        seq(SS_Arch_dataSequencePL->get(PT), Global::updatesPL->get(), true);
  

  if (writeSnapshotGenomeFiles) 
    realtimeOrganismSequence =
        seq(SS_Arch_organismSequencePL->get(PT), Global::updatesPL->get(), true);
  

  // this avoids bounds check on ...Index, since the ...Sequence can never
  // evaluate true for last element
  realtimeSequence.push_back(-1);
  realtimeDataSequence.push_back(-1);
  realtimeOrganismSequence.push_back(-1);


}

DefaultArchivist::DefaultArchivist(std::vector<std::string> & popFileColumns,
                                   std::shared_ptr<Abstract_MTree> max_formula,
                                   std::shared_ptr<ParametersTable> PT_,
                                   const std::string & group_prefix)
    : DefaultArchivist(std::move(PT_),group_prefix) {

  convertCSVListToVector(PopFileColumnNames, default_pop_file_columns_);
  max_formula_ = std::move(max_formula);

  if (default_pop_file_columns_.empty()) // hack because somehow getting passed empty string
    default_pop_file_columns_ = popFileColumns;

  for (auto &key : default_pop_file_columns_) {
    if (key == "update") {
      unique_column_name_to_output_behaviors_[key] = 0;
      continue;
    }

    // Now check to see if key ends in an '_' and a known output method (from
    // DataMap i.e. AVE, PROD, etc.)
    auto seperatorCharPos = key.find_last_of('_');
    if (seperatorCharPos != std::string::npos &&
        DataMap::knownOutputBehaviors.find(key.substr(seperatorCharPos + 1)) !=
            DataMap::knownOutputBehaviors.end())
      // if it does end in a known output method then add this method to the
      // uiqueColumnNameToOutputBehaviors map for that key
      unique_column_name_to_output_behaviors_[key.substr(0, seperatorCharPos)] |=
          DataMap::knownOutputBehaviors[key.substr(seperatorCharPos + 1)];
    else // add key normally, because it has no special flags specified
      unique_column_name_to_output_behaviors_[key] |= DataMap::AVE;
  }
}

// save Max and pop file data
// keys named all* will be converted to *. These should key for lists of values.
// These values will be averaged (used to average world repeats)
void DefaultArchivist::writeRealTimeFiles(
    std::vector<std::shared_ptr<Organism>> &population) {
  // write out population data

  if (writePopFile) {
    DataMap PopMap;
    for (auto &kv : unique_column_name_to_output_behaviors_) {
      if (kv.first != "update")
        for (auto const &org : population)
          if (org->timeOfBirth < Global::update || save_new_orgs_)
            PopMap.append(kv.first, org->dataMap.getAverage(kv.first));

      PopMap.setOutputBehavior(kv.first, kv.second);
    }
    PopMap.set("update", Global::update);
    PopMap.openAndWriteToFile(
        PopFileName, {}); // write the PopMap to file with empty list (save all)
  }

  // write out Max data
  if (writeMaxFile && max_formula_ != nullptr) {

    std::shared_ptr<Organism> best_org;
    auto score = std::numeric_limits<double>::lowest();
    for (auto const &org : population)
      if (org->timeOfBirth < Global::update || save_new_orgs_) {
        auto sc = max_formula_->eval(org->dataMap, org->PT)[0];
        if (sc > score) {
          score = sc;
          best_org = org;
        }
      }

    if (score == std::numeric_limits<double>::lowest()) {
      std::cout
          << " Error: could not find Max score organism to save to MaxFile"
          << std::endl;
      exit(1);
    }
    best_org->dataMap.set("update", Global::update);
    best_org->dataMap.openAndWriteToFile(MaxFileName);
    best_org->dataMap.clear("update");
  }
}

void DefaultArchivist::saveSnapshotData(
    std::vector<std::shared_ptr<Organism>> & population) {

  	// write out data
  std::string dataFileName =
      DataFilePrefix + "_" + std::to_string(Global::update) + ".csv";

  if (files_.find("snapshotData") ==
      files_.end()) { // first make sure that the dataFile has been set up.
    // population[0]->dataMap.Set("ancestors", "placeHolder");  // add ancestors
    // so it will be in files (holds columns to be output for each file)
    files_["snapshotData"] =
        population[0]->dataMap.getKeys(); // get all keys from the valid orgs
                                          // dataMap (all orgs should have the
                                          // same keys in their dataMaps)
	files_["snapshotData"].push_back("snapshotAncestors");
	files_["snapshotData"].push_back("update");
  }

  // first, determine which orgs in population need to be saved.
  auto saveList = population;

  if (!save_new_orgs_)
    saveList.erase(std::remove_if(std::begin(saveList), std::end(saveList),
                                  [](std::shared_ptr<Organism> org) {
                                    return org->timeOfBirth >= Global::update;
                                  }),
                   std::end(saveList));

  // now for each org, update ancestors and save if in saveList
  for (auto const &org : population) {
    // now that we know that ancestor list is good for this org...
    if (org->timeOfBirth < Global::update || save_new_orgs_) 
		saveOrgToFile(org,dataFileName);
  }

  FileManager::closeFile(dataFileName); // since this is a snapshot, we will not
                                        // be writting to this file again.
}

void DefaultArchivist::saveOrgToFile(const std::shared_ptr<Organism> &org, const std::string &data_file_name) {

  for (auto ancestorID : org->snapshotAncestors) {
    org->dataMap.append("snapshotAncestors", ancestorID);
  }

  org->dataMap.setOutputBehavior("snapshotAncestors", DataMap::LIST);

  org->snapshotAncestors.clear(); // now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
  org->snapshotAncestors.insert(org->ID);

  org->dataMap.set("update", Global::update);
  org->dataMap.setOutputBehavior("update", DataMap::FIRST);
  org->dataMap.openAndWriteToFile(data_file_name, files_["snapshotData"]); // append new data to the file
  org->dataMap.clear("snapshotAncestors");
  org->dataMap.clear("update");
}


//void DefaultArchivist::resolveAncestors(
//    const std::shared_ptr<Organism> &org,
//    std::vector<std::shared_ptr<Organism>> &save_list, int min_birth_time) {
      // if this org does not only contain only itself in snapshotAncestors then
      // it has not been saved before.
      // we must confirm that snapshotAncestors is correct because things may
      // have changed while we were not looking
      // this process does 2 things:
      // a) if this org is being saved then it makes sure it's up to date
      // b) it makes sure that it's ancestor list is correct so that it's
      // offspring will pass on the correct ancestor info.
      //
      // How does it work? (good question)
      // get a checklist of parents of the current org
      // for each parent, if they are going to be saved in this update, yay, we
      // can just assign their ID to the ancestor list
      // ... if they are not going to be saved then we need to check their
      // ancestors to see if they are going to be saved,
      // unless they are atleast as old as the oldest org being saved to this
      // file.
      // if they are at least as old as the oldest org being saved to this file
      // then we can simply append their ancestors


    /*
  auto parent_check_list = org->parents;

  while (!parent_check_list.empty()) {
    auto parent = parent_check_list.back(); // this is "this parent"
    parent_check_list.pop_back();           // remove this parent from checklist

    if (find(save_list.begin(), save_list.end(), parent) !=
        save_list.end()) { // if this parent is being saved, they will serve
                          // as an ancestor
      org->snapshotAncestors.insert(parent->ID);
      continue;
    }

	// this parent is not being saved
    if (parent->timeOfBirth < min_birth_time ||
        (parent->snapshotAncestors.size() == 1 &&
         parent->snapshotAncestors.find(parent->ID) !=
             parent->snapshotAncestors.end())) {
      // if this parent is old enough that it can not have a parent in the
      // save list (and is not in save list),
      // or this parent has self in it's ancestor list (i.e. it has
      // already been saved to another file),
      // copy ancestors from this parent
      for (auto ancestor_id : parent->snapshotAncestors) {
        org->snapshotAncestors.insert(ancestor_id);
      }
      continue;
    }

	// this parent not old enough (see if above), add this
    // parents parents to check list (we need to keep looking)
    for (auto const &p : parent->parents) {
      parent_check_list.push_back(p);
    }
  }
  */


//}

void DefaultArchivist::saveSnapshotOrganisms(
    std::vector<std::shared_ptr<Organism>> & population) {
  // write out organims
  std::string organismFileName =
      OrganismFilePrefix + "_" + std::to_string(Global::update) + ".csv";

  for (auto const &org : population) {
    if (org->timeOfBirth < Global::update || save_new_orgs_) {
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
    }
  }
  FileManager::closeFile(organismFileName); // since this is a snapshot, we will
                                            // not be writting to this file
                                            // again.
}

void DefaultArchivist::writeDefArchFiles(
    std::vector<std::shared_ptr<Organism>> &population) {

  if (Global::update ==
      realtimeSequence[realtime_sequence_index_]) {
    writeRealTimeFiles(population); // write to Max and Pop files
    realtime_sequence_index_++;
  }

  if (Global::update == realtimeDataSequence[realtime_data_seq_index_] &&
     writeSnapshotDataFiles) {
    saveSnapshotData(population);
    realtime_data_seq_index_++;
  }

  if (Global::update ==
          realtimeOrganismSequence[realtime_organism_seq_index_] &&
      writeSnapshotGenomeFiles) {
    saveSnapshotOrganisms(population);
    realtime_organism_seq_index_++;
  }
}


// save data and manage in memory data
// return true if next save will be > updates + terminate after
// archive MUST be called on every Global::update - ENFORCE 
bool DefaultArchivist::archive(
    std::vector<std::shared_ptr<Organism>> & population, int flush) {

  if (finished_)
    return finished_;

  finished_ = Global::update >= Global::updatesPL->get();
 
  if (flush == 1) 
    return finished_;

  if (!flush) // do not write files on flush - these
  			  // organisms have not been evaluated!
    writeDefArchFiles(population);

  if (!writeSnapshotDataFiles) {
    // we don't need to worry about tracking parents or
    // lineage, so we clear out this data every generation.
    for (auto const &org : population)
      org->parents.clear();
  } else {
	cleanUpParents(population);
  }


  // if we are at the end of the run
  return finished_;
}

void DefaultArchivist::cleanUpParents(
    std::vector<std::shared_ptr<Organism>> &population) {

  //auto need_to_clean = population;
  //need_to_clean.clear(); // we haven't cleaned anything yet

  for (auto const &org : population)
    if (org->snapshotAncestors.find(org->ID) != org->snapshotAncestors.end())
      // if ancestors contains self, then this org has been saved
      // and it's ancestor list has been collapsed
      org->parents.clear();
    //else                            // org has not ever been saved to file...
      //need_to_clean.push_back(org); // we will need to check to see if we can do
                                    // clean up related to this org

/*  auto const minBirthTime = // no generic lambdas in c++11 :(
      (*std::min_element(
           std::begin(population), std::end(population),
           [](std::shared_ptr<Organism> lhs, std::shared_ptr<Organism> rhs) {
             return lhs->timeOfBirth < rhs->timeOfBirth;
           }))
          ->timeOfBirth;

  auto logged = population;

  while (!need_to_clean.empty()) {
    auto org = need_to_clean.back();
    need_to_clean.pop_back();
    if (org->timeOfBirth < minBirthTime)
      // no living org can be this orgs ancestor
      org->parents.clear(); // so we can safely release parents
    else
      for (auto const &parent : org->parents) // we need to check parents (if any)
        if (std::find(std::begin(logged), std::end(logged), parent) ==
            logged.end()) { // if parent is not already in
                            // logged list (i.e. either
                            // logged or going to be)
          need_to_clean.push_back(parent);
          logged.push_back(parent); // make a note, so we don't clean twice
        }
  }
  */
}



