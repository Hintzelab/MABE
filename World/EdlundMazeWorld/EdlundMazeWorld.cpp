//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "EdlundMazeWorld.h"

shared_ptr<ParameterLink<string>> EdlundMazeWorld::groupNamePL = Parameters::register_parameter("WORLD_EDLUND_MAZE_NAMES-groupName", (string)"root::", "name of group to be evaluated");
shared_ptr<ParameterLink<string>> EdlundMazeWorld::brainNamePL = Parameters::register_parameter("WORLD_EDLUND_MAZE_NAMES-brainName", (string)"root::", "name of brains used to control organisms\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");
shared_ptr<ParameterLink<int>> EdlundMazeWorld::evaluationsPerGenerationPL = Parameters::register_parameter("WORLD_EDLUND_MAZE-evaluationsPerGeneration", 1, "Number of times to test each Genome per generation (useful with non-deterministic brains)");

std::ostream_iterator<int> intToStream(std::cout, "");
std::ostream_iterator<string> strToStream(std::cout, "");
std::ostream_iterator<int> intToStreamWithCommas(std::cout, ",");
template <class Type>
void printVec(std::vector<Type> &v, int colWidth, int ignoreVal=-99) {
	std::vector<int> idx(std::distance(begin(v),end(v)));
	std::iota(begin(idx),end(idx),0);
	std::cout << std::left;
	for (auto& e : idx) std::cout << std::setw(colWidth) << e;
	std::cout << std::endl;
	for (auto& e : v)
		if (e==ignoreVal)
			std::cout << std::setw(colWidth) << ' ';
		else
			std::cout << std::setw(colWidth) << e;
	std::cout << std::endl << std::right;
}

EdlundMazeWorld::EdlundMazeWorld(shared_ptr<ParametersTable> _PT) :
	AbstractWorld(_PT) {
    using namespace EdlundMaze;
	groupName = groupNamePL->get(_PT);
	brainName = brainNamePL->get(_PT);
	// columns to be added to ave file
	popFileColumns.clear();
	popFileColumns.push_back("score");
	popFileColumns.push_back("doorsPassed");
	popFileColumns.push_back("exitsPassed");
	popFileColumns.push_back("stepsTaken");
	popFileColumns.push_back("doorsAchievable");
	popFileColumns.push_back("fitness");


	// init params
	maze.resize(M_LENGTH+1,EMPTY);
	signals.resize(M_LENGTH+1,EMPTY);
	solutions.resize(M_LENGTH+1,0);
	nextDoorLocations.resize(M_LENGTH+1,0);
	verticalSteps=0; /// used for solution, recalculated for each maze upon construction
	horizontalSteps=M_LENGTH-1; /// used for solution
}

// score is number of outputs set to 1 (i.e. output > 0) squared
void EdlundMazeWorld::evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
    using namespace EdlundMaze;
	int total_doorsPassed(0),
		total_exitsPassed(0),
		total_stepsTaken(0),
		total_doorsAchievable(0);
	float total_g_of_m(0),
			total_g_opt_of_m(0);
	double score = 1.0;
	int totalRepeats(evaluationsPerGenerationPL->get(PT));
	totalDoorsPassed = 0;
	totalDoorsAchievable = 0;
	auto brain = org->brains[brainName];
	for (int repeats=totalRepeats-1; repeats>=0; --repeats) {
		int orgRowi(0), /// start in front of 1st door - set later
			orgColi(0), /// organism position
			doorsPassed(0), /// counter
			exitsPassed(0), /// counter
			stepsTaken(0), /// counter
			maxDoors(0), /// maze dependent
			solutionSteps(0), /// maze dependent
			D(0);
		float g_of_m(0); /// accumulator
		makeNewMaze();
		orgRowi = maze[M_MAXLENGTH];
		orgColi = 0;
		findBestCaseSolutions();
		D = getLongestShortestPath();
		maxDoors = getNumberOfDoors();
		brain->resetBrain();
		int stepsAgentTook(0);
		vector<string> eachStep;
		float attainableFitness = playGameAndReturnGofM();
		for (int worldStep=T-1; worldStep>=0; worldStep--) { /// simulate for 2x perfect solution time
			if (orgColi != M_LENGTH) g_of_m += ((D-getDistanceToExit(orgRowi,orgColi))/(float)D)+exitsPassed;
			/// translate door position to inputs
			/// by taking a diff of position and door (if there is one)
			/// then truncating to -1,0,1, then adding 1 to get 0,1,2
			if (maze[orgColi+1]==EMPTY) {
				brain->setInput(S0_EYE_LEFT,0);
				brain->setInput(S1_EYE_CENTER,0);
				brain->setInput(S2_EYE_RIGHT,0);
			} else { /// looking at non-empty (w/ possibly door)
				brain->setInput(S0_EYE_LEFT,1);
				brain->setInput(S1_EYE_CENTER,1);
				brain->setInput(S2_EYE_RIGHT,1);
				int doorRowi = maze[orgColi+1];
				int doorPosDiff = doorRowi-orgRowi; /// relative door position
				if ((doorPosDiff >= -1) and (doorPosDiff <= 1)) { /// door is inside field of view
					doorPosDiff += 1; /// change into 0,1,2 offset
					brain->setInput(doorPosDiff,0); // it's an opening!
				}
			}
			brain->setInput(S3_DOOR_CUE,0);
			brain->setInput(S4_TOUCH_LEFT,0);
			brain->setInput(S5_TOUCH_RIGHT,0);
			if (maze[orgColi] == orgRowi) brain->setInput(S3_DOOR_CUE,signals[orgColi]); /// if standing in door
			if (orgRowi == 0) brain->setInput(S4_TOUCH_LEFT,1); /// hit the left edge
			if (orgRowi == M_MAXWIDTH) brain->setInput(S5_TOUCH_RIGHT,1); /// hit the right edge
			brain->update();
			int action = Bit(brain->readOutput(1))<<1 | Bit(brain->readOutput(0));
			switch(action) {
				case 0: /// go forward
				case 3:
					if (maze[orgColi+1]==EMPTY) {
						orgColi++;
						stepsTaken++;
						stepsAgentTook++;
						eachStep.push_back("f");
					}
					else if (maze[orgColi+1]==orgRowi) { /// entered a door
						totalDoorsPassed++;
						orgColi++; doorsPassed++; stepsTaken++;
						if (orgColi == M_MAXLENGTH) exitsPassed++;
						stepsAgentTook++;
						eachStep.push_back("f");
					}
					break;
				case 1:
					if ((orgRowi < M_MAXWIDTH) and (maze[orgColi]==EMPTY)) {
						orgRowi++;
						stepsTaken++;
						stepsAgentTook++;
						eachStep.push_back("r");
					}
					break;
				case 2:
					if ((orgRowi > 0) and (maze[orgColi]==EMPTY)) {
						orgRowi--;
						stepsTaken++;
						stepsAgentTook++;
						eachStep.push_back("l");
					}
					break;
			}
			if (orgColi == M_MAXLENGTH+1) {
				orgColi = 0; /// circular world - restart at beginning
				stepsAgentTook=0;
				eachStep.push_back("*");
			}
		}
		score *= (g_of_m / attainableFitness);
		total_stepsTaken += stepsTaken;
		total_doorsPassed += doorsPassed;
		total_exitsPassed += exitsPassed;
		total_g_of_m += g_of_m;
		total_g_opt_of_m += attainableFitness;
	
	}
	score = std::pow(score,(1.0/totalRepeats))*100.0;
	org->dataMap.set("score", score);
	org->dataMap.set("fitness", (total_g_of_m/totalRepeats) / (total_g_opt_of_m/totalRepeats));
	org->dataMap.set("stepsTaken", total_stepsTaken/(float)totalRepeats);
	org->dataMap.set("doorsPassed", totalDoorsPassed);
	org->dataMap.set("exitsPassed", total_exitsPassed/(float)totalRepeats);
	org->dataMap.set("doorsAchievable", totalDoorsAchievable);
}

int EdlundMazeWorld::getDistanceToExit(int rowi, int coli) {
	/// prerequires: makeNewMaze(), findBestCaseSolutions()
    using namespace EdlundMaze;
	int vertDiffFromSolutions(0);
	if (maze[coli]==EMPTY)
		vertDiffFromSolutions = std::abs(maze[nextDoorLocations[coli]]-rowi);
	return( solutions[coli] + vertDiffFromSolutions );
}

int EdlundMazeWorld::getLongestShortestPath() {
	/// prerequires: makeNewMaze(), findBestCaseSolutions()
    using namespace EdlundMaze;
	int leftLength = getDistanceToExit(0,0);
	int rightLength = getDistanceToExit(M_MAXWIDTH,0);
	return( std::max(leftLength, rightLength) );
}

float EdlundMazeWorld::playGameAndReturnGofM() {
    using namespace EdlundMaze;
	int row,col;
	int doorSig(signals[M_MAXLENGTH]);
	float g_of_m(0);
	int D = getLongestShortestPath();
	int exitsPassed(0);
	int stepsTaken(0);
	row=maze[M_MAXLENGTH]; col=0;
	std::vector<std::string> steps;
	int counter(0);
	for (int time=T-1; time>=0; time--) {
		counter++;
		/// if clear in front, go forward
		if (col!=M_LENGTH) g_of_m += (D-getDistanceToExit(row,col))/(float)D+exitsPassed;
		if ((maze[col+1]==row) or (maze[col+1]==EMPTY)) {
			steps.push_back("f");
			col++;
			if (col==M_MAXLENGTH) exitsPassed++;
			if (maze[col]!=EMPTY) {
				doorSig=signals[col];
				totalDoorsAchievable++;
			}
		} else { /// if wall, then do what our doorSig says
			if ((row==0) and (doorSig==0)) doorSig = 1;
			if ((row==M_MAXWIDTH) and (doorSig==1)) doorSig = 0;
			row += ((doorSig*2)-1);
			std::string signalChar[2] = {"l","r"};
			steps.push_back(signalChar[doorSig]);
		}
		stepsTaken++;
		if (col==M_MAXLENGTH+1) {
			col = 0;
			steps.push_back("*");
			stepsTaken=0;
		} /// wrap around
	}
	return g_of_m;
}

int EdlundMazeWorld::getNumberOfDoors() {
	/// prerequires: makeNewMaze()
    using namespace EdlundMaze;
	int total(0);
	for (int i=0; i<M_LENGTH; i++)
		if (maze[i]!=EMPTY) total++;
	return total;
}

void EdlundMazeWorld::findBestCaseSolutions() {
	/// prerequisites: makeNewMaze()
    using namespace EdlundMaze;
	int curi(M_MAXLENGTH-1),previ(M_MAXLENGTH);
	int steps(0); /// accumulator
	for( ; curi>=1; curi--) {
		nextDoorLocations[curi]=previ;
		if (maze[curi]==EMPTY) { /// simply step forward to door 1
			steps++;
			solutions[curi]=steps;
		} else { /// must traverse vertically toward door
			steps+=std::abs(maze[previ]-maze[curi])+1; /// 1 out the door, +diff for vert.
			solutions[curi]=steps;
			previ=curi;
		}
	}
	nextDoorLocations[0]=1;
	solutions[0]=solutions[1]+1;
	nextDoorLocations[M_MAXLENGTH]=1;
}

/// only used for debugging or possibly visualization
void EdlundMazeWorld::makeTestMaze() {
	std::vector<int> tempMaze({-1,11,1,1,1,1,-1});
	std::vector<int> tempSolutions({9,10,3,2,1,0,0});
	std::vector<int> tempNextDoorLocations({1,3,3,4,5,1,0});
	std::vector<int> tempSignals({-1,0,0,0,0,1,-1});
	maze=tempMaze;
	solutions=tempSolutions;
	nextDoorLocations=tempNextDoorLocations;
	signals=tempSignals;
	verticalSteps=6;
	horizontalSteps=5;
}

void EdlundMazeWorld::makeNewMaze() {
	/// manually fill first 2 columns, and last column
	/// blank(default), wall, ..., wall, respectively
    using namespace EdlundMaze;
	int prevcoli(1);
	verticalSteps=0;
	std::fill(begin(maze),end(maze),EMPTY);
	maze[1] = Random::getInt(0,M_MAXWIDTH);
	for (int coli=2; coli<M_MAXLENGTH; ) {
		int spaces = Random::getInt(0,3); /// how many spaces between next wall
		if (spaces == 0) {
			maze[coli] = maze[coli-1]; /// ensure aligned doors
			signals[prevcoli] = SIGNAL_LEFT_OR_STRAIGHT; prevcoli = coli; /// update signals
			coli++;
		} else { /// non-zero space until next wall
			if (coli+spaces>=M_MAXLENGTH) break; /// bounds check
			int projectedWalli=coli+spaces;
			for (coli; coli<projectedWalli; coli++) {maze[coli]=EMPTY;}
			maze[coli] = Random::getInt(0,M_MAXWIDTH);
			verticalSteps += std::abs(maze[coli]-maze[prevcoli]);
			signals[prevcoli] = (maze[coli]-maze[prevcoli] > 0) ? SIGNAL_RIGHT : SIGNAL_LEFT_OR_STRAIGHT; prevcoli = coli;
			coli++;
		}
	}
	/// ensure last wall is created and aligned if necessary
	maze[M_MAXLENGTH]=(maze[M_MAXLENGTH-1]!=EMPTY) ? maze[M_MAXLENGTH-1] : Random::getInt(0,M_MAXWIDTH);
	signals[M_MAXLENGTH] = (maze[1]-maze[M_MAXLENGTH] > 0) ? SIGNAL_RIGHT : SIGNAL_LEFT_OR_STRAIGHT;
	verticalSteps += std::abs(maze[M_MAXLENGTH]-maze[prevcoli]);
	signals[prevcoli] = (maze[M_MAXLENGTH]-maze[prevcoli] > 0) ? SIGNAL_RIGHT : SIGNAL_LEFT_OR_STRAIGHT; prevcoli = M_MAXLENGTH;
}
