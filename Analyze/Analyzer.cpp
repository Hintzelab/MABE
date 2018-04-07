
#include <iostream>
#include <memory>

#include "Analyzer.h"

std::shared_ptr<ParameterLink<bool>> Analyzer::calculateKnockoutsPL =
    Parameters::register_parameter("ANALYZER-calculateKnockouts", false,
                                   "If true, will calculate single gate "
                                   "knockout effects(for Markov Brains), for "
                                   "provided world settings");

std::shared_ptr<ParameterLink<bool>> Analyzer::calculateStateTransitionsPL =
    Parameters::register_parameter("ANALYZER-calculateStateTransitions", false,
                                   "If true, will calculate State Transitions "
                                   "dot file for every loaded org(for Markov "
                                   "Brains), for provided world settings");

std::shared_ptr<ParameterLink<std::string>> Analyzer::outputFileKnockoutNamePL =
    Parameters::register_parameter(
        "ANALYZER-outputFileKnockoutName", std::string("KO_aux.csv"),
        "Name of file to store results of Knockout Experiment");

std::shared_ptr<ParameterLink<std::string>>
    Analyzer::outputDotFilePrefixForStateTransitionPL =
        Parameters::register_parameter(
            "ANALYZER-outputDotFilePrefixForStateTransition", std::string("stt"),
            "Prefix of dot file that stores State Transition results. "
            "Additionally, each dot file is tagged with the file the org was "
            "loaded from and it's ID in the original file");

void Analyzer::analyze(std::shared_ptr<AbstractWorld> world,
                       std::map<std::string, std::shared_ptr<Group>> &groups) {

  fulfil_requirements(world, groups);

  if (calculateKnockoutsPL->get())
	  knockoutExperiment(world, groups);

  if (calculateStateTransitionsPL->get())
	  stateTransition(world, groups);
}

void Analyzer::fulfil_requirements(
    std::shared_ptr<AbstractWorld> world,
    std::map<std::string, std::shared_ptr<Group>> &groups) {
	
  if (groups.size() != 1) {
    std::cout << " expect groups size 1\n";
    exit(1);
  }

  if (groups["root::"]->templateOrg->brains["root::"]->brainTypeStrPL->get() !=
      "Markov") {
    std::cout << " Analyzer only works with MarkovBrains\n";
    exit(1);
  }
}

void Analyzer::knockoutExperiment(
    std::shared_ptr<AbstractWorld> world,
    std::map<std::string, std::shared_ptr<Group>> &groups) {

  auto population = groups["root::"]->population;
//  std::vector<shared_ptr<Organism>> mutated_population;
  for (auto org : population) {

    world->evaluateSolo(org, 0, 0, 0);
    auto mb =
        std::make_shared<MarkovBrain>(dynamic_cast<MarkovBrain &>(*org->brain));

    for (auto b : mb->getAllSingleGateKnockouts()) {

      auto mutant = org->makeCopy(Parameters::root);

      mutant->brain = b;

      world->evaluateSolo(mutant, 0, 0, 0);

      auto ko_fitness = mutant->dataMap.getAverage("score");

      org->dataMap.append("KO_Scores", ko_fitness);
    }
    
	org->dataMap.setOutputBehavior("KO_Scores",
                                   DataMap::VAR + DataMap::AVE + DataMap::LIST);
    auto name = Analyzer::outputFileKnockoutNamePL->get();
    org->dataMap.writeToFile(name);
  }
}

  
void
Analyzer::  stateTransition(std::shared_ptr<AbstractWorld> world,
                     std::map<std::string, std::shared_ptr<Group>> &groups){

//  DayNightWorld::startWithDayPL->set(false);

  auto population = groups["root::"]->population;
  for (auto org : population) {

    auto mb =
        std::make_shared<MarkovBrain>(dynamic_cast<MarkovBrain &>(*org->brain));

	// set up IO recording
	mb->recordIOMapPL->set(true);
   auto id = org->dataMap.getStringOfVector("orig_ID");
	id =  id.substr(2,id.length()-4);

	auto mutant = org->makeCopy(Parameters::root);
	  mutant->brain = mb;

	for (int i=0;i<100;i++) {
         /*
		   mb->IOMapFileNamePL->set("Night_" + id.substr(2, id.length() - 4) +
                                   "_" + std::to_string(i) + "_" +
                                   outputFileStateTransitionPL->get());
          */
                world->evaluateSolo(mutant, 0, 0, 0);
                mb->IOMapFileNamePL->set(
                    "Night_" + id + "_" + std::to_string(i) + "_" +
                    outputDotFilePrefixForStateTransitionPL->get());
  }
  }
}
