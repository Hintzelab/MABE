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

#include <Archivist/DefaultArchivist.h>

class LODwAPArchivist
    : public DefaultArchivist { // Line of Decent with Active Pruning
public:
  static std::shared_ptr<ParameterLink<std::string>>
      LODwAP_Arch_dataSequencePL; // how often to write out data
  static std::shared_ptr<ParameterLink<std::string>>
      LODwAP_Arch_organismSequencePL; // how often to write out genomes
  static std::shared_ptr<ParameterLink<int>>
      LODwAP_Arch_pruneIntervalPL; // how often to attempt to prune the LOD
  static std::shared_ptr<ParameterLink<int>>
      LODwAP_Arch_terminateAfterPL; // how long to run after updates (to get
                                    // better coalescence)
  static std::shared_ptr<ParameterLink<bool>>
      LODwAP_Arch_writeDataFilePL; // if true, write data file
  static std::shared_ptr<ParameterLink<bool>>
      LODwAP_Arch_writeOrganismFilePL; // if true, write genome file

  static std::shared_ptr<ParameterLink<std::string>> LODwAP_Arch_FilePrefixPL;

  std::vector<int> dataSequence;     // how often to write out data
  std::vector<int> organismSequence; // how often to write out genomes
  int pruneInterval;            // how often to attempt to prune the LOD
  int terminateAfter;           // how long to run after updates (to get better
                                // coalescence)
  bool writeDataFile;           // if true, write data file
  bool writeOrganismFile;       // if true, write genome file

  void constructLODFiles(const std::shared_ptr<Organism> &/*org*/);

  void writeLODDataFile(std::vector<std::shared_ptr<Organism>> & /*LOD*/,
                        const std::shared_ptr<Organism> &/*real_MRCA*/,
                        const std::shared_ptr<Organism> &/*effective_MRCA*/);

  void
  writeLODOrganismFile(std::vector<std::shared_ptr<Organism>> & /*LOD*/,
                       const std::shared_ptr<Organism> & /*effective_MRCA*/);

  LODwAPArchivist() = delete;
  LODwAPArchivist(std::vector<std::string> popFileColumns = {},
                  std::shared_ptr<Abstract_MTree> _maxFormula = nullptr,
                  std::shared_ptr<ParametersTable> PT_ = nullptr,
                  const std::string &_groupPrefix = "");

  virtual ~LODwAPArchivist() = default;

  virtual bool archive(std::vector<std::shared_ptr<Organism>> &population,
                       int flush = 0) override;
 
 
  std::string data_file_name_;          // name of the Data file
  std::string organism_file_name_;      // name of the Genome file (genomes on LOD)
  int last_prune_ = -1; // last time Genome was Pruned
  int time_to_coalescence = -1;

  //// info about files under management
  int next_data_write_;     // next time data files will be written to disk
  int next_organism_write_; // next time a genome file will be written to
                         // genome.csv

  int data_seq_index = 0;
  int organism_seq_index = 0;

  // int nextDataCheckPoint;  // next time data files contents need to be
  // checkpointed (this is needed particularly to handle delay > interval)
  // int nextGenomeCheckPoint;  // next time a genome files contents need to be
  // checkpointed (this is needed particularly to handle delay > interval)

  // unordered_map<int, vector<weak_ptr<Organism>>> checkpoints;  // used by
  // SSwD only - this keeps lists of orgs that may be written (if they have
  // living decendents)
  //// key is Global::nextGenomeWrite or Global::nextDataWrite
};

