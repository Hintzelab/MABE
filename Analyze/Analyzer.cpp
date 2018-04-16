
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

std::shared_ptr<ParameterLink<std::string>> Analyzer::outputDotFileDirectoryPL =
    Parameters::register_parameter(
        "ANALYZER-outputDotFileDirectory", std::string("./"),
        "Directory to place dot files that stores State Transition results. "
        "Additionally, each dot file is tagged with the file the org was "
        "loaded from and it's ID in the original file");

std::shared_ptr<ParameterLink<int>> Analyzer::stateTransitionResolutionPL =
    Parameters::register_parameter(
        "ANALYZER-stateTransitionResolutionPL", 1,
        "Number of times to run a world and record IOMaps. Higher "
        "resolution will give increasingly accurate results. (note that "
        "worlds that are constant deterministic only need to be run once");

void Analyzer::analyze(std::shared_ptr<AbstractWorld> world,
                       std::map<std::string, std::shared_ptr<Group>> &groups) {

  // fulfil_requirements(world, groups);
  if (groups.size() != 1 || groups.find("root::") == groups.end()) {
    std::cout << "error: Analyzer cannot actually handle groups. Pass in a "
                 "single group named \"root::\" please. \n";
    exit(1);
  }

  if (calculateKnockoutsPL->get())
    knockoutExperiment(world, groups);

  if (calculateStateTransitionsPL->get())
    stateTransition(world, groups);
}

void Analyzer::fulfil_requirements(
    std::shared_ptr<AbstractWorld> world,
    std::map<std::string, std::shared_ptr<Group>> &groups) {}

void Analyzer::knockoutExperiment(
    std::shared_ptr<AbstractWorld> world,
    std::map<std::string, std::shared_ptr<Group>> &groups) {
  // this functionality tests gate knockout effects for Markov Brains

  // check if labelled MarkovBrains
  // because this is necessary ...
  // auto check_mb =
  //    std::make_shared<MarkovBrain>(dynamic_cast<MarkovBrain
  //    &>(*(groups["root::"]->templateOrg->brains["root::"])));
  if (groups["root::"]->templateOrg->brains["root::"]->brainTypeStrPL->get() !=
      "Markov") {
    std::cout << "error: Knockout Experiments only work with MarkovBrain\n";
    exit(1);
  }

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
    //	safe-ish because of the check above
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

      //  accumulate all the knockout scores into the original organisms DataMap
      org->dataMap.append("KO_Scores", mutant->dataMap.getAverage("score"));
    }

    org->dataMap.setOutputBehavior("KO_Scores",
                                   DataMap::VAR + DataMap::AVE + DataMap::LIST);
    org->dataMap.writeToFile(Analyzer::outputFileKnockoutNamePL->get());
  }
}

std::pair<std::array<long, 4>, long> Analyzer::parseMBnodeLayout(
    std::pair<std::vector<double>, std::vector<double>> &state, int in, int out,
    int hid) {
  // this is where the assumptions are made to allow for brain State
  // Transitions.
  // Nodes are laid out (0,in,out,hid). All values are Bit'ted. A unique label
  // is assigned by reading each component  as a binary-encoded little-endian
  // decimal number. Inputs are overwritten after every update. Since outputs
  // may
  // or may not be overwritten they are considered at the end of the update.
  auto current_in = 0l;
  for (int i = 0; i < in; i++)
    current_in += std::pow(2, i) * Bit(state.first[i]);

  auto current_out = 0l;
  for (int i = 0; i < out; i++)
    current_out += std::pow(2, i) * Bit(state.first[i + in]);

  auto current_hid = 0l;
  for (int i = 0; i < hid; i++)
    current_hid += std::pow(2, i) * Bit(state.first[i + in + out]);

  // if OUTPUT nodes are reset by the world on every update,
  // next_out should be current_out
  auto next_out = 0l;
  for (int i = 0; i < out; i++)
    next_out += std::pow(2, i) * Bit(state.second[i + in]);

  auto next_hid = 0l;
  for (int i = 0; i < hid; i++)
    next_hid += std::pow(2, i) * Bit(state.second[i + in + out]);

  // and we ignore the next in
  // unless the world doesn't write into all inputs on every update

  return std::make_pair(
      std::array<long, 4>{current_hid, current_out, next_hid, next_out},
      current_in);
}

void Analyzer::writeDotFile(
    std::string file_name,
    std::map<std::array<long, 4>, std::map<long, long>> &all_node_edges) {

  // nodes are  [hidden_output]
  // edges between nodes are list of
  // [input[number_of_times_in_input_was_seen]
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
      label += std::to_string(e.first) + "[" + std::to_string(e.second) + "],";
    label.pop_back();
    dot_file << "[label=\"" + label + "\"]\n";
  }
  dot_file << "}";
  dot_file.close();
}

void Analyzer::stateTransition(
    std::shared_ptr<AbstractWorld> world,
    std::map<std::string, std::shared_ptr<Group>> &groups) {

  // check if labelled MarkovBrains
  // because this is necessary ...
  // auto check_mb =
  //    std::make_shared<MarkovBrain>(dynamic_cast<MarkovBrain
  //    &>(*(groups["root::"]->templateOrg->brains["root::"])));
  if (groups["root::"]->templateOrg->brains["root::"]->brainTypeStrPL->get() !=
      "Markov") {
    std::cout
        << "error: State Tranisition Experiments only work with MarkovBrain\n";
    exit(1);
  }

  // this map is only needed since the world API needs a group
  std::map<std::string, std::shared_ptr<Group>> mutated_groups;

  // yank population out of the group that was passed in
  auto population = groups["root::"]->population;

  // for each brain(org) in the population
  for (auto org : population) {

    // set up IO recording
    org->brain->setRecordActivity(true);

	// yank out the MarkovBrain
    auto mb =
        std::make_shared<MarkovBrain>(dynamic_cast<MarkovBrain &>(*org->brain));


    // make a shell organism
    auto mutant = org->makeCopy(Parameters::root);

    // wrap the brain in this organism
    mutant->brain = mb;

    // make a population containg just this org
    std::vector<shared_ptr<Organism>> mutated_population = {mutant};

    // wrap the population in a group
    mutated_groups["root::"] =
        std::make_shared<Group>(mutated_population, groups["root::"]->optimizer,
                                groups["root::"]->archivist);

    // records collated history of hidden_output->hidden_output on
    // input[number_of_times]
    // This data structure assumes that the world RESETS ALL INPUTS on every
    // update and
    // does NOT RESET OUTPUTS on every update.
    // Simply use another data structure if these constraints are violated or
    // the
    // output is wanted in some other form. e.g. to just spew out the raw data
    // to file, there's no need for a data structure at all :)
    std::map<std::array<long, 4>, std::map<long, long>> all_node_edges;

    // higher resolution gives more accurate State Transitions
    auto resolution = stateTransitionResolutionPL->get();
    for (int i = 0; i < resolution; i++) {

      // evaluate the brain
      world->evaluate(mutated_groups, 0, 0, 0);

      // yank the brain out again to look at its update history
      // We don't need to wrap it up in a shared_ptr since we only want
      // to look at it locally
      auto mb_wh = dynamic_cast<MarkovBrain &>(
          *mutated_groups["root::"]->population[0]->brain);

      // structure of nodes in a MarkovBrain
      auto in = mb_wh.nrInputValues;   // from AbstractBrain
      auto out = mb_wh.nrOutputValues; // from AbstractBrain
      auto hid = mb_wh.hiddenNodes; // from MarkovBrain -- which is not ideal,
      // but brains might not store their hidden state as a vector of doubles

      // collate the update history
      for (auto update_state : mb_wh.update_history) {
        // this function makes the same assumptions about the world-brain
        // interaction as mentioned above
        auto con_state = parseMBnodeLayout(update_state, in, out, hid);
        all_node_edges[con_state.first][con_state.second]++;
      }
      // important since we're evaluating the same brain again
      mb_wh.update_history.clear();
    }

    // now we have the data, we can spew it out. This is a uniquely named dot
    // file
    auto orig_id = org->dataMap.getStringOfVector("Orig_ID");
    auto from_file = org->dataMap.getStringOfVector("From_File");
    std::replace(std::begin(from_file), std::end(from_file), '/', '_');
    std::replace(std::begin(from_file), std::end(from_file), '.', '_');
    auto name = from_file.substr(2, from_file.length() - 4) + "__" +
                orig_id.substr(2, orig_id.length() - 4) + ".dot";
    writeDotFile(name, all_node_edges);
  }
}

