//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

// A single agent is evaluated to perform the xor logic operation,
// and is tested on each standard 2-bit pattern: 0^0, 0^1, 1^0, 1^1
// it receives 1.0 point for each correct computation.

#include "XorWorld.h"
#include <cmath>

std::shared_ptr<ParameterLink<std::string>> XorWorld::groupNamePL =
    Parameters::register_parameter(
		"WORLD_XOR_NAMES-groupName", (std::string) "root::",
        "name of group to be evaluated");
std::shared_ptr<ParameterLink<std::string>> XorWorld::brainNamePL =
    Parameters::register_parameter(
        "WORLD_XOR_NAMES-brainName", (std::string) "root::",
        "namespace for parameters used to define brain");
std::shared_ptr<ParameterLink<int>> XorWorld::evaluationsPerGenerationPL =
    Parameters::register_parameter("WORLD_XOR-evaluationsPerGeneration", 1,
                                   "Number of times to test each Genome per "
                                   "generation (useful with non-deterministic "
                                   "brains)");
std::shared_ptr<ParameterLink<int>> XorWorld::brainUpdatesPL =
    Parameters::register_parameter("WORLD_XOR-brainUpdates", 10,
                                   "Number of times the brain gets to receive "
                                   "input and perform 1 brain update, before "
                                   "the brain's output is queried.");

XorWorld::XorWorld(std::shared_ptr<ParametersTable> PT_) : AbstractWorld(PT_) {

  groupName = groupNamePL->get(PT_);
  brainName = brainNamePL->get(PT_);
  brainUpdates = brainUpdatesPL->get(PT);

  // columns to be added to ave file
  popFileColumns.clear();
  popFileColumns.push_back("score");
  popFileColumns.push_back("score_VAR"); // specifies to also record the
                                         // variance (performed automatically
                                         // because _VAR)
}

// score 1.0 points accumulated per correct xor answer
void XorWorld::evaluateSolo(std::shared_ptr<Organism> org, int analyze,
                            int visualize, int debug) {
  auto brain = org->brains[brainName];
  double score = 0.0000001;
  int questions[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
  double answers[4] = {0.0, 1.0, 1.0, 0.0};
  double answer = 0.0;
  for (int tests = evaluationsPerGenerationPL->get(PT); tests >= 0; --tests) {
    for (int bitBattern = 0; bitBattern < 4; bitBattern++) {
      brain->resetBrain();
      for (int thinkLoopi = brainUpdates - 1; thinkLoopi >= 0;
           --thinkLoopi) { // allow multiple brain updates
        // and provide the bitPattern input on each update
        for (int ins = 0; ins < 2; ins++)
          brain->setInput(ins, questions[bitBattern][ins]);
        brain->update();
      }
      answer = brain->readOutput(0);
      if ((std::isnan(answer)) || (std::isinf(answer)))
        answer = -1.0;

      if (answer > 1.0)
        answer = 1.0;
      if (answer < 0.0)
        answer = 0.0;
      score += 1.0 -
               ((answers[bitBattern] - answer) *
                (answers[bitBattern] - answer)); // add 1.0 for a correct answer
    }
  }
  org->dataMap.set("score", score / evaluationsPerGenerationPL->get(PT));
}
