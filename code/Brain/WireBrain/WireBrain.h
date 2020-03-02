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

#include <cmath>
#include <memory>
#include <iostream>
#include <set>
#include <unordered_set>
#include <vector>

#include "../../Genome/AbstractGenome.h"

#include "../../Utilities/Random.h"

#include "../AbstractBrain.h"

// define the wire states

class WireBrain : public AbstractBrain {

  static std::shared_ptr<ParameterLink<int>> bitsPerCodonPL;

  static std::shared_ptr<ParameterLink<bool>> allowNegativeChargePL;
  static std::shared_ptr<ParameterLink<int>> defaultWidthPL;
  static std::shared_ptr<ParameterLink<int>> defaultHeightPL;
  static std::shared_ptr<ParameterLink<int>> defaultDepthPL;
  static std::shared_ptr<ParameterLink<int>> worldConnectionsSeparationPL;
  static std::shared_ptr<ParameterLink<int>> overchargeThresholdPL;
  static std::shared_ptr<ParameterLink<int>> decayDurationPL;
  static std::shared_ptr<ParameterLink<int>> chargeUpdatesPerUpdatePL;
  static std::shared_ptr<ParameterLink<bool>> constantInputsPL;
  static std::shared_ptr<ParameterLink<bool>> cacheResultsPL;
  static std::shared_ptr<ParameterLink<int>> cacheResultsCountPL;

  static std::shared_ptr<ParameterLink<std::string>>
      genomeDecodingMethodPL; // "bitmap" = convert genome directly, "wiregenes"
                              // = genes defined by start codeons, location,
                              // direction and location
  static std::shared_ptr<ParameterLink<int>> wiregenesInitialGeneCountPL;
  static std::shared_ptr<ParameterLink<double>> bitmapInitialFillRatioPL;

  static std::shared_ptr<ParameterLink<bool>> wiregenesAllowSimpleWiresPL;
  static std::shared_ptr<ParameterLink<int>> wiregenesSimpleWireMaxLengthPL;
  static std::shared_ptr<ParameterLink<std::string>>
      wiregenesSimpleWireDirectionsPL;

  static std::shared_ptr<ParameterLink<bool>> wiregenesAllowWormholesPL;
  static std::shared_ptr<ParameterLink<int>> wiregenesWormholesBidirectionalPL;

  static std::shared_ptr<ParameterLink<bool>> wiregenesAllowSquiggleWiresPL;
  static std::shared_ptr<ParameterLink<int>> wiregenesSquiggleWireMinLengthPL;
  static std::shared_ptr<ParameterLink<int>> wiregenesSquiggleWireMaxLengthPL;
  static std::shared_ptr<ParameterLink<std::string>>
      wiregenesSquiggleWireDirectionsPL;

  static std::shared_ptr<ParameterLink<int>> hiddenValuesPL;

  bool allowNegativeCharge;
  int defaultWidth;
  int defaultHeight;
  int defaultDepth;
  int worldConnectionsSeparation;
  int overchargeThreshold;
  int decayDuration;
  int chargeUpdatesPerUpdate;
  bool constantInputs;
  bool cacheResults;
  int cacheResultsCount;

  std::string genomeDecodingMethod; // "bitmap" = convert genome directly,
                                    // "wiregenes" = genes defined by start
                                    // codeons, location, direction and location
  int wiregenesInitialGeneCount;
  double bitmapInitialFillRatio;

  bool wiregenesAllowSimpleWires;
  int wiregenesSimpleWireMaxLength;
  std::string wiregenesSimpleWireDirections;

  bool wiregenesAllowWormholes;
  int wiregenesWormholesBidirectional;

  bool wiregenesAllowSquiggleWires;
  int wiregenesSquiggleWireMinLength;
  int wiregenesSquiggleWireMaxLength;
  std::string wiregenesSquiggleWireDirections;

  int nrHiddenValues;

  std::vector<double> nodes;
  std::vector<double> nextNodes;

  int CHARGE;    // = 2 + *decayDuration;
  int NEGCHARGE; // = CHARGE * -1;

  const int WIRE = 1;
  const int HOLLOW = 0;

  const int START_CODE = 0;
  const int LOCATION_CODE = 1;
  const int DIRECTION_CODE = 2;
  const int LENGTH_CODE = 3;
  const int DESTINATION_CODE = 4;

public:
  int width, depth, height;

  std::vector<int> nodesAddresses,
      nodesNextAddresses; // where the nodes connect to the brain

  std::vector<int> allCells, nextAllCells; // list of all cells in this brain
  std::vector<std::vector<int>>
      neighbors; // for every cell list of wired neighbors (most will be empty)
  std::vector<int> wireAddresses; // list of addresses for all cells which are
                                  // wireAddresses (uncharged, charged and
                                  // decay)

  std::vector<std::vector<long>>
      inputLookUpTable;        // table that contains output for a given input
  std::vector<int> inputCount; // table that contains a count of the number of
                               // times we have seen a given input

  static std::shared_ptr<ParameterLink<std::string>> genomeNamePL;
  std::string genomeName;

  int connectionsCount;

  int nrValues;

  WireBrain(int _nrInNodes, int _nrOutNodes,
            std::shared_ptr<ParametersTable> PT_ = nullptr);
  WireBrain(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
                &_genomes,
            int _nrInNodes, int _nrOutNodes,
            std::shared_ptr<ParametersTable> PT_ = nullptr);
  WireBrain(const std::vector<bool> &genome, int _nrInNodes, int _nrOutNodes,
            std::shared_ptr<ParametersTable> PT_ = nullptr);
  virtual ~WireBrain() = default;

  virtual void initialize();
  virtual void
  connectPruneAndSetPopColumns(std::vector<std::pair<int, int>> wormholeList);
  virtual std::shared_ptr<AbstractBrain>
  makeBrain(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
                &_genomes) override;
  virtual std::shared_ptr<AbstractBrain>
  makeCopy(std::shared_ptr<ParametersTable> PT_ = nullptr) override;

  virtual void chargeUpdate();
  virtual void chargeUpdateTrit();
  virtual void update() override;
  virtual void SaveBrainState(std::string fileName);
  virtual void displayBrainState();
  virtual std::string description() override;
  virtual DataMap getStats(std::string &prefix) override;
  virtual std::string getType() override { return "Wire"; }

  virtual void initializeGenomes(
      std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
          &_genomes) override;
  virtual std::unordered_set<std::string> requiredGenomes() override {
    return {genomeName};
  }
};

inline std::shared_ptr<AbstractBrain>
WireBrain_brainFactory(int ins, int outs, std::shared_ptr<ParametersTable> PT) {
  return std::make_shared<WireBrain>(ins, outs, PT);
}

