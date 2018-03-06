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

////// ARCHIVIST-outputMethod is actually set by Modules.h //////
std::shared_ptr<ParameterLink<std::string>>
    DefaultArchivist::Arch_outputMethodStrPL = Parameters::register_parameter(
        "ARCHIVIST-outputMethod",
        (std::string) "This_string_is_set_by_modules.h",
        "This_string_is_set_by_modules.h"); // string parameter for
                                            // outputMethod;
////// ARCHIVIST-outputMethod is actually set by Modules.h //////

std::shared_ptr<ParameterLink<std::string>>
    DefaultArchivist::Arch_realtimeSequencePL = Parameters::register_parameter(
        "ARCHIVIST_DEFAULT-realtimeSequence", (std::string) ":10",
        "How often to write to realtime data files. (format: x = single value, "
        "x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = "
        "from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
std::shared_ptr<ParameterLink<std::string>>
    DefaultArchivist::SS_Arch_dataSequencePL = Parameters::register_parameter(
        "ARCHIVIST_DEFAULT-snapshotDataSequence", (std::string) ":100",
        "How often to save a realtime snapshot data file. (format: x = single "
        "value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on "
        "z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
std::shared_ptr<
    ParameterLink<std::string>> DefaultArchivist::SS_Arch_organismSequencePL =
    Parameters::register_parameter(
        "ARCHIVIST_DEFAULT-snapshotOrganismsSequence", (std::string) ":1000",
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
            "ARCHIVIST_DEFAULT-popFileColumns", (std::string) "[]",
            "data to be saved into average file (must be values that can "
            "generate an average). If empty, MABE will try to figure it out");

std::shared_ptr<ParameterLink<std::string>>
    DefaultArchivist::Arch_FilePrefixPL =
        Parameters::register_parameter("ARCHIVIST_DEFAULT-filePrefix",
                                       (std::string) "NONE",
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

DefaultArchivist::DefaultArchivist(std::shared_ptr<ParametersTable> _PT,
                                   std::string _groupPrefix)
    : PT(_PT), groupPrefix(_groupPrefix) {

  writePopFile = Arch_writePopFilePL->get(PT);
  writeMaxFile = Arch_writeMaxFilePL->get(PT);

  PopFileName =
      (groupPrefix == "")
          ? "pop.csv"
          : groupPrefix.substr(0, groupPrefix.size() - 2) + "__pop.csv";
  PopFileName = (Arch_FilePrefixPL->get(PT) == "NONE")
                    ? PopFileName
                    : Arch_FilePrefixPL->get(PT) + PopFileName;

  MaxFileName =
      (groupPrefix == "")
          ? "max.csv"
          : groupPrefix.substr(0, groupPrefix.size() - 2) + "__max.csv";
  MaxFileName = (Arch_FilePrefixPL->get(PT) == "NONE")
                    ? MaxFileName
                    : Arch_FilePrefixPL->get(PT) + MaxFileName;

  PopFileColumnNames = Arch_DefaultPopFileColumnNamesPL->get(PT);

  DataFilePrefix = (groupPrefix == "")
                       ? "snapshot_data"
                       : groupPrefix.substr(0, groupPrefix.size() - 2) + "__" +
                             "snapshot_data";
  DataFilePrefix = (Arch_FilePrefixPL->get(PT) == "NONE")
                       ? DataFilePrefix
                       : Arch_FilePrefixPL->get(PT) + DataFilePrefix;

  OrganismFilePrefix = (groupPrefix == "")
                           ? "snapshot_organisms"
                           : groupPrefix.substr(0, groupPrefix.size() - 2) +
                                 "__" + "snapshot_organisms";
  OrganismFilePrefix = (Arch_FilePrefixPL->get(PT) == "NONE")
                           ? OrganismFilePrefix
                           : Arch_FilePrefixPL->get(PT) + OrganismFilePrefix;

  writeSnapshotDataFiles = SS_Arch_writeDataFilesPL->get(PT);
  writeSnapshotGenomeFiles = SS_Arch_writeOrganismsFilesPL->get(PT);

  realtimeSequence.push_back(0);
  realtimeDataSequence.push_back(0);
  realtimeOrganismSequence.push_back(0);

  if (writePopFile || writeMaxFile ) {
    auto realtimeSequenceStr = Arch_realtimeSequencePL->get(PT);
    realtimeSequence.clear();
    realtimeSequence = seq(realtimeSequenceStr, Global::updatesPL->get(), true);
    if (realtimeSequence.size() == 0) {
      std::cout << "unable to translate ARCHIVIST_DEFAULT-realtimeSequence \""
                << realtimeSequenceStr << "\".\nExiting." << std::endl;
      exit(1);
    }
  }

  if (writeSnapshotDataFiles) {
    auto dataSequenceStr = SS_Arch_dataSequencePL->get(PT);
    realtimeDataSequence.clear();
    realtimeDataSequence = seq(dataSequenceStr, Global::updatesPL->get(), true);
    if (realtimeDataSequence.size() == 0) {
      std::cout
          << "unable to translate ARCHIVIST_DEFAULT-snapshotDataSequence \""
          << dataSequenceStr << "\".\nExiting." << std::endl;
      exit(1);
    }
  }

  if (writeSnapshotGenomeFiles) {
    auto organismIntervalStr = SS_Arch_organismSequencePL->get(PT);
    realtimeOrganismSequence.clear();
    realtimeOrganismSequence =
        seq(organismIntervalStr, Global::updatesPL->get(), true);
    if (realtimeOrganismSequence.size() == 0) {
      std::cout
          << "unable to translate ARCHIVIST_DEFAULT-snapshotOrganismsSequence "
             "\""
          << organismIntervalStr << "\".\nExiting." << std::endl;
      exit(1);
    }
  }

  // this avoids bounds check on ...Index, since the ...Sequence can never
  // evaluate true for last element
  realtimeSequence.push_back(-1);
  realtimeDataSequence.push_back(-1);
  realtimeOrganismSequence.push_back(-1);

  realtimeSequenceIndex = 0;
  realtimeDataSeqIndex = 0;
  realtimeOrganismSeqIndex = 0;

  finished = false;
}

DefaultArchivist::DefaultArchivist(std::vector<std::string> popFileColumns,
                                   std::shared_ptr<Abstract_MTree> _maxFormula,
                                   std::shared_ptr<ParametersTable> _PT,
                                   std::string _groupPrefix)
    : DefaultArchivist(_PT, _groupPrefix) {
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

    // Now check to see if key ends in an '_' and a known output method (from
    // DataMap i.e. AVE, PROD, etc.)
    size_t seperatorCharPos = key.find_last_of('_');
    if (seperatorCharPos != std::string::npos &&
        DataMap::knownOutputBehaviors.find(key.substr(seperatorCharPos + 1)) !=
            DataMap::knownOutputBehaviors.end()) { // if there is an '&'
      // if it does end in a known output method then add this method to the
      // uiqueColumnNameToOutputBehaviors map for that key
      // key[seperatorCharPos] = '_';
      if (uniqueColumnNameToOutputBehaviors.find(
              key.substr(0, seperatorCharPos)) ==
          uniqueColumnNameToOutputBehaviors.end()) { // if key not in map
        uniqueColumnNameToOutputBehaviors[key.substr(0, seperatorCharPos)] =
            DataMap::knownOutputBehaviors[key.substr(seperatorCharPos + 1)];
        // std::cout << "set behavior: " << key.substr(seperatorCharPos + 1) <<
        // " to
        // " << key.substr(0, seperatorCharPos);
      } else { // key already in map
        uniqueColumnNameToOutputBehaviors[key.substr(0, seperatorCharPos)] =
            uniqueColumnNameToOutputBehaviors[key.substr(0, seperatorCharPos)] |
            DataMap::knownOutputBehaviors[key.substr(seperatorCharPos + 1)];
        // std::cout << "added behavior: " << key.substr(seperatorCharPos + 1)
        // << "
        // to " << key.substr(0, seperatorCharPos);
      }
    } else { // add key normally, because it has no special flags specified
      if (uniqueColumnNameToOutputBehaviors.find(key) ==
          uniqueColumnNameToOutputBehaviors.end()) {
        uniqueColumnNameToOutputBehaviors[key] = DataMap::AVE;
      } else {
        uniqueColumnNameToOutputBehaviors[key] |= DataMap::AVE;
      }
    }
  }
}

// save Max and pop file data
// keys named all* will be converted to *. These should key for lists of values.
// These values will be averaged (used to average world repeats)
void DefaultArchivist::writeRealTimeFiles(
    std::vector<std::shared_ptr<Organism>> &population) {
  // write out population data

  if (writePopFile) {
    double aveValue;
    DataMap PopMap;

    // for (auto key : DefaultPopFileColumns) {
    //	if (key != "update") {
    for (auto kv : uniqueColumnNameToOutputBehaviors) {
      if (kv.first != "update") {
        aveValue = 0;
        for (auto org : population) {
          if (org->timeOfBirth < Global::update || saveNewOrgs) {
            PopMap.append(kv.first, org->dataMap.getAverage(kv.first));
          }
        }
      }
      PopMap.setOutputBehavior(kv.first, kv.second);
    }
    PopMap.set("update", Global::update);
    PopMap.writeToFile(
        PopFileName, {}); // write the PopMap to file with empty list (save all)
  }

  // write out Max data
  if (writeMaxFile && maxFormula != nullptr) {
    double bestScore;
    std::shared_ptr<Organism> bestOrg;
    for (size_t i = 0; i < population.size(); i++) {
      if (population[i]->timeOfBirth < Global::update || saveNewOrgs) {
        // find a valid score!
        bestScore =
            maxFormula->eval(population[i]->dataMap, population[i]->PT)[0];
        bestOrg = population[i];
        i = population.size();
      }
    }
    for (size_t i = 0; i < population.size(); i++) {
      if (population[i]->timeOfBirth < Global::update || saveNewOrgs) {
        double newScore =
            maxFormula->eval(population[i]->dataMap, population[i]->PT)[0];
        if (newScore > bestScore) {
          bestScore = newScore;
          bestOrg = population[i];
        }
      }
    }
    bestOrg->dataMap.set("update", Global::update);
    bestOrg->dataMap.writeToFile(MaxFileName);
    bestOrg->dataMap.clear("update");
  }
}

void DefaultArchivist::saveSnapshotData(
    std::vector<std::shared_ptr<Organism>> population) {
  // write out data
  std::string dataFileName =
      DataFilePrefix + "_" + std::to_string(Global::update) + ".csv";
  if (files.find("snapshotData") ==
      files.end()) { // first make sure that the dataFile has been set up.
    // population[0]->dataMap.Set("ancestors", "placeHolder");  // add ancestors
    // so it will be in files (holds columns to be output for each file)
    files["snapshotData"] =
        population[0]->dataMap.getKeys(); // get all keys from the valid orgs
                                          // dataMap (all orgs should have the
                                          // same keys in their dataMaps)
    files["snapshotData"].push_back("snapshotAncestors");
  }

  // first, determine which orgs in population need to be saved.
  std::vector<std::shared_ptr<Organism>> saveList;
  int minBirthTime = population[0]->timeOfBirth; // time of birth of oldest org
                                                 // being saved in this update
                                                 // (init with random value)

  if (saveNewOrgs) {
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

  // now for each org, update ancestors and save if in saveList
  for (auto org : population) {

    // std::cout << "---------------\n now looking at: " << org->ID <<
    // std::endl;
    // std::cout << "  with parents List (" << org->parents.size() << "): ";
    // for (auto a : org->parents) {
    //	std::cout << a->ID << "  ";
    //}
    // std::cout << std::endl;

    if (org->snapshotAncestors.size() != 1 ||
        org->snapshotAncestors.find(org->ID) == org->snapshotAncestors.end()) {
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

      org->snapshotAncestors.clear();
      std::vector<std::shared_ptr<Organism>> parentCheckList = org->parents;

      while (parentCheckList.size() > 0) {
        auto parent = parentCheckList.back(); // this is "this parent"
        parentCheckList.pop_back(); // remove this parent from checklist

        // std::cout << "\n org: " << org->ID << " parent: " << parent->ID <<
        // std::endl;
        if (find(saveList.begin(), saveList.end(), parent) !=
            saveList.end()) { // if this parent is being saved, they will serve
                              // as an ancestor
          org->snapshotAncestors.insert(parent->ID);
        } else { // this parent is not being saved
          if (parent->timeOfBirth < minBirthTime ||
              (parent->snapshotAncestors.size() == 1 &&
               parent->snapshotAncestors.find(parent->ID) !=
                   parent->snapshotAncestors.end())) {
            // if this parent is old enough that it can not have a parent in the
            // save list (and is not in save list),
            // or this parent has self in it's ancestor list (i.e. it has
            // already been saved to another file),
            // copy ancestors from this parent
            // std::cout << "getting ancestors for " << org->ID << " parent " <<
            // parent->ID << " is old enough or has self as ancestor..." <<
            // std::endl;
            for (auto ancestorID : parent->snapshotAncestors) {
              // std::cout << "adding from parent " << parent->ID << " ancestor
              // " <<
              // ancestorID << std::endl;
              org->snapshotAncestors.insert(ancestorID);
            }
          } else { // this parent not old enough (see if above), add this
                   // parents parents to check list (we need to keep looking)
            for (auto p : parent->parents) {
              parentCheckList.push_back(p);
            }
          }
        }
      }

      /* // uncomment to see updated ancesstors list
      std::cout << "  new snapshotAncestors List: ";
      for (auto a : org->snapshotAncestors) {
              std::cout << a << "  ";
      }
      std::cout << std::endl;
      */

    } else {                                    // org has self for ancestor
      if (org->timeOfBirth >= Global::update) { // if this is a new org...
        std::cout
            << "  WARRNING :: in DefaultArchivist::saveSnapshotData(), found "
               "new org (age < 1) with self as ancestor (with ID: "
            << org->ID
            << "... this will result in a new root to the phylogony tree!"
            << std::endl;
        if (saveNewOrgs) {
          std::cout << "    this org is being saved" << std::endl;
        } else {
          std::cout
              << "    this org is not being saved (this may be very bad...)"
              << std::endl;
        }
      }
    }
    // now that we know that ancestor list is good for this org...
    if (org->timeOfBirth < Global::update || saveNewOrgs) {
      // std::cout << "  is being saved" << std::endl;
      for (auto ancestorID : org->snapshotAncestors) {
        // std::cout << org->ID << " adding ancestor " << ancestorID << " to
        // dataMap"
        // << std::endl;
        org->dataMap.append("snapshotAncestors", ancestorID);
      }
      org->dataMap.setOutputBehavior("snapshotAncestors", DataMap::LIST);

      org->snapshotAncestors.clear(); // now that we have saved the ancestor
                                      // data, set ancestors to self (so that
                                      // others will inherit correctly)
      org->snapshotAncestors.insert(org->ID);
      org->dataMap.set("update", Global::update);
      org->dataMap.setOutputBehavior("update", DataMap::FIRST);
      org->dataMap.writeToFile(
          dataFileName, files["snapshotData"]); // append new data to the file
      org->dataMap.clear("snapshotAncestors");
      org->dataMap.clear("update");
    }
  }
  FileManager::closeFile(dataFileName); // since this is a snapshot, we will not
                                        // be writting to this file again.
}

void DefaultArchivist::saveSnapshotOrganisms(
    std::vector<std::shared_ptr<Organism>> population) {
  // write out organims
  std::string organismFileName =
      OrganismFilePrefix + "_" + std::to_string(Global::update) + ".csv";

  for (auto org : population) {
    if (org->timeOfBirth < Global::update || saveNewOrgs) {
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
      OrgMap.writeToFile(organismFileName); // append new data to the file
    }
  }
  FileManager::closeFile(organismFileName); // since this is a snapshot, we will
                                            // not be writting to this file
                                            // again.
}

// save data and manage in memory data
// return true if next save will be > updates + terminate after
bool DefaultArchivist::archive(
    std::vector<std::shared_ptr<Organism>> population, int flush) {

  if (finished)
    return finished;

  finished = Global::update >= Global::updatesPL->get();
 
  if (flush == 1) 
    return finished;

  if ((Global::update == realtimeSequence[realtimeSequenceIndex]) &&
      !flush) { // do not write files on flush - these organisms have
                // not been evaluated!
    writeRealTimeFiles(population); // write to Max and Pop files
    realtimeSequenceIndex++;
  }

  if ((Global::update == realtimeDataSequence[realtimeDataSeqIndex]) &&
      !flush && writeSnapshotDataFiles) { // do not write files on flush - these
                                          // organisms have not been evaluated!
    saveSnapshotData(population);
    realtimeDataSeqIndex++;
  }

  if ((Global::update == realtimeOrganismSequence[realtimeOrganismSeqIndex]) &&
      !flush &&
      writeSnapshotGenomeFiles) { // do not write files on flush - these
                                  // organisms have not been evaluated!
    saveSnapshotOrganisms(population);
    realtimeOrganismSeqIndex++;
  }

  if (!writeSnapshotDataFiles) {
    // we don't need to worry about tracking parents or
    // lineage, so we clear out this data every generation.
    for (auto org : population)
      org->parents.clear();
    return finished;
  }

  auto const minBirthTime = // no generic lambdas in c++11 :(
      (*std::min_element(
           std::begin(population), std::end(population),
           [](std::shared_ptr<Organism> lhs, std::shared_ptr<Organism> rhs) {
             return lhs->timeOfBirth < rhs->timeOfBirth;
           }))
          ->timeOfBirth;


  auto need_to_clean = population;
  need_to_clean.clear();  // we haven't cleaned anything yet

  for (auto org : population)
    if (org->snapshotAncestors.find(org->ID) != org->snapshotAncestors.end())
      // if ancestors contains self, then this org has been saved
      // and it's ancestor list has been collapsed
      org->parents.clear();
    else                      // org has not ever been saved to file...
      need_to_clean.push_back(org); // we will need to check to see if we can do
                              // clean up related to this org

  auto cleaned = population;

  while (!need_to_clean.empty()) {
    auto org = need_to_clean.back();
    need_to_clean.pop_back();
    if (org->timeOfBirth < minBirthTime)
      // no living org can be this orgs ancestor
      org->parents.clear(); // so we can safely release parents
    else
      for (auto parent : org->parents) // we need to check parents (if any)
        if (std::find(std::begin(cleaned), std::end(cleaned), parent) ==
            cleaned.end()) { // if parent is not already in
                             // cleaned list (i.e. either
                             // cleaned or going to be)
          need_to_clean.push_back(parent);

		  /*** def a bug ***/
          cleaned.push_back(org); // make a note, so we don't clean twice
        }
  }

  // if we are at the end of the run
  return finished;
}

bool DefaultArchivist::isDataUpdate(int check_update) {

  check_update = check_update == -1 ? Global::update : check_update;

  return std::find(std::begin(realtimeSequence), std::end(realtimeSequence),
                   check_update) != std::end(realtimeSequence) or
         std::find(std::begin(realtimeDataSequence),
                   std::end(realtimeDataSequence),
                   check_update) != std::end(realtimeSequence);
}

bool DefaultArchivist::isOrganismUpdate(int check_update) {

  check_update = check_update == -1 ? Global::update : check_update;

  return std::find(std::begin(realtimeOrganismSequence),
                   std::end(realtimeOrganismSequence),
                   check_update) != std::end(realtimeOrganismSequence);
}


