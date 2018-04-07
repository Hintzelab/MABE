/*
 * Analysis support
 * Takes a world (configurable)
 * Takes a population
 * For each org: Do Analysis as defined by org
 *
 *
 */

#include <iostream>
#include <memory>
#include <map>
//#include "../World/DayNightWorld/DayNightWorld.h"
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
  static std::shared_ptr<ParameterLink<std::string>>
      outputDotFilePrefixForStateTransitionPL;
  static std::shared_ptr<ParameterLink<bool>> calculateKnockoutsPL;
  static std::shared_ptr<ParameterLink<bool>> calculateStateTransitionsPL;

Analyzer() = default;
  void analyze(std::shared_ptr<AbstractWorld> world,
               std::map<std::string, std::shared_ptr<Group>> &groups);
  void
  knockoutExperiment(std::shared_ptr<AbstractWorld> world,
                     std::map<std::string, std::shared_ptr<Group>> &groups);
  void
  stateTransition(std::shared_ptr<AbstractWorld> world,
                     std::map<std::string, std::shared_ptr<Group>> &groups);

};

