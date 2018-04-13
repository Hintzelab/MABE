
#include <iostream>
#include <memory>
#include <set>
#include <utility>
#include <cmath>
#include <array>

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
    Analyzer::outputDotFileDirectoryPL =
        Parameters::register_parameter(
            "ANALYZER-outputDotFileDirectory", std::string("./"),
            "Directory to place dot files that stores State Transition results. "
            "Additionally, each dot file is tagged with the file the org was "
            "loaded from and it's ID in the original file");

std::shared_ptr<ParameterLink<int>>
    Analyzer::stateTransitionResolutionPL =
        Parameters::register_parameter(
            "ANALYZER-stateTransitionResolutionPL", 1,
            "Number of times to run a world and record IOMaps. Higher "
            "resolution will give increasingly accurate results. (note that "
            "worlds that are constant deterministic only need to be run once");

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
  // this functionality tests gate knockout effects for Markov Brains

  // this map is only needed since the world API needs a group
  std::map<std::string, std::shared_ptr<Group>> mutated_groups;

  // yank population out of the group that was passed in
  auto population = groups["root::"]->population;

  // for each brain(org) in the population
  for (auto org : population) {

    // make a population containg just this org
    std::vector<shared_ptr<Organism>> unmutated_population = {org};

	// wrap this population up in a group
    // for some reason this group needs the key to be exactly "root::"
    mutated_groups["root::"] = std::make_shared<Group>(
        unmutated_population, groups["root::"]->optimizer,
        groups["root::"]->archivist);

    // finally, we can evaluate the organism to get a baseline score
    world->evaluate(mutated_groups, 0, 0, 0);

    //	now yank the MarkovBrain out of the organism
    auto mb =
        std::make_shared<MarkovBrain>(dynamic_cast<MarkovBrain &>(*org->brain));

    // for every brain with a single gate knockout
    for (auto b : mb->getAllSingleGateKnockouts()) {

      // make a shell organism
      auto mutant = org->makeCopy(Parameters::root);

      // wrap the brain in this organism
      mutant->brain = b;

      // wrap the organism in a population
      std::vector<shared_ptr<Organism>> mutated_population = {mutant};

      // wrap the population in a group
      mutated_groups["root::"] = std::make_shared<Group>(
          mutated_population, groups["root::"]->optimizer,
          groups["root::"]->archivist);

      // finally  we can evaluate the knockout brain in the world
      world->evaluate(mutated_groups, 0, 0, 0);

      auto ko_fitness = mutant->dataMap.getAverage("score");

      //  accumulate all the knockout scores into the original organisms DataMap
      org->dataMap.append("KO_Scores", ko_fitness);
    }

    org->dataMap.setOutputBehavior("KO_Scores",
                                   DataMap::VAR + DataMap::AVE + DataMap::LIST);
    auto name = Analyzer::outputFileKnockoutNamePL->get();
    org->dataMap.writeToFile(name);
  }
}

std::pair<std::array<long, 4>,long>
Analyzer::parseMBnodeLayout(std::pair<std::vector<double>, std::vector<double>> state,
                  int in, int out, int hid) {
// this is where the assumptions are made to allow for brain State Transitions.
// Nodes are laid out (0,in,out,hid). All values are Bit'ted. A unique label
// is assigned by reading each component  as a binary-encoded little-endian
// decimal number. Inputs are overwritten after every update. Since outputs may
// or may not be overwritten they are considered at the end of the update.

		auto current_in  = 0l;
		for(int i=0;i<in;i++)
			current_in += std::pow(i,2)*state.first[i];

		auto current_out   = 0l;
		for(int i=0;i<in;i++)
			current_out += std::pow(i,2)*state.first[i+in];

		auto current_hid  = 0l;
		for(int i=0;i<in;i++)
			current_hid += std::pow(i,2)*state.first[i+in+out];
	
		auto next_out = 0l;
		for(int i=0;i<in;i++)
			next_out += std::pow(i,2)*state.second[i+in];

		auto next_hid = 0l;
		for(int i=0;i<in;i++)
			next_hid += std::pow(i,2)*state.second[i+in+out];

		// and we ignore the next in
		
		return std::make_pair(std::array<long,4>{current_hid,current_out,next_hid,next_out},current_in);

}

void Analyzer::writeDotFile(
    std::string file_name,
    std::map<std::array<long, 4>, std::set<long>> &all_node_edges) {

  std::ofstream dot_file(outputDotFileDirectoryPL->get() + "/" + file_name);
  dot_file << "digraph {\n";
  for (auto ne : all_node_edges) {
    dot_file << "\""
             << std::to_string(ne.first[0]) + "_" +
                    std::to_string(ne.first[1]) + "\" -> \"" +
                    std::to_string(ne.first[2]) + "_" +
                    std::to_string(ne.first[3]) + "\"";
    std::string label;
    for (auto &e : ne.second)
      label += std::to_string(e) + ",";
    label.pop_back();
    dot_file << "[label=" + label + "\n";
  }
  dot_file << "}";
  dot_file.close();
}

void Analyzer::stateTransition(
    std::shared_ptr<AbstractWorld> world,
    std::map<std::string, std::shared_ptr<Group>> &groups) {

  std::map<std::string, std::shared_ptr<Group>> mutated_groups;

  auto population = groups["root::"]->population;
  for (auto org : population) {

    auto mb =
        std::make_shared<MarkovBrain>(dynamic_cast<MarkovBrain &>(*org->brain));

    // set up IO recording
    mb->record_update_history = true;
	auto & loc_uh = mb->update_history;
	auto id = org->dataMap.getStringOfVector("Orig_ID");
    id = id.substr(2, id.length() - 4);

    auto mutant = org->makeCopy(Parameters::root);
    mutant->brain = mb;

    // wrap the organism in a population
    std::vector<shared_ptr<Organism>> mutated_population = {mutant};

    // wrap the population in a group
    mutated_groups["root::"] =
        std::make_shared<Group>(mutated_population, groups["root::"]->optimizer,
                                groups["root::"]->archivist);

    // records collated history of hidden_output->hidden_output on input
    std::map<std::array<long, 4>, std::set<long>> all_node_edges;
    auto resolution = stateTransitionResolutionPL->get();
    for (int i = 0; i < resolution; i++) {

	  std::cout  << "&" << loc_uh.size();
      world->evaluate(mutated_groups, 0, 0, 0);
	  std::cout  << "&" << loc_uh.size();
      auto mb_wh = std::make_shared<MarkovBrain>(dynamic_cast<MarkovBrain &>(
          *mutated_groups["root::"]->population[0]->brain));

      auto in = mb_wh->nrInputValues;
      auto out = mb_wh->nrOutputValues;
      auto hid = mb_wh->hiddenNodes;
	  std::cout  << mb_wh->update_history.size();
	  std::cout  << mb->update_history.size();

      for (auto update_state : mb_wh->update_history) {
        auto con_state = parseMBnodeLayout(update_state, in, out, hid);
        all_node_edges[con_state.first].insert(con_state.second);
      }
	  mb_wh->update_history.clear();
    }

    writeDotFile(id, all_node_edges);
  }
}

