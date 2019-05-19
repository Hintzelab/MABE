#include "MTree.h"

std::vector<double> Abstract_MTree::eval(DataMap &dataMap) {
  std::vector<std::vector<double>> placeholder = {};
  return eval(dataMap, nullptr, placeholder);
}

std::vector<double> Abstract_MTree::eval(std::shared_ptr<ParametersTable> PT) {
  DataMap dataMap;
  std::vector<std::vector<double>> placeholder = {};
  return eval(dataMap, PT, placeholder);
}

std::vector<double> Abstract_MTree::eval(std::vector<std::vector<double>> &vectorData) {
  DataMap dataMap;
  return eval(dataMap, nullptr, vectorData);
}

std::vector<double> Abstract_MTree::eval(DataMap &dataMap,
                                 std::shared_ptr<ParametersTable> PT) {
  std::vector<std::vector<double>> placeholder = {};
  return eval(dataMap, PT, placeholder);
}


