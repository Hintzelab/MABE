//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once

#include <Archivist/DefaultArchivist.h>
#include <Global.h>
#include <Optimizer/AbstractOptimizer.h>
#include <Organism/Organism.h>

class Group {
public:
  std::vector<std::shared_ptr<Organism>> population;
  std::shared_ptr<Organism> templateOrg;
  std::shared_ptr<DefaultArchivist> archivist;
  std::shared_ptr<AbstractOptimizer> optimizer;

  Group();
  Group(std::vector<std::shared_ptr<Organism>> _population,
        std::shared_ptr<AbstractOptimizer> _optimizer,
        std::shared_ptr<DefaultArchivist> _archivist);
  ~Group();

  bool archive(int flush = 0);
  void optimize();
  void cleanup();
};

