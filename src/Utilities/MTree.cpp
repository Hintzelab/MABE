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

void Abstract_MTree::explode(std::shared_ptr<Abstract_MTree> tree,
                     std::vector<std::shared_ptr<Abstract_MTree>> &nodeList) {
  nodeList.push_back(tree);
  for (auto b : tree->branches) {
    explode(b, nodeList);
  }
}

MOD_MTree::MOD_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
  branches = _branches;
  if (requiredBranches.size() != 0 &&
			find(requiredBranches.begin(), requiredBranches.end(),
           (int)branches.size()) == requiredBranches.end()) {
    // now check for requiredBranches < 0
    bool OKay = false;
    for (auto r : requiredBranches) {
      if (r < 0) {
        if (abs(r) <= branches.size()) {
          OKay = true;
        }
      }
    }
    if (!OKay) { // if no < 0 value passed
      std::cout << "  In " << type() << "_MTree::constructor - branches does not "
        "contain a legal number of element(s)!"
                << std::endl;
      std::cout << "    " << branches.size()
                << " elements were provided, but function requires : ";
      for (auto n : requiredBranches) {
        std::cout << n;
        if (n != requiredBranches.back()) {
          std::cout << " or ";
        }
      }
      std::cout << "  NOTE: values < 0 indicate requirement > abs(value)." << std::endl;
      exit(1);
    }
  }
}

std::shared_ptr<Abstract_MTree>
MOD_MTree::makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
  // make copy is needed to support brain (must perform a deep copy)
  // copy any local data as well as all branches
  if (_branches.size() == 0) {
    for (auto b : branches) {
      _branches.push_back(b->makeCopy());
    }
  }
  std::shared_ptr<Abstract_MTree> newTree = std::make_shared<MOD_MTree>(_branches);
  return newTree;
}

std::vector<double>
MOD_MTree::eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
     const std::vector<std::vector<double>> &vectorData) {
  int temp = ((int)branches[1]->eval(dataMap, PT, vectorData)[0] == 0)
    ? (int)1
    : (int)branches[1]->eval(dataMap, PT, vectorData)[0];
  double returnValue =
    (int)branches[0]->eval(dataMap, PT, vectorData)[0] % temp;
  return { returnValue }; // return vector with one element
}

void MOD_MTree::show(int indent) {
  std::cout << std::string(indent, '\t') << "** " << type() << std::endl;
  indent++;
  for (auto b : branches) {
    b->show(indent);
  }
}

std::string MOD_MTree::getFormula() {
  std::string args = type() + "[";
  for (auto b : branches) {
    args += b->getFormula();
    if (b != branches.back()) {
      args += ",";
    }
  }
  args += "]";
  return args;
}

std::vector<int> MOD_MTree::numBranches() { return requiredBranches; }

std::string ABS_MTree::type() {
  return "ABS"; // SET TYPE NAME HERE //
}

ABS_MTree::ABS_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
  branches = _branches;
  if (requiredBranches.size() != 0 &&
			find(requiredBranches.begin(), requiredBranches.end(),
           (int)branches.size()) == requiredBranches.end()) {
    // now check for requiredBranches < 0
    bool OKay = false;
    for (auto r : requiredBranches) {
      if (r < 0) {
        if (abs(r) <= branches.size()) {
          OKay = true;
        }
      }
    }
    if (!OKay) { // if no < 0 value passed
      std::cout << "  In " << type() << "_MTree::constructor - branches does not "
        "contain a legal number of element(s)!"
                << std::endl;
      std::cout << "    " << branches.size()
                << " elements were provided, but function requires : ";
      for (auto n : requiredBranches) {
        std::cout << n;
        if (n != requiredBranches.back()) {
          std::cout << " or ";
        }
      }
      std::cout << "  NOTE: values < 0 indicate requirement > abs(value)." << std::endl;
      exit(1);
    }
  }
}

std::shared_ptr<Abstract_MTree>
ABS_MTree::makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
  // make copy is needed to support brain (must perform a deep copy)
  // copy any local data as well as all branches
  if (_branches.size() == 0) {
    for (auto b : branches) {
      _branches.push_back(b->makeCopy());
    }
  }
  std::shared_ptr<Abstract_MTree> newTree = std::make_shared<ABS_MTree>(_branches);
  return newTree;
}

std::vector<double>
ABS_MTree::eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
     const std::vector<std::vector<double>> &vectorData) {
  return { std::abs(branches[0]->eval(
                                      dataMap, PT, vectorData)[0]) }; // return vector with one element
}

void ABS_MTree::show(int indent) {
  std::cout << std::string(indent, '\t') << "** " << type() << std::endl;
  indent++;
  for (auto b : branches) {
    b->show(indent);
  }
}

std::string ABS_MTree::getFormula() {
  std::string args = type() + "[";
  for (auto b : branches) {
    args += b->getFormula();
    if (b != branches.back()) {
      args += ",";
    }
  }
  args += "]";
  return args;
}

std::vector<int> ABS_MTree::numBranches() { return requiredBranches; }

std::string IF_MTree::type() {
  return "IF"; // SET TYPE NAME HERE //
}

IF_MTree::IF_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
  branches = _branches;
  if (requiredBranches.size() != 0 &&
			find(requiredBranches.begin(), requiredBranches.end(),
           (int)branches.size()) == requiredBranches.end()) {
    std::cout << "  In " << type() << "_MTree::constructor - branches does not "
      "contain a legal number of element(s)!"
              << std::endl;
    std::cout << "    " << branches.size()
              << " elements were provided, but function requires : ";
    for (auto n : requiredBranches) {
      std::cout << n;
      if (n != requiredBranches.back()) {
        std::cout << " or ";
      }
    }
    std::cout << "  NOTE: values < 0 indicate requirement > abs(value)." << std::endl;
    exit(1);
  }
}

std::shared_ptr<Abstract_MTree>
IF_MTree::makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
  // make copy is needed to support brain (must perform a deep copy)
  if (_branches.size() == 0) {
    for (auto b : branches) {
      _branches.push_back(b->makeCopy());
    }
  }
  std::shared_ptr<Abstract_MTree> newTree = std::make_shared<IF_MTree>(_branches);
  return newTree;
}

std::vector<double>
IF_MTree::eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
     const std::vector<std::vector<double>> &vectorData) {
  return { (branches[0]->eval(dataMap, PT, vectorData)[0] > 0)
			? branches[1]->eval(dataMap, PT, vectorData)[0]
			: branches[2]->eval(dataMap, PT, vectorData)[0] };
}

void IF_MTree::show(int indent) {
  std::cout << std::string(indent, '\t') << "** " << type() << std::endl;
  indent++;
  for (auto b : branches) {
    b->show(indent);
  }
}

std::string IF_MTree::getFormula() {
  std::string args = type() + "[";
  for (auto b : branches) {
    args += b->getFormula();
    if (b != branches.back()) {
      args += ",";
    }
  }
  args += "]";
  return args;
}

std::vector<int> IF_MTree::numBranches() { return requiredBranches; }
