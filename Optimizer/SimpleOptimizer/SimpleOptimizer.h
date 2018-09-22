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


  static std::shared_ptr<ParameterLink<double>> cullBelowPL;
  static std::shared_ptr<ParameterLink<double>> cullRemapPL;
  static std::shared_ptr<ParameterLink<bool>> cullByRangePL;

  std::string selectionMethod;
  int numberParents;

  ///

  int oldPopulationSize;
  int culledPopulationSize;
  int nextPopulationTargetSize;
  int nextPopulationSize;

  int selfCount;
  int eliteCount;
  int surviveCount;

  double cullBelow;
  double cullRemap;
  bool cullByRange;

  double aveScore;
  double maxScore;
  double culledMaxScore;
  double minScore;
  double culledMinScore;

  std::vector<int> elites;
  std::vector<double> scores;
  std::vector<double> scoresAfterCull;

  ///
  class AbstractSelector {
  public:
    SimpleOptimizer *SO;
    AbstractSelector() = default;
    AbstractSelector(SimpleOptimizer *SO_) : SO(SO_){};
    virtual int select() = 0;
    virtual std::string getType() = 0;
  };

  class RouletteSelector : public AbstractSelector {
  public:
    RouletteSelector(std::string &argumentsString, SimpleOptimizer *SO_)
        : AbstractSelector(SO_) {
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
	  //std::cout << SO->culledMaxScore << std::endl;
      do {
        pick = Random::getIndex(SO->culledPopulationSize); // keep choosing a
                                                        // random genome from
                                                        // population until we
                                                        // get one that's good
                                                        // enough
		//std::cout << "  " << pick << " score() " << SO->scoresAfterCull[pick] << std::endl;
	  } while (Random::getDouble(1) > (SO->scoresAfterCull[pick] / SO->culledMaxScore));
      return pick;
    }

    virtual std::string getType() override { return (std::string) "Roulette"; }
  };

  class TournamentSelector : public AbstractSelector {
  public:
    int tournamentSize;
    TournamentSelector(std::string &argumentsString, SimpleOptimizer *SO_)
        : AbstractSelector(SO_) {
      // now decode arguments
      std::vector<std::string> arguments;
      std::stringstream ss(argumentsString); // Turn the string into a stream.
      std::string tok;

      while (getline(ss, tok, ',')) {
        arguments.push_back(tok);
      }
	  if (arguments.size() == 0) {
		  std::cout << "  in TournamentSelector constructor, did not find = in settings. Did you forget (size=n) ? "
			  << "\n  exiting..." << std::endl;
		  exit(1);
	  }
      for (auto arg : arguments) {
        std::vector<std::string> parts;
        std::stringstream ss(arg);
		std::string tok;
		if (ss.str().find('=') == std::string::npos) {
			std::cout << "  in TournamentSelector constructor, did not find = in settings. Did you forget (size=n) ? "
				<< "\n  exiting..." << std::endl;
			exit(1);
		}
        while (getline(ss, tok, '=')) {
          parts.push_back(tok);
        }
        if (parts[0] == "size") {
          if (!convertString(parts[1], tournamentSize)) {
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
      winner = Random::getIndex(SO->culledPopulationSize);
      for (int i = 0; i < tournamentSize - 1; i++) {
        challanger = Random::getIndex(SO->culledPopulationSize);
		//std::cout << tournamentSize << " " << i << "  " <<
         //challanger<<"("<<SO->scoresAfterCull[challanger] << "),winner(" <<
         //SO->scoresAfterCull[winner] << ")";
        if (SO->scoresAfterCull[challanger] > SO->scoresAfterCull[winner]) {
			//std::cout << " *";
          winner = challanger;
        }
         //std::cout << std::endl;
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

  SimpleOptimizer(std::shared_ptr<ParametersTable> PT_ = nullptr);

  virtual void
  optimize(std::vector<std::shared_ptr<Organism>> &population) override;

  // virtual string maxValueName() override {
  //	return (PT == nullptr) ? optimizeValuePL->lookup() :
  //PT->lookupString("OPTIMIZER_Simple-optimizeValue");
  //}
};

