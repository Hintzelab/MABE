
//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include <module_factories.h>
#include <Global.h>
#include <Group/Group.h>
#include <Organism/Organism.h>
#include <Utilities/Utilities.h>
#include <Utilities/Data.h>
#include <Utilities/Loader.h>
#include <Utilities/MTree.h>
#include <Utilities/Parameters.h>
#include <Utilities/Random.h>
#include <Utilities/Utilities.h>
#include <Utilities/gitversion.h>
#include <Utilities/Filesystem.h>

#include <algorithm>
#include <csignal> // sigint
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <regex>
#include <vector>


#if defined(__MINGW32__)
#include <windows.h> /// for getting PID, for proper RNG for MinGW
#endif

volatile sig_atomic_t userExitFlag = 0;
void catchCtrlC(int signalID) {
  if (userExitFlag==1) {
      printf("Early termination requested. Results may be incomplete.\n");
      raise(SIGTERM);
  }
  userExitFlag = 1;
  printf("\nQuitting after current update. (ctrl-c again to force quit)\n");
}

std::map<std::string, std::shared_ptr<Group>>
constructAllGroupsFrom(const std::shared_ptr<AbstractWorld> &world,
                       std::shared_ptr<ParametersTable> PT);

int main(int argc, const char *argv[]) {
  signal(SIGINT, catchCtrlC);


  configureDefaultsAndDocumentation(); // sets up values from modules.h
  Parameters::initializeParameters(argc, argv); // loads command line and
                                                // configFile values into
                                                // registered parameters
  std::cout << MABE_pretty_logo;

  // also writes out a settings files if requested
  if (Parameters::save_files) { // if saveFiles (save settings files) is set
    int maxLineLength = Global::maxLineLengthPL->get();
    int commentIndent = Global::commentIndentPL->get();

    auto prefix = Parameters::save_file_prefix;
    auto dir_part = prefix.substr(0, prefix.find_last_of('/'));

    //if (!zz::os::is_directory(dir_part)) {
    if (!isDirectory(dir_part)) {
      std::cout << "Error : Directory \"" << dir_part
                << "/\" does not exist. Settings Files will not be saved.\n";
      exit(1);
    }

    if (!Parameters::save_files_stdout) {
      std::cout << "Saving settings files ..." << std::flush;
    }

    Parameters::saveSettingsFiles(
        maxLineLength, commentIndent, {"*"},
        {{prefix + "settings_organism.cfg", {"GATE*", "GENOME*", "BRAIN*"}},
         {prefix + "settings_world.cfg", {"WORLD*"}},
         {prefix + "settings.cfg", {""}}});

    if (!Parameters::save_files_stdout) {
      std::cout << std::endl
          << "Settings files saved in Directory \"" << dir_part
          << "\" with prefix \""
          << prefix.substr(prefix.find_last_of('/') + 1) << "\""
          << std::endl;
    }
    exit(0);
  }

  ////////////////////////////////////////////////////////////////////////////
  /// IF YOU WANT TO HACK PARAMETERS, DO IT HERE!
  /// /////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  ///
  /// Parameters::root->setExistingParameter("BRAIN-brainType", "LSTM");
  ///
  ///////////////////////////////////////////////////////////////////////////

  auto output_prefix = "./" + Global::outputPrefixPL->get();
  auto output_dir = output_prefix.substr(0, output_prefix.find_last_of('/'));

  //if (output_prefix.back() != '/' && zz::os::is_directory(output_prefix)) {
  if (output_prefix.back() != '/' && isDirectory(output_prefix)) {
    std::cout << "Warning: Output files will be saved in Directory \""
              << output_dir << "\" with the prefix \""
              << output_prefix.substr(output_prefix.find_last_of('/') + 1)
              << "\"\n         If you mean to save outputFiles to the Directory \""
              << output_prefix << "/\", please end outputPrefix with /\n"
              << std::endl;
  }

  //if (!zz::os::is_directory(output_dir)) {
  if (!isDirectory(output_dir)) {
    std::cout << "Error : outputDirectory \"" <<output_dir
              << "\" does not exist\n";
    exit(1);
  }
  FileManager::outputPrefix = output_prefix;

  // set up random number generator
  if (Global::randomSeedPL->get() == -1) {
    std::random_device rd;
/// random_device is not implemented for MinGW on windows (it's like the old
/// rand())
/// so we need to seed it with entropy
#if defined(__MINGW32__)
    DWORD temp = GetCurrentProcessId();
#else
    int temp = rd();
#endif
    Random::getCommonGenerator().seed(temp);
    std::cout << "Generating Random Seed\n  " << temp << "\n";
  } else {
    Random::getCommonGenerator().seed(Global::randomSeedPL->get());
    std::cout << "Using Random Seed: " << Global::randomSeedPL->get() << "\n";
  }

  // make world uses WORLD-worldType to determine type of world
  auto world = makeWorld(Parameters::root);

  std::cout << "\nRunning World " << world->worldTypePL->get() << "\n";

  std::shared_ptr<ParametersTable> PT;
  auto groups = constructAllGroupsFrom(world, PT);

  Global::update = 0;


  if (Global::modePL->get() == "run") {
    ////////////////////////////////////////////////////////////////////////////////////
    // run mode - evolution loop
    ////////////////////////////////////////////////////////////////////////////////////
    std::cout << "\n  You are running MABE in run mode."
              << "\n"
              << "\n";

    // in run mode we evolve organsims
    auto done = false;
    while ((!done) && (!userExitFlag)) { //! groups[defaultGroup]->archivist->finished) {
      world->evaluate(groups, false, false,
                      AbstractWorld::debugPL->get()); // evaluate each organism
                                                      // in the population using
                                                      // a World
      std::cout << "update: " << Global::update << "   " << std::flush;
      done = true; // until we find out otherwise, assume we are done.
      for (auto const &group : groups) {
        if (!group.second->archivist->finished_) {
          group.second->optimize(); // create the next updates population
          group.second->archive(); // save data, update memory and delete unneeded data;
          if (!group.second->archivist->finished_) {
            done = false; // if any groups archivist says we are not done, then
                          // we are not done
          }
          group.second->optimizer->cleanup(group.second->population);
        }
      }
	  std::cout << std::endl;
      Global::update++; // advance time to create new population(s)
    }

    // the run is finished... flush any data that has not been output yet
    for (auto const &group : groups) {
      group.second->archive(1);
    }
  } else if (Global::modePL->get() == "visualize") {
    ////////////////////////////////////////////////////////////////////////////////////
    // visualize mode
    ////////////////////////////////////////////////////////////////////////////////////
    std::cout << "\n  You are running MABE in visualize mode."
              << "\n"
              << "\n";

    world->evaluate(groups, 0, 1, AbstractWorld::debugPL->get());
  } else if (Global::modePL->get() == "analyze") {
    ////////////////////////////////////////////////////////////////////////////////////
    // analyze mode
    ////////////////////////////////////////////////////////////////////////////////////
    std::cout << "\n  You are running MABE in analyze mode."
              << "\n"
              << "\n";

    world->evaluate(groups, 1, 0, 0);
  } else {
    std::cout << "error: unrecognized GLOBAL-mode " << Global::modePL->get()
              << std::endl;
    exit(1);
  }
  return 0;
}

// for each name space in the GLOBAL-groups create a group. if GLOBAL-groups
// is empty, create "default" group.
std::map<std::string, std::shared_ptr<Group>>
constructAllGroupsFrom(const std::shared_ptr<AbstractWorld> &world,
                       std::shared_ptr<ParametersTable> PT) {

  std::map<std::string, std::shared_ptr<Group>> groups;
  auto worldRequirements = world->requiredGroups();
  for (auto const &groupInfo : worldRequirements) {
    std::cout << "\n";
    auto NS = groupInfo.first;
    PT = NS == "root::" ? Parameters::root : Parameters::root->getTable(NS);
    // create (or get a pointer to) a new
    // parameters table with this name

    std::cout << "Building group with name space: " << groupInfo.first << "\n";

    // create an optimizer of type defined by OPTIMIZER-optimizer
    auto optimizer = makeOptimizer(PT);

    std::unordered_set<std::string> brainNames;

    std::unordered_map<std::string, std::shared_ptr<AbstractBrain>>
        templateBrains; // templates for brains in organisms in this group
    std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
        templateGenomes; // templates for genomes in organisms in this group

    std::unordered_set<std::string> strSet; // temporary holder
    auto genomeNames =
        optimizer->requiredGenomes(); // get genome names from optimizer
    std::map<std::string, int> brainIns;
    std::map<std::string, int> brainOuts;

    for (auto const &s : groupInfo.second) {
      // for each group required by world, determine the brains and genomes
      // required by the world. Also determine if any
      // of the required brains require genomes. If two requirements have the
      // same name, this indicated that only a single
      // instance should be created and at any time that this name is accessed,
      // this will refer to this single instance
      // (i.e. if the world required "bodyGenome" and the brain required
      // "genome" and both were given the same name, then
      // when the world looked up "bodyGenome" and when the brain looked up
      // "genome" they would be looking at the same object.
      if (s.size() <= 4) {
        std::cout << "\n\nwhile converting world requirements in \"" << NS
                  << "\" group, found requirement \"" << s
                  << "\".\n requirements must start with B: (brain) or G: "
                     "(genome), followed with a name and end with "
                     "\"::\".\nexiting."
                  << std::endl;
        exit(1);
      }
      if (s[0] == 'G' && s[1] == ':') {
        genomeNames.insert(s.substr(2));
      } else if (s[0] == 'B' && s[1] == ':') {
        auto workingString = s.substr(2);
        auto brainName = workingString.substr(0, workingString.find(','));
        brainNames.insert(brainName);
        workingString = workingString.substr(workingString.find(',') + 1);
        int ins, outs;
        convertString(workingString.substr(0, workingString.find(',')), ins);
        convertString(workingString.substr(workingString.find(',') + 1), outs);
        brainIns[brainName] = ins;
        brainOuts[brainName] = outs;
      } else {
        std::cout << "\n\nwhile converting world requirements in \"" << NS
                  << "\" group, found requirement \"" << s
                  << "\".\n requirements must start with B: (brain) or G: "
                     "(genome)!\nexiting."
                  << std::endl;
        exit(1);
      }
    }

    std::cout << "\n"
              << " building brains..."
              << "\n";

    for (auto const &brainName : brainNames) {
      std::cout << "  found brain: " << brainName << "\n";
      std::shared_ptr<ParametersTable> This_PT;
      if (brainName.empty()) {
        std::cout
            << "\n\nfound empty brain name, this is not allowed. Exiting..."
            << std::endl;
        exit(1);
      }
      This_PT = brainName == "root::" ? Parameters::root
                                      : Parameters::root->getTable(brainName);

      std::cout << "    ... building a "
                << This_PT->lookupString("BRAIN-brainType") << " brain using "
                << brainName << " name space."
                << "\n";
      templateBrains[brainName] =
          makeTemplateBrain(brainIns[brainName], brainOuts[brainName], This_PT);
      strSet = templateBrains[brainName]->requiredGenomes();
      if (!strSet.empty()) {
        std::cout << "    ..... this brain requires genomes: ";
        for (auto const &g : strSet) {
          std::cout << g << "  ";
        }
        std::cout << "\n";
      }
      genomeNames.insert(
          strSet.begin(),
          strSet.end()); // add this brains required genomes to genome names
    }

    std::cout << "\n"
              << " building genomes..."
              << "\n";

    for (auto const &genomeName : genomeNames) {
      std::cout << "  found genome: " << genomeName << "\n";
      std::shared_ptr<ParametersTable> This_PT;
      if (genomeName.empty()) {
        std::cout
            << "\n\nfound empty genome name, this is not allowed. Exiting..."
            << std::endl;
        exit(1);
      }
      This_PT = genomeName == "root::" ? Parameters::root
                                       : Parameters::root->getTable(genomeName);
      // this genome is not at root

      std::cout << "    ... building a "
                << This_PT->lookupString("GENOME-genomeType")
                << " genome using " << genomeName << " name space."
                << "\n";
      templateGenomes[genomeName] = makeTemplateGenome(This_PT);
    }

	std::cout << std::flush;
    // make a organism with a templateGenomes and templateBrains - progenitor
    // serves as an ancestor to all and a template organism
    Global::update = -2; // in the begining there was a progenitor
      auto progenitor =
        std::make_shared<Organism>(templateGenomes, templateBrains, PT);
    Global::update = -1; // then in the image of the progenitor the first generation was crafted

    std::vector<std::shared_ptr<Organism>> population;

    auto file_to_load = Global::initPopPL->get(PT);
    // start of initPop fix :: adding support so that a number can be given for initPop
    if (std::isdigit(file_to_load[0])) {
        //std::cout << "  updataing initPop parameter to \"defaut " << file_to_load << "\"" << std::endl;
        file_to_load = "default " + file_to_load;
    }
    // end of initPop fix

    Loader loader;
    auto orgs_to_load = loader.loadPopulation(file_to_load);
    int population_size = orgs_to_load.size();

    if (!population_size) {
      std::cout << "error: MASTER must contain at least one organism"
                << std::endl;
      exit(1);
    }
    // add population_size organisms which look like progenitor but could be
    // loaded from file
    for (auto &orgData : orgs_to_load) {
      // make a new genome like the template genome
      std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> newGenomes;
      std::unordered_map<std::string, std::shared_ptr<AbstractBrain>> newBrains;
      for (auto const &genome : templateGenomes) {
        if (orgData.first < 0) { // if this org is not loaded...
          newGenomes[genome.first] = genome.second->makeLike();
        } else { // if this file is loaded ...
          auto name = "GENOME_" + genome.first;
          genome.second->deserialize(genome.second->PT, orgData.second, name);
          newGenomes[genome.first] = genome.second;
        }
      }
      for (auto const &brain : templateBrains) {
        if (orgData.first < 0) { // if this brain is not loaded
          brain.second->initializeGenomes(newGenomes);
		  newBrains[brain.first] = brain.second->makeBrain(newGenomes);
		}
		else { // if this brain is loaded
	      newBrains[brain.first] = brain.second->makeBrain(newGenomes);
		  auto name = "BRAIN_" + brain.first;
		  newBrains[brain.first]->deserialize(brain.second->PT, orgData.second, name);
		}
      }
      auto newOrg = std::make_shared<Organism>(progenitor, newGenomes, newBrains, PT);

      // transfer provenance data to newly constructed org datamaps
/* copy of code below is this needed?
	  if ( orgData.first >= 0 ) {
        if (orgData.second.find("loadedFrom.File") != orgData.second.end()) {
          newOrg->dataMap.set("loadedFrom.File", std::string(orgData.second["loadedFrom.File"]));
        }
        if (orgData.second.find("loadedFrom.ID") != orgData.second.end()) {
          newOrg->dataMap.set("loadedFrom.ID", static_cast<int>(std::stol(orgData.second["loadedFrom.ID"])));
        }
        if (orgData.second.find("loadedFrom.Update") != orgData.second.end()) {
          newOrg->dataMap.set("loadedFrom.Update", static_cast<int>(std::stol(orgData.second["loadedFrom.Update"])));
        }
      }

      if ( orgData.first >= 0 ) {
        if (orgData.second.find("loadedFrom.File") != orgData.second.end()) {
          newOrg->dataMap.set("loadedFrom.File", std::string(orgData.second["loadedFrom.File"]));
        }
        if (orgData.second.find("loadedFrom.ID") != orgData.second.end()) {
          newOrg->dataMap.set("loadedFrom.ID", static_cast<int>(std::stol(orgData.second["loadedFrom.ID"])));
        }
        if (orgData.second.find("loadedFrom.Update") != orgData.second.end()) {
          newOrg->dataMap.set("loadedFrom.Update", static_cast<int>(std::stol(orgData.second["loadedFrom.Update"])));
        }
      }
*/
      if ( orgData.first >= 0 ) {
        if (orgData.second.find("loadedFrom.File") != orgData.second.end()) {
          newOrg->dataMap.set("loadedFrom.File", std::string(orgData.second["loadedFrom.File"]));
		  newOrg->dataMap.setOutputBehavior("loadedFrom.File", DataMap::NO_OUTPUT);
        }
        if (orgData.second.find("loadedFrom.ID") != orgData.second.end()) {
          newOrg->dataMap.set("loadedFrom.ID", static_cast<int>(std::stol(orgData.second["loadedFrom.ID"])));
          newOrg->dataMap.setOutputBehavior("loadedFrom.ID", DataMap::NO_OUTPUT);
        }
        if (orgData.second.find("loadedFrom.Update") != orgData.second.end()) {
          newOrg->dataMap.set("loadedFrom.Update", static_cast<int>(std::stol(orgData.second["loadedFrom.Update"])));
          newOrg->dataMap.setOutputBehavior("loadedFrom.Update", DataMap::NO_OUTPUT);
        }
        std::cout << "[" << orgData.second["loadedFrom.File"] << "," << orgData.second["loadedFrom.ID"] << "," << orgData.second["loadedFrom.Update"] << "]" << std::endl;
      }

      // add new organism to population
      population.push_back(newOrg);
    }

    // popFileColumns holds a list of data titles which various modules indicate
    // are interesting/should be tracked and which are averageable
    // ** popFileColumns define what will appear in the pop.csv file **
    // the following code asks world, genomes and brains for ave file columns
    std::vector<std::string> popFileColumns;
    popFileColumns.clear();
    popFileColumns.push_back("update");
    popFileColumns.insert(popFileColumns.end(), world->popFileColumns.begin(),
                          world->popFileColumns.end());
    popFileColumns.insert(popFileColumns.end(),
                          optimizer->popFileColumns.begin(),
                          optimizer->popFileColumns.end());
    for (auto const &genome : progenitor->genomes) {
      for (auto const &c : genome.second->popFileColumns) {
        (genome.first == "root::") ? popFileColumns.push_back(c)
                                   : popFileColumns.push_back(genome.first + c);
      }
    }
    for (auto const &brain : progenitor->brains) {
      for (auto const &c : brain.second->popFileColumns) {
        (brain.first == "root::") ? popFileColumns.push_back(c)
                                  : popFileColumns.push_back(brain.first + c);
      }
    }

    // create an archivist of type determined by ARCHIVIST-outputMethod
    auto archivist =
        makeArchivist(popFileColumns, optimizer->optimizeFormula, PT,
                      (groupInfo.first == "root::") ? "" : groupInfo.first);

    // create a new group with the new population, optimizer and archivist and
    // place this group in the map groups
    groups[groupInfo.first] =
        std::make_shared<Group>(population, optimizer, archivist);

    groups[groupInfo.first]->templateOrg = progenitor->makeCopy();
    // the progenitor has served it's purpose. Killing an organsim is important
    // as it allows for cleanup.
    progenitor->kill();

    // report on what was just built
    std::cout << "\nFinished Building Group: " << groupInfo.first
              << "   Group name space: " << NS
              << "\n  population size: " << population_size
              << "     Optimizer: " << PT->lookupString("OPTIMIZER-optimizer")
              << "     Archivist: "
              << PT->lookupString("ARCHIVIST-outputMethod") << "\n"
              << "\n";
    // end of report
  }
  return groups;
}
