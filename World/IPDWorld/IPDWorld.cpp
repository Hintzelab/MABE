//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "IPDWorld.h"
#include "../../Brain/IPDBrain/IPDBrain.h"

shared_ptr<ParameterLink<bool>> IPDWorld::roundsFixedPerGenerationPL = Parameters::register_parameter("WORLD_IPD-roundsFixedPerGeneration", true,
		"if true, number of rounds played will be randomized once per generation, else, each game will be or random duration");
shared_ptr<ParameterLink<int>> IPDWorld::roundsMinPL = Parameters::register_parameter("WORLD_IPD-roundsMin", 100, "minimum number of rounds played");
shared_ptr<ParameterLink<int>> IPDWorld::roundsMaxPL = Parameters::register_parameter("WORLD_IPD-roundsMax", 200, "maximum number of rounds played");
shared_ptr<ParameterLink<double>> IPDWorld::R_payOffPL = Parameters::register_parameter("WORLD_IPD-R_payOff", 3.0, "Reward Payoff - received when both organisms cooperate");
shared_ptr<ParameterLink<double>> IPDWorld::S_payOffPL = Parameters::register_parameter("WORLD_IPD-S_payOff", 0.0, "Sucker Payoff - received when an organism cooperates, and their opponent defects");
shared_ptr<ParameterLink<double>> IPDWorld::T_payOffPL = Parameters::register_parameter("WORLD_IPD-T_payOff", 5.0, "Temptation Payoff - received when an organism defects, and their opponent cooperates");
shared_ptr<ParameterLink<double>> IPDWorld::P_payOffPL = Parameters::register_parameter("WORLD_IPD-P_payOff", 1.0, "Punishment Payoff - received when both organisms defect");

shared_ptr<ParameterLink<int>> IPDWorld::numCompetitorsPL = Parameters::register_parameter("WORLD_IPD-numCompetitors", -1,
		"number of each organism will challenge (and also be challenged by).\nTotal matches played will be 2 times this number * population size.\nIf -1, then every organism will play every other organism once.");

shared_ptr<ParameterLink<bool>> IPDWorld::CPL = Parameters::register_parameter("WORLD_IPD-C", true, "How will Cooperate be represented internally (Defect will = 1 - Cooperate)");

shared_ptr<ParameterLink<bool>> IPDWorld::skipFirstMovePL = Parameters::register_parameter("WORLD_IPD-skipFirstMove", true, "if true, the first move will not be scored (note: and extra round will be played)");
shared_ptr<ParameterLink<bool>> IPDWorld::randomFirstMovePL = Parameters::register_parameter("WORLD_IPD-randomFirstMove", false, "if true, the first move will not be scored");

shared_ptr<ParameterLink<bool>> IPDWorld::saveMovesListPL = Parameters::register_parameter("WORLD_IPD-saveMovesList", false, "if true, save list of moves made by each agent to dataMap");

shared_ptr<ParameterLink<string>> IPDWorld::fixedStrategiesPL = Parameters::register_parameter("WORLD_IPD-fixedStrategies", (string)"[AllD,AllC,TFT,2TFT,Rand,SIMP]", "list of strategies which this brain can use");
shared_ptr<ParameterLink<int>> IPDWorld::playsVsFixedStrategiesPL = Parameters::register_parameter("WORLD_IPD-playsVsFixedStrategies", 1, "number of matches each organism will play against each fixed strategy");

shared_ptr<ParameterLink<string>> IPDWorld::groupNamePL = Parameters::register_parameter("WORLD_IPD_NAMES-groupName", (string)"root", "name of group to be evaluated\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");
shared_ptr<ParameterLink<string>> IPDWorld::brainNamePL = Parameters::register_parameter("WORLD_IPD_NAMES-brainName", (string)"root", "name of brains used to control organisms\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");

IPDWorld::IPDWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {

	roundsFixedPerGeneration = (PT == nullptr) ? roundsFixedPerGenerationPL->lookup() : PT->lookupBool("WORLD_IPD-roundsFixedPerGeneration");

	roundsMax = (PT == nullptr) ? roundsMaxPL->lookup() : PT->lookupInt("WORLD_IPD-roundsMax");
	roundsMin = (PT == nullptr) ? roundsMinPL->lookup() : PT->lookupInt("WORLD_IPD-roundsMin");

	R_payOff = (PT == nullptr) ? R_payOffPL->lookup() : PT->lookupDouble("WORLD_IPD-R_payOff");
	S_payOff = (PT == nullptr) ? S_payOffPL->lookup() : PT->lookupDouble("WORLD_IPD-S_payOff");
	T_payOff = (PT == nullptr) ? T_payOffPL->lookup() : PT->lookupDouble("WORLD_IPD-T_payOff");
	P_payOff = (PT == nullptr) ? P_payOffPL->lookup() : PT->lookupDouble("WORLD_IPD-P_payOff");

	numCompetitors = (PT == nullptr) ? numCompetitorsPL->lookup() : PT->lookupInt("WORLD_IPD-numCompetitors");

	C = (PT == nullptr) ? CPL->lookup() : PT->lookupBool("WORLD_IPD-C");
	D = !C;

	skipFirstMove = (PT == nullptr) ? skipFirstMovePL->lookup() : PT->lookupBool("WORLD_IPD-skipFirstMove");
	randomFirstMove = (PT == nullptr) ? randomFirstMovePL->lookup() : PT->lookupBool("WORLD_IPD-randomFirstMove");

	saveMovesList = (PT == nullptr) ? saveMovesListPL->lookup() : PT->lookupBool("WORLD_IPD-saveMovesList");

	inputNodesCount = 2;  // this is first move, other players last move
	outputNodesCount = 1;  // my move

	numRounds = 0;
	currentUpdate = -1;

	// set up fixed strat organisms

	playsVsFixedStrategies = (PT == nullptr) ? playsVsFixedStrategiesPL->lookup() : PT->lookupInt("WORLD_IPD-playsVsFixedStrategies");

	shared_ptr<AbstractBrain> templateBrain = IPDBrain_brainFactory(inputNodesCount, outputNodesCount, PT);
	shared_ptr<IPDBrain> castBrain = dynamic_pointer_cast<IPDBrain>(templateBrain);
	
	convertCSVListToVector((PT == nullptr) ? fixedStrategiesPL->lookup() : PT->lookupString("WORLD_IPD-fixedStrategies"), fixedStrategies);

	groupName = (PT == nullptr) ? groupNamePL->lookup() : PT->lookupString("WORLD_IPD_NAMES-groupName");
	brainName = (PT == nullptr) ? brainNamePL->lookup() : PT->lookupString("WORLD_IPD_NAMES-brainName");
	
	for (auto s : fixedStrategies) {
		bool foundStrat = false;
		for (int i = 0; i < (int)castBrain->availableStrategies.size(); i++) {
			//cout << castBrain->availableStrategies[i] << endl;
			if (s == castBrain->availableStrategies[i]) {
				unordered_map<string, shared_ptr<AbstractGenome>> tempGenomes;
				unordered_map<string, shared_ptr<AbstractBrain>> tempBrains;
				tempBrains[brainName] = castBrain->makeBrainFromValues({ (double)i });
			
				fixedOrgs.push_back(make_shared<Organism>(tempGenomes, tempBrains, PT));
				foundStrat = true;
			}
		}
		if (!foundStrat) {
			cout << "While initalizing fixed strategies in IPD World, can not find stratagy '" << s << "'. Exiting." << endl;
			exit(1);
		}
	}


	// done with setup


	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("score");
	aveFileColumns.push_back("CC");
	aveFileColumns.push_back("DD");
	aveFileColumns.push_back("CD");
	aveFileColumns.push_back("DC");
	if (playsVsFixedStrategies > 0) {
		for (auto s : fixedStrategies) {
			aveFileColumns.push_back("score_vs_" + s);
		}
	}
}


pair<double,double> IPDWorld::runDuel(shared_ptr<Organism> p1, shared_ptr<Organism> p2, bool analyse, bool visualize, bool debug) {
	auto brain1 = p1->brains[brainName];
	auto brain2 = p2->brains[brainName];

	int p1Move = (randomFirstMove) ? Random::getInt(0, 1) : C;
	int p2Move = (randomFirstMove) ? Random::getInt(0, 1) : C;

	double P1score = 0;
	double P2score = 0;

	int P1CC = 0;
	int P2CC = 0;
	int P1CD = 0;
	int P2CD = 0;
	int P1DC = 0;
	int P2DC = 0;
	int P1DD = 0;
	int P2DD = 0;
	if (saveMovesList) {
		p1->dataMap.Append("moves", (string) "X"); // add X to indicate begining of new game
		p2->dataMap.Append("moves", (string) "X");
	}
	brain1->resetBrain();
	brain2->resetBrain();

	if (roundsFixedPerGeneration) {
		if (currentUpdate != Global::update) {
			numRounds = Random::getInt(roundsMax - roundsMin) + roundsMin;
			currentUpdate = Global::update;
		}
	} else {
		numRounds = Random::getInt(roundsMax - roundsMin) + roundsMin;
	}

	for (int t = 0; t < numRounds + (int) skipFirstMove; t++) {
		if (t == 0) {
			brain1->setInput(0, 1);
			brain2->setInput(0, 1);
		} else {
			brain1->setInput(0, 0);
			brain2->setInput(0, 0);
		}
		brain1->setInput(1, p2Move);
		brain2->setInput(1, p1Move);

		brain1->update();
		brain2->update();

		p1Move = Bit(brain1->readOutput(0));
		p2Move = Bit(brain2->readOutput(0));

		if (saveMovesList) {
			string p1M = (p1Move == C) ? "C" : "D";
			string p2M = (p2Move == C) ? "C" : "D";
			p1->dataMap.Append("moves", p1M + p2M);
			p2->dataMap.Append("moves", p2M + p1M);
		}
		if ((skipFirstMove && t > 0) || !skipFirstMove) { // don't count the first move
			if (p1Move == C && p2Move == C) {
				P1score += R_payOff;
				P2score += R_payOff;
				P1CC++;
				P2CC++;
			}
			if (p1Move == C && p2Move == D) {
				P1score += S_payOff;
				P2score += T_payOff;
				P1CD++;
				P2DC++;
			}
			if (p1Move == D && p2Move == C) {
				P1score += T_payOff;
				P2score += S_payOff;
				P1DC++;
				P2CD++;
			}
			if (p1Move == D && p2Move == D) {
				P1score += P_payOff;
				P2score += P_payOff;
				P1DD++;
				P2DD++;
			}
		}
	}

	//p1->score = P1score / (double) numRounds;
	//p2->score = P2score / (double) numRounds;

	p1->dataMap.Append("score", P1score / (double) numRounds);
	p2->dataMap.Append("score", P2score / (double) numRounds);

	p1->dataMap.Append("CC", (double) P1CC / (double) numRounds);
	p2->dataMap.Append("CC", (double) P2CC / (double) numRounds);

	p1->dataMap.Append("CD", (double) P1CD / (double) numRounds);
	p2->dataMap.Append("CD", (double) P2CD / (double) numRounds);

	p1->dataMap.Append("DC", (double) P1DC / (double) numRounds);
	p2->dataMap.Append("DC", (double) P2DC / (double) numRounds);

	p1->dataMap.Append("DD", (double) P1DD / (double) numRounds);
	p2->dataMap.Append("DD", (double) P2DD / (double) numRounds);

	pair<double, double> scores = { P1score / (double)numRounds, P2score / (double)numRounds };
	return (scores);
}


void IPDWorld::evaluate(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
	shared_ptr<Group> group = groups[groupName];
	//if (group->population.size() < 2) {
	//	cout << "  IPDWorld must be run with WORLD::groupEvaluation = true (1) and must be passed a group who's population size > 1.\n  Please update your parameters and rerun." << endl;
	//	exit(1);
	//}
	vector<double> scores(group->population.size(), 0);


	// first, play games against fixed IPDBrains;

	for (int i = 0; i < (int)group->population.size(); i++) {
		for (int fixedOrgsCount = 0; fixedOrgsCount < (int)fixedOrgs.size(); fixedOrgsCount++) {
			//cout << "fixedOrgsCount:" << fixedOrgsCount << "  fixedOrgs.size()" << fixedOrgs.size() << endl;
			for (int j = 0; j < playsVsFixedStrategies; j++) {
				//cout << "score_vs_" + fixedStrategies[fixedOrgsCount] << endl;
				//cout << "j:" << j << "  playsVsFixedStrategies:" << playsVsFixedStrategies << endl;
				pair<double,double> scores = runDuel(group->population[i], fixedOrgs[fixedOrgsCount], analyse, visualize, debug);
				//cout << scores.first << "  " << scores.second << endl;
				group->population[i]->dataMap.Append("score_vs_" + fixedStrategies[fixedOrgsCount], scores.first);
				
				////runDuel(group->population[i], fixedOrgs[fixedOrgsCount], analyse, visualize, debug);
			}
			
		}
		
	}


	// next, play games agaist other brains
	int n = numCompetitors;
	if (n == -1) {
		for (int i = 0; i < (int) group->population.size(); i++) {
			for (int j = i + 1; j < (int) group->population.size(); j++) {
				runDuel(group->population[i], group->population[j], analyse, visualize, debug);
//				scores[i] += group->population[i]->score;
//				scores[j] += group->population[j]->score;
			}
		}
	} else {
		if (n >= (int) group->population.size()) {
			cout << "In IPDWorld, WORLD_IPD-numCompetitors is to large. must be > 0 and < population size.\nExiting." << endl;
			exit(1);
		//} else if (n == 0) {
		//	cout << "In IPDWorld, WORLD_IPD-numCompetitors is 0. must be > 0 and < population size.\nExiting." << endl;
		//	exit(1);
		} else {
			for (int i = 0; i < (int) group->population.size(); i++) {
				for (int j = i + 1; j <= i + n; j++) {
					int competitorIndex = j % (int) group->population.size();
					//cout << i << " plays " << competitorIndex << endl;
					IPDWorld::runDuel(group->population[i], group->population[competitorIndex], analyse, visualize, debug);
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
