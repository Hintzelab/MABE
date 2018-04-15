//	author: cgnitash
//  DayNightWorld.cpp
//
//

#include "DayNightWorld.h"
#include "../../Utilities/Data.h"

// register all PARAMETERS
std::shared_ptr<ParameterLink<int>> DayNightWorld::WorldXPL =
    Parameters::register_parameter(
        "WORLD_DAYNIGHT-worldX", 10,
        "World X size (at least as much as WorldY) ");

std::shared_ptr<ParameterLink<int>> DayNightWorld::WorldYPL =
    Parameters::register_parameter("WORLD_DAYNIGHT-worldY", 10,
                                   "World Y size (at least 8) ");

std::shared_ptr<ParameterLink<int>> DayNightWorld::worldUpdatesPL =
    Parameters::register_parameter("WORLD_DAYNIGHT-worldUpdates", 100,
                                   "amount of time a brain is tested");

std::shared_ptr<ParameterLink<int>> DayNightWorld::evolCycleLengthPL =
    Parameters::register_parameter("WORLD_DAYNIGHT-evolCycleLength", 0,
                                   "# of generation in day/night environment "
                                   "(0 means the startWithDay condition lasts "
                                   "for the entire worldUpdates)");

std::shared_ptr<ParameterLink<double>> DayNightWorld::foodDensityPL =
    Parameters::register_parameter(
        "WORLD_DAYNIGHT-foodDensity", 0.1,
        "proportion(approx) of empty spots filled with food");
std::shared_ptr<ParameterLink<bool>> DayNightWorld::startWithDayPL =
    Parameters::register_parameter("WORLD_DAYNIGHT-startWithDay", false,
                                   "true: starts off with daytime, false:start "
                                   "with night (for an evolutionary run) ");

std::shared_ptr<ParameterLink<int>> DayNightWorld::evaluationsPerGenerationPL =
    Parameters::register_parameter("WORLD_DAYNIGHT-evaluationsPerGeneration", 1,
                                   "Number of times to test each Genome per "
                                   "generation (useful with non-deterministic "
                                   "brains)");

// these should be automatic
std::shared_ptr<ParameterLink<std::string>> DayNightWorld::groupNamePL =
    Parameters::register_parameter("WORLD_DAYNIGHT_NAMES-groupNameSpace",
                                   (std::string) "root::",
                                   "namespace of group to be evaluated");
std::shared_ptr<ParameterLink<std::string>> DayNightWorld::brainNamePL =
    Parameters::register_parameter(
        "WORLD_DAYNIGHT_NAMES-brainNameSpace", (std::string) "root::",
        "namespace for parameters used to define brain");

DayNightWorld::DayNightWorld(std::shared_ptr<ParametersTable> PT_)
    : AbstractWorld(PT_) {

  // localize all parameters
  worldY = WorldYPL->get(PT);
  worldX = WorldXPL->get(PT);

  worldUpdates = worldUpdatesPL->get(PT);
  evolCycleLength = evolCycleLengthPL->get(PT);

  foodDensity = foodDensityPL->get(PT);
  //  evolCycleDayNight = evolCycleDayNightPL->get(PT);
  startWithDay = startWithDayPL->get(PT);

  // should this be here?
  requiredInputs = 8;
  requiredOutputs = 2;

  popFileColumns.clear();
  popFileColumns.push_back("score");
  popFileColumns.push_back("score_VAR");
  //  popFileColumns.push_back("dayScore");
  //  popFileColumns.push_back("nightScore");

  evaluationsPerGeneration = evaluationsPerGenerationPL->get();
}

void DayNightWorld::EvaluateWithKnockout(std::shared_ptr<Organism> org,
                                         int analyse, int visualize,
                                         int debug) {

  /*
//  org->dataMap.append("optimizeValue", -1);
//  org->dataMap.append("score", 0);
//  org->dataMap.append("update", org->ID);
auto day_score = EvalInMode(org->brain, analyse, visualize, debug, true);
auto night_score = EvalInMode(org->brain, analyse, visualize, debug, false);
org->dataMap.append("dayScore", day_score);
org->dataMap.append("nightScore", night_score);
auto modularity_measure = 0.0;
for (auto &b : org->brain->getMutants()) {
auto day_score = EvalInMode(b, analyse, visualize, debug, true);
auto night_score = EvalInMode(b, analyse, visualize, debug, false);
modularity_measure += std::pow(day_score - night_score, 2);
org->dataMap.append("KO_DayScore", day_score);
org->dataMap.append("KO_NightScore", night_score);
}
// cout << org->dataMap.getIntVector("markovBrainGates").at(0);
org->dataMap.append("modularity_measure",
                modularity_measure /
                    org->dataMap.getIntVector("markovBrainGates").at(0));
org->dataMap.writeToFile("anal_data.csv");
// org->dataMap.writeToFile("anal_data" + to_string(org->ID) + ".csv");
*/
}

void DayNightWorld::evaluateSolo(std::shared_ptr<Organism> org, int analyse,
                                 int visualize, int debug) {

  // depending on global update number pick mode
  bool mode = evolCycleLength &&
                      Global::update % (evolCycleLength * 2) >= evolCycleLength
                  ? !startWithDay
                  : startWithDay;

  for (auto i = 0; i < evaluationsPerGeneration; i++) {
    auto score = EvalInMode(org->brain, analyse, visualize, debug, mode);
    org->dataMap.append("score", score);
  }
 // org->dataMap.setOutputBehavior("score", DataMap::AVE + DataMap::VAR);
}

void DayNightWorld::InputEnvIntoBrain(
    std::shared_ptr<AbstractBrain> brain,
    const std::vector<std::vector<std::vector<int>>> &smells_and_sights,
    std::pair<int, int> location, int facing_direction, bool mode) {

  // input environment into sensory nodes (input nodes)
  auto smell_input = smells_and_sights[location.first][location.second][4];
  auto sight_input =
      smells_and_sights[location.first][location.second][facing_direction];

    for (int j = 0; j < 8; ++j)
      brain->setInput(j,0.0);

  	if (mode)
    for (int j = 0; j < 4; ++j)
      // input nodes 0,1,2,3 have Day bits of sensory information
      brain->setInput(j, (sight_input > j));
  else
    for (int j = 0; j < 4; ++j)
      // input nodes 4,5,6,7 have Night bits of sensory information
      brain->setInput(j + 4, (smell_input > j));
}

// read behaviour from actuators (output nodes) and change the environment
void DayNightWorld::ChangeEnvFromBrainOutput(
    std::shared_ptr<AbstractBrain> brain, std::vector<std::vector<cell>> &grid,
    std::vector<std::vector<std::vector<int>>> &smells_and_sights,
    std::pair<int, int> &location, int &facing_direction, double &score) {
  // output has 2 actuator bits of information
  // [00 eat, 01 left, 10 right, 11 move]
  int output = Bit(brain->readOutput(0)) * 2 + (Bit(brain->readOutput(1)));

  //  output = Random::getInt(3);
  //  cout << output;
  // organism takes action in the world
  switch (output) {
  case 3: // tries to eat
  {
    if (grid[location.first][location.second] == cell::food) {
      score += 1.0;
      grid[location.first][location.second] = cell::empty;
      std::pair<int, int> new_food;
      do
        new_food = {Random::getInt(worldX - 1), Random::getInt(worldY - 1)};
      while (grid[new_food.first][new_food.second] != cell::empty);
      grid[new_food.first][new_food.second] = cell::food; // replace food
      smells_and_sights = MakeSmellsAndSights(grid);
    }
    break;
  }
  case 0: // tries to move
  {
    auto new_location = Facing(location, facing_direction);
    if (grid[new_location.first][new_location.second] != cell::wall)
      location = new_location;
    break;
  }
  case 1: // turns left
  {
    facing_direction = (facing_direction + 3) % 4;
    break;
  }
  case 2: // turns right
  {
    facing_direction = (facing_direction + 1) % 4;
    break;
  }
  }
}

double DayNightWorld::EvalInMode(std::shared_ptr<AbstractBrain> brain,
                                 int analyse, int visualize, int debug,
                                 bool mode) {

  /*
  if (visualize)
    vis_file.open("vis_data");
  */


  // setup the environment
  auto grid = MakeGrid(worldX, worldY);
  auto smells_and_sights = MakeSmellsAndSights(grid);

  // organism starts randomly(not on a wall) facing randomly
  std::pair<int, int> location;
  do {
    location = {Random::getInt(worldX - 1), Random::getInt(worldY - 1)}; //
  } while (grid[location.first][location.second] == cell::wall);

  int facing_direction = Random::getInt(3); // 0-North 1-East 2-South 3-West

  // setup organism
  brain->resetBrain();
  double score = 0.0;

  // run the environment
  for (int i = 0; i < worldUpdates; ++i) {
    // cout << location.first << " " << location.second << " " <<
    // facing_direction <<  endl;

    // input environment into sensory nodes (input nodes)
    InputEnvIntoBrain(brain, smells_and_sights, location, facing_direction,
                      mode);

    // run brain
    //brain->resetOutputs();
	//std::cout << "wu" << i << std::endl;
	brain->update();

    // read behaviour from actuators (output nodes) and change the environment
    ChangeEnvFromBrainOutput(brain, grid, smells_and_sights, location,
                             facing_direction, score);

	/*
    if (visualize) { // yuck - MUSt be an Archivist responsibility
      vis_file << location.first << "\t" << location.second << "\t"
               << facing_direction << "\t" << score << "\t" << mode << "\n";
      for (const auto &r : smells_and_sights) {
        for (const auto &c : r) {
          vis_file << c.at(4) << "\t";
        }
        vis_file << "\n";
      }
    }
	*/

  } // end world loop

  /*
  if (visualize) { // yuck - MUSt be an Archivist responsibility
   vis_file.close();
  }
  */

  return score;
}

// preserves toroidal invariant. always use
// location on toroid  the org is facing
/*
pair<int, int> DayNightWorld::Facing(const pair<int, int> &loc, int dir) {
  // 0-North 1-East 2-South 3-West
  // static_assert(fac%4 == fac,"worm looking into oblivion :(");
  return {dir % 2 ? loc.first : (loc.first + (dir - 1) + worldX) % worldX,
          dir % 2 ? (loc.second + (2 - dir) + worldY) % worldY : loc.second};
}
*/

std::vector<std::vector<std::vector<int>>>
DayNightWorld::MakeSmellsAndSights(const std::vector<std::vector<cell>> &grid) {
  // does some "clever" stuff to encode the envirnoment conveniently
  std::vector<std::vector<std::vector<int>>> smells_and_sights(
      worldX, std::vector<std::vector<int>>(worldY, std::vector<int>(5, 0)));

  for (int i = 0; i < worldX; ++i)
    for (int j = 0; j < worldY; ++j)
      if (grid[i][j] == cell::food) {

        for (int sensor = 0; sensor < 5; sensor++)
          smells_and_sights[i][j][sensor] = 4; // food in all sensors :)

        std::pair<int, int> pos = {i, j};
        for (int dir = 0; dir < 4; dir++) { // in all 4 directions
          auto l = Facing(pos, dir);
          for (int len = 1; len < 4; len++) { // for 4 steps
            // directional vision
            smells_and_sights[l.first][l.second][(dir + 2) % 4] = std::max(
                4 - len, smells_and_sights[l.first][l.second][(dir + 2) % 4]);

            // surround smell
            auto k = l;
            for (int klen = 0; klen < 4 - len;
                 klen++) { // with decreasing intensity of smell
              smells_and_sights[k.first][k.second][4] = std::max(
                  4 - len - klen, smells_and_sights[k.first][k.second][4]);
              k = Facing(k, (dir + 1) % 4);
            }
            l = Facing(l, dir);
          }
        }
      }

  return smells_and_sights;
}

// map<pair<int,int>,int>
std::vector<std::vector<DayNightWorld::cell>> DayNightWorld::MakeGrid(int X,
                                                                      int Y) {

  std::vector<std::vector<cell>> grid(X, std::vector<cell>(Y, cell::empty));

  // place food
  std::pair<int, int> loc;

  for (auto i = foodDensity * X * Y; i-- > 0;) {

    do
      loc = {Random::getInt(X - 1), Random::getInt(Y - 1)};
    while (grid[loc.first][loc.second] != cell::empty);
    grid[loc.first][loc.second] = cell::food;
  }
  return grid;
}

