//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "BerryWorld.h"

std::shared_ptr<ParameterLink<int>> BerryWorld::worldSizeXPL =
    Parameters::register_parameter("WORLD_BERRY-worldSizeX", 8,
                                   "width of world if no map files are "
                                   "provided (random map will be generated "
                                   "from initialFoodDistribution)");
std::shared_ptr<ParameterLink<int>> BerryWorld::worldSizeYPL =
    Parameters::register_parameter(
        "WORLD_BERRY-worldSizeY", 8,
        "height of world if no map files are provided");
std::shared_ptr<ParameterLink<bool>> BerryWorld::worldHasWallPL =
    Parameters::register_parameter("WORLD_BERRY-worldHasWall", true,
                                   "if world is being generated randomly (i.e. "
                                   "not loaded from file),\ndoes the world "
                                   "have a wall around the edge? (maps must "
                                   "define their own walls)");
std::shared_ptr<ParameterLink<int>> BerryWorld::rotationResolutionPL =
    Parameters::register_parameter("WORLD_BERRY_GEOMETRY-directions", 8,
                                   "the number of directions organisms can "
                                   "face\n  i.e. 36 = 10 degree turns, 8 = 45 "
                                   "degree turns, etc.");
std::shared_ptr<ParameterLink<double>> BerryWorld::maxTurnPL =
    Parameters::register_parameter(
        "WORLD_BERRY_GEOMETRY-maxTurn", .25,
        "harvesters can turn this amount per world update (i.e. if directions "
        "= 16 and maxTurn = .25, then 4 turns are allowed per\nworld update). "
        "This is implemented by allowing multipile turn (by 360 / directions "
        "degrees) actions in a single world update.");

std::shared_ptr<ParameterLink<bool>> BerryWorld::allowTurnPL =
    Parameters::register_parameter(
        "WORLD_BERRY_GEOMETRY-allowTurn", true,
        "if true, harvesters will be able to turn(change facing direction)");
std::shared_ptr<ParameterLink<bool>> BerryWorld::allowSidestepPL =
    Parameters::register_parameter("WORLD_BERRY_GEOMETRY-allowSidestep", false,
                                   "if true, harvesters will be able to move "
                                   "sideways (i.e. 90 degrees to facing)");
std::shared_ptr<ParameterLink<bool>> BerryWorld::allowBackstepPL =
    Parameters::register_parameter(
        "WORLD_BERRY_GEOMETRY-allowBackstep", false,
        "if true, harvesters will be able to move backwards");

std::shared_ptr<ParameterLink<bool>> BerryWorld::seeOtherPL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_VISION-seeOther", false,
        "can harvesters detect other harvesters with their vision sensor?");
std::shared_ptr<ParameterLink<bool>> BerryWorld::smellOtherPL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_SMELL-smellOther", false,
        "can harvesters detect other harvesters with their smell sensor?");
std::shared_ptr<ParameterLink<bool>> BerryWorld::seeFoodPL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_VISION-seeFood", true,
        "can harvesters detect food with their vision sensor?");
std::shared_ptr<ParameterLink<bool>> BerryWorld::smellFoodPL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_SMELL-smellFood", true,
        "can harvesters detect food with their smell sensor?");
std::shared_ptr<ParameterLink<bool>> BerryWorld::seeWallsPL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_VISION-seeWalls", true,
        "can harvesters detect walls with their vision sensor?");
std::shared_ptr<ParameterLink<bool>> BerryWorld::smellWallsPL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_SMELL-smellWalls", false,
        "can harvesters detect walls with their smell sensor?");
std::shared_ptr<ParameterLink<bool>> BerryWorld::useDownSensorPL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS-downSensorSwitch", true,
        "can harvesters detect food at their current location?");
std::shared_ptr<ParameterLink<bool>> BerryWorld::useSmellSensorPL =
    Parameters::register_parameter("WORLD_BERRY_SENSORS-smellSensorSwitch",
                                   false, "do harvesters have a smell sensor?");
std::shared_ptr<ParameterLink<bool>> BerryWorld::useVisionSensorPL =
    Parameters::register_parameter("WORLD_BERRY_SENSORS-visionSensorSwitch",
                                   false,
                                   "do harvesters have a vision sensor?");
std::shared_ptr<ParameterLink<bool>> BerryWorld::usePerfectSensorPL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS-perfectSensorSwitch", true,
        "do harvesters have a perfect sensor? (reqires direction is 4 or 8)");
std::shared_ptr<ParameterLink<std::string>> BerryWorld::perfectSensorFilePL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_PERFECT-perfectSensorFile",
        (std::string) "World/BerryWorld/perfectSensors/smallFront.txt",
        "file which defines perfect sensor?");

std::shared_ptr<ParameterLink<bool>> BerryWorld::perfectDetectsFoodPL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_PERFECT-perfectSensorDetectsFood", true,
        "can perfect sensor detect food?");
std::shared_ptr<ParameterLink<bool>> BerryWorld::perfectDetectsOtherPL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_PERFECT-perfectSensorDetectsOther", false,
        "can perfect sensor detect other?");
std::shared_ptr<ParameterLink<bool>> BerryWorld::perfectDetectsWallsPL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_PERFECT-perfectSensorDetectsWalls", false,
        "can perfect sensor detect walls?");

std::shared_ptr<ParameterLink<bool>> BerryWorld::wallsBlockVisonSensorsPL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_VISION-wallsBlockVisonSensors", false,
        "if true, objects behind walls can not been seen (setting to false "
        "allows for larger sensors)");
std::shared_ptr<ParameterLink<bool>> BerryWorld::wallsBlockSmellSensorsPL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_SMELL-wallsBlockSmellSensors", false,
        "if true, objects behind walls can not been smelled (setting to false "
        "allows for larger sensors)");

std::shared_ptr<ParameterLink<int>> BerryWorld::visionSensorDistancePL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_VISION-visionSensorDistanceMax", 6,
        "how far can orgs see?");
std::shared_ptr<ParameterLink<int>> BerryWorld::visionSensorArcSizePL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_VISION-visionSensorArcSize", 5,
        "how wide is a vision arc (degrees)");
std::shared_ptr<ParameterLink<std::string>>
    BerryWorld::visionSensorDirectionsPL = Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_VISION-visionSensorDirections",
        (std::string) "-2,0,1", "what directions can org see? (if value is < "
                                  "1, then this value will be multipied by "
                                  "directions)");

std::shared_ptr<ParameterLink<int>> BerryWorld::smellSensorDistancePL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_SMELL-smellSensorDistanceMax", 2,
        "how far can orgs smell?");
std::shared_ptr<ParameterLink<int>> BerryWorld::smellSensorArcSizePL =
    Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_SMELL-smellSensorArcSize", 180,
        "how wide is a smell arc (degrees)");
std::shared_ptr<ParameterLink<std::string>>
    BerryWorld::smellSensorDirectionsPL = Parameters::register_parameter(
        "WORLD_BERRY_SENSORS_SMELL-smellSensorDirections",
        (std::string) "0,.25,.5,.75", "what directions can org smell? (if "
                                        "value is < 1, then this value will be "
                                        "multipied by directions)");

std::shared_ptr<ParameterLink<int>> BerryWorld::evalTimePL =
    Parameters::register_parameter("WORLD_BERRY-evalationTime", 400,
                                   "number of world updates per evaluation "
                                   "(i.e. number of actions each agent can "
                                   "perform per map)");
std::shared_ptr<ParameterLink<int>> BerryWorld::evaluationsPerGenerationPL =
    Parameters::register_parameter("WORLD_BERRY-evaluationsPerGeneration", 1,
                                   "The world will be run one the entire "
                                   "population this number of times (useful to "
                                   "correct for chance in world or organism)");
std::shared_ptr<ParameterLink<int>> BerryWorld::evaluateGroupSizePL =
    Parameters::register_parameter(
        "WORLD_BERRY_GROUP-groupSize", 1,
        "organisms will be evaluated in groups of this size, the groups will "
        "be determined randomly.  \nIf (population size / groupSize) has a "
        "remainder, some organisms will be evaluated more then once.\n  -1 "
        "indicates to evaluate all organisms at the same time.");
std::shared_ptr<ParameterLink<std::string>> BerryWorld::groupScoreRulePL =
    Parameters::register_parameter(
        "WORLD_BERRY_GROUP-groupScoreRule", (std::string) "SOLO",
        "how should groups be scored? SOLO = no group scoring. ALL = each "
        "group member receives all scores(effectivly average), BEST = each "
        "group member receives score of best in group, WORST = each group "
        "member receives score of worst in group");
std::shared_ptr<ParameterLink<std::string>> BerryWorld::cloneScoreRulePL =
    Parameters::register_parameter(
        "WORLD_BERRY_GROUP-cloneScoreRule", (std::string) "ALL",
        "if clones is > 0 what score rule will be applied to the parent "
        "organism. ALL = score from all clones(effectivly average), BEST = "
        "only the score from the best clone, WORST = only the score from the "
        "worst clone. if groupScoreRule is not SOLO this parametdr is "
        "ignored.");
std::shared_ptr<ParameterLink<int>> BerryWorld::clonesPL =
    Parameters::register_parameter("WORLD_BERRY_GROUP-clones", 0,
                                   "evaluation groups will be padded with this "
                                   "number of clones for each harvester. I.e. "
                                   "if group size is 3 and clones is 1, the "
                                   "actual group size will be 6");

std::shared_ptr<ParameterLink<double>> BerryWorld::switchCostPL =
    Parameters::register_parameter(
        "WORLD_BERRY_SCORE-switchCost", 1.4,
        "cost paid when switching between food types");
std::shared_ptr<ParameterLink<double>> BerryWorld::hitWallCostPL =
    Parameters::register_parameter(
        "WORLD_BERRY_SCORE-hitWallCost", 0.0,
        "cost paid if a move failed because of a wall being in the way");
std::shared_ptr<ParameterLink<double>> BerryWorld::hitOtherCostPL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-hitOtherCost", 0.0,
                                   "cost paid if a move failed because of "
                                   "another harvester being in the way");
std::shared_ptr<ParameterLink<int>> BerryWorld::alwaysStartOnPL =
    Parameters::register_parameter("WORLD_BERRY-alwaysStartOnResource", -1,
                                   "all organisms will start on this type of "
                                   "food (-1 = random non-wall start "
                                   "locations)");
std::shared_ptr<ParameterLink<std::string>>
    BerryWorld::validStartConfigurationsPL = Parameters::register_parameter(
        "WORLD_BERRY-startConfigurations", (std::string) "-1",
        "If alwaysStartOn is -1 and no start locations are provided by a map, "
        "these will determin inital placment of harvesters.\nformat: "
        "x,y,facing,x,y,facing,x,y,facing if value = -1 = all random "
        "starts (default); -1 in facing position will pick random facing for "
        "that location.");

std::shared_ptr<ParameterLink<int>> BerryWorld::foodTypesPL =
    Parameters::register_parameter(
        "WORLD_BERRY_FOOD-foodTypes", 2,
        "how many different types of food (valid values:1,2,3,4,5,6,7,8)");

std::shared_ptr<ParameterLink<std::string>>
    BerryWorld::initialFoodDistributionPL = Parameters::register_parameter(
        "WORLD_BERRY_FOOD-initialFoodDistribution", (std::string) "-1",
        "values used to generate world when maps are not loaded.\nList "
        "provides types of foods and probability of that food on each "
        "location.\n  1,1,2 = 2/3 1, 1/3 2\n  -1,0,2 = 1/3 random from "
        "food types, 1/3 0, 1/3 2");

std::shared_ptr<ParameterLink<double>> BerryWorld::reward1PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-reward1", 1.0,
                                   "reward for eating food1");
std::shared_ptr<ParameterLink<double>> BerryWorld::reward2PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-reward2", 1.0,
                                   "reward for eating food2");
std::shared_ptr<ParameterLink<double>> BerryWorld::reward3PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-reward3", 1.0,
                                   "reward for eating food3");
std::shared_ptr<ParameterLink<double>> BerryWorld::reward4PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-reward4", 1.0,
                                   "reward for eating food4");
std::shared_ptr<ParameterLink<double>> BerryWorld::reward5PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-reward5", 1.0,
                                   "reward for eating food5");
std::shared_ptr<ParameterLink<double>> BerryWorld::reward6PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-reward6", 1.0,
                                   "reward for eating food6");
std::shared_ptr<ParameterLink<double>> BerryWorld::reward7PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-reward7", 1.0,
                                   "reward for eating food7");
std::shared_ptr<ParameterLink<double>> BerryWorld::reward8PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-reward8", 1.0,
                                   "reward for eating food8");

std::shared_ptr<ParameterLink<std::string>> BerryWorld::replace1PL =
    Parameters::register_parameter(
        "WORLD_BERRY_FOOD-replace1", (std::string) "-1",
        "when food 1 is eaten, what should replace it?\nreplacement is chosen "
        "randomly from this list\nelements that appear more then once are more "
        "likely to be selected\n-1 will choose randomly from available food "
        "types");
std::shared_ptr<ParameterLink<std::string>> BerryWorld::replace2PL =
    Parameters::register_parameter("WORLD_BERRY_FOOD-replace2",
                                   (std::string) "-1",
                                   "replace rule for food2");
std::shared_ptr<ParameterLink<std::string>> BerryWorld::replace3PL =
    Parameters::register_parameter("WORLD_BERRY_FOOD-replace3",
                                   (std::string) "-1",
                                   "replace rule for food3");
std::shared_ptr<ParameterLink<std::string>> BerryWorld::replace4PL =
    Parameters::register_parameter("WORLD_BERRY_FOOD-replace4",
                                   (std::string) "-1",
                                   "replace rule for food4");
std::shared_ptr<ParameterLink<std::string>> BerryWorld::replace5PL =
    Parameters::register_parameter("WORLD_BERRY_FOOD-replace5",
                                   (std::string) "-1",
                                   "replace rule for food5");
std::shared_ptr<ParameterLink<std::string>> BerryWorld::replace6PL =
    Parameters::register_parameter("WORLD_BERRY_FOOD-replace6",
                                   (std::string) "-1",
                                   "replace rule for food6");
std::shared_ptr<ParameterLink<std::string>> BerryWorld::replace7PL =
    Parameters::register_parameter("WORLD_BERRY_FOOD-replace7",
                                   (std::string) "-1",
                                   "replace rule for food7");
std::shared_ptr<ParameterLink<std::string>> BerryWorld::replace8PL =
    Parameters::register_parameter("WORLD_BERRY_FOOD-replace8",
                                   (std::string) "-1",
                                   "replace rule for food8");

std::shared_ptr<ParameterLink<double>> BerryWorld::poison0PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-poison0", 0.0,
                                   "cost for landing on empty");
std::shared_ptr<ParameterLink<double>> BerryWorld::poison1PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-poison1", 0.0,
                                   "cost for landing on food1");
std::shared_ptr<ParameterLink<double>> BerryWorld::poison2PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-poison2", 0.0,
                                   "cost for landing on food2");
std::shared_ptr<ParameterLink<double>> BerryWorld::poison3PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-poison3", 0.0,
                                   "cost for landing on food3");
std::shared_ptr<ParameterLink<double>> BerryWorld::poison4PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-poison4", 0.0,
                                   "cost for landing on food4");
std::shared_ptr<ParameterLink<double>> BerryWorld::poison5PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-poison5", 0.0,
                                   "cost for landing on food5");
std::shared_ptr<ParameterLink<double>> BerryWorld::poison6PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-poison6", 0.0,
                                   "cost for landing on food6");
std::shared_ptr<ParameterLink<double>> BerryWorld::poison7PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-poison7", 0.0,
                                   "cost for landing on food7");
std::shared_ptr<ParameterLink<double>> BerryWorld::poison8PL =
    Parameters::register_parameter("WORLD_BERRY_SCORE-poison8", 0.0,
                                   "cost for landing on food8");

std::shared_ptr<ParameterLink<bool>> BerryWorld::allowMoveAndEatPL =
    Parameters::register_parameter(
        "WORLD_BERRY-allowEatAndMove", false,
        "if false, and there is an eat output and move output, move outputs "
        "will be ignored\nif true and there is both an eat and move output, "
        "eat will happen first, then move.");
std::shared_ptr<ParameterLink<bool>> BerryWorld::alwaysEatPL =
    Parameters::register_parameter(
        "WORLD_BERRY-alwaysEat", false,
        "if true, organism will have only two outputs. organism will always "
        "attempt eat before moving. If false, there will be a third input for "
        "eat which must be activated for the agent to eat");

std::shared_ptr<ParameterLink<std::string>> BerryWorld::groupNameSpacePL =
    Parameters::register_parameter("WORLD_BERRY_NAMESPACE-NameSpace_group",
                                   (std::string) "root::",
                                   "namespace of group to be evaluated");
std::shared_ptr<ParameterLink<std::string>> BerryWorld::brainNameSpacePL =
    Parameters::register_parameter(
        "WORLD_BERRY_NAMESPACE-NameSpace_brain", (std::string) "root::",
        "namespace for parameters used to define brain");

std::shared_ptr<ParameterLink<double>> BerryWorld::moveDefaultPL =
    Parameters::register_parameter(
        "WORLD_BERRY_GEOMETRY-moveDefault", 1.0,
        "distance harvester moves when output is move");
std::shared_ptr<ParameterLink<double>> BerryWorld::moveMinPL =
    Parameters::register_parameter("WORLD_BERRY_GEOMETRY-moveMin", 0.0,
                                   "min distance harvester moves every world "
                                   "update (even if output is not move)");
std::shared_ptr<ParameterLink<double>> BerryWorld::moveMinPerTurnPL =
    Parameters::register_parameter("WORLD_BERRY_GEOMETRY-moveMinPerTurn", 0.0,
                                   "min distance harvester moves every time "
                                   "harvester turns (there may be more then "
                                   "one turn in a world update)");
std::shared_ptr<ParameterLink<bool>> BerryWorld::snapToGridPL =
    Parameters::register_parameter(
        "WORLD_BERRY_GEOMETRY-snapTpGrid", true,
        "if true, harvester will always move to center of nearest space");

std::shared_ptr<ParameterLink<std::string>> BerryWorld::mapFilesPL =
    Parameters::register_parameter(
        "WORLD_BERRY_MAPS-mapFiles", (std::string) "",
        "list of names of file containing maps (if NONE, random map will be "
        "generated) e.g. [World/HarvestXWorld/maps/patch.txt]");
std::shared_ptr<ParameterLink<std::string>> BerryWorld::whichMapsPL =
    Parameters::register_parameter(
        "WORLD_BERRY_MAPS-whichMaps", (std::string) "1/1",
        "list of maps from mapFiles to use to evaluate each harvester, "
        "type/name will select named map,\n* in either position = all,\na "
        "number = randomly select that number of maps (will fail if there are "
        "not enough maps)");

std::shared_ptr<ParameterLink<std::string>> BerryWorld::triggerFoodsPL =
    Parameters::register_parameter(
        "WORLD_BERRY_TRIGGERS-triggerFoods", (std::string) "",
        "list of foods with triggers, for each food in this list, there must "
        "also be a level and event\nmore then one food may be included in a "
        "trigger (seperated by +). All food must be <= level in this case");
std::shared_ptr<ParameterLink<std::string>> BerryWorld::triggerFoodLevelsPL =
    Parameters::register_parameter(
        "WORLD_BERRY_TRIGGERS-triggerFoodLevels", (std::string) "",
        "list of levels when food triggers will activate, events will trigger "
        "when the associated food drops to or below this level");
std::shared_ptr<ParameterLink<std::string>> BerryWorld::triggerFoodEventsPL =
    Parameters::register_parameter(
        "WORLD_BERRY_TRIGGERS-triggerFoodEvents", (std::string) "",
        "list of events which will take place when trigger activates\nT*[num] "
        "= all harvesters in the current group gain score of (world updates "
        "remaining * num)\nS[num] = all harvesters gain score of num\n "
        "R[A,B,...] replace A with B (i.e. replace all 2 with 1 can be used to "
        "reset map)\nG[x,y,f,...] = generate f at x,y; if any value == -1 then "
        "use random in world range or food range\nQ = stop running this "
        "evaluation\nrules may be combined with + (i.e. S10+Q)");

template <class T>
inline static bool load_value(const std::string &value, T &target) {
  std::stringstream ss(value);
  std::string remaining;
  return ss >> target ? !(ss >> remaining) : false;
}
inline std::vector<std::string> parseCSVLine(std::string raw_line,
                                             const char separator = ',',
                                             const char sep_except = '"') {
  std::vector<std::string> data_line;
  std::string s(1,separator), se(1,sep_except);
  const std::regex piece(R"((.*?)()" + s + "|" + se + R"(|$))");
  bool in_quotes = false;
  std::string quoted_string;
  for (auto &m : forEachRegexMatch(raw_line, piece)) {
    if (m[2].str() == se) {
      if (!in_quotes) {
        data_line.push_back(m[1].str());
        in_quotes = true;
      } else {
        quoted_string += m[1].str();
        data_line.push_back(quoted_string);
        quoted_string = "";
        in_quotes = false;
      }
    } else {
      if (!in_quotes)
        data_line.push_back(m[1].str());
      else
        quoted_string += m[0].str();
    }
  }
  data_line.erase(std::remove_if(data_line.begin(), data_line.end(),
                                 [](std::string s) { return s == ""; }),
                  data_line.end());
  return data_line;
}
template <class T>
inline void convertCSVListToVector_BERRY(std::string string_data,
                                   std::vector<T> &return_data,
                                   const char separator = ',',
                                   const char sep_except = '"') {
  return_data.clear();
  // check all uses of this function to see if leading and trailing quotes are
  // needed
  static const std::regex stripoff_qoute(R"(^"(.*?)?"$)");
  static const std::regex stripoff_square_brackets(R"(^\[(.*?)\]$)");
  std::smatch m_quote;
  string_data = std::regex_match(string_data, m_quote, stripoff_qoute)
                    ? m_quote[1].str()
                    : string_data;
  std::smatch m_square;
  string_data = std::regex_match(string_data, m_square, stripoff_square_brackets)
                    ? m_square[1].str()
                    : string_data;
  T temp; // immediately assign from load_value
  for (auto &s : parseCSVLine(string_data, separator, sep_except)) {
    if (!load_value(s, temp)) {
      std::cout << " --- while parsing: " << string_data << " .... "
                << std::endl;
      std::cout << " In convertCSVListToVector_BERRY() attempt to convert string "
                << s << " to  value failed\n " << std::endl;
      exit(1);
    }
    return_data.push_back(temp);
  }
}
inline bool loadLineToSS(std::ifstream &file, std::string &rawLine,
                         std::stringstream &ss) {
  rawLine.resize(0);
  if (file.is_open() && !file.eof()) {
    while ((rawLine.size() == 0 || rawLine[0] == '#') && !file.eof()) {
      getline(file, rawLine);
    }
    ss.clear();
    ss.str(std::string());
    ss << rawLine;
  } else if (!file.eof()) {
    std::cout << "in loadSS, file is not open!\n  Exiting." << std::endl;
    exit(1);
  }
  // cout << "from file:  " << rawLine << endl;
  return file.eof();
}

std::vector<int> pickUnique(int numAvalible, int numPicks) {
  std::vector<int> picks;
  for (int i = 0; i < numAvalible; ++i) {
    picks.push_back(i);
  }
  if (numPicks != numAvalible) {
    for (int i = 0; i < numPicks; ++i) {
      int j = Random::getInt(i, numAvalible - 1);
      if (j != i) {
        auto temp = picks[i];
        picks[i] = picks[j];
        picks[j] = temp;
      }
    }
  }
  std::vector<int> newVec(picks.begin(), picks.begin() + numPicks);
  return newVec;
}

bool readFileLine(std::ifstream &FILE, std::string &rawLine,
                  std::stringstream &ss) {
  bool atEOF = false;
  atEOF = loadLineToSS(FILE, rawLine, ss); // read next line of file
  std::string cleanLine = rawLine;
  cleanLine.erase(remove_if(cleanLine.begin(), cleanLine.end(), ::isspace),
                  cleanLine.end());
  while (!atEOF && cleanLine[0] != '#' &&
         cleanLine.size() == 0) { // keep reading lines until we find a line
                                  // that is not end of file, or starting with #
                                  // or empty
    atEOF = loadLineToSS(FILE, rawLine, ss); // read next line of file
    cleanLine = rawLine;
    cleanLine.erase(remove_if(cleanLine.begin(), cleanLine.end(), ::isspace),
                    cleanLine.end());
  }
  return atEOF;
}

Vector2d<char> loadSquareGridOfIntsFromFile(std::ifstream &FILE) {
  // this assumes we have a file with a square char "image"
  std::string rawLine, rubbishString;
  char charBuffer;
  std::stringstream ss("");
  bool atEOF = false;
  bool goodRead = false;

  int gridSize = 0;

  std::vector<char> rawgrid;
  Vector2d<char> grid;

  atEOF = readFileLine(FILE, rawLine, ss);
  gridSize = rawLine.size();
  for (int r = 0; r < gridSize; r++) {
    for (int c = 0; c < gridSize; c++) {
      rawgrid.push_back(ss.get());
    }
    atEOF = readFileLine(FILE, rawLine, ss);
  }
  grid.reset(gridSize, gridSize);
  grid.assign(rawgrid);
  // grid.showGrid();
  return grid;
}

void fillInVision(std::vector<std::vector<std::vector<Point2d>>> &vision,
                  bool reflectFirst = false) {
  std::vector<std::vector<Point2d>> tempVec;
  int numArcs = vision.size();

  // if vision is size one then there is one arc (forward (0)) which will be
  // reflected to left(1), down(2) and right(3).
  // i.e. 4 directions

  // if vision is size two then there are two arcs (forward(0) and forward
  // right(1)) which will be reflected to right(2), back right (3),
  // back(4)...forwad left (7)
  // i.e. 8 directions

  if (reflectFirst) { // this is used when there are non semetic arcs (i.e. left
                      // vs. right arc)
    // one arc is passed in with reflectFirst = true and this will return the
    // rotations for the reflected arc.
    for (auto distance : vision[0]) {
      tempVec.push_back({});
      for (auto p : distance) {
        tempVec.back().push_back(Point2d(-p.x, p.y)); // reflect around x
      }
    }
    vision[0] = tempVec;
    if (numArcs == 2) {
      tempVec.clear();
      for (auto distance : vision[1]) {
        tempVec.push_back({});
        for (auto p : distance) {
          tempVec.back().push_back(Point2d(-p.y, -p.x)); // reflect around x,-y
        }
      }
      vision[1] = tempVec;
      tempVec.clear();
    }
  }
  if (numArcs == 2) { // we need to make right, down right, down, down left,
                      // left and up left
    for (int i = 0; i < 6; i++) {
      for (auto line : vision[i % 2]) {
        tempVec.push_back({});
        for (auto p : line) {
          if ((int)i / 2 == 0) {
            tempVec.back().push_back(Point2d(-p.y, p.x));
          } else if ((int)i / 2 == 1) {
            tempVec.back().push_back(Point2d(-p.x, -p.y));
          } else {
            tempVec.back().push_back(Point2d(p.y, -p.x));
          }
        }
      }
      vision.push_back(tempVec);
      tempVec.clear();
    }
  } else { // numArcs = 1, we only need to create left, down and right
    for (int i = 0; i < 3; i++) {
      for (auto line : vision[0]) {
        tempVec.push_back({});
        for (auto p : line) {
          if ((int)i == 0) {
            tempVec.back().push_back(Point2d(-p.y, p.x));
          } else if ((int)i == 1) {
            tempVec.back().push_back(Point2d(-p.x, -p.y));
          } else {
            tempVec.back().push_back(Point2d(p.y, -p.x));
          }
        }
      }
      vision.push_back(tempVec);
      tempVec.clear();
    }
  }
}

bool BerryWorld::WorldMap::loadMap(std::ifstream &FILE,
                                   const std::string _fileName) {
  file = _fileName;
  std::string rawLine, rubbishString;
  char charBuffer;
  std::stringstream ss("");
  bool atEOF = false;
  bool goodRead = false;

  worldX = 0;
  worldY = 0;

  int intConverter;
  std::vector<int> rawMap;
  std::vector<int> rawStartMap;

  if (FILE.is_open()) {
    atEOF = readFileLine(FILE, rawLine, ss);
    if (atEOF) { // if what's left in file is only whitespace...
      name = "XXXXXXXXXXXXXXXXXXXX";
      return true;
    }
    ss >> rubbishString >> rubbishString >> name;
    // now that we have the name and type, we can make the PT. The namespace
    // will be based on the parents (the world) name space.
    // PT =
    // make_shared<ParametersTable>(parentPT->getTableNameSpace()+"::"+fileName+"__"+name,Parameters::root);
    if (name != "") {
      atEOF = readFileLine(FILE, rawLine, ss); // read next line of file
      worldX = rawLine.size(); // x size of map is length of this line
      while (!atEOF && ss.peek() != '*' && ss.peek() != '+') {
        if (rawLine.size() != worldX) {
          std::cout << "  While loading map " << name << " from file: " << file
                    << ", map has a line with the incorrect length\nexiting"
                    << std::endl;
          exit(1);
        }
        for (int x = 0; x < worldX;
             x++) { // load first line of map, keep loading chars until end of
                    // line (i.e. ss.fail because char could not be read)
          charBuffer = ss.get();
          rawMap.push_back(charBuffer - '0');
          //// experiment to allow random values in maps - causes problems when
          ///trying to creat valid start locations... ////////////////////
          // if (charBuffer == 'r') {
          //	rawMap.push_back(-1);
          //}
          // else {
          //	rawMap.push_back(charBuffer - '0');
          //}
          ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        }
        worldY++;
        atEOF = readFileLine(FILE, rawLine, ss); // read next line of file
      }
      if (ss.peek() == '+') {
        useStartMap = true;
        for (int y = 0; y < worldY; y++) {
          atEOF = readFileLine(FILE, rawLine, ss); // read next line of file
          if (ss.peek() == '*') {
            std::cout
                << "  While loading map " << name << " from file: " << file
                << ", starting locations size is smaller then map size\nexiting"
                << std::endl;
            exit(1);
          }
          if (rawLine.size() != worldX) {
            std::cout << "  While loading map " << name
                      << " from file: " << file
                      << ", starting locations has a line with the incorrect "
                         "length\nexiting"
                      << std::endl;
            exit(1);
          }
          for (int x = 0; x < worldX; x++) {
            rawStartMap.push_back(ss.get() - '0');
          }
        }
        atEOF = readFileLine(FILE, rawLine, ss); // read next line of file
      }

      // LOAD GENERATORS
      while (ss.peek() == 'G') { // this line defines a generator
        if (AbstractWorld::debugPL->get()) {
          std::cout << "  While loading map found generator definition: "
                    << rawLine << std::endl;
        }
        std::string cleanLine;
        for (auto c :
             rawLine) { // remove spaces for rawLine to create cleanLine
          if (c != ' ') {
            cleanLine.push_back(c);
          }
        }
        std::vector<std::string> splitLine(1);
        for (auto c :
             cleanLine) { // make splitLine by breaking up cleanLine on ':'
          if (c == ':') {
            splitLine.push_back("");
          } else {
            splitLine.back().push_back(c);
          }
        }

        std::vector<int> locations;
        std::vector<int> size;
        std::vector<double> rate;
        std::vector<std::vector<double>> resourcePrograms;

        convertCSVListToVector_BERRY(splitLine[1], locations);
        convertCSVListToVector_BERRY(splitLine[2], size);
        convertCSVListToVector_BERRY(splitLine[3], rate);

        std::vector<std::string> resourceRulesStrings(
            1); // this will hold std::string for resource rules

        bool inList = false;
        // get resource programs
        for (int c = 1; c < (int)splitLine[4].size() - 1; c++) {
          if (splitLine[4][c] ==
              '[') { // if we are in a list, ignore ','s until out of list
            inList = true;
          }
          if (splitLine[4][c] == ']') { // found end of list
            inList = false;
          }
          if (!inList && splitLine[4][c] == ',') { // if we fond a ',' outsize
                                                   // of a list then add a new
                                                   // program
            resourceRulesStrings.push_back("");
          } else {
            resourceRulesStrings.back().push_back(
                splitLine[4][c]); // else add char to current program
          }
        }

        for (auto rule : resourceRulesStrings) {
          resourcePrograms.push_back({});
          convertCSVListToVector_BERRY(rule, resourcePrograms.back()); // convert each
                                                                 // resource
                                                                 // rule from
                                                                 // std::string
                                                                 // to numbers
        }

        int replaceRule; // set replacment rule
        if (splitLine[5] == "RN") {
          replaceRule = 0;
        } else if (splitLine[5] == "RF") {
          replaceRule = 1;
        } else if (splitLine[5] == "RW") {
          replaceRule = 2;
        } else if (splitLine[5] == "RA") {
          replaceRule = 3;
        } else {
          std::cout << "  While loading generator for map " << name
                    << " from file: " << file
                    << ", found generator with bad replace rule (must be "
                       "RN,RF,RW or, RA)\nexiting"
                    << std::endl;
          exit(1);
        }
        for (int i = 0; i < locations.size();
             i +=
             2) { // make a resource generator for each pair in locations list
          ResourceGenerator newGenerator;
          newGenerator.x = locations[i];
          newGenerator.y = locations[i + 1];
          if (size[0] > 1 || size[1] > 1) {
            newGenerator.geo = 1; // area/square generator
            newGenerator.x1 = newGenerator.x + size[0] - 1;
            newGenerator.y1 = newGenerator.y + size[1] - 1;
          } else {
            newGenerator.geo = 0; // point generator
          }
          newGenerator.rate = rate;
          newGenerator.resourcePrograms = resourcePrograms;
          newGenerator.replaceRule = replaceRule;
          generators.push_back(newGenerator);
        }
        atEOF = readFileLine(FILE, rawLine, ss); // read next line of file
      }
      // LOAD TRIGGERS
      while (ss.peek() == 'T') { // this line defines a Trigger
        // T : [F[+F]] : [L] : [R[+R]]
        // T indicates that this line in the map file is a trigger
        // F is a food, more then one can be listed speperated by '+' (i.e.
        // [2+3])
        // L is the level below which all indicated foods must drop to trigger
        // event
        // R is a rule to apply when tigger occures, more then one can be listed
        // sperated by '+' (i.e. [S10+Q])

        if (AbstractWorld::debugPL->get()) {
          std::cout << "  While loading map found trigger definition: "
                    << rawLine << std::endl;
        }
        // convert line from file to list of std::strings
        std::string cleanLine;
        for (auto c :
             rawLine) { // remove spaces for rawLine to create cleanLine
          if (c != ' ') {
            cleanLine.push_back(c);
          }
        }
        std::vector<std::string> splitLine(1);
        for (auto c :
             cleanLine) { // make splitLine by breaking up cleanLine on ':'
          if (c == ':') {
            splitLine.push_back("");
          } else {
            splitLine.back().push_back(c);
          }
        }

        // load values into this maps triggerFoods, triggerFoodLevels and,
        // triggerFoodEvents
        std::vector<std::string> tempList;
        double tempValue;
        convertCSVListToVector_BERRY(splitLine[1], tempList);
        for (auto e :
             tempList) { // collect foods for this trigger in a std::vector
          std::vector<int> foodsInThisTrigger;
          convertCSVListToVector_BERRY(e, foodsInThisTrigger, '+');
          triggerFoods.push_back(foodsInThisTrigger);
        }
        convertCSVListToVector_BERRY(splitLine[2], tempList);
        for (auto e : tempList) {
          load_value(e, tempValue);
          triggerFoodLevels.push_back(tempValue);
        }

        // get the rules (these may have ','s in them and '[]'s ... am I mad?!
        auto tempString = splitLine[3];
        if (tempString[0] == '[') {
          tempString = tempString.substr(
              1, tempString.size() -
                     2); // strip off leading and trailing square brackets
        }
        if (tempString.size() > 0) {
          triggerFoodEvents.push_back("");
        }
        bool inElement = false;
        for (auto c : tempString) {
          if (c == ',' && !inElement) {
            triggerFoodEvents.push_back("");
          } else {
            if (c == '[') {
              inElement = true;
            }
            if (c == ']') {
              inElement = false;
            }
            triggerFoodEvents.back().push_back(c);
          }
        }                                        // end get trigger rules
        atEOF = readFileLine(FILE, rawLine, ss); // read next line of file
      }
      if (ss.peek() ==
          '*') { // this checks that we read to the end of the map definition
        goodRead = true;
      } else {
        std::cout << "  While loading map " << name << " from file: " << file
                  << ", a problem was encountered. possibly, starting "
                     "locations size is larger then map size (or it's "
                     "something else...)\nexiting"
                  << std::endl;
        exit(1);
      }
    }
  }
  data.reset(worldX, worldY);
  data.assign(rawMap);
  if (useStartMap) {
    startData.reset(worldX, worldY);
    startData.assign(rawStartMap);
  }
  return goodRead;
}

BerryWorld::BerryWorld(std::shared_ptr<ParametersTable> PT_)
    : AbstractWorld(PT_) {

  std::cout << "Berry world setup:" << std::endl;

  ////////////////////////////////////////////////////////
  // localize values /////////////////////////////////////
  ////////////////////////////////////////////////////////

  convertCSVListToVector_BERRY(initialFoodDistributionPL->get(PT),
                         initialFoodDistribution);
  convertCSVListToVector_BERRY(validStartConfigurationsPL->get(PT),
                         validStartConfigurations);
  moveDefault = moveDefaultPL->get(PT);
  moveMin = moveMinPL->get(PT);
  moveMinPerTurn = moveMinPerTurnPL->get(PT);
  snapToGrid = snapToGridPL->get(PT);
  worldX = worldSizeXPL->get(PT);
  worldY = worldSizeYPL->get(PT);
  worldHasWall = worldHasWallPL->get(PT);
  alwaysEat = alwaysEatPL->get(PT);
  allowMoveAndEat = allowMoveAndEatPL->get(PT) || alwaysEat;

  allowTurn = allowTurnPL->get(PT);
  allowSidestep = allowSidestepPL->get(PT);
  allowBackstep = allowBackstepPL->get(PT);

  usePerfectSensor = usePerfectSensorPL->get(PT);
  useDownSensor = useDownSensorPL->get(PT);

  seeFood = seeFoodPL->get(PT);
  smellFood = smellFoodPL->get(PT);
  perfectDetectsFood = perfectDetectsFoodPL->get(PT);
  seeOther = seeOtherPL->get(PT);
  smellOther = smellOtherPL->get(PT);
  perfectDetectsOther = perfectDetectsOtherPL->get(PT);
  seeWalls = seeWallsPL->get(PT);
  smellWalls = smellWallsPL->get(PT);
  perfectDetectsWalls = perfectDetectsWallsPL->get(PT);

  std::vector<std::string> tempList;
  double tempValue;
  convertCSVListToVector_BERRY(triggerFoodsPL->get(PT), tempList);
  for (auto e : tempList) { // collect foods for this trigger in a std::vector
    std::vector<int> foodsInThisTrigger;
    convertCSVListToVector_BERRY(e, foodsInThisTrigger, '+');
    triggerFoods.push_back(foodsInThisTrigger);
  }
  convertCSVListToVector_BERRY(triggerFoodLevelsPL->get(PT), tempList);
  for (auto e : tempList) {
    load_value(e, tempValue);
    triggerFoodLevels.push_back(tempValue);
  }

  // get the rules (these may have ','s in them and '[]'s ... am I mad?!
  auto tempString = triggerFoodEventsPL->get(PT);
  if (tempString[0] == '[') {
    tempString = tempString.substr(
        1, tempString.size() -
               2); // strip off leading and trailing square brackets
  }

  bool inElement = false;
  if (tempString.size() > 0) {
    triggerFoodEvents.push_back("");
  }
  for (auto c : tempString) {
    if (c == ',' && !inElement) {
      triggerFoodEvents.push_back("");
    } else {
      if (c == '[') {
        inElement = true;
      }
      if (c == ']') {
        inElement = false;
      }
      triggerFoodEvents.back().push_back(c);
    }
  } // end get trigger rules
  // backup triggers from config files. if using random maps we won't need
  // these, but if useing file maps, these will allow resets
  configTriggerFoods = triggerFoods;
  configTriggerFoodLevels = triggerFoodLevels;
  configTriggerFoodEvents = triggerFoodEvents;

  if (cloneScoreRulePL->get(PT) == "ALL") {
    cloneScoreRule = 0;
  } else if (cloneScoreRulePL->get(PT) == "BEST") {
    cloneScoreRule = 1;
  } else if (cloneScoreRulePL->get(PT) == "WORST") {
    cloneScoreRule = 2;
  } else {
    std::cout
        << "    cloneScoreRule \"" << cloneScoreRulePL->get(PT)
        << "\" is not a valid rule - must be ALL, BEST or WORST.\n    exiting."
        << std::endl;
    exit(1);
  }

  if (groupScoreRulePL->get(PT) == "SOLO") {
    groupScoreRule = -1;
  } else if (groupScoreRulePL->get(PT) == "ALL") {
    groupScoreRule = 0;
  } else if (groupScoreRulePL->get(PT) == "BEST") {
    groupScoreRule = 1;
  } else if (groupScoreRulePL->get(PT) == "WORST") {
    groupScoreRule = 2;
  } else {
    std::cout << "    groupScoreRule \"" << groupScoreRulePL->get(PT)
              << "\" is not a valid rule - must be SOLO, ALL, BEST or WORST.\n "
                 "   exiting."
              << std::endl;
    exit(1);
  }

  ////////////////////////////////////////////////////////
  // setup food and poison ///////////////////////////////
  ////////////////////////////////////////////////////////
  foodRewards = {0,
                 reward1PL->get(PT),
                 reward2PL->get(PT),
                 reward3PL->get(PT),
                 reward4PL->get(PT),
                 reward5PL->get(PT),
                 reward6PL->get(PT),
                 reward7PL->get(PT),
                 reward8PL->get(PT)};

  foodTypes = foodTypesPL->get(PT);
  replaceRules.resize(9);
  replaceRules[0] = {0};
  convertCSVListToVector_BERRY(replace1PL->get(PT), replaceRules[1]);
  convertCSVListToVector_BERRY(replace2PL->get(PT), replaceRules[2]);
  convertCSVListToVector_BERRY(replace3PL->get(PT), replaceRules[3]);
  convertCSVListToVector_BERRY(replace4PL->get(PT), replaceRules[4]);
  convertCSVListToVector_BERRY(replace5PL->get(PT), replaceRules[5]);
  convertCSVListToVector_BERRY(replace6PL->get(PT), replaceRules[6]);
  convertCSVListToVector_BERRY(replace7PL->get(PT), replaceRules[7]);
  convertCSVListToVector_BERRY(replace8PL->get(PT), replaceRules[8]);

  for (int i = 0; i <= foodTypes; i++) {
    for (int repVal : replaceRules[i]) {
      if (repVal > foodTypes && repVal != 9) {
        std::cout << "    replacement rule for food" << i
                  << " includes values > foodTypes.\n    exiting." << std::endl;
        exit(1);
      }
    }
  }

  poisonRules = {poison0PL->get(PT), poison1PL->get(PT), poison2PL->get(PT),
                 poison3PL->get(PT), poison4PL->get(PT), poison5PL->get(PT),
                 poison6PL->get(PT), poison7PL->get(PT), poison8PL->get(PT)};

  ////////////////////////////////////////////////////////
  // setup vision ////////////////////////////////////////
  ////////////////////////////////////////////////////////
  rotationResolution = rotationResolutionPL->get(PT);
  maxTurn = maxTurnPL->get(PT);

  // vistion and smell sensor arcs are defines with lists which contian
  // directions
  // if < 1 then, this value indicates a direction = rotationResolution * value
  // if >= then this value defines the direction directly

  visionSensorDistance = visionSensorDistancePL->get(PT);
  visionSensorArcSize = visionSensorArcSizePL->get(PT);
  if (useVisionSensorPL->get(PT)) {
    std::vector<double> tempVec;
    convertCSVListToVector_BERRY(visionSensorDirectionsPL->get(PT), tempVec);
    for (int i = 0; i < tempVec.size(); i++) {
      visionSensorDirections.push_back(
          abs(tempVec[i]) < 1 ? (int)(tempVec[i] * rotationResolution)
                              : (int)tempVec[i]);
    }
  }
  visionSensorCount = (int)visionSensorDirections.size();

  smellSensorDistance = smellSensorDistancePL->get(PT);
  smellSensorArcSize = smellSensorArcSizePL->get(PT);
  if (useSmellSensorPL->get(PT)) {
    std::vector<double> tempVec;
    convertCSVListToVector_BERRY(smellSensorDirectionsPL->get(PT), tempVec);
    for (int i = 0; i < tempVec.size(); i++) {
      smellSensorDirections.push_back(
          abs(tempVec[i] < 1) ? (int)(tempVec[i] * rotationResolution)
                              : (int)tempVec[i]);
    }
  }
  smellSensorCount = (int)smellSensorDirections.size();

  wallsBlockVisonSensors = wallsBlockVisonSensorsPL->get(PT) ? WALL : -1;
  wallsBlockSmellSensors = wallsBlockSmellSensorsPL->get(PT) ? WALL : -1;
  if (useDownSensorPL->get(PT)) {
    std::cout << "  using Down Sensor." << std::endl;
  }
  if (visionSensorCount > 0) {
    std::cout << "  using Vision Sensor: (arc: " << visionSensorArcSize * -.5
              << "," << visionSensorArcSize * .5 << "    distances: " << 1
              << "," << visionSensorDistance
              << "     resolution: " << rotationResolution
              << "  blocking: " << wallsBlockVisonSensors << ")" << std::endl;
    std::cout << "    sensor directions: ";
    for (auto d : visionSensorDirections) {
      std::cout << d << " ";
    }
    std::cout << std::endl;
    Sensor newVisionSensor(visionSensorArcSize * -.5, visionSensorArcSize * .5,
                           visionSensorDistance, 1, rotationResolution,
                           wallsBlockVisonSensors);
    visionSensor = newVisionSensor;
  }
  if (smellSensorCount > 0) {
    std::cout << "  using Smell Sensor:  (arc: " << smellSensorArcSize * -.5
              << "," << smellSensorArcSize * .5 << "    distances: " << 1 << ","
              << smellSensorDistance
              << "     resolution: " << rotationResolution
              << "  blocking: " << wallsBlockSmellSensors << ")" << std::endl;
    std::cout << "    sensor directions: ";
    for (auto d : smellSensorDirections) {
      std::cout << d << " ";
    }
    std::cout << std::endl;
    Sensor newSmellSensor(smellSensorArcSize * -.5, smellSensorArcSize * .5,
                          smellSensorDistance, 1, rotationResolution,
                          wallsBlockSmellSensors);
    smellSensor = newSmellSensor;
  }

  int perfectSensorCount = 0;
  ///// load perfect sensor
  if (usePerfectSensorPL->get(PT) &&
      (rotationResolution == 8 || rotationResolution == 4)) {
    std::ifstream FILE(perfectSensorFilePL->get(PT));
    if (!FILE.is_open()) {
      std::cout
          << "  while trying to load perfect sensor, failed to open file: "
          << perfectSensorFilePL->get(PT) << "\n  exiting." << std::endl;
      exit(1);
    }

    auto rawGrid = loadSquareGridOfIntsFromFile(FILE);
    if (rawGrid.x() != rawGrid.x()) {
      std::cout << "  while trying to load perfect sensor, found non-square "
                   "definition in file: "
                << perfectSensorFilePL->get(PT) << "\n  exiting." << std::endl;
      exit(1);
    }
    int halfSize = rawGrid.x() / 2;
    // convert sensor values so that they start at 0 and load this into
    // perfectSensorSites[0] (where 0 is up)
    perfectSensorSites.push_back({});
    std::map<char, int> lookup;
    int count = 0;
    for (int x = 0; x < rawGrid.x(); x++) {
      for (int y = 0; y < rawGrid.y(); y++) {
        if (rawGrid(x, y) != '^' && rawGrid(x, y) != '.') {
          if (lookup.find(rawGrid(x, y)) == lookup.end()) {
            lookup[rawGrid(x, y)] = count++;
            perfectSensorSites[0].push_back({});
            perfectSensorCount++;
          }
          perfectSensorSites[0][lookup[rawGrid(x, y)]].push_back(
              Point2d(x - halfSize, y - halfSize));
        }
      }
    }

    // now preform transformations to create 4 or 8 directions.
    if (rotationResolution == 8) {
      // set perfectSensorSites[1] to rotation 1 (45 degrees right)
      perfectSensorSites.push_back({});
      for (int line = 0; line < perfectSensorSites[0].size(); line++) {
        perfectSensorSites[1].push_back({});
        for (auto site : perfectSensorSites[0][line]) {
          if (site.x == 0) {
            perfectSensorSites[1][line].push_back(Point2d(-1 * site.y, site.y));
          }
          if (site.y == 0) {
            perfectSensorSites[1][line].push_back(Point2d(site.x, site.x));
          }
          if ((site.y < 0 && site.x < 0) || (site.y > 0 && site.x > 0)) {
            if (abs(site.x) <= abs(site.y)) {
              perfectSensorSites[1][line].push_back(
                  Point2d(site.x - site.y, site.y));
            } else {
              perfectSensorSites[1][line].push_back(
                  Point2d(site.x - site.y, site.x));
            }
          }
          if ((site.y > 0 && site.x < 0) || (site.y < 0 && site.x > 0)) {
            if (abs(site.x) <= abs(site.y)) {
              perfectSensorSites[1][line].push_back(
                  Point2d(-1 * site.y, site.y + site.x));
            } else {
              perfectSensorSites[1][line].push_back(
                  Point2d(site.x, site.y + site.x));
            }
          }
        }
      }
    }
    fillInVision(perfectSensorSites, false);
    std::cout << "  using Perfect Sensor. Perfect Sensor has "
              << perfectSensorSites[0].size() << " inputs." << std::endl;

  } else if (usePerfectSensorPL->get(PT)) {
    std::cout << "  perfect sensor requires directions to be 4 or 8, but "
                 "directions is set to: "
              << rotationResolution << "\n  exiting." << std::endl;
    exit(1);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  setup move offsets (if agent is facing in a given direction and they move,
  //  how much should x and y change by? //
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  moveDeltas.resize(rotationResolution);
  for (int i = 0; i < rotationResolution; i++) {
    int tempi = ((rotationResolution - i) + (rotationResolution / 2)) %
                rotationResolution;
    moveDeltas[i] = {
        sin(tempi * (360 / (double)rotationResolution) * (3.14159 / 180.0)),
        cos(tempi * (360 / (double)rotationResolution) * (3.14159 / 180.0))};
    moveDeltas[i].x = ((double)((int)(moveDeltas[i].x * 10000))) / 10000;
    moveDeltas[i].y = ((double)((int)(moveDeltas[i].y * 10000))) / 10000;

    if (AbstractWorld::debugPL->get(PT)) {
      std::cout << "   " << i << "    move deltas     " << moveDeltas[i].x
                << "," << moveDeltas[i].y << std::endl;
      if (visionSensorCount > 0) {
        visionSensor.angles[i]->drawArc();
      }
      if (smellSensorCount > 0) {
        smellSensor.angles[i]->drawArc();
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////
  //  LOAD MAPS FROM FILES
  //  ///////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////
  auto alwaysStartOn = alwaysStartOnPL->get(PT);
  convertCSVListToVector_BERRY(mapFilesPL->get(PT), mapFiles);

  if (mapFiles.size() == 0) {
    mapFiles.push_back("NONE");
  }

  if (mapFiles[0] != "NONE") {
    std::cout << "Berry World loading maps..." << std::endl;
    for (auto mapFile : mapFiles) {
      std::ifstream FILE(mapFile);
      if (!FILE.is_open()) {
        std::cout << "  failed to open file: " << mapFile << "\n  exiting."
                  << std::endl;
        exit(1);
      }
      bool goodRead = false;
      while (!FILE.eof()) {
        WorldMap newMap;
        goodRead = newMap.loadMap(FILE, mapFile);
        if (goodRead && newMap.name != "XXXXXXXXXXXXXXXXXXXX") {
          // set up valid locations
          if (newMap.useStartMap || validStartConfigurations[0] == -1 ||
              alwaysStartOn != -1) {
            // this map is providing starts or there are no
            // validStartConfigurations (i.e. we need to make them up) or there
            // is an always start on defined
            for (int x = 0; x < newMap.worldX;
                 x++) { // for every location in the map
              for (int y = 0; y < newMap.worldY; y++) {
                Point2d loc(x, y);
                int valueHere;
                if (newMap.useStartMap) { // if a start map was provied with
                                          // this map
                  valueHere = newMap.startData(loc) > 0
                                  ? alwaysStartOn
                                  : WALL; // if value in start is > then make
                                          // value here the always start on
                                          // value,
                  // or (if this is not a start on location) make value here a
                  // wall
                  //(to block this as a start location)
                } else {
                  valueHere = (int)newMap.data(loc);
                }
                if ((alwaysStartOn == -1 && valueHere < WALL) ||
                    valueHere == alwaysStartOn) { // if start anywhere and here
                                                  // is not a wall or here is a
                                                  // valid start location
                  newMap.startLocations.push_back(
                      loc); // if random start locations (-1) or food at this
                            // space matches alwaysStartOn, add to validSpaces
                  if (newMap.useStartMap) {
                    newMap.startFacing.push_back(
                        (int)newMap.startData(loc)); // if using start map, get
                                                     // location from start map
                  } else {
                    newMap.startFacing.push_back(1); // otherwise put a 1 in the
                                                     // list (random direction
                                                     // will be generated when
                                                     // harverster is placed)
                  }
                }
                if ((int)newMap.data(loc) > foodTypes && valueHere != 9 &&
                    valueHere != -1) {
                  std::cout
                      << "  map " << newMap.name << " from file: " << mapFile
                      << " contains foodype not currently in use.\nexiting."
                      << std::endl;
                  exit(1);
                }
              }
            }
          } // end set up valid starts when not using validStartConfigurations
          else { // this map is not providing starts and there are
                 // validStartConfigurations
            for (int i = 0; i < validStartConfigurations.size(); i += 3) {
              newMap.startLocations.push_back(
                  Point2d(validStartConfigurations[i],
                          validStartConfigurations[i + 1]));
              newMap.startFacing.push_back(validStartConfigurations[i + 2]);
            }
          }
          worldMaps[mapFile][newMap.name] = newMap;
          mapNames[mapFile].push_back(newMap.name);
          std::cout << "    loaded map: " << newMap.name
                    << " from file: " << mapFile << std::endl;
        } else {
          if (newMap.name != "XXXXXXXXXXXXXXXXXXXX") {
            std::cout << "  While loading map " << newMap.name
                      << " from file: " << mapFile
                      << ", a problem was encountered.\nexiting" << std::endl;
            exit(1);
          }
        }
      }
      FILE.close();
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////
  //  get map usage
  //  //////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////
  std::vector<std::string> temp;
  if (mapFiles[0] != "NONE") {
    convertCSVListToVector_BERRY(whichMapsPL->get(PT), temp);
    std::cout << "    found the following whichMaps values:" << std::endl;
    for (auto v : temp) {
      std::string delimiter = "/";
      whichMaps.push_back(v.substr(0, v.find(delimiter)));
      whichMaps.push_back(v.substr(v.find(delimiter) + 1, v.size()));
      std::cout << "      file: " << whichMaps[whichMaps.size() - 2]
                << "  map: " << whichMaps.back() << std::endl;
    }
  }
  /////////////////////////////////////////////////////////////////////////////////////
  //  set Required inputs and Required outputs
  //  ////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////
  if (foodTypesPL->get(PT) < 1 || foodTypesPL->get(PT) > 8) {
    std::cout << "In BerryWorld you either have too few or too many foodTypes "
                 "(must be >0 and <=8)\n\nExiting\n\n";
    exit(1);
  }

  // if only turn or only sidestep then 2 moveOutputs are needed
  // if turn and sidestep or any combo with move back then 3 moveOutputs are
  // needed
  // actions are maped to outputs with actionLookup:
  //  0 = no action
  //  1 = move forward
  //  2 = move back
  //  3 = turn left
  //  4 = turn right
  //  5 = step left
  //  6 = step right
  // if !alwaysEat then requiredOutputs + 1 (if this output is on harvester will
  // try to eat)

  if (allowTurn && !allowSidestep && !allowBackstep) {
    requiredOutputs = 2; // 01:turnleft,10:turnright,11:forward
    moveOutputs = 2;
    actionLookup[0] = 0;
    actionLookup[1] = 3; // 01 turn right
    actionLookup[2] = 4; // 10 turn left
    actionLookup[3] = 1; // 11 move forward
  } else if (!allowTurn && allowSidestep && !allowBackstep) {
    requiredOutputs = 2; // 01:stepleft,10:stepright,11:forward
    moveOutputs = 2;
    actionLookup[0] = 0;
    actionLookup[1] = 5; // 01 step right
    actionLookup[2] = 6; // 10 step left
    actionLookup[3] = 1; // 11 move forward
  } else if (allowTurn && allowSidestep && !allowBackstep) {
    requiredOutputs = 3;
    moveOutputs = 3;
    actionLookup[0] = 0;
    actionLookup[1] = 3; // 001 turn right
    actionLookup[2] = 4; // 010 turn left
    actionLookup[3] = 1; // 011 move forward
    actionLookup[4] = 0;
    actionLookup[5] = 5; // 101 step right
    actionLookup[6] = 6; // 110 step left
    actionLookup[7] = 0;
  } else if (allowTurn && !allowSidestep && allowBackstep) {
    requiredOutputs = 3;
    moveOutputs = 3;
    actionLookup[0] = 0;
    actionLookup[1] = 3; // 001 turn right
    actionLookup[2] = 4; // 010 turn left
    actionLookup[3] = 1; // 011 move forward
    actionLookup[4] = 0;
    actionLookup[5] = 0;
    actionLookup[6] = 0;
    actionLookup[7] = 2; // 111 move back
  } else if (!allowTurn && allowSidestep && allowBackstep) {
    requiredOutputs = 3;
    moveOutputs = 3;
    actionLookup[0] = 0;
    actionLookup[1] = 5; // 001 step right
    actionLookup[2] = 6; // 010 step left
    actionLookup[3] = 1; // 011 move forward
    actionLookup[4] = 0;
    actionLookup[5] = 0;
    actionLookup[6] = 0;
    actionLookup[7] = 2; // 111 move back
  } else if (allowTurn && allowSidestep && allowBackstep) {
    requiredOutputs = 3;
    moveOutputs = 3;
    actionLookup[0] = 0;
    actionLookup[1] = 2; // 001 turn right
    actionLookup[2] = 3; // 010 turn left
    actionLookup[3] = 1; // 011 move forward
    actionLookup[4] = 0;
    actionLookup[5] = 5; // 101 step right
    actionLookup[6] = 6; // 110 step left
    actionLookup[7] = 2; // 111 move back
  } else {
    std::cout << "both allowTurn and allowSidestep are false, atleast one must "
                 "be true. Exiting"
              << std::endl;
    exit(1);
  }

  foodCounts.resize(10, 0); // this will hold counts on map

  if (!alwaysEat) {
    requiredOutputs += 1; // brain needs extra output for eat
  }

  requiredInputs =
      visionSensorCount * ((foodTypes * seeFood) + seeOther + seeWalls);
  requiredInputs +=
      smellSensorCount * ((foodTypes * smellFood) + smellOther + smellWalls);
  requiredInputs +=
      perfectSensorCount * ((foodTypes * perfectDetectsFood) +
                            perfectDetectsOther + perfectDetectsWalls);
  requiredInputs +=
      useDownSensorPL->get(PT) * foodTypes; // down sensor (just food)
  std::cout << "  BerryWorld (with current settings) requires organisms with "
               "a brain ("
            << brainNameSpacePL->get(PT) << ") with " << requiredInputs
            << " inputs and " << requiredOutputs << " outputs.\n";
  /////////////////////////////////////////////////////////////////////////////////////
  //  set up pop file columns
  //  /////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////
  popFileColumns.clear();
  popFileColumns.push_back("score");
  // popFileColumns.push_back("score_VAR"); // specifies to also record the
  // variance (performed automatically by DataMap because _VAR)
  popFileColumns.push_back("switches");
  popFileColumns.push_back("consumptionRatio");
  for (int f = 1; f <= foodTypes; f++) {
    popFileColumns.push_back("food" + std::to_string(f));
  }
  for (int f = 0; f <= foodTypes; f++) {
    if (poisonRules[f] != 0) {
      popFileColumns.push_back("poison" + std::to_string(f));
    }
  }
}

void BerryWorld::evaluate(std::map<std::string, std::shared_ptr<Group>> &groups,
                          int analyse, int visualize, int debug) {
  // call runWorld evaluations per generation times
  for (int i = 0; i < evaluationsPerGenerationPL->get(PT); i++) {
    runWorld(groups, analyse, visualize, debug);
  }
  if (visualize) { // save state of world before we get started.
    FileManager::writeToFile("HarvestWorldData.txt", "EOF");
  }
}

void BerryWorld::runWorld(std::map<std::string, std::shared_ptr<Group>> &groups,
                          int analyse, int visualize, int debug) {
  auto tempPopulation =
      groups[groupNameSpacePL->get(PT)]->population; // make a copy of the
                                                     // population so we can
                                                     // pull unique organisms
                                                     // when making evaluation
                                                     // groups
  auto populationSize = tempPopulation.size();
  auto groupSize = evaluateGroupSizePL->get(PT);

  int numberOfEvalGroups = ceil(((double)populationSize) / ((double)groupSize));
  std::vector<std::vector<std::shared_ptr<Organism>>> evalGroups;

  // eval groups are generated each with evalGroupSize organisms
  // Some organisms may be evaluated twice (if populaiton is not divisable by
  // evalGroupSize)

  if (groupSize > populationSize) {
    std::cout << "In BerryWorld:: groupSize > populationSize! This is now "
                 "allowed.\n  Please provide a larger population or reduce "
                 "groupSize.\n  If you want to force clones in eval groups, "
                 "use clones parameter.\n  exiting."
              << std::endl;
    exit(1);
  }
  for (int p = 0; p < numberOfEvalGroups; p++) { // for each eval group
    std::vector<std::shared_ptr<Organism>>
        thisGroup; // create a temporary holder for this
    if (tempPopulation.size() > groupSize) { // if there are enough organisms in
                                             // tempPopulation for this group
      for (int o = 0; o < groupSize; o++) {  // pull evalGroupSize organisms
        auto pick =
            Random::getIndex(tempPopulation.size()); // get a random index
        thisGroup.push_back(
            tempPopulation[pick]); // place org at index into thisGroup
        tempPopulation[pick] =
            tempPopulation
                .back(); // copy last org in tempPopulation to index location
        tempPopulation.pop_back(); // remove unneeded copy
      }
    } else { // tempPopulation.size() is < evalGroupSize, some orgs need to be
             // evaluateded twice (i.e. be in more then one eval group)
      thisGroup = tempPopulation; // place the rest of the orgs in temp
                                  // population into thisGroup
      tempPopulation = groups[groupNameSpacePL->get(PT)]
                           ->population; // refresh tempPopulation

      for (auto o : thisGroup) {
        bool foundCopy = false;
        int oo = 0;
        while (!foundCopy) {
          if (o->ID == tempPopulation[oo]->ID) {
            tempPopulation[oo] = tempPopulation.back();
            tempPopulation.pop_back();
            foundCopy = true;
          }
          oo++;
        }
      }

      // fill in any remaining spaces randomly...
      for (int o = thisGroup.size(); o < groupSize;
           o++) { // fill in the rest of thisGroup
        auto pick =
            Random::getIndex(tempPopulation.size()); // get a random index
        thisGroup.push_back(
            tempPopulation[pick]); // place org at index into thisGroup
        tempPopulation[pick] =
            tempPopulation
                .back(); // copy last org in tempPopulation to index location
        tempPopulation.pop_back(); // remove unneeded copy
      }
    }
    evalGroups.push_back(thisGroup); // add thisGroup to evalGroups
  }                                  // end create evalGroups for loop

  if (debug) { // display which organisms are in which evaluation group
    std::cout << "populationSize:" << populationSize
              << "  groupSize:" << groupSize
              << "  number of groups:" << numberOfEvalGroups << std::endl;
    for (int gID = 0; gID < evalGroups.size(); gID++) {
      std::cout << "group " << gID << " contains: ";
      for (auto o : evalGroups[gID]) {
        std::cout << o->ID << " ";
      }
      std::cout << std::endl;
    }
  }

  // localize some values. This makes using these values easier.
  auto evalTime = evalTimePL->get(PT);
  auto alwaysStartOn = alwaysStartOnPL->get(PT);

  Vector2d<int> foodMap; // what food is here?

  std::vector<Point2d> validSpaces;
  std::vector<int> startFacing;

  std::vector<std::string> whichMapsActual; // because whichMaps contains wild
                                            // cards (* and #) whichMapsActual
                                            // will be filled in once per
                                            // generation to determin the actual
                                            // maps for this generation

  if (mapFiles[0] != "NONE" &&
      whichMaps.size() > 0) { // if there are defined maps
    // make a list of file names for each entry in whichMaps
    for (int i = 0; i < whichMaps.size(); i += 2) {
      std::vector<std::string> FILENAMES;
      if (whichMaps[i] == "*") { // if the file name provided in a *, then we
                                 // will be pulling from all files
        FILENAMES = mapFiles;
      } else if (isdigit(whichMaps[i][0])) { // if the file name provided in a
                                             // number, then pick that number of
                                             // random file names
        int numPicks;
        load_value(whichMaps[i], numPicks);
        int avalibleFiles = mapFiles.size();
        if (numPicks > avalibleFiles) {
          std::cout << "  In Berry world, while selecting maps number of files "
                       "requested is > number of map files.\n  exiting."
                    << std::endl;
          exit(1);
        }
        auto picks = pickUnique(avalibleFiles, numPicks);
        for (auto pick : picks) {
          FILENAMES.push_back(mapFiles[pick]);
        }
      } else if (find(mapFiles.begin(), mapFiles.end(), whichMaps[i]) !=
                 mapFiles.end()) { // if the file names provided is a name, make
                                   // sure that name is in file list and then
                                   // pick form that file
        FILENAMES.push_back(whichMaps[i]);
      } else { // FILE ERROR
        std::cout
            << "    In Berry world, while selecting maps was not able resolve: "
            << whichMaps[i] << "/" << whichMaps[i + 1] << ".\n    exiting."
            << std::endl;
        exit(1);
      }
      // now that we have the files names for this entry, figure out which maps
      // from those files
      for (auto FILENAME : FILENAMES) {
        if (whichMaps[i + 1] ==
            "*") { // if the map name is * then include all maps from FILENAME
          for (auto mapName : mapNames[FILENAME]) {
            whichMapsActual.push_back(FILENAME);
            whichMapsActual.push_back(mapName);
          }
        } else if (isdigit(whichMaps[i + 1][0])) { // if the map name is a #
                                                   // then include this # of
                                                   // maps from FILENAME
          int numPicks;
          load_value(whichMaps[i + 1], numPicks);
          int avalibleMaps = mapNames[FILENAME].size();
          if (numPicks > avalibleMaps) {
            std::cout << "  In Berry world, while selecting maps from file "
                      << FILENAME << " number of maps requested is > number of "
                                     "maps in file.\n  exiting."
                      << std::endl;
            exit(1);
          }
          auto picks = pickUnique(avalibleMaps, numPicks);
          for (auto pick : picks) {
            whichMapsActual.push_back(FILENAME);
            whichMapsActual.push_back(mapNames[FILENAME][pick]);
          }
        } else if (find(mapNames[FILENAME].begin(), mapNames[FILENAME].end(),
                        whichMaps[i + 1]) !=
                   mapNames[FILENAME].end()) { // If the map name is a name, and
                                               // that name is in FILENAME,
                                               // include that
          whichMapsActual.push_back(FILENAME);
          whichMapsActual.push_back(whichMaps[i + 1]);
        } else { // FILE * ERROR
          std::cout << "    In Berry world, while selecting maps was not able "
                       "resolve: "
                    << whichMaps[i] << "/" << whichMaps[i + 1]
                    << ".\n    exiting." << std::endl;
          exit(1);
        }
      }
    }
  } else { // if no maps were defined...
    whichMapsActual = {"NONE", "NONE"};
  }

  generators.clear();
  std::vector<WorldMap::ResourceGenerator> savedGenerators;

  // for each map in whichMapsActual, run evaluation (if NONE, make a random
  // map)
  for (int whichMapIndex = 0; whichMapIndex < (int)whichMapsActual.size();
       whichMapIndex += 2) {
    if (debug) {
      std::cout << "running: " << whichMapsActual[whichMapIndex] << "   "
                << whichMapsActual[whichMapIndex + 1] << std::endl;
    }
    if (whichMapsActual[0] != "NONE") {
      worldX = worldMaps[whichMapsActual[whichMapIndex]]
                        [whichMapsActual[whichMapIndex + 1]]
                            .worldX;
      worldY = worldMaps[whichMapsActual[whichMapIndex]]
                        [whichMapsActual[whichMapIndex + 1]]
                            .worldY;
      foodMap = worldMaps[whichMapsActual[whichMapIndex]]
                         [whichMapsActual[whichMapIndex + 1]]
                             .data;
      validSpaces = worldMaps[whichMapsActual[whichMapIndex]]
                             [whichMapsActual[whichMapIndex + 1]]
                                 .startLocations;
      startFacing = worldMaps[whichMapsActual[whichMapIndex]]
                             [whichMapsActual[whichMapIndex + 1]]
                                 .startFacing;
      generators = worldMaps[whichMapsActual[whichMapIndex]]
                            [whichMapsActual[whichMapIndex + 1]]
                                .generators;
      savedGenerators = generators;

      // combine trigger info from comfig file with map trigger info
      triggerFoods = configTriggerFoods;
      triggerFoodLevels = configTriggerFoodLevels;
      triggerFoodEvents = configTriggerFoodEvents;
      for (auto triggerFood : worldMaps[whichMapsActual[whichMapIndex]]
                                       [whichMapsActual[whichMapIndex + 1]]
                                           .triggerFoods) {
        triggerFoods.push_back(triggerFood);
      }
      for (auto triggerFoodLevel : worldMaps[whichMapsActual[whichMapIndex]]
                                            [whichMapsActual[whichMapIndex + 1]]
                                                .triggerFoodLevels) {
        triggerFoodLevels.push_back(triggerFoodLevel);
      }
      for (auto triggerFoodEvent : worldMaps[whichMapsActual[whichMapIndex]]
                                            [whichMapsActual[whichMapIndex + 1]]
                                                .triggerFoodEvents) {
        triggerFoodEvents.push_back(triggerFoodEvent);
      }
    } else { // no whichMaps were provided... generate an uninitalize map.
      foodMap.reset(worldX, worldY);
      for (int x = 0; x < worldX; x++) {
        for (int y = 0; y < worldY; y++) {
          if (worldHasWall &&
              (x == 0 || y == 0 || x == worldX - 1 || y == worldY - 1)) {
            foodMap(x, y) =
                WALL; // if this location is on the edge, make it a wall
          } else {
            auto pick = initialFoodDistribution[Random::getIndex(
                initialFoodDistribution.size())];
            if (pick == -1) { // if the value picked from distribution is -1,
                              // replace with a random food type
              pick = Random::getInt(1, foodTypes);
            }
            foodMap(x, y) = pick;
          }
        }
      }
      if (validStartConfigurations[0] != -1 &&
          alwaysStartOn ==
              -1) { // if there is are start values and no start on value
        for (int i = 0; i < validStartConfigurations.size(); i += 3) {
          std::cout << "i:" << i << std::endl;
          validSpaces.push_back(Point2d(validStartConfigurations[i],
                                        validStartConfigurations[i + 1]));
          if (validStartConfigurations[i + 2] ==
              -1) { // if random facing for this start configuration
            startFacing.push_back(1);
          } else {
            startFacing.push_back(validStartConfigurations[i + 2] + 2);
          }
        }
        std::cout << validSpaces.size();
        std::cout << startFacing.size();
      } else {
        for (int x = 0; x < worldX; x++) { // for every location in the map
          for (int y = 0; y < worldY; y++) {
            Point2d loc(x, y);
            if ((alwaysStartOn == -1 && (int)foodMap(loc) < WALL) ||
                (int)foodMap(loc) == alwaysStartOn) {
              validSpaces.push_back(loc); // if random start locations (-1) or
                                          // food at this space matches
                                          // alwaysStartOn, add to validSpaces
              startFacing.push_back(1);   // if random start locations (-1) or
                                          // food at this space matches
                                          // alwaysStartOn, add 1 (start random
                                          // facing) to startFacing vector
            }
          }
        }
      }
    }

    if (debug) {
      std::cout << "inital map:" << std::endl;
      foodMap.showGrid();
    }

    Vector2d<int> foodLastMap = foodMap; // what food what here before?
    auto foodMapCopy = foodMap;

    // count food on this map
    foodCounts.assign(10, 0);
    for (int x = 0; x < worldX; x++) { // for every location in the map
      for (int y = 0; y < worldY; y++) {
        Point2d loc(x, y);
        foodCounts[foodMap(loc)]++;
      }
    }
    auto foodCountsCopy = foodCounts;  // backup, used when map is reset
    auto foodCountsPrior = foodCounts; // this will be one update behind actual
                                       // and will be used to test if value
                                       // passed trigger

    // make sure there are enough valid starting locations
    int clones = clonesPL->get(PT);
    ;

    if (groupSize * (clones + 1) > validSpaces.size()) {
      std::cout << "  In BerryWorld: world is too small or has too few "
                   "alwaysStartOn foods\n  i.e. (WORLD_HARVEST-groupSize * "
                   "WORLD_HARVEST-clones) > valid starting locations\n  Please "
                   "correct and try again. Exitting."
                << std::endl;
      exit(1);
    }

    int moveOutput, eatOutput;
    std::string visualizeData;
    // now evaluate each evalGroup

    int evalGroupCount = 0; // used if saving a group report
    int saveCount = 0;      // used if saving a group report

    for (auto evalGroup : evalGroups) {

      foodMap = foodMapCopy;
      foodLastMap = foodMapCopy;
      foodCounts = foodCountsCopy;
      foodCountsPrior = foodCountsCopy;

      std::vector<std::shared_ptr<Harvester>> harvesters;
      auto tempValidSpaces = validSpaces; // make tempValidSpaces so we can pull
                                          // elements from it to select unque
                                          // locations.
      auto tempStartFacing = startFacing;
      // for each org in this group, create a harvester and pick a location and
      // faceing direction
      int IDCount = 0;
      for (auto org : evalGroup) {
        auto newHarvester = std::make_shared<Harvester>(); // make a new container
        newHarvester->ID = IDCount++;
        newHarvester->cloneID = newHarvester->ID;
        newHarvester->org = org; // provide access to org though harvester
        newHarvester->brain = org->brains[brainNameSpacePL->get(PT)];
        newHarvester->brain->resetBrain();
        // set inital location
        auto pick =
            Random::getIndex(tempValidSpaces.size()); // get a random index
        newHarvester->loc = tempValidSpaces[pick];    // assign location
        newHarvester->loc.x += .5; // place in center of location
        newHarvester->loc.y += .5;
        tempValidSpaces[pick] = tempValidSpaces.back(); // copy last location in
                                                        // tempValidSpaces to
                                                        // pick location
        tempValidSpaces.pop_back(); // remove last location in tempValidSpaces

        // set inital facing direction
        newHarvester->face =
            tempStartFacing[pick] == 1
                ? Random::getIndex(rotationResolution)
                : (tempStartFacing[pick] - 2) *
                      (double(rotationResolution) / 8.0); // if startFacing is 1
                                                          // then pick random, 2
                                                          // is up, 3 is up
                                                          // right, etc...
        tempStartFacing[pick] = tempStartFacing.back();
        tempStartFacing.pop_back();

        newHarvester->foodCollected.resize(foodTypes + 1);
        newHarvester->poisonTotals.resize(foodTypes + 1);

        // add to harvesters
        harvesters.push_back(newHarvester);

        // place harvester in world
        foodMap(newHarvester->loc) +=
            10; // set this location to occupied in foodMap - locations in map
                // are 0 if empty, 1->8 if food, 9 if wall, 10 if occupied with
                // no food, 11->18 if occupied with food
        if (debug) {
          std::cout << "placed ID:" << newHarvester->org->ID << " @ "
                    << newHarvester->loc.x << "," << newHarvester->loc.y << "  "
                    << newHarvester->face << std::endl;
        }
      } // end create harvesters for loop

      // now add clones to harvesters
      for (int i = 0; i < groupSize; i++) {
        for (int j = 0; j < clones; j++) {
          auto newHarvester = std::make_shared<Harvester>(); // make a new container
          newHarvester->ID = IDCount++;
          newHarvester->cloneID = harvesters[i]->ID;
          newHarvester->isClone = true;
          newHarvester->org =
              harvesters[i]->org; // provide access to org though harvester
          newHarvester->brain = harvesters[i]->brain->makeCopy();
          // set inital location
          auto pick =
              Random::getIndex(tempValidSpaces.size()); // get a random index
          newHarvester->loc = tempValidSpaces[pick];    // assign location
          newHarvester->loc.x += .5; // place in center of location
          newHarvester->loc.y += .5;
          tempValidSpaces[pick] = tempValidSpaces.back(); // copy last location
                                                          // in tempValidSpaces
                                                          // to pick location
          tempValidSpaces.pop_back(); // remove last location in tempValidSpaces

          // set inital facing direction
          newHarvester->face = tempStartFacing[pick] == 1
                                   ? Random::getIndex(rotationResolution)
                                   : (tempStartFacing[pick] - 2) *
                                         (double(rotationResolution) /
                                          8.0); // if startFacing is 1 then pick
                                                // random, 2 is up, 3 is up
                                                // right, etc...
          tempStartFacing[pick] = tempStartFacing.back();
          tempStartFacing.pop_back();

          newHarvester->foodCollected.resize(foodTypes + 1);
          newHarvester->poisonTotals.resize(foodTypes + 1);

          // add to harvesters
          harvesters.push_back(newHarvester);
          harvesters[i]->clones.push_back(newHarvester);
          // place harvester in world
          foodMap(newHarvester->loc) +=
              10; // set this location to occupied in foodMap - locations in map
                  // are 0 if empty, 1->8 if food, 9 if wall, 10 if occupied
                  // with no food, 11->18 if occupied with food
          if (debug) {
            std::cout << "placed clone of ID:" << newHarvester->org->ID << " @ "
                      << newHarvester->loc.x << "," << newHarvester->loc.y
                      << "  " << newHarvester->face << std::endl;
          }
        }
      }

      if (visualize) { // save state inital world and Harvester locations
        visualizeData = "**InitializeWorld**\n";
        visualizeData += std::to_string(rotationResolution) + "," +
                         std::to_string(worldX) + "," + std::to_string(worldY) + "," +
                         std::to_string(groupSize + (groupSize * clones)) + "\n";
        // save the map
        for (int y = 0; y < worldY; y++) {
          for (int x = 0; x < worldX; x++) {
            visualizeData += std::to_string(foodMap(x, y) % 10);
            if (x % worldX == worldX - 1) {
              visualizeData += "\n";
            } else {
              visualizeData += ",";
            }
          }
        }
        visualizeData += "-\n**InitializeHarvesters**\n";
        for (auto harvester : harvesters) {
          visualizeData += std::to_string(harvester->ID) + "," +
                           std::to_string(harvester->loc.x) + "," +
                           std::to_string(harvester->loc.y) + "," +
                           std::to_string(harvester->face) + "," +
                           std::to_string(harvester->cloneID) + "\n";
        }
        visualizeData += "-";
        FileManager::writeToFile("HarvestWorldData.txt", visualizeData);
      }

      // init generatorEvents
      generators.clear();
      for (auto g : savedGenerators) {
        generators.push_back(g);
      }

      generatorEvents.clear();
      for (int i = 0; i < (int)generators.size(); i++) {
        // for each generator, find out next time that generator will fire and
        // add that to generatorEvents
        // generatorEvents[time][generatorIndex]
        generatorEvents[generators[i].nextEvent()].push_back(i);
      }

      // run evaluation
      for (int t = 0; t < evalTime; t++) {
        if (visualize) {
          visualizeData = "U," + std::to_string(t) + "\n";
        }
        // check to see if there is any inflow
        if (generatorEvents.find(t) !=
            generatorEvents
                .end()) { // if there are generator events at this time?
          while (generatorEvents[t].size() > 0) {
            int genIndex =
                generatorEvents[t].back(); // get id of last generator in list
            generatorEvents[t].pop_back();
            Point2d genLoc = generators[genIndex].getLocation();
            auto replacement = generators[genIndex].getNextResource(
                foodMap((int)genLoc.x, (int)genLoc.y));
            if (replacement >= 0) {
              foodCounts[foodMap(genLoc)]--;
              foodMap(genLoc) = replacement;
              foodCounts[foodMap(genLoc)]++;
              if (visualize) {
                visualizeData += "I," + std::to_string((int)genLoc.x) + "," +
                                 std::to_string((int)genLoc.y) + "," +
                                 std::to_string(replacement) + "\n";
              }
            }
            int nextT = generators[genIndex].nextEvent() + t;
            // std::cout << std::to_string((int)genLoc.x) << "," <<
            // std::to_string((int)genLoc.y) << "   t  = " << t << "    " << nextT <<
            // std::endl;
            generatorEvents[nextT].push_back(genIndex);
          }
        }

        auto tempHarvesters =
            harvesters; // make a copy of the current harverster group
        std::shared_ptr<Harvester>
            harvester; // this will point to the harvester being updated
        while (tempHarvesters.size() > 0) {
          auto pick =
              Random::getIndex(tempHarvesters.size()); // get a random index
          harvester = tempHarvesters[pick];
          tempHarvesters[pick] =
              tempHarvesters
                  .back(); // move last in tempHarvesters to pick location
          tempHarvesters.pop_back(); // remove last harvester in tempHarvesters

          // get a pointer to the brain. if clone, then use harvester local
          // brain
          std::shared_ptr<AbstractBrain> brain;
          brain = harvester->brain;

          double localTime = 0; // localTime is used to make turns cheaper. If
                                // all actions cost 1 world update then turns
                                // are discuraged.
          // in each world update organisms have some number of localTime
          // updates... turns cost one localUpdate, but move costs more.
          // the number of localTime actions avalible is set by the maxTurn
          // parameter
          double turnCost = 1.0 / (rotationResolution * maxTurn);
          while (localTime < 1.0) {
            /////////////////
            // set the inputs
            /////////////////
            int inputCounter =
                0; // This counter is used while setting brain inputs

            std::vector<int> sensorValues(19);

            // for each sensor, collect data and set inputs
            int sensorFacing;

            int locX = (int)harvester->loc.x;
            int locY = (int)harvester->loc.y;

            for (int i = 0; i < visionSensorCount;
                 i++) { // set inputs for vision sensors
              sensorFacing =
                  loopMod(harvester->face + visionSensorDirections[i],
                          rotationResolution);
              if (wallsBlockVisonSensors) {
                visionSensor.senseTotals(
                    foodMap, locX, locY, sensorFacing, sensorValues, WALL,
                    true); // load what sensor sees into sensorValues
              } else {
                visionSensor.senseTotals(
                    foodMap, locX, locY, sensorFacing, sensorValues, -1,
                    true); // load what sensor sees into sensorValues
              }
              if (seeFood) {
                for (int food = 1; food <= foodTypes; food++) {
                  brain->setInput(inputCounter++,
                                  sensorValues[food] + sensorValues[food + 10]);
                }
              }
              if (seeOther) {
                int others = 0;
                for (int val = 10; val < 19; val++) {
                  others += sensorValues[val];
                }
                brain->setInput(inputCounter++, others); // set occupied
              }
              if (seeWalls) {
                brain->setInput(inputCounter++, sensorValues[WALL]); // set wall
              }
            }

            for (int i = 0; i < smellSensorCount;
                 i++) { // set inputs for smell sensors
              sensorFacing = loopMod(harvester->face + smellSensorDirections[i],
                                     rotationResolution);
              if (wallsBlockSmellSensors) {
                smellSensor.senseTotals(
                    foodMap, locX, locY, sensorFacing, sensorValues, WALL,
                    true); // load what sensor sees into sensorValues
              } else {
                smellSensor.senseTotals(
                    foodMap, locX, locY, sensorFacing, sensorValues, -1,
                    true); // load what sensor sees into sensorValues
              }
              if (smellFood) {
                for (int food = 1; food <= foodTypes; food++) {
                  brain->setInput(inputCounter++,
                                  sensorValues[food] + sensorValues[food + 10]);
                }
              }
              if (smellOther) {
                int others = 0;
                for (int val = 10; val < 19; val++) {
                  others += sensorValues[val];
                }
                brain->setInput(inputCounter++, others); // set occupied
              }
              if (smellWalls) {
                brain->setInput(inputCounter++, sensorValues[WALL]); // set wall
              }
            }

            //// uncomment to see perfect sensors
            /*
            for (auto sensor : perfectSensorSites) {
                    Vector2d<int> test(11, 11, 0);
                    int lineCount = 1;
                    for (auto line : sensor) {
                            for (auto p : line) {
                                    test(p.x+5, p.y+5) = lineCount;
                            }
                            lineCount++;
                    }
                    test.showGrid();
                    std::cout << std::endl;
            }
            exit(1);
            */

            // set inputs for perfect Sensor
            if (usePerfectSensor) {
              for (auto line :
                   perfectSensorSites[harvester->face]) { // for all the lines
                                                          // (senson inputs) in
                                                          // the perfect sensor
                                                          // for the current
                                                          // dirrection
                fill(sensorValues.begin(), sensorValues.end(), 0);
                for (auto p :
                     line) { // for each location in the current line (sensor
                             // input) get the information at that location
                  sensorValues[foodMap(loopMod(locX + p.x, worldX),
                                       loopMod(locY + p.y, worldY))]++;
                }
                if (perfectDetectsFood) { // for each type of food, set a brain
                                          // input
                  for (int food = 1; food <= foodTypes; food++) {
                    brain->setInput(inputCounter++,
                                    sensorValues[food] +
                                        sensorValues[food + 10]);
                  }
                }
                if (perfectDetectsOther) {
                  int others = 0;
                  for (int val = 10; val < 19; val++) {
                    others += sensorValues[val];
                  }
                  brain->setInput(inputCounter++, others); // set occupied
                }
                if (perfectDetectsWalls) {
                  brain->setInput(inputCounter++,
                                  sensorValues[WALL]); // set wall
                }
              }
            }

            // set inputs for down sensor
            if (useDownSensor) {
              for (int food = 1; food <= foodTypes; food++) {
                brain->setInput(inputCounter++,
                                foodMap(harvester->loc) == (food + 10));
              }
            }

            if (debug) {
              std::cout << "\n----------------------------\n";
              std::cout << "\ngeneration update: " << Global::update
                        << "  world update: " << t
                        << "  local time: " << localTime << "\n";
              std::cout << "currentLocation: " << harvester->loc.x << ","
                        << harvester->loc.y << "  :  " << harvester->face
                        << "\n";
              std::cout << "inNodes: ";
              for (int i = 0; i < requiredInputs; i++) {
                std::cout << brain->readInput(i) << " ";
              }
              std::cout << "\nlast outNodes: ";
              for (int i = 0; i < requiredOutputs; i++) {
                std::cout << brain->readOutput(i) << " ";
              }
              std::cout << std::endl << std::endl;
              foodMap.showGrid();
              std::cout << std::endl << std::endl;
              for (int i = 0; i < 10; i++) {
                std::cout << "foodCount " << i << " : " << foodCounts[i]
                          << std::endl;
              }
              std::cout << "\n\n  -- brain update --\n\n";
            }

            /////////////////
            // update the brain
            /////////////////

            brain->update();

            /////////////////////
            // read brain outputs
            /////////////////////

            // moveOutput has info about the first 2 or 3 output bits and uses
            // actionLookup to map output to action
            // actions are maped to outputs with actionLookup:
            //  0 = no action
            //  1 = move forward
            //  2 = move back
            //  3 = turn left
            //  4 = turn right
            //  5 = step left
            //  6 = step right

            // first get brain values for movement (either 2 or 3 bits) and
            // generate a number in moveOutput (either from 0 to 3 or 0 to 7)
            int outputCounter = 0;
            moveOutput = 0;
            for (int i = 0; i < moveOutputs; i++) {
              moveOutput = Bit(brain->readOutput(outputCounter++)) +
                           (2 * moveOutput); // read each bit and add to the
                                             // (previous values * 2)
            }
            if (alwaysEat) {
              eatOutput = 1;
            } else { // read addtional output for eat action
              eatOutput = Bit(brain->readOutput(outputCounter++));
            }

            // convert move output to an action
            moveOutput = actionLookup[moveOutput];

            ////////////////////////
            // update world --- food
            ////////////////////////

            if (eatOutput) {  // attempt to eat what's here
              localTime += 1; // this action will used up all the time left for
                              // this agents world update
              Point2d currentSpace((int)harvester->loc.x,
                                   (int)harvester->loc.y);
              // for (int f = 1; f <= foodTypes; f++) {
              if (foodMap(currentSpace) - 10 > 0) { // there is a food here
                                                    // (subtract 10 because
                                                    // harvester is here)
                auto f = foodMap(currentSpace) - 10;
                if (harvester->lastFoodCollected != f &&
                    harvester->lastFoodCollected !=
                        0) { // if last food was 0, this is the first food
                             // collected, so no switch
                  harvester->switches++;
                }
                harvester->lastFoodCollected = f;
                harvester->foodCollected[f]++;
                foodCounts[f]--;
                foodCounts[0]++;
                foodMap(currentSpace) = 10; // set map to occupied with no food
                if (visualize) {
                  visualizeData += "E," + std::to_string((int)currentSpace.x) + "," +
                                   std::to_string((int)currentSpace.y) + "\n";
                }
              }
              //}
            }

            /////////////////////////////
            // update world --- no action
            /////////////////////////////
            if (moveOutput == 0) {
              localTime += 1;
            }
            ///////////////////////////
            // update world --- turning
            ///////////////////////////

            double moveDistance = 0;

            // is the harvester turning
            if (!eatOutput || alwaysEat) { // if harvester did not eat or
                                           // alwaysEat is set, then harvester
                                           // may move
              if (moveOutput == 3) {       // turn right
                localTime += turnCost;
                harvester->face =
                    loopMod(harvester->face + 1, rotationResolution);
                moveDistance = moveMinPerTurn;
                // std::cout << "turned ID:" << harvester->org->ID << " @ " <<
                // harvester->loc.x << "," << harvester->loc.y << "  " <<
                // harvester->face << std::endl;
                if (visualize) {
                  visualizeData += "TR," + std::to_string(harvester->ID) + "," +
                                   std::to_string(harvester->face) + "\n";
                }
              }
              if (moveOutput == 4) { // turn left
                localTime += turnCost;
                harvester->face =
                    loopMod(harvester->face - 1, rotationResolution);
                moveDistance = moveMinPerTurn;
                // std::cout << "turned ID:" << harvester->org->ID << " @ " <<
                // harvester->loc.x << "," << harvester->loc.y << "  " <<
                // harvester->face << std::endl;
                if (visualize) {
                  visualizeData += "TL," + std::to_string(harvester->ID) + "," +
                                   std::to_string(harvester->face) + "\n";
                }
              }
            }

            ///////////////////////////
            // update world --- moving
            ///////////////////////////
            // see if harvester is moving

            int moveOffset = 0; // where (0 = forward), (rotationResolution/2 =
                                // backward), (rotationResolution/4 = right) and
                                // (right * -1 = left)

            if (moveOutput != 1 && moveOutput != 2 && moveOutput != 5 &&
                moveOutput != 6 && localTime >= 1) {
              moveDistance = moveMin; // if always move and localtime is
                                      // expired, and this action is not a move
                                      // (forward, back, or sidestep) we will
                                      // move moveMin forwad
            } else if (moveOutput == 1) { // move forward
              localTime += 1;
              moveDistance = moveDefault;
            } else if (moveOutput == 2) { // move back
              localTime += 1;
              moveDistance = -1 * moveDefault;
            } else if (moveOutput == 5) { // step right
              localTime += 1;
              moveDistance = moveDefault;
              moveOffset = rotationResolution / 4;
            } else if (moveOutput == 6) { // step left
              localTime += 1;
              moveDistance = moveDefault;
              moveOffset = -1 * (rotationResolution / 4);
            }

            if ((!eatOutput || alwaysEat) &&
                moveDistance != 0.0) { // if either no eat happend, or always
                                       // eat is on AND there is some movement
              auto currentLoc = harvester->loc; // where are we now?
              Point2d currentSpace(
                  (int)currentLoc.x,
                  (int)currentLoc.y); // which grid space is this?
              auto targetLoc =
                  moveOnGrid(harvester, moveDistance,
                             moveOffset); // where are we going (if we move)?
              Point2d targetSpace(
                  (int)targetLoc.x,
                  (int)targetLoc
                      .y); // which space will we move into (if we move?)
              if (currentSpace == targetSpace) { // if this mode does not change
                                                 // space, then just make the
                                                 // move
                if (snapToGrid) {
                  harvester->loc.x = targetSpace.x + .5;
                  harvester->loc.y = targetSpace.y + .5;
                } else {
                  harvester->loc = targetLoc; // if harvester is not moving from
                                              // current space, just update
                                              // location
                }
                if (visualize) {
                  visualizeData += "M," + std::to_string(harvester->ID) + "," +
                                   std::to_string(harvester->loc.x) + "," +
                                   std::to_string(harvester->loc.y) + "\n";
                }
              } else { // this move would change world location
                if (foodMap(targetLoc) < 9) { // if the proposed move is not a
                                              // wall (9) and is not occupied by
                                              // another org (>9)
                  if (foodMap(currentSpace) - 10 !=
                      foodLastMap(currentSpace)) { // if the food here changed,
                                                   // repacment needed.
                    auto newFoodPick =
                        Random::getIndex(replaceRules[foodLastMap(currentSpace)]
                                             .size()); // get new food
                    auto newFood =
                        replaceRules[foodLastMap(currentSpace)][newFoodPick] ==
                                -1
                            ? Random::getInt(1, foodTypes)
                            : replaceRules[foodLastMap(
                                  currentSpace)][newFoodPick];
                    foodMap(currentSpace) = newFood;
                    foodLastMap(currentSpace) = newFood;
                    foodCounts[0]--;
                    foodCounts[newFood]++;
                    if (visualize) {
                      visualizeData += "R," + std::to_string((int)currentSpace.x) +
                                       "," + std::to_string((int)currentSpace.y) +
                                       "," + std::to_string(newFood) + "\n";
                    }
                  } else {
                    foodMap(currentSpace) -= 10; // food did not change and
                                                 // harvester is no longer here
                  }

                  if (snapToGrid) { // now move
                    harvester->loc.x = targetSpace.x + .5;
                    harvester->loc.y = targetSpace.y + .5;
                  } else {
                    harvester->loc = targetLoc; // if harvester is not moving
                                                // from current space, just
                                                // update location
                  }

                  if (visualize) {
                    visualizeData += "M," + std::to_string(harvester->ID) + "," +
                                     std::to_string(harvester->loc.x) + "," +
                                     std::to_string(harvester->loc.y) + "\n";
                  }
                  harvester->poisonTotals[foodMap(targetSpace)]++; // update
                                                                   // poision
                                                                   // totals -
                                                                   // later we
                                                                   // will
                                                                   // subtract
                                                                   // poison if
                                                                   // this food
                                                                   // is in fact
                                                                   // poison
                  foodMap(targetSpace) += 10; // there is a harvester here, so
                                              // add 10 to the current location
                                              // value
                  // std::cout << "moved ID:" << harvester->org->ID << " @ " <<
                  // harvester->loc.x << "," << harvester->loc.y << "  " <<
                  // harvester->face << std::endl;
                } else {
                  // move is blocked... was it blocked by wall or other?
                  if (foodMap(targetLoc) == 9) { // blocked by wall
                    harvester->wallHits++;
                  } else { // blocked by other
                    harvester->otherHits++;
                  }
                  // move is blocked (by wall or other), no action needed.
                }
              } // end this move would change world location else statement
            }   // end movement
          }     // end localTime while loop
        }       // end evaluate this harvester
        // save map to visualize file
        if (visualize) { // save state of world on this update.
          FileManager::writeToFile("HarvestWorldData.txt", visualizeData);
        }

        // Determine if a food related event occured
        // events -- event[+event+event] (i.e. spit on +)
        // events are defined here, and executed in order
        // T*[num] = all harvesters score += (world updates remaining * num)
        // S[num] = all harvesters score += (num)
        // R[a,b,...] = in map, replace all a with b (i.e. replace all 2 with 1
        // to reset map)
        // G[x,y,f,...] = in map, generate f at x,y; if any value == -1 then use
        // random in world range or food range
        // M = reset map
        // Q = set t = worldUpdates

        // see if a foodEvent occurs
        for (int i = 0; i < (int)triggerFoods.size();
             i++) { // for each trigger, see if all conditions are met

          bool eventTriggerPrimed = false; // did atlease one trigger food for
                                           // this trigger pass the limit?
          bool eventCondtionsMet = true; // are all thef trigger foods for this
                                         // trigger at or below level?
          // both must be true (all foods <= level and at least one crossed this
          // update) to trigger event
          for (auto tf : triggerFoods[i]) {
            if (foodCounts[tf] > triggerFoodLevels[i]) { // if this trigger
                                                         // passed level this
                                                         // turn
              eventCondtionsMet = false; // atleast one food is greater then
                                         // level needed to trigger
            }
            if (foodCounts[tf] <= triggerFoodLevels[i] &&
                foodCountsPrior[tf] > triggerFoodLevels[i]) { // if this trigger
                                                              // passed level
                                                              // this turn
              eventTriggerPrimed = true; // atleast one food trigger this update
            }
          }
          if (eventCondtionsMet && eventTriggerPrimed) {
            // did this food pass the trigger level for triggerFoods[i] last
            // update or during a generation event this update?

            // update world/haresters based on rules(s)
            std::vector<std::string> rules;
            double value;
            convertCSVListToVector_BERRY(triggerFoodEvents[i], rules, '+');
            for (auto rule : rules) {
              if (rule[0] == 'T') { // score based on remaining time
                load_value(rule.substr(2, rule.size() - 2), value);
                value *= (evalTime - t);
                for (auto harvester : harvesters) {
                  harvester->score += value;
                }
              }                          // end time remaining based score rule
              else if (rule[0] == 'S') { // add value to score
                load_value(rule.substr(1, rule.size() - 1), value);
                for (auto harvester : harvesters) {
                  harvester->score += value;
                }
              }                          // end simple score rule
              else if (rule[0] == 'R') { // replace
                std::vector<std::string> replacePairsStrings;
                std::vector<int> replacePairs;
                convertCSVListToVector_BERRY(rule.substr(1, rule.size() - 1),
                                       replacePairsStrings);
                for (auto e : replacePairsStrings) {
                  load_value(e, value);
                  replacePairs.push_back(value);
                }
                for (int x = 0; x < worldX;
                     x++) { // for every location in the map
                  for (int y = 0; y < worldY; y++) {
                    Point2d loc(x, y);
                    for (int replaceIndex = 0;
                         replaceIndex < (int)replacePairs.size();
                         replaceIndex += 2) {
                      if (foodMap(loc) == replacePairs[replaceIndex]) {
                        foodCounts[foodMap(loc)]--;
                        foodMap(loc) = replacePairs[replaceIndex + 1];
                        foodLastMap(loc) =
                            foodMap(loc); // update last map so that food
                                          // replacement will only happen on an
                                          // eat (not just a visit and move
                                          // away)

                        foodCounts[foodMap(loc)]++;
                        if (visualize) {
                          visualizeData = "R," + std::to_string((int)loc.x) + "," +
                                          std::to_string((int)loc.y) + "," +
                                          std::to_string(foodMap(loc)) + "\n";
                          FileManager::writeToFile("HarvestWorldData.txt",
                                                   visualizeData);
                        }
                      }
                    }
                  }
                }
              }                          // end replace rule
              else if (rule[0] == 'G') { // generate
                std::vector<std::string> genStrings;
                std::vector<int> genRules;
                convertCSVListToVector_BERRY(rule.substr(1, rule.size() - 1),
                                       genStrings);
                for (auto e : genStrings) {
                  load_value(e, value);
                  genRules.push_back(value);
                }
                for (int genIndex = 0; genIndex < (int)genRules.size();
                     genIndex += 3) {
                  Point2d loc(genRules[genIndex], genRules[genIndex + 1]);
                  if (foodMap(loc) <
                      10) { // do not generate if location is occupied!
                    foodCounts[foodMap(loc)]--;
                    foodMap(loc) = genRules[genIndex + 2];
                    foodLastMap(loc) =
                        foodMap(loc); // update last map so that food
                                      // replacement will only happen on an eat
                                      // (not just a visit and move away)
                    foodCounts[foodMap(loc)]++;
                    if (visualize) {
                      visualizeData = "R," + std::to_string((int)loc.x) + "," +
                                      std::to_string((int)loc.y) + "," +
                                      std::to_string(foodMap(loc)) + "\n";
                      FileManager::writeToFile("HarvestWorldData.txt",
                                               visualizeData);
                    }
                  }
                }
              }                          // end generate rule
              else if (rule[0] == 'M') { // reset map
                // first, if visualizing, send current world and harvester
                // states to visualize file
                if (visualize) { // save state inital world and Harvester
                                 // locations
                  visualizeData = "**TriggerWorld**\n";
                  visualizeData +=
                      std::to_string(rotationResolution) + "," + std::to_string(worldX) +
                      "," + std::to_string(worldY) + "," +
                      std::to_string(groupSize + (groupSize * clones)) + "\n";
                  // save the map
                  for (int y = 0; y < worldY; y++) {
                    for (int x = 0; x < worldX; x++) {
                      visualizeData += std::to_string(foodMap(x, y) % 10);
                      if (x % worldX == worldX - 1) {
                        visualizeData += "\n";
                      } else {
                        visualizeData += ",";
                      }
                    }
                  }
                  visualizeData += "-\n**TriggerHarvesters**\n";
                  for (auto harvester : harvesters) {
                    visualizeData += std::to_string(harvester->ID) + "," +
                                     std::to_string(harvester->loc.x) + "," +
                                     std::to_string(harvester->loc.y) + "," +
                                     std::to_string(harvester->face) + "," +
                                     std::to_string(harvester->cloneID) + "\n";
                  }
                  visualizeData += "-";
                  FileManager::writeToFile("HarvestWorldData.txt",
                                           visualizeData);
                }

                // next reset map
                foodMap = foodMapCopy;
                foodLastMap = foodMapCopy;
                foodCounts = foodCountsCopy;
                foodCountsPrior = foodCountsCopy;

                // now place harvesters
                auto tempValidSpaces =
                    validSpaces; // make tempValidSpaces so we can pull elements
                                 // from it to select unque locations.
                auto tempStartFacing = startFacing;

                for (auto harvey : harvesters) {
                  // set new location
                  auto pick = Random::getIndex(
                      tempValidSpaces.size());         // get a random index
                  harvey->loc = tempValidSpaces[pick]; // assign location
                  harvey->loc.x += .5; // place in center of location
                  harvey->loc.y += .5;
                  tempValidSpaces[pick] =
                      tempValidSpaces.back(); // copy last location in
                                              // tempValidSpaces to pick
                                              // location
                  tempValidSpaces
                      .pop_back(); // remove last location in tempValidSpaces

                  // set inital new direction
                  harvey->face = tempStartFacing[pick] == 1
                                     ? Random::getIndex(rotationResolution)
                                     : (tempStartFacing[pick] - 2) *
                                           (double(rotationResolution) /
                                            8.0); // if startFacing is 1 then
                                                  // pick random, 2 is up, 3 is
                                                  // up right, etc...
                  tempStartFacing[pick] = tempStartFacing.back();
                  tempStartFacing.pop_back();

                  // place harvester in world
                  foodMap(harvey->loc) +=
                      10; // set this location to occupied in foodMap -
                          // locations in map are 0 if empty, 1->8 if food, 9 if
                          // wall, 10 if occupied with no food, 11->18 if
                          // occupied with food
                  if (debug) {
                    std::cout << "placed ID:" << harvey->org->ID << " @ "
                              << harvey->loc.x << "," << harvey->loc.y << "  "
                              << harvey->face << std::endl;
                  }
                }
                // reset generators
                generators.clear();
                for (auto g : savedGenerators) {
                  generators.push_back(g);
                }
                // reset generator events
                generatorEvents.clear();
                for (int i = 0; i < (int)generators.size(); i++) {
                  // for each generator, find out next time that generator will
                  // fire and add that to generatorEvents
                  // generatorEvents[time][generatorIndex]
                  generatorEvents[generators[i].nextEvent()].push_back(i);
                }
                // finally, if visualizing, send current world and harvester
                // states (after rule(s) have been applied) to visualize file
                if (visualize) { // save state inital world and Harvester
                                 // locations
                  visualizeData = "**TriggerWorld**\n";
                  visualizeData +=
                      std::to_string(rotationResolution) + "," + std::to_string(worldX) +
                      "," + std::to_string(worldY) + "," +
                      std::to_string(groupSize + (groupSize * clones)) + "\n";
                  // save the map
                  for (int y = 0; y < worldY; y++) {
                    for (int x = 0; x < worldX; x++) {
                      visualizeData += std::to_string(foodMap(x, y) % 10);
                      if (x % worldX == worldX - 1) {
                        visualizeData += "\n";
                      } else {
                        visualizeData += ",";
                      }
                    }
                  }
                  visualizeData += "-\n**TriggerHarvesters**\n";
                  for (auto harvester : harvesters) {
                    visualizeData += std::to_string(harvester->ID) + "," +
                                     std::to_string(harvester->loc.x) + "," +
                                     std::to_string(harvester->loc.y) + "," +
                                     std::to_string(harvester->face) + "," +
                                     std::to_string(harvester->cloneID) + "\n";
                  }
                  visualizeData += "-";
                  FileManager::writeToFile("HarvestWorldData.txt",
                                           visualizeData);
                }
              } else if (rule[0] == 'Q') { // quit now!
                t = evalTime;              // finish this evaluation!
              }
            }
          }
        }
        foodCountsPrior = foodCounts; // update prior to be current

      } // end this evalGroup

      // now save data based on cloneScoreRule where: 0 = all, 1 = best, 2 =
      // worst.
      // first score all harvesters
      for (auto harvester : harvesters) {
        // here we will do some math and save values to each harvesters dataMap
        for (int f = 1; f <= foodTypes; f++) {
          harvester->maxFood =
              std::max(harvester->foodCollected[f], harvester->maxFood);
          harvester->totalFood += harvester->foodCollected[f];
          harvester->foodScore += foodRewards[f] * harvester->foodCollected[f];
        }
        for (int f = 0; f <= foodTypes; f++) {
          if (poisonRules[f] !=
              0) { // for each food with a poison rule, add to poision cost
            harvester->poisonCost +=
                harvester->poisonTotals[f] * poisonRules[f];
          }
        }

        harvester->score += harvester->foodScore -
                            ((harvester->switches * switchCostPL->get(PT)) +
                             harvester->poisonCost +
                             (harvester->wallHits * hitWallCostPL->get()) +
                             (harvester->otherHits * hitOtherCostPL->get()));
      }

      // if visualizing and there are groups, save a group report
      if (visualize && (groupSize != 1 || clones != 0)) {
        std::cout << "creating HarvestWorldGroupReport.csv" << std::endl;
        DataMap tempDM;

        for (auto harvester : harvesters) {
          for (int f = 1; f <= foodTypes; f++) {
            tempDM.set("food" + std::to_string(f), harvester->foodCollected[f]);
            tempDM.setOutputBehavior("food" + std::to_string(f), DataMap::FIRST);
          }
          for (int f = 0; f <= foodTypes; f++) {
            if (poisonRules[f] != 0) {
              tempDM.set("poison" + std::to_string(f), harvester->poisonTotals[f]);
              tempDM.setOutputBehavior("poison" + std::to_string(f), DataMap::FIRST);
            }
          }
          tempDM.set("switches", harvester->switches);
          tempDM.setOutputBehavior("switches", DataMap::FIRST);
          tempDM.set("consumptionRatio",
                     harvester->maxFood /
                         (harvester->totalFood - harvester->maxFood + 1));
          tempDM.setOutputBehavior("consumptionRatio", DataMap::FIRST);
          tempDM.set("wallHits", harvester->wallHits);
          tempDM.setOutputBehavior("wallHits", DataMap::FIRST);
          tempDM.set("otherHits", harvester->otherHits);
          tempDM.setOutputBehavior("otherHits", DataMap::FIRST);
          tempDM.set("score", harvester->score);
          tempDM.setOutputBehavior("score", DataMap::FIRST);
          tempDM.set("index", saveCount++);
          tempDM.setOutputBehavior("index", DataMap::FIRST);
          tempDM.set("orgID", harvester->org->ID);
          tempDM.setOutputBehavior("orgID", DataMap::FIRST);
          tempDM.set("groupID", harvester->ID);
          tempDM.setOutputBehavior("groupID", DataMap::FIRST);
          tempDM.set("whichGroup", evalGroupCount);
          tempDM.setOutputBehavior("whichGroup", DataMap::FIRST);
          tempDM.writeToFile("HarvestWorldGroupReport.csv");
        }
        evalGroupCount++;
      }

      if (groupScoreRule ==
          -1) { // group score rule is SOLO, deal with clone score rule
        // for each organisms, figure out which clone (or clones need to have
        // their data saved)
        std::vector<std::shared_ptr<Harvester>> saveHarvesters; // this will be
                                                                // a list of
                                                                // harvesters
                                                                // which we save
                                                                // data for

        if (cloneScoreRule == 0) { // score ALL
          saveHarvesters = harvesters;
        } else if (cloneScoreRule == 1) { // score BEST
          for (int i = 0; i < groupSize;
               i++) { // for each parent harvester (groups are ordered parents
                      // first then clones)
            int bestIndex = i; // set parent to best
            for (auto clone :
                 harvesters[i]->clones) { // for each of that parents clones
              if (clone->score > harvesters[bestIndex]->score) {
                bestIndex =
                    clone->ID; // if the clone is better, set that id as best
              }
            }
            saveHarvesters.push_back(harvesters[bestIndex]);
          }
        } else { // score WORST
          for (int i = 0; i < groupSize;
               i++) { // for each parent harvester (groups are ordered parents
                      // first then clones)
            int worstIndex = i; // set parent to worst
            for (auto clone :
                 harvesters[i]->clones) { // for each of that parents clones
              if (clone->score < harvesters[worstIndex]->score) {
                worstIndex =
                    clone->ID; // if the clone is worse, set that id as worst
              }
            }
            saveHarvesters.push_back(harvesters[worstIndex]);
          }
        } // end select saveHarvesters

        // now resolve group scoring
        // score all saveHarvesters

        // now save data to dataMaps for everytone in saveHarvesters
        for (auto harvester : saveHarvesters) {
          for (int f = 1; f <= foodTypes; f++) {
            harvester->org->dataMap.append("food" + std::to_string(f),
                                           harvester->foodCollected[f]);
          }
          for (int f = 0; f <= foodTypes; f++) {
            if (poisonRules[f] != 0) {
              harvester->org->dataMap.append("poison" + std::to_string(f),
                                             harvester->poisonTotals[f]);
            }
          }
          harvester->org->dataMap.append("switches", harvester->switches);
          harvester->org->dataMap.append(
              "consumptionRatio",
              harvester->maxFood /
                  (harvester->totalFood - harvester->maxFood + 1));
          harvester->org->dataMap.append("wallHits", harvester->wallHits);
          harvester->org->dataMap.append("otherHits", harvester->otherHits);
          harvester->org->dataMap.append("score", harvester->score);
        }
      }      // end cloneScoreRules when groupScoreRule == SOLO
      else { // groupScoreRule is not SOLO
        if (groupScoreRule == 0) {            // group score ALL
          for (auto harvester : harvesters) { // for each harvester
            if (!harvester->isClone) { // if not a clone assign scores from all
                                       // harversters in this group
              for (auto scoreHarvester :
                   harvesters) { // append the score of every harvester
                                 // (including clones)
                for (int f = 1; f <= foodTypes; f++) {
                  harvester->org->dataMap.append(
                      "food" + std::to_string(f), scoreHarvester->foodCollected[f]);
                }
                for (int f = 0; f <= foodTypes; f++) {
                  if (poisonRules[f] != 0) {
                    harvester->org->dataMap.append(
                        "poison" + std::to_string(f),
                        scoreHarvester->poisonTotals[f]);
                  }
                }
                harvester->org->dataMap.append("switches",
                                               scoreHarvester->switches);
                harvester->org->dataMap.append(
                    "consumptionRatio",
                    scoreHarvester->maxFood / (scoreHarvester->totalFood -
                                               scoreHarvester->maxFood + 1));
                harvester->org->dataMap.append("wallHits",
                                               scoreHarvester->wallHits);
                harvester->org->dataMap.append("otherHits",
                                               scoreHarvester->otherHits);
                harvester->org->dataMap.append("score", scoreHarvester->score);
              }
            }
          }
        }                               // end groupScoreRule ALL
        else if (groupScoreRule == 1) { // score BEST
          // find best (might be clone)
          int bestIndex = 0;
          for (auto harvester : harvesters) {
            if (harvester->score > harvesters[bestIndex]->score) {
              bestIndex = harvester->ID;
            }
          }
          for (auto harvester : harvesters) {
            if (!harvester->isClone) { // if not a clone assign values from best
              for (int f = 1; f <= foodTypes; f++) {
                harvester->org->dataMap.append(
                    "food" + std::to_string(f),
                    harvesters[bestIndex]->foodCollected[f]);
              }
              for (int f = 0; f <= foodTypes; f++) {
                if (poisonRules[f] != 0) {
                  harvester->org->dataMap.append(
                      "poison" + std::to_string(f),
                      harvesters[bestIndex]->poisonTotals[f]);
                }
              }
              harvester->org->dataMap.append("switches",
                                             harvesters[bestIndex]->switches);
              harvester->org->dataMap.append(
                  "consumptionRatio", harvesters[bestIndex]->maxFood /
                                          (harvesters[bestIndex]->totalFood -
                                           harvesters[bestIndex]->maxFood + 1));
              harvester->org->dataMap.append("wallHits",
                                             harvesters[bestIndex]->wallHits);
              harvester->org->dataMap.append("otherHits",
                                             harvesters[bestIndex]->otherHits);
              harvester->org->dataMap.append("score",
                                             harvesters[bestIndex]->score);
            }
          }
        }                               // end groupScoreRule BEST
        else if (groupScoreRule == 2) { // score WORST
          // find worst (might be clone)
          int worstIndex = 0;
          for (auto harvester : harvesters) {
            if (harvester->score < harvesters[worstIndex]->score) {
              worstIndex = harvester->ID;
            }
          }
          for (auto harvester : harvesters) {
            if (!harvester
                     ->isClone) { // if not a clone assign values from worst
              for (int f = 1; f <= foodTypes; f++) {
                harvester->org->dataMap.append(
                    "food" + std::to_string(f),
                    harvesters[worstIndex]->foodCollected[f]);
              }
              for (int f = 0; f <= foodTypes; f++) {
                if (poisonRules[f] != 0) {
                  harvester->org->dataMap.append(
                      "poison" + std::to_string(f),
                      harvesters[worstIndex]->poisonTotals[f]);
                }
              }
              harvester->org->dataMap.append("switches",
                                             harvesters[worstIndex]->switches);
              harvester->org->dataMap.append(
                  "consumptionRatio",
                  harvesters[worstIndex]->maxFood /
                      (harvesters[worstIndex]->totalFood -
                       harvesters[worstIndex]->maxFood + 1));
              harvester->org->dataMap.append("wallHits",
                                             harvesters[worstIndex]->wallHits);
              harvester->org->dataMap.append("otherHits",
                                             harvesters[worstIndex]->otherHits);
              harvester->org->dataMap.append("score",
                                             harvesters[worstIndex]->score);
            }
          }
        } // end groupScoreRule WORST
      }   // end groupScoreRule != SOLO
    }     // end evaluations all evalGroups
  }       // end current map
} // end HarvestWorld::evaluate

std::unordered_map<std::string, std::unordered_set<std::string>>
BerryWorld::requiredGroups() {
  return {{groupNameSpacePL->get(PT),
           {"B:" + brainNameSpacePL->get(PT) + "," + std::to_string(requiredInputs) +
            "," + std::to_string(requiredOutputs)}}};
}

