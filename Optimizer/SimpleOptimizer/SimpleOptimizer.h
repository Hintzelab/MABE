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

	static shared_ptr<ParameterLink<string>> selectionMethodPL; // Roullette([Exp=1.05 or Lin=1]),Tounament(size=5)
	static shared_ptr<ParameterLink<int>> numberParentsPL; // number of parents (default 1, asexual)

	static shared_ptr<ParameterLink<string>> optimizeValuePL; // what value is used to generate
	static shared_ptr<ParameterLink<string>> surviveRatePL; // value between 0 and 1 chance to survive
	static shared_ptr<ParameterLink<string>> selfRatePL; // value between 0 and 1 chance to self (if more then one parent)
	static shared_ptr<ParameterLink<string>> elitismCountPL; // this number of organisms will reproduce asexualy (i.e. copy w/ mutation)
	static shared_ptr<ParameterLink<string>> elitismRangePL; // best n organisms will each produce

	static shared_ptr<ParameterLink<string>> nextPopSizePL;  // number of genomes in the population

	string selectionMethod;
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

	vector<int> elites;
	vector<double> scores;

	///
	class AbstractSelector {
	public:
		SimpleOptimizer *SO;
		AbstractSelector() = default;
		AbstractSelector(SimpleOptimizer *_SO) : SO(_SO){};
		virtual int select() = 0;
		virtual string getType() = 0;
	};

	class RouletteSelector : public AbstractSelector {
	public:
		RouletteSelector(string& argumentsString, SimpleOptimizer *_SO) : AbstractSelector(_SO) {
			// now decode arguments
			vector<string> arguments;
			stringstream ss(argumentsString); // Turn the string into a stream.
			string tok;

			while (getline(ss, tok, ',')) {
				arguments.push_back(tok);
			}
			if (arguments.size() > 0) {
				cout << "  in RouletteSelector constructor, found arguments \"" << argumentsString << "\" but Roulette takes no arguments.\n  exiting..." << endl;
				exit(1);
			}
		}

		virtual int select() override {
			int pick;
			do {
				pick = Random::getIndex(SO->oldPopulationSize);  //keep choosing a random genome from population until we get one that's good enough
			} while (Random::getDouble(1) > (SO->scores[pick] / SO->maxScore));
			return pick;
		}

		virtual string getType() override {
			return (string)"Roulette";
		}
	};


	class TournamentSelector : public AbstractSelector {
	public:
		int tournamentSize;
		TournamentSelector(string& argumentsString, SimpleOptimizer *_SO) : AbstractSelector(_SO) {
			// now decode arguments
			vector<string> arguments;
			stringstream ss(argumentsString); // Turn the string into a stream.
			string tok;

			while (getline(ss, tok, ',')) {
				arguments.push_back(tok);
			}
			for (auto arg : arguments) {
				vector<string> parts;
				stringstream ss(arg);
				string tok;
				while (getline(ss, tok, '=')) {
					parts.push_back(tok);
				}
				if (parts[0] == "size") {
					if (!load_value(parts[1], tournamentSize)) {
						cout << "  in TournamentSelector constructor, got bad value for size \"" << parts[1] << "\"\n  exiting..." << endl;
						exit(1);
					}
				}
				else {
					cout << "  in TournamentSelector constructor, found unknown argument pair \"" << parts[0] << "=" << parts[1] << "\"\n  exiting..." << endl;
					exit(1);
				}
			}
		}

		virtual int select() override {
			int winner, challanger;
			winner = Random::getIndex(SO->oldPopulationSize);
			for (int i = 0; i < tournamentSize - 1; i++) {
				challanger = Random::getIndex(SO->oldPopulationSize);
				//cout << tournamentSize << " " <<i << "  " << challanger<<"("<<SO.scores[challanger] << "),winner(" << SO.scores[winner] << ")";
				if (SO->scores[challanger] > SO->scores[winner]) {
					//cout << " *";
					winner = challanger;
				}
				//cout << endl;
			}
			return winner;
		}

		virtual string getType() override {
			return (string)"Tournament";
		}
	};

	shared_ptr<AbstractSelector> selector;

	shared_ptr<Abstract_MTree> optimizeValueMT, surviveRateMT, selfRateMT, elitismCountMT, elitismRangeMT, nextPopSizeMT;

	SimpleOptimizer(shared_ptr<ParametersTable> _PT = nullptr);
	
	virtual void optimize(vector<shared_ptr<Organism>> &population) override;

	//virtual string maxValueName() override {
	//	return (PT == nullptr) ? optimizeValuePL->lookup() : PT->lookupString("OPTIMIZER_Simple-optimizeValue");
	//}
};

