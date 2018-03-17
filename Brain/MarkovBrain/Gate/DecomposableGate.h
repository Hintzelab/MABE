//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#pragma once

#include "AbstractGate.h"

class DecomposableGate : public AbstractGate { // conventional probabilistic
                                               // gate
public:
  static std::shared_ptr<ParameterLink<std::string>> IO_RangesPL;

  std::vector<std::vector<double>> table;
  DecomposableGate() = delete;
  DecomposableGate(std::shared_ptr<ParametersTable> _PT = nullptr)
      : AbstractGate(_PT) {
    table = {};
  }
  DecomposableGate(std::pair<std::vector<int>, std::vector<int>> addresses,
                   std::vector<std::vector<int>> _rawTable, int _ID,
                   std::shared_ptr<ParametersTable> _PT = nullptr);
  virtual ~DecomposableGate() = default;
  virtual void update(std::vector<double> &states,
                      std::vector<double> &nextStates) override;
  virtual std::string gateType() override { return "Decomposable"; }
  virtual std::shared_ptr<AbstractGate>
  makeCopy(std::shared_ptr<ParametersTable> _PT = nullptr) override;
};

