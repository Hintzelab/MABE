//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

// Evaluates agents on how many '1's they can output. This is a purely fixed
// task
// that requires to reactivity to stimuli.
// Each correct '1' confers 1.0 point to score, or the decimal output determined
// by 'mode'.

#include "NBackWorld.h"


std::shared_ptr<ParameterLink<int>> NBackWorld::testMutantsPL = Parameters::register_parameter("WORLD_NBACK-testMutants", 0, "if > 0, this number of mutants of each agent will be tested");

std::shared_ptr<ParameterLink<int>> NBackWorld::evaluationsPerGenerationPL =
Parameters::register_parameter("WORLD_NBACK-evaluationsPerGeneration", 10, "Number of times to evaluate each agent per generation");

std::shared_ptr<ParameterLink<int>> NBackWorld::testsPerEvaluationPL =
Parameters::register_parameter("WORLD_NBACK-testsPerEvaluation", 10, "Number of times to test each agent per evaluation");

std::shared_ptr<ParameterLink<std::string>> NBackWorld::NsListsPL =
Parameters::register_parameter("WORLD_NBACK-NsList", (std::string)"1,2,3:100|2,3,4:-1", "comma seperated list of n values followed by ':' and a time\n"
	"more then one list can be defined seperated by '|'. The last list time must be -1 (i.e. forever)\n"
	"eg: 1,2,3:100|2,3,4:-1");

std::shared_ptr<ParameterLink<int>> NBackWorld::scoreMultPL =
Parameters::register_parameter("WORLD_NBACK-scoreMult", 1, "score multiplier");

std::shared_ptr<ParameterLink<int>> NBackWorld::RMultPL =
Parameters::register_parameter("WORLD_NBACK-RMult", 1, "score R multiplier");

std::shared_ptr<ParameterLink<int>> NBackWorld::delayOutputEvalPL =
Parameters::register_parameter("WORLD_NBACK-delayOutputEval", 0, "generation delay for ouput evalutation");

std::shared_ptr<ParameterLink<bool>> NBackWorld::tritInputsPL =
Parameters::register_parameter("WORLD_NBACK-tritInputs", false, "if false (defaut) then inputs to brain are 0 or 1. If true, inputs are -1,0,1");

std::shared_ptr<ParameterLink<std::string>> NBackWorld::groupNamePL =
Parameters::register_parameter("WORLD_NBACK-groupNameSpace", (std::string) "root::", "namespace of group to be evaluated");
std::shared_ptr<ParameterLink<std::string>> NBackWorld::brainNamePL =
Parameters::register_parameter("WORLD_NBACK-brainNameSpace", (std::string) "root::", "namespace for parameters used to define brain");

std::shared_ptr<ParameterLink<bool>> NBackWorld::saveFragOverTimePL =
Parameters::register_parameter("WORLD_NBACK_ANALYZE-saveFragOverTime", false,
	"");
std::shared_ptr<ParameterLink<bool>> NBackWorld::saveBrainStructureAndConnectomePL =
Parameters::register_parameter("WORLD_NBACK_ANALYZE-saveBrainStructureAndConnectome", true,
	"");
std::shared_ptr<ParameterLink<bool>> NBackWorld::saveStateToStatePL =
Parameters::register_parameter("WORLD_NBACK_ANALYZE-saveStateToState", true,
	"");
std::shared_ptr<ParameterLink<bool>> NBackWorld::save_R_FragMatrixPL =
Parameters::register_parameter("WORLD_NBACK_ANALYZE-save_R_FragMatrix", false,
	"");
std::shared_ptr<ParameterLink<bool>> NBackWorld::saveFlowMatrixPL =
Parameters::register_parameter("WORLD_NBACK_ANALYZE-saveFlowMatrix", false,
	"");
std::shared_ptr<ParameterLink<bool>> NBackWorld::saveStatesPL =
Parameters::register_parameter("WORLD_NBACK_ANALYZE-saveStates", true,
	"");


#include "../../Utilities/PowerSet.h"

NBackWorld::NBackWorld(std::shared_ptr<ParametersTable> PT_) : AbstractWorld(PT_) {

	saveFragOverTime = saveFragOverTimePL->get(PT);
	saveBrainStructureAndConnectome = saveBrainStructureAndConnectomePL->get(PT);
	saveStateToState = saveStateToStatePL->get(PT);
	save_R_FragMatrix = save_R_FragMatrixPL->get(PT);
	saveFlowMatrix = saveFlowMatrixPL->get(PT);
	saveStates = saveStatesPL->get(PT);

	std::vector<std::string> NListsBreakDown1; // used to parse nLists
	std::vector<std::string> NListsBreakDown2; // used to parse nLists

	convertCSVListToVector(NsListsPL->get(PT), NListsBreakDown1, '|'); // lists (i.e. Ns to score + time) are sperated by '|'

	int temp = 0;

	for (auto elem : NListsBreakDown1) {
		convertCSVListToVector(elem, NListsBreakDown2, ':'); // list of Ns is sperated from time with a ':'
		convertString(NListsBreakDown2[1], temp); // get the time for this list
		if (NListSwitchTimes.size() == 0) { // if this is the first list, then put this time on NListSwitchTimes
			NListSwitchTimes.push_back(temp);
		}
		else if (temp > 0) { // else, if it's not -1 (i.e. last list), put this time + sum of previous times
			NListSwitchTimes.push_back(NListSwitchTimes.back() + temp);
		}
		else { // else it's -1, this is the last list (note, if the first list has time -1, that is handled by the if)
			NListSwitchTimes.push_back(-1);
		}
		NListLists.push_back({}); // add a blank list so we have a container to fill
		convertCSVListToVector(NListsBreakDown2[0], NListLists.back(), ','); // fill the container we just added to NListLists
	}
	
	int nextOut = 0;
	
	std::cout << "testing Lists will change on updates:";
	for (auto elem : NListSwitchTimes) {
		std::cout << "  " << elem;
	}
	std::cout << std::endl;

	std::cout << "testing Lists:\n";
	for (auto elem : NListLists) {
		for (auto elem2 : elem) {
			std::cout << "  " << elem2;
			largestN = std::max(largestN, elem2);
			if (!N2OutMap.count(elem2)) {
				N2OutMap[elem2] = nextOut++;
			}
		}
		std::cout << std::endl;
	}

	std::cout << "  largest N found was : " << largestN << ". Brains will be run for this number of world steps before testing begins." << std::endl;

	// now get currentLargestN

	for (auto elem : NListLists[0]) {
		currentLargestN = std::max(currentLargestN, elem);
	}

	evaluationsPerGeneration = evaluationsPerGenerationPL->get(PT); // each agent sees this number of inputs (+largest N) and is scored this number of times each evaluation
	testsPerEvaluation = testsPerEvaluationPL->get(PT); // each agent is reset and evaluated this number of times

	std::cout << "output map:\n";
	for (auto elem : N2OutMap) {
		std::cout << "  N: " << elem.first << " <- output: " << elem.second << std::endl;
	}
	std::cout << "brains will have 1 input and " << N2OutMap.size() << " outputs." << std::endl;

	groupName = groupNamePL->get(PT);
	brainName = brainNamePL->get(PT);



	std::vector<std::string> temp_analyzeWhatStr;


	testMutants = testMutantsPL->get(PT);

	tritInputs = tritInputsPL->get(PT);

	// columns to be added to ave file
	popFileColumns.clear();
	popFileColumns.push_back("score");
	//////popFileColumns.push_back("R");
	for (auto elem : N2OutMap) {
		popFileColumns.push_back("nBack_" + std::to_string(elem.first));
		std::cout << "adding: " << "nBack_" + std::to_string(elem.first) << std::endl;
	}
}

void NBackWorld::evaluate(std::map<std::string, std::shared_ptr<Group>> &groups, int analyze, int visualize, int debug) {
	// check to see if we need to advance to next NsList
	if (Global::update >= NListSwitchTimes[currentNList] && NListSwitchTimes[currentNList] != -1) {
		currentNList += 1;
		//std::cout << "advancing to next list... " << currentNList << std::endl;
		currentLargestN = 0;
		for (auto elem : NListLists[currentNList]) {
			currentLargestN = std::max(currentLargestN, elem);
		}
	}

	int popSize = groups[groupNamePL->get(PT)]->population.size();
	for (int i = 0; i < popSize; i++) {
		// eval this agent
		evaluateSolo(groups[groupNamePL->get(PT)]->population[i], analyze, visualize, debug);
		// now lets test some god damn dirty mutants!

		if (testMutants > 0) {
			std::vector<double> mutantScores;
			double mutantScoreSum = 0;
			for (int j = 0; j < testMutants; j++) {
				auto mutantOffspring = groups[groupNamePL->get(PT)]->population[i]->makeMutatedOffspringFrom(groups[groupNamePL->get(PT)]->population[i]);
				evaluateSolo(mutantOffspring, 0, 0, 0);
				auto s = mutantOffspring->dataMap.getAverage("score");
				mutantScores.push_back(s);
				mutantScoreSum += s;
			}
			std::cout << "score: " << groups[groupNamePL->get(PT)]->population[i]->dataMap.getAverage("score") << "  mutantAveScore(" << testMutants << "): " << mutantScoreSum / testMutants << std::endl;
			//std::ofstream mutantScoreFile;
			//mutantScoreFile.open("mutantScoreFile.csv",
			//	std::ios::out |
			//	std::ios::app);
			//mutantScoreFile << groups[groupNamePL->get(PT)]->population[i]->dataMap.getAverage("score") << "," << mutantScoreSum / testMutants << std::endl;
			//mutantScoreFile.close();
			FileManager::writeToFile("mutantScoreFile.txt", std::to_string(groups[groupNamePL->get(PT)]->population[i]->dataMap.getAverage("score")) + "," + std::to_string(mutantScoreSum / testMutants));


		}

	}
	
	
	//if (analyze) {
	//	groups[groupNamePL->get(PT)]->archive();
	//}
}

void NBackWorld::evaluateSolo(std::shared_ptr<Organism> org, int analyze, int visualize, int debug) {
		
	auto brain = org->brains[brainName];
	brain->setRecordActivity(true);

	double score = 0.0;
	std::vector<int> tallies(N2OutMap.size(), 0); // how many times did brain get each N in current list correct?
	std::vector<int> inputList(testsPerEvaluation + currentLargestN, 0);

	TS::intTimeSeries worldStates;

	for (int r = 0; r < evaluationsPerGeneration; r++) {
		brain->resetBrain();

		int t = 0;


		for (int t = 0; t < inputList.size(); t++) {

			int lowBound = 0;
			if (tritInputs) {
				lowBound = -1;
			}
			inputList[t] = Random::getInt(lowBound,1);
			brain->setInput(0, inputList[t]);

			brain->update();

			// collect score and world data but only once we have reached currentLargestN
			if (t >= currentLargestN) {
				// add space in world data vector
				worldStates.push_back({});
				for (auto elem : NListLists[currentNList]) {
					worldStates.back().push_back(inputList[t - elem + 1]);
					if (Global::update >= delayOutputEvalPL->get(PT) && // if update is greater than delay time
						Trit(brain->readOutput(N2OutMap[elem])) == inputList[t - elem]) { // if output is correct 
						score += 1; // add 1 to score
						tallies[N2OutMap[elem]] += 1; // add 1 to correct outputs for this N
					}
				}
			}
		}
	}
	
	org->dataMap.append("score", (score*scoreMultPL->get(PT)) / (evaluationsPerGeneration*testsPerEvaluation*NListLists[currentNList].size()));
	// score is divided by number of evals * number of tests * number of N's in current list

	for (auto elem : N2OutMap) {
		org->dataMap.append("nBack_" + std::to_string(elem.first), (double)tallies[elem.second] / (double)(evaluationsPerGeneration*testsPerEvaluation));
		// since this is for one N at a time, it's just divided by number of evals * number of tests
	}

	if (visualize) {
		std::cout << "organism with ID " << org->ID << " scored " << org->dataMap.getAverage("score") << std::endl;
	}
	
	
	
	
	
	
	auto lifeTimes = brain->getLifeTimes();
	auto inputStates = TS::remapToIntTimeSeries(brain->getInputStates(), TS::RemapRules::TRIT);
	auto outputStates = TS::remapToIntTimeSeries(brain->getOutputStates(), TS::RemapRules::TRIT);
	auto brainStates = TS::remapToIntTimeSeries(brain->getHiddenStates(), TS::RemapRules::TRIT);

	TS::intTimeSeries shortInputStates = TS::trimTimeSeries(inputStates,TS::Position::FIRST,lifeTimes, currentLargestN);

	TS::intTimeSeries shortOutputStatesBefore;// only needed if recurrent
	TS::intTimeSeries shortOutputStatesAfter; // always needed
	if (brain->recurrentOutput) {
		shortOutputStatesBefore = TS::trimTimeSeries(outputStates, TS::Position::FIRST, lifeTimes, currentLargestN); // trim off first currentLargestN for ramp up time
		auto tempStorterLifeTimes = TS::updateLifeTimes(lifeTimes, -1 * currentLargestN); // remove currentLargestN from each lifetime
		shortOutputStatesBefore = TS::trimTimeSeries(shortOutputStatesBefore, TS::Position::LAST, tempStorterLifeTimes, 1); // remove 1 from end
		shortOutputStatesAfter = TS::trimTimeSeries(outputStates, TS::Position::FIRST, lifeTimes, currentLargestN + 1); // remove currentLargestN+1 from front for ramp up + 1
	}
	else {
		shortOutputStatesAfter = TS::trimTimeSeries(outputStates, TS::Position::FIRST, lifeTimes, currentLargestN);
	}

	// always recurrent
	TS::intTimeSeries shortBrainStatesBefore = TS::trimTimeSeries(brainStates, TS::Position::FIRST, lifeTimes, currentLargestN);
	auto tempStorterLifeTimes = TS::updateLifeTimes(lifeTimes, -1 * currentLargestN);
	shortBrainStatesBefore = TS::trimTimeSeries(shortBrainStatesBefore, TS::Position::LAST, tempStorterLifeTimes, 1);
	TS::intTimeSeries shortBrainStatesAfter = TS::trimTimeSeries(brainStates, TS::Position::FIRST, lifeTimes, currentLargestN+1);

	std::vector<int> shortLifeTimes = TS::updateLifeTimes(lifeTimes, -1 * currentLargestN);

	double R = ENT::ConditionalMutualEntropy(worldStates,shortBrainStatesAfter,shortInputStates);
	org->dataMap.append("R", R * RMultPL->get(PT));

	double rawR = ENT::MutualEntropy(worldStates, shortBrainStatesAfter);
	org->dataMap.append("rawR", rawR);

	



	double earlyRawR50 = ENT::MutualEntropy(TS::trimTimeSeries(worldStates, { 0,.5 }, shortLifeTimes), TS::trimTimeSeries(shortBrainStatesAfter, { 0,.5 }, shortLifeTimes));
	org->dataMap.append("earlyRawR50", earlyRawR50);

	double earlyRawR20 = ENT::MutualEntropy(TS::trimTimeSeries(worldStates, { 0,.2 }, shortLifeTimes), TS::trimTimeSeries(shortBrainStatesAfter, { 0,.2 }, shortLifeTimes));
	org->dataMap.append("earlyRawR20", earlyRawR20);

	double lateRawR50 = ENT::MutualEntropy(TS::trimTimeSeries(worldStates, { .5,1 }, shortLifeTimes), TS::trimTimeSeries(shortBrainStatesAfter, { .5,1 }, shortLifeTimes));
	org->dataMap.append("lateRawR50", lateRawR50);

	double lateRawR20 = ENT::MutualEntropy(TS::trimTimeSeries(worldStates, { .8,1 }, shortLifeTimes), TS::trimTimeSeries(shortBrainStatesAfter, { .8,1 }, shortLifeTimes));
	org->dataMap.append("lateRawR20", lateRawR20);


	



	if (analyze) {
		std::cout << "NBack World analyze... organism with ID " << org->ID << " scored " << org->dataMap.getAverage("score") << std::endl;
		FileManager::writeToFile("score_id_" + std::to_string(org->ID) + ".txt", std::to_string(org->dataMap.getAverage("score")));



		if (saveFragOverTime) { // change to 1 to save frag over time
			std::cout << "  saving frag over time..." << std::endl;
			std::string header = "LOD_order, score,";
			std::string outStr = std::to_string(org->dataMap.getIntVector("ID")[0]) + "," + std::to_string(org->dataMap.getAverage("score")) + ",";
			std::vector<int> save_levelsThresholds = { 50,75,100 };
			for (auto th : save_levelsThresholds) {
				auto frag = FRAG::getFragmentationSet(worldStates, shortBrainStatesAfter, ((double)th) / 100.0, "feature");
				for (int f = 0; f < frag.size(); f++) {
					header += "Threshold_" + std::to_string(th) + "__feature_" + std::to_string(f) + ",";
					outStr += std::to_string(frag[f]) + ",";
				}
			}
			FileManager::writeToFile("fragOverTime.csv", outStr.substr(0, outStr.size() - 1), header.substr(0, header.size() - 1));
		}

		if (saveBrainStructureAndConnectome) {
			std::cout << "saving brain connectome and structrue..." << std::endl;

			brain->saveConnectome("brainConnectome_id_" + std::to_string(org->ID) + ".py");
			brain->saveStructure("brainStructure_id_" + std::to_string(org->ID) + ".dot");
		}

			//if (1) {
			//	auto smearPair = SMR::getSmearednessConceptsNodesPair(shortInputStates, worldStates, shortBrainStatesAfter);
			//	FileManager::writeToFile("smear.txt", std::to_string(smearPair.second));
			//	FileManager::writeToFile("smear.txt", std::to_string(smearPair.first));
			//
			//	std::cout << "rawR for worldStateSet { i,j }, brainStateSet, { i,j } " << std::endl;
			//
			//	for (double i = 0; i <= 1; i += .1) {
			//		std::cout << i << " : ";
			//		for (double j = i + .1; j <= 1; j += .1) {
			//			std::cout << ENT::MutualEntropy(TS::trimTimeSeries(worldStates, { i,j }, shortLifeTimes), TS::trimTimeSeries(shortBrainStatesAfter, { i,j }, shortLifeTimes)) / ENT::Entropy(TS::trimTimeSeries(shortBrainStatesAfter, { i,j }, shortLifeTimes)) << " , ";
			//		}
			//		std::cout << std::endl;
			//	}
			//}


		if (saveStateToState) {
			std::cout << "  saving state to state..." << std::endl;
			std::string fileName = "StateToState_id_" + std::to_string(org->ID) + ".txt";
			if (brain->recurrentOutput) {
				S2S::saveStateToState({ brainStates, outputStates }, { inputStates }, lifeTimes, "H_O__I_" + fileName);
				S2S::saveStateToState({ brainStates }, { inputStates }, lifeTimes, "H_I_" + fileName);
			}
			else {
				S2S::saveStateToState({ brainStates, TS::extendTimeSeries(outputStates, lifeTimes, {0}, TS::Position::FIRST) }, { inputStates }, lifeTimes, "H_O__I_" + fileName);
				S2S::saveStateToState({ brainStates }, { outputStates, inputStates }, lifeTimes, "H__O_I_" + fileName);
				S2S::saveStateToState({ brainStates }, { inputStates }, lifeTimes, "H_I_" + fileName);
			}
		}
			//std::cout << "worldEnt: " << ENT::Entropy(worldStates) << "  brainEnt: " << ENT::Entropy(shortBrainStatesAfter) << "  worldBrainEnt: " << ENT::Entropy(TS::Join(worldStates, shortBrainStatesAfter)) << "  rawR: " << rawR << std::endl;
			//std::cout << "earlyRawR20: " << earlyRawR20 << "  earlyRawR50: " << earlyRawR50 << "  lateRawR50: " << lateRawR50 << "  lateRawR20: " << lateRawR20 << std::endl;

			// save fragmentation matrix of brain(hidden) predictions of world features
		if (save_R_FragMatrix) {
			std::cout << "  saving R frag matrix..." << std::endl;

			FRAG::saveFragMatrix(worldStates, shortBrainStatesAfter, "R_FragmentationMatrix_id_" + std::to_string(org->ID) + ".py", "feature");
		}
			// save data flow information - 
			//std::vector<std::pair<double, double>> flowRanges = { {0,1},{0,.333},{.333,.666},{.666,1},{0,.5},{.5,1} };
			////std::vector<std::pair<double, double>> flowRanges = { {0,1},{.5,1} };///, { 0,.1 }, { .9,1 }};


		if (saveFlowMatrix) {
			std::cout << "  saving flow matix..." << std::endl;
			std::vector<std::pair<double, double>> flowRanges = { {0,.25}, {.75,1}, {0,1} };//, { 0,.1 }, { .9,1 }};

			//std::cout << TS::TimeSeriesToString(TS::trimTimeSeries(brainStates, TS::Position::LAST, lifeTimes), ",",",") << std::endl;
			//std::cout << TS::TimeSeriesToString(TS::trimTimeSeries(brainStates, TS::Position::FIRST, lifeTimes), ",",",") << std::endl;
			if (brain->recurrentOutput) {
				FRAG::saveFragMatrixSet(
					TS::Join({ TS::trimTimeSeries(brainStates, TS::Position::FIRST, lifeTimes), TS::trimTimeSeries(outputStates, TS::Position::FIRST, lifeTimes) }),
					TS::Join({ TS::trimTimeSeries(brainStates, TS::Position::LAST, lifeTimes), inputStates, TS::trimTimeSeries(outputStates, TS::Position::LAST, lifeTimes) }),
					lifeTimes, flowRanges, "flowMap_id_" + std::to_string(org->ID) + ".py", "shared", -1);
			}
			else {
				FRAG::saveFragMatrixSet(
					TS::Join(TS::trimTimeSeries(brainStates, TS::Position::FIRST, lifeTimes), outputStates),
					TS::Join(TS::trimTimeSeries(brainStates, TS::Position::LAST, lifeTimes), inputStates),
					lifeTimes, flowRanges, "flowMap_id_" + std::to_string(org->ID) + ".py", "shared", -1);
			}
		}
			//auto flowMatrix = FRAG::getFragmentationMatrix(TS::Join(TS::trimTimeSeries(brainStates, TS::Position::FIRST, lifeTimes), outputStates), TS::Join(TS::trimTimeSeries(brainStates, TS::Position::LAST, lifeTimes), inputStates), "feature");




		if (saveStates) {
			std::cout << "  saving brain states information..." << std::endl;
			std::string fileStr = "";
			if (brain->recurrentOutput) {

				auto discreetInput = inputStates;
				auto discreetOutputBefore = TS::trimTimeSeries(outputStates, TS::Position::LAST, lifeTimes);;
				auto discreetOutputAfter = TS::trimTimeSeries(outputStates, TS::Position::FIRST, lifeTimes);
				auto discreetHiddenBefore = TS::trimTimeSeries(brainStates, TS::Position::LAST, lifeTimes);
				auto discreetHiddenAfter = TS::trimTimeSeries(brainStates, TS::Position::FIRST, lifeTimes);

				fileStr += "input,outputBefore,outputAfter,hiddenBefore,hiddenAfter\n";
				for (int i = 0; i < discreetInput.size(); i++) {
					fileStr += "\"" + TS::TimeSeriesSampleToString(discreetInput[i], ",") + "\",";
					fileStr += "\"" + TS::TimeSeriesSampleToString(discreetOutputBefore[i], ",") + "\","; // every other
					fileStr += "\"" + TS::TimeSeriesSampleToString(discreetOutputAfter[i], ",") + "\","; // the other ones
					fileStr += "\"" + TS::TimeSeriesSampleToString(discreetHiddenBefore[i], ",") + "\","; // every other
					fileStr += "\"" + TS::TimeSeriesSampleToString(discreetHiddenAfter[i], ",") + "\"\n"; // the other ones
				}
			}
			else {

				auto discreetInput = inputStates;
				auto discreetOutput = outputStates;
				auto discreetHiddenBefore = TS::trimTimeSeries(brainStates, TS::Position::LAST, lifeTimes);
				auto discreetHiddenAfter = TS::trimTimeSeries(brainStates, TS::Position::FIRST, lifeTimes);

				fileStr += "input,output,hiddenBefore,hiddenAfter\n";
				for (int i = 0; i < discreetInput.size(); i++) {
					fileStr += "\"" + TS::TimeSeriesSampleToString(discreetInput[i], ",") + "\",";
					fileStr += "\"" + TS::TimeSeriesSampleToString(discreetOutput[i], ",") + "\",";
					fileStr += "\"" + TS::TimeSeriesSampleToString(discreetHiddenBefore[i], ",") + "\","; // every other
					fileStr += "\"" + TS::TimeSeriesSampleToString(discreetHiddenAfter[i], ",") + "\"\n"; // the other ones
				}
			}

			FileManager::writeToFile("NBack_BrainActivity_id_" + std::to_string(org->ID) + ".csv", fileStr);


		}
		std::cout << "  ... analyze done" << std::endl;
	} // end analyze

	


}

std::unordered_map<std::string, std::unordered_set<std::string>> NBackWorld::requiredGroups() {
	return { {groupName, {"B:" + brainName + ",1," + std::to_string(N2OutMap.size())}} };
}



