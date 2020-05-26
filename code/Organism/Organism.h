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

#include <cstdlib>
#include <vector>
#include <unordered_set>

#include <Brain/AbstractBrain.h>
#include <Genome/AbstractGenome.h>

#include <Utilities/Data.h>
#include <Utilities/Parameters.h>

class Organism {
private:
  static int organismIDCounter; // used to issue unique ids to Genomes
  int registerOrganism();       // get an Organism_id (uses organismIDCounter)

public:
  DataMap dataMap; // holds all data (genome size, score, world data, etc.)
  std::map<int, DataMap> snapShotDataMaps; // Used only with SnapShot with Delay
  // (SSwD) stores contents of dataMap when
  // an ouput interval is reached so that
  // after the delay we have the correct data for the given time. key is
  // 'update'. This possibly should be wrapped into Archivist.

  std::shared_ptr<ParametersTable> PT;

  std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> genomes;
  std::unordered_map<std::string, std::shared_ptr<AbstractBrain>> brains;

  int offspringCount;

  std::vector<std::shared_ptr<Organism>>
      parents; // parents are pointers to parents of
               // this organism. In asexual populations
               // this will have one element
  std::unordered_set<int>
      ancestors; // list of the IDs of organisms in the last data
                 // files who are ancestors of this organism
                 // (i.e. all files saved on data interval)
  std::unordered_set<int>
      snapshotAncestors; // like ancestors, but for snapshot files.

  int ID;
  int timeOfBirth; // the time this organism was made
  int timeOfDeath; // the time this organism stopped being alive (this organism
                   // may be stored for archival reasons)

  bool alive; // is this organism alive (1) or dead (0)
  bool trackOrganism =
      false; // if false, genome will be deleted when organism dies.

  void initOrganism(std::shared_ptr<ParametersTable> PT_);

  Organism() = delete;
  Organism(
      std::shared_ptr<ParametersTable> PT_ = nullptr); // make an empty organism

  Organism(
      std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
          &_genomes,
      std::unordered_map<std::string, std::shared_ptr<AbstractBrain>> &_brains,
      std::shared_ptr<ParametersTable> PT_ =
          nullptr); // make a parentless organism with a genome, and a brain
  Organism(
      const std::shared_ptr<Organism> &from,
      std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
          &_genomes,
      std::unordered_map<std::string, std::shared_ptr<AbstractBrain>> &_brains,
      std::shared_ptr<ParametersTable> PT_ = nullptr); // make an organism with
                                                       // one parent, a genome
                                                       // and a brain determined
                                                       // from the parents brain
                                                       // type.
  Organism(
      std::vector<std::shared_ptr<Organism>> from,
      std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
          &_genomes,
      std::unordered_map<std::string, std::shared_ptr<AbstractBrain>> &_brains,
      std::shared_ptr<ParametersTable> PT_ = nullptr); // make a organism with
                                                       // many parents, a
                                                       // genome, and a brain
                                                       // determined from the
                                                       // parents brain type.

  virtual ~Organism();

  virtual void kill(); // sets alive = 0 (on org and in dataMap)

  virtual std::vector<std::shared_ptr<Organism>>
  getLOD(std::shared_ptr<Organism> org);
  virtual std::shared_ptr<Organism>
  getMostRecentCommonAncestor(std::shared_ptr<Organism> org);
  virtual std::shared_ptr<Organism>
  getMostRecentCommonAncestor(std::vector<std::shared_ptr<Organism>> LOD);
  virtual std::shared_ptr<Organism>
  makeMutatedOffspringFrom(std::shared_ptr<Organism> parent);
  virtual std::shared_ptr<Organism>
  makeMutatedOffspringFromMany(std::vector<std::shared_ptr<Organism>> from);
  virtual std::shared_ptr<Organism>
  makeCopy(std::shared_ptr<ParametersTable> PT_ = nullptr);
};

