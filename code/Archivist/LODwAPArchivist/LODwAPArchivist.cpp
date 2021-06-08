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

std::shared_ptr<ParameterLink<std::string>> LODwAPArchivist::
    LODwAP_Arch_dataSequencePL = Parameters::register_parameter(
        "ARCHIVIST_LODWAP-dataSequence", std::string(":100"),
        "How often to write to data file. (format: x = single value, x-y = x "
        "to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x "
        "to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
std::shared_ptr<ParameterLink<std::string>>
    LODwAPArchivist::LODwAP_Arch_organismSequencePL =
        Parameters::register_parameter(
            "ARCHIVIST_LODWAP-organismsSequence", std::string(":1000"),
            "How often to write genome file. (format: x = single value, x-y = "
            "x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = "
            "from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
std::shared_ptr<ParameterLink<int>> LODwAPArchivist::LODwAP_Arch_pruneIntervalPL =
    Parameters::register_parameter(
        "ARCHIVIST_LODWAP-pruneInterval", 100,
        "How often to attempt to prune LOD and actually write out to files");
std::shared_ptr<ParameterLink<int>> LODwAPArchivist::LODwAP_Arch_terminateAfterPL =
    Parameters::register_parameter(
        "ARCHIVIST_LODWAP-terminateAfter", 10,
        "how long to run after updates (to get allow time for coalescence)");
std::shared_ptr<ParameterLink<bool>> LODwAPArchivist::LODwAP_Arch_writeDataFilePL =
    Parameters::register_parameter("ARCHIVIST_LODWAP-writeDataFile", true,
                                   "if true, a data file will be written");
std::shared_ptr<ParameterLink<bool>>
    LODwAPArchivist::LODwAP_Arch_writeOrganismFilePL =
        Parameters::register_parameter(
            "ARCHIVIST_LODWAP-writeOrganismsFile", true,
            "if true, an organisms file will be written");
std::shared_ptr<ParameterLink<std::string>>
    LODwAPArchivist::LODwAP_Arch_FilePrefixPL =
        Parameters::register_parameter("ARCHIVIST_LODWAP-filePrefix",
                                       std::string("NONE"),
                                       "prefix for files saved by "
                                       "this archivst. \"NONE\" "
                                       "indicates no prefix.");

LODwAPArchivist::LODwAPArchivist(std::vector<std::string> popFileColumns,
                                 std::shared_ptr<Abstract_MTree> _maxFormula,
                                 std::shared_ptr<ParametersTable> PT_,
                                 const std::string &group_prefix)
    : DefaultArchivist(popFileColumns, std::move(_maxFormula), std::move(PT_),
                       group_prefix) {

  pruneInterval = LODwAP_Arch_pruneIntervalPL->get(PT);
  terminateAfter = LODwAP_Arch_terminateAfterPL->get(PT);
  data_file_name_ = (LODwAP_Arch_FilePrefixPL->get(PT) == "NONE"
                      ? ""
                      : LODwAP_Arch_FilePrefixPL->get(PT)) +
                 (group_prefix_.empty()
                       ? "LOD_data.csv"
                       : group_prefix_.substr(0, group_prefix_.size() - 2) +
                             "__" + "LOD_data.csv");
  organism_file_name_ = (LODwAP_Arch_FilePrefixPL->get(PT) == "NONE"
                          ? ""
                          : LODwAP_Arch_FilePrefixPL->get(PT)) +
                     (group_prefix_.empty()
                           ? "LOD_organisms.csv"
                           : group_prefix_.substr(0, group_prefix_.size() - 2) +
                                 "__" + "LOD_organisms.csv");

  writeDataFile = LODwAP_Arch_writeDataFilePL->get(PT);
  writeOrganismFile = LODwAP_Arch_writeOrganismFilePL->get(PT);

  dataSequence = seq(LODwAP_Arch_dataSequencePL->get(PT), Global::updatesPL->get(), true);
  organismSequence = seq(LODwAP_Arch_organismSequencePL->get(PT), Global::updatesPL->get(), true);

  dataSequence.push_back(Global::updatesPL->get() + terminateAfter + 2);
  organismSequence.push_back(Global::updatesPL->get() + terminateAfter + 2);

  next_data_write_ = dataSequence[data_seq_index];
  next_organism_write_ = organismSequence[organism_seq_index];
}

void LODwAPArchivist::constructLODFiles(const std::shared_ptr<Organism> &org) {
  files_[data_file_name_].push_back("update");
  files_[data_file_name_].push_back("timeToCoalescence");
  auto all_keys = org->dataMap.getKeys();
  for (auto &key : all_keys) { // store keys from data map
                               // associated with file name
    files_[data_file_name_].push_back(key);
  }
}

void LODwAPArchivist::writeLODDataFile(
    std::vector<std::shared_ptr<Organism>> &LOD,
    const std::shared_ptr<Organism> &real_MRCA,
    const std::shared_ptr<Organism> &effective_MRCA) {

  while (next_data_write_ <=
         std::min(
             effective_MRCA->timeOfBirth,
             Global::updatesPL->get())) { // if there is convergence before the
                                          // next data interval

    // old version does not allow for overlapping generations
    //auto current = LOD[(next_data_write_ - last_prune_) - 1];

    // new version
    std::shared_ptr<Organism> current;
    int currentIndex = 0;
    while (LOD[currentIndex]->timeOfBirth < next_data_write_) {
        currentIndex++;
    }
    currentIndex--;
    current = LOD[currentIndex];
    // end new version

    current->dataMap.set("update", next_data_write_);
    current->dataMap.setOutputBehavior("update", DataMap::FIRST);
    time_to_coalescence =
        std::max(0, current->timeOfBirth - real_MRCA->timeOfBirth);
    current->dataMap.set("timeToCoalescence", time_to_coalescence);
    current->dataMap.setOutputBehavior("timeToCoalescence", DataMap::FIRST);
    current->dataMap.openAndWriteToFile(
        data_file_name_,
        files_[data_file_name_]); // append new data to the file
    current->dataMap.clear("update");
    current->dataMap.clear("timeToCoalescence");

	next_data_write_ = dataSequence[++data_seq_index];
  }
}


void LODwAPArchivist::writeLODOrganismFile(
    std::vector<std::shared_ptr<Organism>> &LOD,
    const std::shared_ptr<Organism> &effective_MRCA) {

  while (next_organism_write_ <=
         std::min(
             effective_MRCA->timeOfBirth,
             Global::updatesPL->get())) { // if there is convergence before the
                                          // next data interval

    // old version does not allow for overlapping generations
    //auto current = LOD[(next_organism_write_ - last_prune_) - 1];

    // new version
    std::shared_ptr<Organism> current;
    int currentIndex = 0;
    while (LOD[currentIndex]->timeOfBirth < next_organism_write_) {
        currentIndex++;
    }
    currentIndex--;
    current = LOD[currentIndex];
    // end new version

    DataMap OrgMap;
    OrgMap.set("ID", current->ID);
    OrgMap.set("update", next_organism_write_);
    OrgMap.setOutputBehavior("update", DataMap::FIRST);

    for (auto & genome : current->genomes) {
      auto name = "GENOME_" + genome.first;
      OrgMap.merge(genome.second->serialize(name));
    }
    for (auto & brain : current->brains) {
      auto name = "BRAIN_" + brain.first;
      OrgMap.merge(brain.second->serialize(name));
    }
    OrgMap.openAndWriteToFile(organism_file_name_); // append new data to the file

    next_organism_write_ = organismSequence[++organism_seq_index];
  }
}


bool LODwAPArchivist::archive(std::vector<std::shared_ptr<Organism>> &population,
                              int flush) {

  if (finished_ && !flush) {
    return finished_;
  }

  // if we have reached the end of time OR we have pruned past updates (i.e.
  // written out all data up to updates), then we are done!
  finished_ = finished_ ||
              (Global::update >= Global::updatesPL->get() + terminateAfter ||
               last_prune_ >= Global::updatesPL->get());

  if (!flush) // do not write files on flush - these
  			  // organisms have not been evaluated!
  	writeDefArchFiles(population);

  if (writeOrganismFile &&
      std::find(organismSequence.begin(), organismSequence.end(),
                Global::update) != organismSequence.end())
    for (auto const &org : population) // if this update is in the genome sequence,
                                // turn on genome tracking.
      org->trackOrganism = true;

  if (Global::update % pruneInterval && flush != 1) 
  return finished_;

  if (files_.find(data_file_name_) ==
      files_.end()) // if file has not be initialized yet
    constructLODFiles(population[0]);

  // get the MRCA
  auto some_org = population[0];
  auto LOD = some_org->getLOD(some_org); // get line of descent

  if (flush) // if flush then we don't care about coalescence
    std::cout << "flushing LODwAP: organism with ID " << population[0]->ID <<
      " has been selected to generate Line of Descent."
      << std::endl;

  auto effective_MRCA =
      flush // this assumes that a population was created, but not tested at
          // the end of the evolution loop!
          ? some_org->parents[0]
          : some_org->getMostRecentCommonAncestor(
                LOD); // find the convergence point in the LOD.
  auto real_MRCA =
      flush ? some_org->getMostRecentCommonAncestor(LOD)
            : effective_MRCA; // find the convergence point in the LOD.

  // Save Data
  if (writeDataFile) {
	  writeLODDataFile(LOD, real_MRCA, effective_MRCA);
	  if (flush) {
		  if (real_MRCA->timeOfBirth == -2) {
			  std::cout << "This run has not coalesced. There is no Most Recent Common Ancestor.\n" <<
				  "None of the organisms in LOD_data.csv are guaranteed to be on LOD." << std::endl;
		  }
		  else if (time_to_coalescence > 0) {
			  std::cout << "Time to Coalescence for the last organism written to LOD_data.csv (at time " << dataSequence[data_seq_index - 1] << ") is "
				  << time_to_coalescence << "." << std::endl;
		  }
		  else if (time_to_coalescence == 0) {
			  std::cout << "The last organism written to LOD_data.csv (at time " << dataSequence[data_seq_index - 1] << ") had a time to coalescence of 0.\n" <<
				  "Rejoice! This organism is on LOD!" << std::endl;
		  }
		  else {
			  std::cout << "The last organism written to LOD_data.csv had a negitive time to coalescence.\n" <<
				  "This is an error and may indicate the LOD tracking has failed! Please make a bug report." << std::endl;
			  exit(1);
		  }
	  }
  }


  // Save Organisms
  if (writeOrganismFile)
    writeLODOrganismFile(LOD, effective_MRCA);

  // data and genomes have now been written out up till the MRCA
  // so all data and genomes from before the MRCA can be deleted
  effective_MRCA->parents.clear();
  last_prune_ = effective_MRCA->timeOfBirth; // this will hold the time of the
                                             // oldest genome in RAM

  return finished_;


  /*
  ////////////////////////////////////////////////////////
  // code to support defualt archivist snapshotData
  ////////////////////////////////////////////////////////
  vector<shared_ptr<Organism>> toCheck;
  unordered_set<shared_ptr<Organism>> checked;
  int minBirthTime = population[0]->timeOfBirth; // time of birth of oldest org
  being saved in this update (init with random value)

  for (auto org : population) {  // we don't need to worry about tracking
  parents or lineage, so we clear out this data every generation.
          if (!writeSnapshotDataFiles) {
                  org->parents.clear();
          }
          else if (org->snapshotAncestors.find(org->ID) !=
  org->snapshotAncestors.end()) { // if ancestors contains self, then this org
  has been saved and it's ancestor list has been collapsed
                  org->parents.clear();
                  checked.insert(org); // make a note, so we don't check this
  org later
                  minBirthTime = min(org->timeOfBirth, minBirthTime);
          }
          else { // org has not ever been saved to file...
                  toCheck.push_back(org); // we will need to check to see if we
  can do clean up related to this org
                  checked.insert(org); // make a note, so we don't check twice
                  minBirthTime = min(org->timeOfBirth, minBirthTime);
          }
  }
  ////////////////////////////////////////////////////////
  // end code to support defualt archivist snapshotData
  ////////////////////////////////////////////////////////
  */

}

