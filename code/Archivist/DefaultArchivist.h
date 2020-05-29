//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once

#include <algorithm>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "../Global.h"
#include "../Organism/Organism.h"
#include "../Utilities/MTree.h"

class DefaultArchivist {
//protected:
public:
  bool writeMaxFile; // if true, Max file will be created
  bool writePopFile; // if true, pop file will be created
  std::string
      MaxFileName; // name of the Max file (all stats for best brain when
                   // file is writtne to)
  std::string
      PopFileName; // name of the Population file (ave and var for population
                   // at each timepoint)
  std::string PopFileColumnNames; // data to be saved into average file (must be
                                  // values that can generate an average)

  std::string DataFilePrefix;     // name of the Data file
  std::string OrganismFilePrefix; // name of the Genome file (genomes on LOD)
  bool writeSnapshotDataFiles;    // if true, write data file
  bool writeSnapshotGenomeFiles;  // if true, write genome file

  std::vector<int> realtimeSequence; // how often to write out data
  std::vector<int> realtimeDataSequence;
  std::vector<int> realtimeOrganismSequence;
  int realtime_sequence_index_ = 0;
  int realtime_data_seq_index_ = 0;
  int realtime_organism_seq_index_ = 0;

  std::string group_prefix_;

  std::shared_ptr<Abstract_MTree>
      max_formula_; // what value will be used to determine
                    // which organism to write to max file

  bool save_new_orgs_ = false;

  std::map<std::string, std::vector<std::string>>
      files_; // list of files (NAME,LIST OF COLUMNS)
  std::vector<std::string> default_pop_file_columns_; // what columns will be
                                                      // written into the
                                                      // PopFile

  std::map<std::string, int> unique_column_name_to_output_behaviors_;

  bool finished_ =
      false; // if finished, then as far as the archivist is concerned, we
             // can stop the run.

  const std::shared_ptr<ParametersTable> PT;

  void
  writeDefArchFiles(std::vector<std::shared_ptr<Organism>> & /*population*/);

  // save Max and average file data
  void
  writeRealTimeFiles(std::vector<std::shared_ptr<Organism>> & /*population*/);

  void
  saveSnapshotData(std::vector<std::shared_ptr<Organism>> & /*population*/);

  // void saveSnapshotGenomes(vector<shared_ptr<Organism>> population);
  void saveSnapshotOrganisms(
      std::vector<std::shared_ptr<Organism>> & /*population*/);

  void saveOrgToFile(const std::shared_ptr<Organism> &/*org*/,
                     const std::string & /*data_file_name*/);

  void cleanUpParents(std::vector<std::shared_ptr<Organism>> & /*population*/);

  //void resolveAncestors(const std::shared_ptr<Organism> &/*org*/,
  //                      std::vector<std::shared_ptr<Organism>> & /*save_file*/,
  //                      int /*min_birth_time*/);

  static std::shared_ptr<ParameterLink<std::string>>
      Arch_outputMethodStrPL; // string parameter for outputMethod;

  static std::shared_ptr<ParameterLink<bool>>
      Arch_writeMaxFilePL; // if true, Max file will be created
  static std::shared_ptr<ParameterLink<bool>>
      Arch_writePopFilePL; // if true, pop file will be created
  static std::shared_ptr<ParameterLink<std::string>>
      Arch_DefaultPopFileColumnNamesPL; // data to be saved into average file
                                        // (must be values that can generate an
                                        // average)

  static std::shared_ptr<ParameterLink<std::string>>
      Arch_realtimeSequencePL; // how often to write out data
  static std::shared_ptr<ParameterLink<std::string>>
      SS_Arch_dataSequencePL; // how often to save data
  static std::shared_ptr<ParameterLink<std::string>>
      SS_Arch_organismSequencePL; // how often to save genomes

  static std::shared_ptr<ParameterLink<std::string>>
      Arch_FilePrefixPL; // name of the Data file
  static std::shared_ptr<ParameterLink<bool>>
      SS_Arch_writeDataFilesPL; // if true, write data file
  static std::shared_ptr<ParameterLink<bool>>
      SS_Arch_writeOrganismsFilesPL; // if true, write genome file

  DefaultArchivist(std::shared_ptr<ParametersTable> /*PT*/ = nullptr,
                   const std::string & /*_groupPrefix*/ = "");
  DefaultArchivist(std::vector<std::string> & /*popFileColumns*/,
                   std::shared_ptr<Abstract_MTree> /*_maxFormula*/ = nullptr,
                   std::shared_ptr<ParametersTable> /*PT*/ = nullptr,
                   const std::string & /*_groupPrefix*/ = "");
  virtual ~DefaultArchivist() = default;

  // save data and manage in memory data
  // return true if next save will be > updates + terminate after
  virtual bool archive(std::vector<std::shared_ptr<Organism>> & /*population*/,
                       int /*flush*/ = 0);
};
