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


class SSwDArchivist : public DefaultArchivist { // SnapShot with Delay
public:
  static std::shared_ptr<ParameterLink<std::string>>
      SSwD_Arch_dataSequenceStrPL; // how often to write out data
  static std::shared_ptr<ParameterLink<std::string>>
      SSwD_Arch_organismSequenceStrPL; // how often to write out genomes
  static std::shared_ptr<ParameterLink<int>>
      SSwD_Arch_dataDelayPL; // when using SSwD, how long is the delay
  static std::shared_ptr<ParameterLink<int>>
      SSwD_Arch_organismDelayPL; // when using SSwD, how long is the delay
  static std::shared_ptr<ParameterLink<int>>
      SSwD_Arch_cleanupIntervalPL; // how often to attempt to prune the LOD
  static std::shared_ptr<ParameterLink<std::string>>
      SSwD_Arch_FilePrefixPL; // name of the Data file
  static std::shared_ptr<ParameterLink<bool>>
      SSwD_Arch_writeDataFilesPL; // if true, write data file
  static std::shared_ptr<ParameterLink<bool>>
      SSwD_Arch_writeOrganismFilesPL; // if true, write genome file

  std::vector<int> dataSequence;     // how often to write out data
  std::vector<int> organismSequence; // how often to write out data
  int dataDelay;                // when using SSwD, how long is the delay
  int organismDelay;            // when using SSwD, how long is the delay
  int cleanupInterval;          // how often to attempt to prune the LOD
  std::string DataFilePrefix;        // name of the Data file
  std::string OrganismFilePrefix;    // name of the Genome file (genomes on LOD)
  bool writeDataFiles;          // if true, write data file
  bool writeOrganismFiles;      // if true, write genome file

  //// info about files under management

  int writeDataSeqIndex;
  int checkPointDataSeqIndex;
  int writeOrganismSeqIndex;
  int checkPointOrganismSeqIndex;

  int nextDataWrite;      // next time data files will be written to disk
  int nextOrganismWrite;  // next time a genome file will be written to
                          // genome.csv
  int nextDataCheckPoint; // next time data files contents need to be
                          // checkpointed (this is needed particularly to handle
                          // delay > interval)
  int nextOrganismCheckPoint; // next time a genome files contents need to be
                              // checkpointed (this is needed particularly to
                              // handle delay > interval)

  std::map<int, std::vector<std::weak_ptr<Organism>>> checkpoints; // used by SSwD only - this
                                                    // keeps lists of orgs that
                                                    // may be written (if they
                                                    // have living decendents)
  // key is Global::nextGenomeWrite or Global::nextDataWrite

  SSwDArchivist() = delete;
  SSwDArchivist(std::vector<std::string> popFileColumns = {},
                std::shared_ptr<Abstract_MTree> _maxFormula = nullptr,
                std::shared_ptr<ParametersTable> PT_ = nullptr,
                std::string _groupPrefix = "");

  virtual ~SSwDArchivist() = default;

  ///// CLEANUP / DELETE STALE CHECKPOINTS
  // if a checkpoint is from before Global::update - archivist::intervalDelay
  // than delete the checkpoint
  // and all of it's org parents (with clear) assuming org was dead at the time
  // this will have the effect of a delayed pruning, but should do a good enough
  // job keeping memory down.
  void cleanup();

  virtual bool archive(std::vector<std::shared_ptr<Organism>> &population,
                       int flush = 0) override;
};
