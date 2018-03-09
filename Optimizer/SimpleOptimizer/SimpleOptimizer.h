//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "../AbstractOptimizer.h"
#include "../../Utilities/MTree.h"

#include <iostream>
#include <sstream>

class SimpleOptimizer : public AbstractOptimizer {
public:
  static std::shared_ptr<ParameterLink<std::string>>
      selectionMethodPL; // Roullette([Exp=1.05 or Lin=1]),Tounament(size=5)
  static std::shared_ptr<ParameterLink<int>>
      numberParentsPL; // number of parents (default 1, asexual)

  static std::shared_ptr<ParameterLink<std::string>>
      optimizeValuePL; // what value is used to generate
  static std::shared_ptr<ParameterLink<std::string>>
      surviveRatePL; // value between 0 and 1 chance to survive
  static std::shared_ptr<ParameterLink<std::string>> selfRatePL; // value
                                                                 // between 0
                                                                 // and 1 chance
                                                                 // to self (if
                                                                 // more then
                                                                 // one parent)
  static std::shared_ptr<ParameterLink<std::string>>
      elitismCountPL; // this number of organisms will reproduce asexualy (i.e.
                      // copy w/ mutation)
  static std::shared_ptr<ParameterLink<std::string>>
      elitismRangePL; // best n organisms will each produce

  static std::shared_ptr<ParameterLink<std::string>>
      nextPopSizePL; // number of genomes in the population

  std::string selectionMethod;
  int numberParents;

  ///

  int oldPopulationSize;
  int nextPopulationTargetSize;
  int nextPopulationSize;

  int selfCount;
  int eliteCount;
  int surviveCount;

  double aveScore;
  double maxScore;
  double minScore;

  std::vector<int> elites;
  std::vector<double> scores;

  ///
  class AbstractSelector {
  public:
    SimpleOptimizer *SO;
    AbstractSelector() = default;
    AbstractSelector(SimpleOptimizer *_SO) : SO(_SO){};
    virtual int select() = 0;
    virtual std::string getType() = 0;
  };

  class RouletteSelector : public AbstractSelector {
  public:
    RouletteSelector(std::string &argumentsString, SimpleOptimizer *_SO)
        : AbstractSelector(_SO) {
      // now decode arguments
      std::vector<std::string> arguments;
      std::stringstream ss(argumentsString); // Turn the string into a stream.
      std::string tok;

      while (getline(ss, tok, ',')) {
        arguments.push_back(tok);
      }
      if (arguments.size() > 0) {
        std::cout << "  in RouletteSelector constructor, found arguments \""
                  << argumentsString
                  << "\" but Roulette takes no arguments.\n  exiting..."
                  << std::endl;
        exit(1);
      }
    }

    virtual int select() override {
      int pick;
      do {
        pick = Random::getIndex(SO->oldPopulationSize); // keep choosing a
                                                        // random genome from
                                                        // population until we
                                                        // get one that's good
                                                        // enough
      } while (Random::getDouble(1) > (SO->scores[pick] / SO->maxScore));
      return pick;
    }

    virtual std::string getType() override { return (std::string) "Roulette"; }
  };

  class TournamentSelector : public AbstractSelector {
  public:
    int tournamentSize;
    TournamentSelector(std::string &argumentsString, SimpleOptimizer *_SO)
        : AbstractSelector(_SO) {
      // now decode arguments
      std::vector<std::string> arguments;
      std::stringstream ss(argumentsString); // Turn the string into a stream.
      std::string tok;

      while (getline(ss, tok, ',')) {
        arguments.push_back(tok);
      }
      for (auto arg : arguments) {
        std::vector<std::string> parts;
        std::stringstream ss(arg);
        std::string tok;
        while (getline(ss, tok, '=')) {
          parts.push_back(tok);
        }
        if (parts[0] == "size") {
          if (!load_value(parts[1], tournamentSize)) {
            std::cout << "  in TournamentSelector constructor, got bad value "
                         "for size \""
                      << parts[1] << "\"\n  exiting..." << std::endl;
            exit(1);
          }
        } else {
          std::cout << "  in TournamentSelector constructor, found unknown "
                       "argument pair \""
                    << parts[0] << "=" << parts[1] << "\"\n  exiting..."
                    << std::endl;
          exit(1);
        }
      }
    }

    virtual int select() override {
      int winner, challanger;
      winner = Random::getIndex(SO->oldPopulationSize);
      for (int i = 0; i < tournamentSize - 1; i++) {
        challanger = Random::getIndex(SO->oldPopulationSize);
        // cout << tournamentSize << " " <<i << "  " <<
        // challanger<<"("<<SO.scores[challanger] << "),winner(" <<
        // SO.scores[winner] << ")";
        if (SO->scores[challanger] > SO->scores[winner]) {
          // cout << " *";
          winner = challanger;
        }
        // cout << endl;
      }
      return winner;
    }

    virtual std::string getType() override {
      return (std::string) "Tournament";
    }
  };

  std::shared_ptr<AbstractSelector> selector;

  std::shared_ptr<Abstract_MTree> optimizeValueMT, surviveRateMT, selfRateMT,
      elitismCountMT, elitismRangeMT, nextPopSizeMT;

  SimpleOptimizer(std::shared_ptr<ParametersTable> _PT = nullptr);

  virtual void
  optimize(std::vector<std::shared_ptr<Organism>> &population) override;

  // virtual string maxValueName() override {
  //	return (PT == nullptr) ? optimizeValuePL->lookup() :
  //PT->lookupString("OPTIMIZER_Simple-optimizeValue");
  //}
};

