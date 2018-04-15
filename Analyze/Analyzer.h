/*
 * Analysis support
 * Takes a population
 * For each org: Do Analysis as defined by org
 *
 *
 */

#include <iostream>
#include <memory>
#include <map>
#include <array>
#include <vector>
#include "../World/AbstractWorld.h"
#include "../Brain/MarkovBrain/MarkovBrain.h"

class Analyzer {

  void
  fulfil_requirements(std::shared_ptr<AbstractWorld> world,
                      std::map<std::string, std::shared_ptr<Group>> &groups);
  // std::shared_ptr<AbstractWorld> world;

  // std::map<std::string, std::shared_ptr<Group>> groups;
public:
  static std::shared_ptr<ParameterLink<std::string>> outputFileKnockoutNamePL;
  static std::shared_ptr<ParameterLink<std::string>> outputDotFileDirectoryPL;
  static std::shared_ptr<ParameterLink<bool>> calculateKnockoutsPL;
  static std::shared_ptr<ParameterLink<bool>> calculateStateTransitionsPL;
  static std::shared_ptr<ParameterLink<int>> stateTransitionResolutionPL;

  
  Analyzer() = default;
  void analyze(std::shared_ptr<AbstractWorld> world,
               std::map<std::string, std::shared_ptr<Group>> &groups);
  void
  knockoutExperiment(std::shared_ptr<AbstractWorld> world,
                     std::map<std::string, std::shared_ptr<Group>> &groups);
  void
  stateTransition(std::shared_ptr<AbstractWorld> world,
                     std::map<std::string, std::shared_ptr<Group>> &groups);

  std::pair<std::array<long, 4>,long>
  parseMBnodeLayout(std::pair<std::vector<double>, std::vector<double>>& state,
                    int in, int out, int hid);

  void
  writeDotFile(std::string file_name,
               std::map<std::array<long, 4>, std::map<long,long>> &all_node_edges);
};

