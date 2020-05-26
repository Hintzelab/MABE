//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once // directive to insure that this .h file is only included one time

#include <cctype>

#include <Utilities/Utilities.h>
#include "Utilities/VectorNd.h"
#include "Utilities/TurnTable.h"
#include "Utilities/SensorArcs.h"

#include <World/AbstractWorld.h> // AbstractWorld defines all the basic function templates for worlds

class BerryWorld : public AbstractWorld {

public:
  // example ParameterLinks (PLs)
  // static shared_ptr<ParameterLink<int>> exampleIntPL;
  // static shared_ptr<ParameterLink<double>> exampleDoublePL;
  // static shared_ptr<ParameterLink<bool>> exampleBoolPL;
  // static shared_ptr<ParameterLink<string>> exampleStringPL;

  // Example code for MTree
  //   static shared_ptr<ParameterLink<string>> exampleMTreePL;
  // MTree to hold the MTree
  //   shared_ptr<Abstract_MTree> exampleMT;

  static std::shared_ptr<ParameterLink<int>> worldSizeXPL;
  static std::shared_ptr<ParameterLink<int>> worldSizeYPL;
  static std::shared_ptr<ParameterLink<bool>> worldHasWallPL;
  static std::shared_ptr<ParameterLink<int>> rotationResolutionPL;
  static std::shared_ptr<ParameterLink<double>> maxTurnPL;

  static std::shared_ptr<ParameterLink<bool>> allowTurnPL;
  static std::shared_ptr<ParameterLink<bool>> allowSidestepPL;
  static std::shared_ptr<ParameterLink<bool>> allowBackstepPL;
  bool allowTurn;
  bool allowSidestep;
  bool allowBackstep;

  static std::shared_ptr<ParameterLink<bool>> allowMoveAndEatPL;
  static std::shared_ptr<ParameterLink<bool>> alwaysEatPL;
  bool alwaysEat;
  bool allowMoveAndEat;

  static std::shared_ptr<ParameterLink<int>> evalTimePL;
  static std::shared_ptr<ParameterLink<int>> foodTypesPL;

  static std::shared_ptr<ParameterLink<double>> switchCostPL;
  static std::shared_ptr<ParameterLink<double>> hitWallCostPL;
  static std::shared_ptr<ParameterLink<double>> hitOtherCostPL;

  static std::shared_ptr<ParameterLink<int>> alwaysStartOnPL;
  static std::shared_ptr<ParameterLink<std::string>> validStartConfigurationsPL;
  std::vector<int> validStartConfigurations;

  static std::shared_ptr<ParameterLink<std::string>> initialFoodDistributionPL;
  std::vector<int> initialFoodDistribution;
  static std::shared_ptr<ParameterLink<double>> reward1PL;
  static std::shared_ptr<ParameterLink<double>> reward2PL;
  static std::shared_ptr<ParameterLink<double>> reward3PL;
  static std::shared_ptr<ParameterLink<double>> reward4PL;
  static std::shared_ptr<ParameterLink<double>> reward5PL;
  static std::shared_ptr<ParameterLink<double>> reward6PL;
  static std::shared_ptr<ParameterLink<double>> reward7PL;
  static std::shared_ptr<ParameterLink<double>> reward8PL;
  static std::shared_ptr<ParameterLink<std::string>> replace1PL;
  static std::shared_ptr<ParameterLink<std::string>> replace2PL;
  static std::shared_ptr<ParameterLink<std::string>> replace3PL;
  static std::shared_ptr<ParameterLink<std::string>> replace4PL;
  static std::shared_ptr<ParameterLink<std::string>> replace5PL;
  static std::shared_ptr<ParameterLink<std::string>> replace6PL;
  static std::shared_ptr<ParameterLink<std::string>> replace7PL;
  static std::shared_ptr<ParameterLink<std::string>> replace8PL;

  static std::shared_ptr<ParameterLink<double>> poison0PL;
  static std::shared_ptr<ParameterLink<double>> poison1PL;
  static std::shared_ptr<ParameterLink<double>> poison2PL;
  static std::shared_ptr<ParameterLink<double>> poison3PL;
  static std::shared_ptr<ParameterLink<double>> poison4PL;
  static std::shared_ptr<ParameterLink<double>> poison5PL;
  static std::shared_ptr<ParameterLink<double>> poison6PL;
  static std::shared_ptr<ParameterLink<double>> poison7PL;
  static std::shared_ptr<ParameterLink<double>> poison8PL;

  static std::shared_ptr<ParameterLink<int>> visionSensorDistancePL;
  static std::shared_ptr<ParameterLink<int>> visionSensorArcSizePL;
  static std::shared_ptr<ParameterLink<std::string>> visionSensorDirectionsPL;

  static std::shared_ptr<ParameterLink<int>> smellSensorDistancePL;
  static std::shared_ptr<ParameterLink<int>> smellSensorArcSizePL;
  static std::shared_ptr<ParameterLink<std::string>> smellSensorDirectionsPL;

  static std::shared_ptr<ParameterLink<bool>> seeFoodPL;
  static std::shared_ptr<ParameterLink<bool>> smellFoodPL;
  static std::shared_ptr<ParameterLink<bool>> perfectDetectsFoodPL;

  static std::shared_ptr<ParameterLink<bool>> seeOtherPL;
  static std::shared_ptr<ParameterLink<bool>> smellOtherPL;
  static std::shared_ptr<ParameterLink<bool>> perfectDetectsOtherPL;

  static std::shared_ptr<ParameterLink<bool>> seeWallsPL;
  static std::shared_ptr<ParameterLink<bool>> smellWallsPL;
  static std::shared_ptr<ParameterLink<bool>> perfectDetectsWallsPL;

  static std::shared_ptr<ParameterLink<bool>> useDownSensorPL;
  static std::shared_ptr<ParameterLink<bool>> useSmellSensorPL;
  static std::shared_ptr<ParameterLink<bool>> useVisionSensorPL;
  static std::shared_ptr<ParameterLink<bool>> usePerfectSensorPL;
  static std::shared_ptr<ParameterLink<std::string>> perfectSensorFilePL;

  static std::shared_ptr<ParameterLink<bool>> wallsBlockVisonSensorsPL;
  static std::shared_ptr<ParameterLink<bool>> wallsBlockSmellSensorsPL;

  static std::shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;
  static std::shared_ptr<ParameterLink<int>> evaluateGroupSizePL;
  static std::shared_ptr<ParameterLink<std::string>> cloneScoreRulePL;
  static std::shared_ptr<ParameterLink<int>> clonesPL;
  static std::shared_ptr<ParameterLink<std::string>> groupScoreRulePL;

  // parameters for group and brain namespaces
  static std::shared_ptr<ParameterLink<std::string>> groupNameSpacePL;
  static std::shared_ptr<ParameterLink<std::string>> brainNameSpacePL;

  static std::shared_ptr<ParameterLink<double>> moveDefaultPL;
  static std::shared_ptr<ParameterLink<double>> moveMinPL;
  static std::shared_ptr<ParameterLink<double>> moveMinPerTurnPL;
  static std::shared_ptr<ParameterLink<bool>> snapToGridPL;

  static std::shared_ptr<ParameterLink<std::string>> mapFilesPL;
  static std::shared_ptr<ParameterLink<std::string>> whichMapsPL;

  static std::shared_ptr<ParameterLink<std::string>> triggerFoodsPL;
  static std::shared_ptr<ParameterLink<std::string>> triggerFoodLevelsPL;
  static std::shared_ptr<ParameterLink<std::string>> triggerFoodEventsPL;

  int requiredOutputs, requiredInputs;

  class WorldMap {
  public:
    class ResourceGenerator {
    public:
      int geo = 0;      // 0 is point generator, 1 is square generator
      int x, y, x1, y1; // x,y point location, x,y,x1,y1 square generator area
      std::vector<double> rate; // list of values which determin when generator
                                // fires. if value < 1, random % chance per
                                // update. if >=1 then inflow on this step
      // if more then one value then they fire in order
      int replaceRule = 3; // 0: : replace none, 1: replace food only, 2:
                           // replace walls only, 4 replace all (food and walls)
      std::vector<std::vector<double>> resourcePrograms; // on inflow, a random
                                                         // element is selected
                                                         // form resources. If
                                                         // this element
                                                         // contains more then
                                                         // one value then
      // the next value in the list is temporaraly the rate and the following
      // value is the next resource (i.e. this allows
      // for a chain of replacement or in the case of [1,5,0] to drop a
      // temporary resource
      int rateIndex = 0; // will iterate over rates
      int resourceIndex =
          Random::getIndex(resourcePrograms.size()); // which resource are we
                                                     // currently looking at,
                                                     // will choose randomly
                                                     // from resourcePrograms
      int programIndex =
          0; // in the current resource, which step of the program are we on

      int val2Time(double val) {
        if (val == 0) {
          return 0;
        }
        if (val < 1) {
          return Random::getInt(1, (int)(1.0 / val));
        }
        return (int)val;
      }

      // find out the time of the next event from this generator
      // advance correct index to the next state
      int nextEvent() {
        if (programIndex >
            0) { // currently in a program, get next element in program
          if (resourcePrograms[resourceIndex].size() >
              programIndex) { // advance program
            return val2Time(resourcePrograms[resourceIndex][programIndex++]);
          } else { // this program is over, set to -1 and advance rateIndex
            programIndex = 0;
            return nextEvent();
          }
        } else { // not in program
          resourceIndex = Random::getIndex(
              resourcePrograms.size()); // get a new resourceIndex
          if (rateIndex >= (int)rate.size()) {
            rateIndex = 0;
          }
          return val2Time(rate[rateIndex++]);
        }
      }

      int getNextResource(int currentVal = 0) {
        int nextResource = resourcePrograms[resourceIndex][programIndex++];
        if (nextResource == currentVal || currentVal > 9) {
          return -1; // newfood = current val, or currentVal indicates that
                     // location is occupied... no change, ignore nextResource
        } else if (currentVal == WALL &&
                   (replaceRule == 0 || replaceRule == 1)) {
          return -1; // current val is wall, but replace rule does not replace
                     // walls... no change, ignore nextResource
        } else if ((currentVal > 0 && currentVal < 9) &&
                   (replaceRule == 0 || replaceRule == 2)) {
          return -1; // current val is food, but replace rule does not replace
                     // food... no change, ignore nextResource
        } else {
          return nextResource;
        }
      }

      Point2d getLocation() { // return a location - if point mode, then x,y if
                              // square mode then random point in x,x1,y,y1
        if (geo == 0) {
          return Point2d(x, y);
        } else {
          return Point2d(Random::getInt(x, x1), Random::getInt(y, y1));
        }
      }
    };

    int worldX;
    int worldY;
    std::string file;
    std::string name;
    Vector2d<int> data;
    Vector2d<int> startData;
    std::vector<Point2d> startLocations;
    std::vector<int> startFacing;
    bool useStartMap = false;

    std::vector<ResourceGenerator>
        generators; // index will act as lookup key in generator events
    std::vector<std::vector<int>> triggerFoods; // = { 1,1,3 };
    std::vector<int> triggerFoodLevels;         // = { 10,0,0 };
    std::vector<std::string>
        triggerFoodEvents; // = { "R[2,3]","T*10+Q","G[1,1,2,0,0,2]" };

    bool loadMap(std::ifstream &ss, const std::string fileName);
  };

  std::vector<WorldMap::ResourceGenerator>
      generators; // index will act as lookup key in generator events
  std::map<int, std::vector<int>> generatorEvents; // each vector<int> holds
                                                   // indexes for generators to
                                                   // run when world update (t)
                                                   // = key.

  enum mapValues { EMPTY = 0, WALL = 9 };

  class Harvester {
  public:
    std::shared_ptr<Organism> org;
    std::shared_ptr<AbstractBrain> brain;
    int ID;
    int cloneID;
    bool isClone = false;
    std::vector<std::shared_ptr<Harvester>> clones;

    Point2d loc; // location
    int face;    // facing direction
    std::vector<int> foodCollected;
    std::vector<int> poisonTotals;
    int lastFoodCollected;
    int switches = 0; // how many times did this harverster switch foods?
    int wallHits =
        0; // how many times did this harverster try to move into a wall?
    int otherHits = 0; // how many times did this harverster try to move into
                       // another harvester?

    // these vars are only used at the end to collect stats
    double score = 0;
    int maxFood;
    int totalFood;
    double foodScore = 0;
    double poisonCost = 0;
  };

  int rotationResolution;
  double maxTurn;
  Sensor visionSensor;
  Sensor smellSensor;
  int visionSensorCount;
  int smellSensorCount;

  int visionSensorDistance;
  int visionSensorArcSize;
  std::vector<int> visionSensorDirections;

  int smellSensorDistance;
  int smellSensorArcSize;
  std::vector<int> smellSensorDirections;

  bool seeFood, smellFood, perfectDetectsFood;
  bool seeOther, smellOther, perfectDetectsOther;
  bool seeWalls, smellWalls, perfectDetectsWalls;
  bool usePerfectSensor, useDownSensor;

  std::vector<Point2d> moveDeltas;

  std::vector<double> foodRewards;
  std::vector<std::vector<int>> replaceRules;
  std::vector<double> poisonRules;

  int worldX;
  int worldY;
  bool worldHasWall;

  double moveDefault;
  double moveMin;
  double moveMinPerTurn;

  bool snapToGrid;

  int foodTypes;

  int wallsBlockVisonSensors;
  int wallsBlockSmellSensors;

  int cloneScoreRule;
  int groupScoreRule;

  int moveOutputs;

  std::map<int, int> actionLookup;

  std::map<std::string, std::map<std::string, WorldMap>>
      worldMaps; // [type][name]
  std::vector<std::string> mapFiles;
  std::map<std::string, std::vector<std::string>> mapNames;
  std::vector<std::string> whichMaps;

  std::vector<std::vector<std::vector<Point2d>>> perfectSensorSites;

  std::vector<int> foodCounts;

  std::vector<std::vector<int>> triggerFoods; // = { 1,1,3 };
  std::vector<int> triggerFoodLevels;         // = { 10,0,0 };
  std::vector<std::string>
      triggerFoodEvents; // = { "R[2,3]","T*10+Q","G[1,1,2,0,0,2]" };
  // backup triggers from config files
  std::vector<std::vector<int>> configTriggerFoods;
  std::vector<int> configTriggerFoodLevels;
  std::vector<std::string> configTriggerFoodEvents;

  BerryWorld(std::shared_ptr<ParametersTable> PT_);
  virtual ~BerryWorld() = default;

  virtual void evaluate(std::map<std::string, std::shared_ptr<Group>> &groups,
                        int analyse, int visualize, int debug) override;
  void runWorld(std::map<std::string, std::shared_ptr<Group>> &groups,
                int analyse, int visualize, int debug);

  virtual std::unordered_map<std::string, std::unordered_set<std::string>>
  requiredGroups() override;

  // takes x,y and updates them by moving one step in facing
  Point2d moveOnGrid(std::shared_ptr<Harvester> harvester, double distance,
                     int offset = 0);
};

