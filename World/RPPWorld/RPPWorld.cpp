//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "RPPWorld.h"

//shared_ptr<ParameterLink<int>> RPPWorld::modePL = Parameters::register_parameter("WORLD_RPP-mode", 0, "0 = bit outputs before adding, 1 = add outputs");
//shared_ptr<ParameterLink<int>> RPPWorld::numberOfOutputsPL = Parameters::register_parameter("WORLD_RPP-numberOfOutputs", 10, "number of outputs in this world");
shared_ptr<ParameterLink<int>> RPPWorld::numMatchesPL = Parameters::register_parameter("WORLD_RPP-numMatches", 10, "Number of times to test each Genome per generation (useful with non-deterministic brains)");
shared_ptr<ParameterLink<int>> RPPWorld::numCompetitorsPL = Parameters::register_parameter("WORLD_RPP-numCompetitors", -1,
	"number of each organism will challenge (and also be challenged by).\nTotal matches played will be 2 times this number * population size.\nIf -1, then every organism will play every other organism once.");
shared_ptr<ParameterLink<string>> RPPWorld::groupNamePL = Parameters::register_parameter("WORLD_RPP_NAMES-groupName", (string)"root", "name of group to be evaluated");
shared_ptr<ParameterLink<string>> RPPWorld::brainANamePL = Parameters::register_parameter("WORLD_RPP_NAMES-brainAName", (string)"root", "name of brains used to control organisms when first player\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");
shared_ptr<ParameterLink<string>> RPPWorld::brainBNamePL = Parameters::register_parameter("WORLD_RPP_NAMES-brainBName", (string)"root", "name of brains used to control organisms when second player\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");
shared_ptr<ParameterLink<bool>> RPPWorld::randomBPL = Parameters::register_parameter("WORLD_RPP-randomB", false, "if true, brain B will be ignored, and random behavior will be used for second player");

RPPWorld::RPPWorld(shared_ptr<ParametersTable> _PT) :
	AbstractWorld(_PT) {
	//mode = (PT == nullptr) ? modePL->lookup() : PT->lookupInt("WORLD_RPP-mode");
	numCompetitors = (PT == nullptr) ? numCompetitorsPL->lookup() : PT->lookupInt("WORLD_RPP-numCompetitors");
	numMatches = (PT == nullptr) ? numMatchesPL->lookup() : PT->lookupInt("WORLD_RPP-numMatches");

	groupName = (PT == nullptr) ? groupNamePL->lookup() : PT->lookupString("WORLD_RPP_NAMES-groupName");
	brainAName = (PT == nullptr) ? brainANamePL->lookup() : PT->lookupString("WORLD_RPP_NAMES-brainAName");
	brainBName = (PT == nullptr) ? brainBNamePL->lookup() : PT->lookupString("WORLD_RPP_NAMES-brainBName");
	randomB = (PT == nullptr) ? randomBPL->lookup() : PT->lookupBool("WORLD_RPP-randomB");

	// columns to be added to ave file
	popFileColumns.clear();
	popFileColumns.push_back("score");
	popFileColumns.push_back("wins");
	popFileColumns.push_back("ties");
	popFileColumns.push_back("winRate");
	popFileColumns.push_back("tieRate");
	popFileColumns.push_back("Rock");
	popFileColumns.push_back("Paper");
	popFileColumns.push_back("Scissor");
	popFileColumns.push_back("badPlay");
}

void RPPWorld::evaluate(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
	shared_ptr<Group> group = groups[groupName];
	//if (group->population.size() < 2) {
	//	cout << "  IPDWorld must be run with WORLD::groupEvaluation = true (1) and must be passed a group who's population size > 1.\n  Please update your parameters and rerun." << endl;
	//	exit(1);
	//}
	vector<double> scores(group->population.size(), 0);
	// play games agaist other brains
	int n = numCompetitors;
	if (n == -1) { // play all other orgs
		for (int i = 0; i < (int)group->population.size(); i++) {
			for (int j = i + 1; j < (int)group->population.size(); j++) {
				evaluateMatch(group->population[i], group->population[j], analyse, visualize, debug);
				//				scores[i] += group->population[i]->score;
				//				scores[j] += group->population[j]->score;
			}
		}
	}
	else { // play only a set of orgs
		if (n >= (int)group->population.size()) {
			cout << "In IPDWorld, WORLD_RPP-numCompetitors is to large. must be > 0 and < population size.\nExiting." << endl;
			exit(1);
			//} else if (n == 0) {
			//	cout << "In IPDWorld, WORLD_RPP-numCompetitors is 0. must be > 0 and < population size.\nExiting." << endl;
			//	exit(1);
		}
		else {
			for (int i = 0; i < (int)group->population.size(); i++) {
				for (int j = i + 1; j <= i + n; j++) {
					int competitorIndex = j % (int)group->population.size();
					//cout << i << " plays " << competitorIndex << endl;
					evaluateMatch(group->population[i], group->population[competitorIndex], analyse, visualize, debug);
					//					scores[i] += group->population[i]->score;
					//					scores[competitorIndex] += group->population[competitorIndex]->score;
				}
			}
		}
	}
	//if (n == -1) {
	//	n = (int) group->population.size() - 1; // if n == -1 the each org plays every other org
	//} else {
	//	n = n * 2; // else each org plays the n orgs "before" and "after" them (in the population vector)
	//}
	//for (int i = 0; i < (int) group->population.size(); i++) {
	//	//group->population[i]->score = scores[i] / (double) n;
	//	//group->population[i]->dataMap.Append("allscore", group->population[i]->score);
	//}
}


// score is number of outputs set to 1 (i.e. output > 0) squared
void RPPWorld::evaluateMatch(shared_ptr<Organism> orgA, shared_ptr<Organism> orgB, int analyse, int visualize, int debug) {
	//cout << "in test world evalSolo" << endl;
	const int rock = 0;
	const int paper = 1;
	const int scissor = 2;
	vector<string> actions = { "rock","paper","scissor" };

	//cout << "AA" << endl;
	auto brainA = orgA->brains[brainAName];
	auto brainB = orgB->brains[brainBName];
	//cout << "BB" << endl;

	brainA->resetBrain();
	brainB->resetBrain();

	vector<int> resourceA; // [0]rocks/[1]papers/[2]scissors
	vector<int> resourceB;
	resourceA.resize(3);
	resourceB.resize(3);
	resourceA[rock] = 1;
	resourceA[paper] = 1;
	resourceA[scissor] = 1;
	resourceB[rock] = 1;
	resourceB[paper] = 1;
	resourceB[scissor] = 1;

	double scoreA = 0.0;
	double scoreB = 0.0;

	double rockCountA = 0.0;
	double rockCountB = 0.0;
	double paperCountA = 0.0;
	double paperCountB = 0.0;
	double scissorCountA = 0.0;
	double scissorCountB = 0.0;
	double badPlayCountA = 0.0;
	double badPlayCountB = 0.0;

	int actionA = -1; // -1=none,0=rock,1=paper,2=scissor
	int actionB = -1;

	//orgA->dataMap.Append("score", scoreA);
	//orgB->dataMap.Append("score", scoreB);
	int inputID;
	bool badA, badB;
	double winsA = 0;
	double winsB = 0;
	double tiesA = 0;
	double tiesB = 0;

	
	orgA->dataMap.Append("games", (string)"a:");
	if (!randomB) {
		orgB->dataMap.Append("games", (string)"b:");
	}
	//orgA->dataMap.Append("Rock", (double)(0));
	//orgA->dataMap.Append("Paper", (double)(0));
	//orgA->dataMap.Append("Scissor", (double)(0));
	//orgA->dataMap.Append("badPlay", (double)0);

	//orgB->dataMap.Append("Rock", (double)(0));
	//orgB->dataMap.Append("Paper", (double)(0));
	//orgB->dataMap.Append("Scissor", (double)(0));
	//orgB->dataMap.Append("badPlay", (double)0);

	for (int t = 0; t < numMatches; t++) {
		badA = false;
		badB = false;

		inputID = 0;

		// set up inputs from orgA
		brainA->setInput(inputID++, actionB == 0);  // last move from other was not rock
		brainA->setInput(inputID++, actionB == 1);  // last move from other was not paper
		brainA->setInput(inputID++, actionB == 2);  // last move from other was not scissor
		brainA->setInput(inputID++, resourceA[rock] > 0);  // one rock
		brainA->setInput(inputID++, resourceA[rock] > 1);  // two rock
		brainA->setInput(inputID++, resourceA[paper] > 0);  // one paper
		brainA->setInput(inputID++, resourceA[paper] > 1);  // two paper
		brainA->setInput(inputID++, resourceA[scissor] > 0);  // one scissor
		brainA->setInput(inputID++, resourceA[scissor] > 1);  // two scissor

		// set up inputs for orgB
		if (!randomB) {
			inputID = 0;
			brainB->setInput(inputID++, actionA == 0);  // last move from other was not rock
			brainB->setInput(inputID++, actionA == 1);  // last move from other was not paper
			brainB->setInput(inputID++, actionA == 2);  // last move from other was not scissor
			brainB->setInput(inputID++, resourceB[rock] > 0);  // one rock
			brainB->setInput(inputID++, resourceB[rock] > 1);  // two rock
			brainB->setInput(inputID++, resourceB[paper] > 0);  // one paper
			brainB->setInput(inputID++, resourceB[paper] > 1);  // two paper
			brainB->setInput(inputID++, resourceB[scissor] > 0);  // one scissor
			brainB->setInput(inputID++, resourceB[scissor] > 1);  // two scissor
			brainB->update();
		}
		
		brainA->update();

		actionA = 0;
		if (Bit(brainA->readOutput(rock)) + Bit(brainA->readOutput(paper)) + Bit(brainA->readOutput(scissor)) != 1) {
			//cout << "more then one thing or 0 things are said, A is bad." << endl;
			badA = true;
		}
		else {
			if (brainA->readOutput(rock) > 0) {
				//cout << "A says rock ... ";
				if (resourceA[rock] > 0) {
					//cout << "and has rock.";
					actionA = rock;
				}
				else {
					//cout << "but has no rock.";
					badA = true;
				}
			}
			else if (brainA->readOutput(paper) > 0) {
				//cout << "A says paper ... ";
				if (resourceA[paper] > 0) {
					//cout << "and had paper.";
					actionA = paper;
				}
				else {
					//cout << "but has no paper.";
					badA = true;
				}
			}
			else if (brainA->readOutput(scissor) > 0) {
				//cout << "A says scissor ... ";
				if (resourceA[scissor] > 0) {
					//cout << "and had scissor.";
					actionA = scissor;
				}
				else {
					//cout << "but has no scissor.";
					badA = true;
				}
			}
			//cout << endl;
		}
		if (badA) {
			actionA = Random::getIndex(3);
			while (resourceA[actionA] == 0) {
				actionA = Random::getIndex(3);
			}
			//cout << "    ... actually playing " << actions[actionA] << endl;
		}


		if (!randomB) {
			actionB = 0;
			if (Bit(brainB->readOutput(rock)) + Bit(brainB->readOutput(paper)) + Bit(brainB->readOutput(scissor)) != 1) {
				//cout << "more then one thing or 0 things are said, A is bad." << endl;
				badB = true;
			}
			else {
				if (brainB->readOutput(rock) > 0) {
					//cout << "B says rock ... ";
					if (resourceB[rock] > 0) {
						//cout << "and has rock.";
						actionB = rock;
					}
					else {
						//cout << "but has no rock.";
						badB = true;
					}
				}
				else if (brainB->readOutput(paper) > 0) {
					//cout << "B says paper ... ";
					if (resourceB[paper] > 0) {
						//cout << "and had paper.";
						actionB = paper;
					}
					else {
						//cout << "but has no paper.";
						badB = true;
					}
				}
				else if (brainB->readOutput(scissor) > 0) {
					//cout << "B says scissor ... ";
					if (resourceB[scissor] > 0) {
						//cout << "and had scissor.";
						actionB = scissor;
					}
					else {
						//cout << "but has no scissor.";
						badB = true;
					}
				}
				//cout << endl;
			}
			if (badB) {
				actionB = Random::getIndex(3);
				while (resourceB[actionB] == 0) {
					actionB = Random::getIndex(3);
				}
				//cout << "    ... actually playing " << actions[actionB] << endl;
			}
		}
		else { // random B
			badB = false;
			actionB = Random::getIndex(3);
			while (resourceB[actionB] == 0) {
				actionB = Random::getIndex(3);
			}
		}


		// if both player make ilegal moves, penalize both and move to next match.
		if (badA && badB) {
			//cout << "    Both bad!" << endl;
			scoreA -= .5;
			scoreB -= .5;
		}
		else {
			string aWin;
			string bWin;
			// both plays should be legal...
			if ((actionA == rock && actionB == scissor) || (actionA == paper && actionB == rock) || (actionA == scissor && actionB == paper)) { // a wins
				//cout << "  ->  A wins!" << endl;
				scoreA += (badA) ? -.5 : 1;
				scoreB += (badB) ? -.5 : 0;
				winsA += (double)(!(badA));
				aWin = '+';
				bWin = '-';
			}
			else if ((actionB == rock && actionA == scissor) || (actionB == paper && actionA == rock) || (actionB == scissor && actionA == paper)) { // a wins
				//cout << "  ->  B wins!" << endl;
				scoreA += (badA) ? -.5 : 0;
				scoreB += (badB) ? -.5 : 1;
				winsB += (double)(!(badB));
				aWin = '-';
				bWin = '+';
			}
			else {// tie!
				//cout << "  ->  tie!" << endl;
				scoreA += (badA) ? -.5 : 0.2;
				scoreB += (badB) ? -.5 : 0.2;
				tiesA += (double)(!(badB));
				tiesB += (double)(!(badB));
				aWin = '|';
				bWin = '|';
			}
			string a;
			if (actionA == rock) {
				a = 'r';
			}
			if (actionA == paper) {
				a = 'p';
			}
			if (actionA == scissor) {
				a = 's';
			}
			string b;
			if (actionB == rock) {
				b = 'r';
			}
			if (actionB == paper) {
				b = 'p';
			}
			if (actionB == scissor) {
				b = 's';
			}
			if (badA) {
				a = 'x' + a;
			}
			if (badB) {
				b = 'x' + b;
			}

			orgA->dataMap.Append("games", aWin + a + b + ":");

			if (!randomB) {
				orgB->dataMap.Append("games", bWin + b + a + ":");
			}

			if (badA) {
				badPlayCountA += 1;
			}
			else {
				rockCountA += (double)(actionA == rock);
				paperCountA += (double)(actionA == paper);
				scissorCountA += (double)(actionA == scissor);
			}
			if (badB) {
				badPlayCountB += 1;
			}
			else {
				rockCountB += (double)(actionB == rock);
				paperCountB += (double)(actionB == paper);
				scissorCountB += (double)(actionB == scissor);
			}

			resourceA[actionA]--;
			resourceA[actionB]++;
			resourceB[actionB]--;
			resourceB[actionA]++;
		}
	}

	orgA->dataMap.Append("Rock", rockCountA / (double)numMatches);
	orgA->dataMap.Append("Paper", paperCountA / (double)numMatches);
	orgA->dataMap.Append("Scissor", scissorCountA / (double)numMatches);
	orgA->dataMap.Append("badPlay", badPlayCountA / (double)numMatches);


	//orgA->dataMap.Append("scoreSum", scoreA);

	orgA->dataMap.Append("score", scoreA / (double)numMatches);
	//orgA->dataMap.setOutputBehavior("score", DataMap::SUM | DataMap::AVE);

	orgA->dataMap.Append("wins", (double)winsA);
	orgA->dataMap.Append("ties", (double)tiesA);

	orgA->dataMap.Append("winRate", (double)winsA / (double)(numMatches - badPlayCountA + 1));
	orgA->dataMap.Append("tieRate", (double)tiesA / (double)(numMatches - badPlayCountA + 1));

	if (!randomB) {
		orgB->dataMap.Append("Rock", rockCountB / (double)numMatches);
		orgB->dataMap.Append("Paper", paperCountB / (double)numMatches);
		orgB->dataMap.Append("Scissor", scissorCountB / (double)numMatches);
		orgB->dataMap.Append("badPlay", badPlayCountB / (double)numMatches);
	
		//orgB->dataMap.Append("scoreSum", scoreB);

		orgB->dataMap.Append("score", scoreB / (double)numMatches);
		//orgB->dataMap.setOutputBehavior("score", DataMap::SUM | DataMap::AVE);

		orgB->dataMap.Append("wins", (double)winsB);
		orgB->dataMap.Append("ties", (double)tiesB);

		orgB->dataMap.Append("winRate", (double)winsB / (double)(numMatches - badPlayCountB + 1));
		orgB->dataMap.Append("tieRate", (double)tiesB / (double)(numMatches - badPlayCountB + 1));
	}
}

