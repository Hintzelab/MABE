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

#include <cwctype>
#include <cmath> // pow
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <unordered_set>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

#include "Utilities.h"
#include "Parameters.h"
#include "Data.h"
#include "Random.h"

#include "../Global.h"

class Abstract_MTree {
public:
	std::shared_ptr<ParametersTable> PT;
	std::vector<std::shared_ptr<Abstract_MTree>> branches;

	Abstract_MTree() = default;
	virtual ~Abstract_MTree() = default;

	virtual std::vector<double> eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
		const std::vector<std::vector<double>> &vectorData) = 0;
	virtual std::vector<double> eval(DataMap &dataMap);
	virtual std::vector<double> eval(std::shared_ptr<ParametersTable> PT);
  virtual std::vector<double> eval(std::vector<std::vector<double>> &vectorData);

	virtual std::vector<double> eval(DataMap &dataMap,
                                   std::shared_ptr<ParametersTable> PT);
	virtual std::vector<double> eval(DataMap &dataMap,
		const std::vector<std::vector<double>> &vectorData) {
		return eval(dataMap, nullptr, vectorData);
	}
	virtual std::vector<double> eval(std::shared_ptr<ParametersTable> PT,
		const std::vector<std::vector<double>> &vectorData) {
		DataMap dataMap;
		return eval(dataMap, PT, vectorData);
	}

	// virtual shared_ptr<Abstract_MTree> makeCopy() {};
	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) = 0;
	virtual void show(int indent = 0) {
		std::cout << std::string(indent, '\t')
			<< "show has not been defined for this MTree type!!" << std::endl;
	}
	virtual std::string getFormula() { return "**undefined**"; }
	virtual std::string type() = 0;
	virtual std::vector<int> numBranches() = 0;

	// return a vector of shared pointers to each node
	virtual void explode(std::shared_ptr<Abstract_MTree> tree,
                       std::vector<std::shared_ptr<Abstract_MTree>> &nodeList);
};

// return (int) branches[0] % (int) branches[1]
class MOD_MTree : public Abstract_MTree {
public:
	const std::vector<int> requiredBranches = {
		2 }; // set this value based on the needs of the function.
			 // -x for variable number of branches >= x
			 // empty vector = no requirement / any number of elements is fine

	virtual std::string type() override {
		return "MOD"; // SET TYPE NAME HERE //
	}

	MOD_MTree() = default;
	MOD_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches);

	virtual ~MOD_MTree() = default;

	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override;

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
         const std::vector<std::vector<double>> &vectorData) override;

	virtual void show(int indent = 0) override;
	virtual std::string getFormula() override;
	virtual std::vector<int> numBranches() override;
};

// return absolute value of first branch
class ABS_MTree : public Abstract_MTree {
public:
	const std::vector<int> requiredBranches = {
		1 }; // set this value based on the needs of the function.
			 // -x for variable number of branches >= x
			 // empty vector = no requirement / any number of elements is fine

	virtual std::string type() override;

	ABS_MTree() = default;
	ABS_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches);

	virtual ~ABS_MTree() = default;

	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override;

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
         const std::vector<std::vector<double>> &vectorData) override;

	virtual void show(int indent = 0) override;
	virtual std::string getFormula() override;
	virtual std::vector<int> numBranches() override;
};

// if first branch > 0 then second branch, else third branch
class IF_MTree : public Abstract_MTree {
public:
	const std::vector<int> requiredBranches = {
		3 }; // set this value based on the needs of the function.
			 // -x for variable number of branches >= x
			 // empty vector = no requirement / any number of elements is fine

	virtual std::string type() override;

	IF_MTree() = default;
	IF_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches);
	virtual ~IF_MTree() = default;

	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override;

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
         const std::vector<std::vector<double>> &vectorData) override;

	virtual void show(int indent = 0) override;
	virtual std::string getFormula() override;
	virtual std::vector<int> numBranches() override;
};

// return min of all banches
class MIN_MTree : public Abstract_MTree {
public:
	const std::vector<int> requiredBranches = { -2 }; // set this value based on the
													  // needs of the function. -1 for
													  // variable number of branches

	virtual std::string type() override {
		return "MIN"; // SET TYPE NAME HERE //
	}

	MIN_MTree() = default;
	MIN_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
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
			if (!OKay) {
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
				std::cout << std::endl;
				exit(1);
			}
		}
	}
	virtual ~MIN_MTree() = default;

	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		// make copy is needed to support brain (must perform a deep copy)
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<MIN_MTree>(_branches);
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		double minVal = branches[0]->eval(
			dataMap, PT, vectorData)[0]; // set min to first branch value
		for (int i = 1; i < branches.size(); i++) {
			minVal = std::min(minVal, branches[i]->eval(dataMap, PT, vectorData)[0]);
		}
		return { minVal };
	}

	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** " << type() << std::endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual std::string getFormula() override {
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
	virtual std::vector<int> numBranches() override { return requiredBranches; }
};

// return max of all banches
class MAX_MTree : public Abstract_MTree {
public:
	const std::vector<int> requiredBranches = { -2 }; // set this value based on the
													  // needs of the function. -1 for
													  // variable number of branches

	virtual std::string type() override {
		return "MAX"; // SET TYPE NAME HERE //
	}

	MAX_MTree() = default;
	MAX_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
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
			if (!OKay) {
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
				std::cout << std::endl;
				exit(1);
			}
		}
	}
	virtual ~MAX_MTree() = default;

	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		// make copy is needed to support brain (must perform a deep copy)
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<MAX_MTree>(_branches);
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		double maxVal = branches[0]->eval(
			dataMap, PT, vectorData)[0]; // set min to first branch value
		for (int i = 1; i < branches.size(); i++) {
			maxVal = std::max(maxVal, branches[i]->eval(dataMap, PT, vectorData)[0]);
		}
		return { maxVal };
	}

	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** " << type() << std::endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual std::string getFormula() override {
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
	virtual std::vector<int> numBranches() override { return requiredBranches; }
};

// return remaped first branch
// if one branch just return branch clamped at 0,1
// if three branches, return first branch clamped between [branch[1],branch[2]]
// and scaled to [0,1]
// if five brances, return first branch clamped as above and then scaled into
// [branch[3],branch[4]]
class REMAP_MTree : public Abstract_MTree {
public:
	const std::vector<int> requiredBranches = { 1, 3, 5 }; // set this value based on the
														   // needs of the function. -1
														   // for variable number of
														   // branches

	virtual std::string type() override {
		return "REMAP"; // SET TYPE NAME HERE //
	}

	REMAP_MTree() = default;
	REMAP_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
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
			if (!OKay) {
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
				std::cout << std::endl;
				exit(1);
			}
		}
	}
	virtual ~REMAP_MTree() = default;

	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		// make copy is needed to support brain (must perform a deep copy)
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<REMAP_MTree>(_branches);
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		auto v = branches[0]->eval(dataMap, PT, vectorData)[0];

		auto oldMin = (branches.size() > 2)
			? branches[1]->eval(dataMap, PT, vectorData)[0]
			: 0;
		auto oldMax = (branches.size() > 2)
			? branches[2]->eval(dataMap, PT, vectorData)[0]
			: 1;

		auto newMin = (branches.size() > 4)
			? branches[3]->eval(dataMap, PT, vectorData)[0]
			: 0;
		auto newMax = (branches.size() > 4)
			? branches[4]->eval(dataMap, PT, vectorData)[0]
			: 1;

		// if min and max are the same, return middle of new range
		// needed to take care of division by 0!
		if (oldMax == oldMin) {
			return { ((newMax + newMin) / 2) };
		}

		return { ((std::max(std::min(v, oldMax), oldMin) - oldMin) * (1 / (oldMax - oldMin)) *
			(newMax - newMin)) +
			newMin };
	}

	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** " << type() << std::endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual std::string getFormula() override {
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
	virtual std::vector<int> numBranches() override { return requiredBranches; }
};

// return value in [0,1] which is remaped first branch to an ease in ease out
// function, with the strength of funciton based on second branch
// with two branches, remapping will be for values between [0,1], values < lower
// bound return 0, values > upper bound return 1
// with four branches, bounds will be [branch[2],branch[3]]
class SIGMOID_MTree : public Abstract_MTree {
public:
	const std::vector<int> requiredBranches = { 2, 4 }; // set this value based on the
														// needs of the function. -1 for
														// variable number of branches

	virtual std::string type() override {
		return "SIGMOID"; // SET TYPE NAME HERE //
	}

	SIGMOID_MTree() = default;
	SIGMOID_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
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
			if (!OKay) {
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
				std::cout << std::endl;
				exit(1);
			}
		}
	}
	virtual ~SIGMOID_MTree() = default;

	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		// make copy is needed to support brain (must perform a deep copy)
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<SIGMOID_MTree>(_branches);
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		auto v = branches[0]->eval(dataMap, PT, vectorData)[0];
		auto e = branches[1]->eval(dataMap, PT, vectorData)[0];
		if (branches.size() > 2) { // if oldMin/oldMax are provided, use them
			auto oldMin = branches[2]->eval(dataMap, PT, vectorData)[0];
			auto oldMax = branches[3]->eval(dataMap, PT, vectorData)[0];
			v = ((std::max(std::min(v, oldMax), oldMin)) - oldMin) * (1 / (oldMax - oldMin));
		}
		else { // if not, clamp to [0,1]
			v = std::max(std::min(v, 1.0), 0.0);
		}
		double returnValue;
		if (v <= .5) {
			returnValue = pow(v * 2, e) / 2;
		}
		else {
			returnValue = 1 - pow((1 - v) * 2, e) / 2;
		}
		return { returnValue };
	}

	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** " << type() << std::endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual std::string getFormula() override {
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
	virtual std::vector<int> numBranches() override { return requiredBranches; }
};

class MANY_MTree : public Abstract_MTree {
public:
	MANY_MTree() = default;
	MANY_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
		branches = _branches;
	}
	virtual ~MANY_MTree() = default;
	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<MANY_MTree>(_branches);
		return newTree;
	}
	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		std::vector<double> output;
		for (auto b : branches) {
			output.push_back(b->eval(dataMap, PT, vectorData)[0]);
		}
		return output;
	}
	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** MANY\n" << std::endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual std::string getFormula() override {
		std::string args = "MANY(";
		for (auto b : branches) {
			args += b->getFormula();
			args += ",";
		}
		args.pop_back();
		args += ")";
		return args;
	}
	virtual std::string type() override { return "MANY"; }
	virtual std::vector<int> numBranches() override {
		return { -1 }; // at least 1 branch
	}
};

class CONST_MTree : public Abstract_MTree {
public:
	double value;

	CONST_MTree() { value = 0; }
	CONST_MTree(double _value) {
		value = _value;
		// std::cout << "made Const: " << value << std::endl;
	}
	virtual ~CONST_MTree() = default;
	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<CONST_MTree>(value);
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		std::vector<double> output;
		output.push_back(value);
		return output;
	}
	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** CONST\t" << value << std::endl;
	}
	virtual std::string getFormula() override { return std::to_string(value); }
	virtual std::string type() override { return "CONST"; }
	virtual std::vector<int> numBranches() override { return { 0 }; }
};

class fromDataMapAve_MTree : public Abstract_MTree {
public:
	std::string key;

	fromDataMapAve_MTree() {
	}
	fromDataMapAve_MTree(std::string _key) : key(_key) {}
	virtual ~fromDataMapAve_MTree() = default;
	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<fromDataMapAve_MTree>(key);
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		std::vector<double> output;
		output.push_back(dataMap.getAverage(key));
		return output;
	}
	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** fromDataMapAve_MTree\t\"" << key << "\""
			<< std::endl;
	}
	virtual std::string getFormula() override { return "DM_AVE[" + key + "]"; }
	virtual std::string type() override { return "DM_AVE"; }
	virtual std::vector<int> numBranches() override { return { 0 }; }
};

class fromDataMapSum_MTree : public Abstract_MTree {
public:
	std::string key;

	fromDataMapSum_MTree() {
	}
	fromDataMapSum_MTree(std::string _key) : key(_key) {}
	virtual ~fromDataMapSum_MTree() = default;
	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<fromDataMapSum_MTree>(key);
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		std::vector<double> output;
		output.push_back(dataMap.getSum(key));
		return output;
	}
	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** fromDataMapSum_MTree\t\"" << key << "\""
			<< std::endl;
	}
	virtual std::string getFormula() override { return "DM_SUM[" + key + "]"; }
	virtual std::string type() override { return "DM_SUM"; }
	virtual std::vector<int> numBranches() override { return { 0 }; }
};

class SUM_MTree : public Abstract_MTree {
public:
	SUM_MTree() = default;
	SUM_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
		branches = _branches;
		if (branches.size() < 2) {
			std::cout << "  In SUM_MTree::constructor - branches does not contain at "
				"least 2 elements!"
				<< std::endl;
			exit(1);
		}
	}
	virtual ~SUM_MTree() = default;
	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<SUM_MTree>(_branches);
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		std::vector<double> output;
		output.push_back(0);
		for (auto b : branches) {
			output[0] += b->eval(dataMap, PT, vectorData)[0];
		}
		return output;
	}
	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** SUM" << std::endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual std::string getFormula() override {
		std::string args = "(";
		for (auto b : branches) {
			args += b->getFormula();
			args += "+";
		}
		args.pop_back();
		args += ")";
		return args;
	}
	virtual std::string type() override { return "SUM"; }
	virtual std::vector<int> numBranches() override {
		return { -2 }; // one or more
	}
};

class MULT_MTree : public Abstract_MTree {
public:
	MULT_MTree() = default;
	MULT_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
		branches = _branches;
		if (branches.size() < 2) {
			std::cout << "  In MULT_MTree::constructor - branches does not contain at "
				"least 2 elements!"
				<< std::endl;
			exit(1);
		}
	}
	virtual ~MULT_MTree() = default;
	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<MULT_MTree>(_branches);
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		std::vector<double> output;
		output.push_back(1);
		for (auto b : branches) {
			output[0] *= b->eval(dataMap, PT, vectorData)[0];
		}
		return output;
	}
	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** MULT" << std::endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual std::string getFormula() override {
		std::string args = "(";
		for (auto b : branches) {
			args += b->getFormula();
			args += "*";
		}
		args.pop_back();
		args += ")";
		return args;
	}
	virtual std::string type() override { return "MULT"; }
	virtual std::vector<int> numBranches() override {
		return { -2 }; // one or more
	}
};

class SUBTRACT_MTree : public Abstract_MTree {
public:
	SUBTRACT_MTree() = default;
	SUBTRACT_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
		branches = _branches;
		if (branches.size() != 2) {
			std::cout << "  In DIVIDE_MTree::constructor - branches does not contain 2 "
				"elements!"
				<< std::endl;
			exit(1);
		}
	}
	virtual ~SUBTRACT_MTree() = default;
	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<SUBTRACT_MTree>(_branches);
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		return { branches[0]->eval(dataMap, PT, vectorData)[0] -
			branches[1]->eval(dataMap, PT, vectorData)[0] };
	}
	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** SUBTRACT" << std::endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual std::string getFormula() override {
		std::string args =
			"(" + branches[0]->getFormula() + "-" + branches[1]->getFormula() + ")";
		return args;
	}
	virtual std::string type() override { return "SUBTRACT"; }
	virtual std::vector<int> numBranches() override { return { 2 }; }
};

class DIVIDE_MTree : public Abstract_MTree {
public:
	DIVIDE_MTree() = default;
	DIVIDE_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
		branches = _branches;
		if (branches.size() != 2) {
			std::cout << "  In DIVIDE_MTree::constructor - branches does not contain 2 "
				"elements!"
				<< std::endl;
			exit(1);
		}
	}
	virtual ~DIVIDE_MTree() = default;
	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<DIVIDE_MTree>(_branches);
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		double denominator = branches[1]->eval(dataMap, PT, vectorData)[0];
		if (denominator == 0) {
			return { 0 };
		}
		else {
			return { branches[0]->eval(dataMap, PT, vectorData)[0] /
				branches[1]->eval(dataMap, PT, vectorData)[0] };
		}
	}
	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** DIVIDE" << std::endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual std::string getFormula() override {
		std::string args =
			"(" + branches[0]->getFormula() + "/" + branches[1]->getFormula() + ")";
		return args;
	}
	virtual std::string type() override { return "DIVIDE"; }
	virtual std::vector<int> numBranches() override { return { 2 }; }
};

class POW_MTree : public Abstract_MTree {
public:
	POW_MTree() = default;
	POW_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
		branches = _branches;
		if (branches.size() != 2) {
			std::cout << "  In POW_MTree::constructor - branches does not contain 2 "
				"elements!"
				<< std::endl;
			exit(1);
		}
	}
	virtual ~POW_MTree() = default;
	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<POW_MTree>(_branches);
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		return { pow(branches[0]->eval(dataMap, PT, vectorData)[0],
			branches[1]->eval(dataMap, PT, vectorData)[0]) };
	}
	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** POW" << std::endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual std::string getFormula() override {
		std::string args =
			"(" + branches[0]->getFormula() + "^" + branches[1]->getFormula() + ")";
		return args;
	}
	virtual std::string type() override { return "POW"; }
	virtual std::vector<int> numBranches() override { return { 2 }; }
};

class SIN_MTree : public Abstract_MTree {
public:
	SIN_MTree() = default;
	SIN_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
		branches = _branches;
		if (branches.size() != 1) {
			std::cout << "  In SIN_MTree::constructor - branches does not contain 1 "
				"element!"
				<< std::endl;
			exit(1);
		}
	}
	virtual ~SIN_MTree() = default;
	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<SIN_MTree>(_branches);
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		return { sin(branches[0]->eval(dataMap, PT, vectorData)[0]) };
	}
	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** SIN" << std::endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual std::string getFormula() override {
		std::string args = "SIN[" + branches[0]->getFormula() + "]";
		return args;
	}
	virtual std::string type() override { return "SIN"; }
	virtual std::vector<int> numBranches() override { return { 1 }; }
};

class COS_MTree : public Abstract_MTree {
public:
	COS_MTree() = default;
	COS_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
		branches = _branches;
		if (branches.size() != 1) {
			std::cout << "  In COS_MTree::constructor - branches does not contain 1 "
				"element!"
				<< std::endl;
			exit(1);
		}
	}
	virtual ~COS_MTree() = default;
	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<COS_MTree>(_branches);
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		return { cos(branches[0]->eval(dataMap, PT, vectorData)[0]) };
	}
	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** COS" << std::endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual std::string getFormula() override {
		std::string args = "COS[" + branches[0]->getFormula() + "]";
		return args;
	}
	virtual std::string type() override { return "COS"; }
	virtual std::vector<int> numBranches() override { return { 1 }; }
};

class VECT_MTree : public Abstract_MTree {
public:
	VECT_MTree() = default;
	VECT_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
		branches = _branches;
	}
	virtual ~VECT_MTree() = default;
	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<VECT_MTree>(_branches);
		return newTree;
	}
	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		int whichVect =
			std::max(0, (int)((branches[0]->eval(dataMap, PT, vectorData))[0]) %
			(int)vectorData.size());
		// int whichVect = 99909;
		int whichVal = std::max(0, (int)branches[1]->eval(dataMap, PT, vectorData)[0] %
			(int)vectorData[whichVect].size());
		if (Global::update > 759) {
			//	std::cout << "  In VECT::eval    whichVect: " << whichVect << "
			//whichVal: " << whichVal << std::endl;
			//	std::cout << "   ";
			//	for (auto a : vectorData) {
			//		for (auto b : a) {
			//			std::cout << b << " ";
			//		}
			//		std::cout << std::endl;
			//	}
		}
		return { vectorData[whichVect][whichVal] };
	}

	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** VECT" << std::endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual std::string getFormula() override {
		std::string args = "VECT[";
		for (auto b : branches) {
			args += b->getFormula();
			args += ",";
		}
		args.pop_back();
		args += "]";
		return args;
	}
	virtual std::string type() override { return "VECT"; }
	virtual std::vector<int> numBranches() override { return { 2 }; }
};

class RANDOM_MTree : public Abstract_MTree {
public:
	RANDOM_MTree() = default;
	RANDOM_MTree(std::vector<std::shared_ptr<Abstract_MTree>> _branches) {
		branches = _branches;
		if (branches.size() != 2) {
			std::cout << "  In RANDOM_MTree::constructor - branches does not contain 2 "
				"elements!"
				<< std::endl;
			exit(1);
		}
	}
	virtual ~RANDOM_MTree() = default;
	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<RANDOM_MTree>(_branches);
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		double min = branches[0]->eval(dataMap, PT, vectorData)[0];
		double max = branches[1]->eval(dataMap, PT, vectorData)[0];
		return { Random::getDouble(branches[0]->eval(dataMap, PT, vectorData)[0],
			branches[1]->eval(dataMap, PT, vectorData)[0]) };
	}
	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** RANDOM" << std::endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual std::string getFormula() override {
		std::string args = "RANDOM[" + branches[0]->getFormula() + "," +
			branches[1]->getFormula() + "]";
		return args;
	}
	virtual std::string type() override { return "RANDOM"; }
	virtual std::vector<int> numBranches() override { return { 2 }; }
};

class UPDATE_MTree : public Abstract_MTree {
public:
	UPDATE_MTree() {
	}
	virtual ~UPDATE_MTree() = default;
	virtual std::shared_ptr<Abstract_MTree>
		makeCopy(std::vector<std::shared_ptr<Abstract_MTree>> _branches = {}) override {
		std::shared_ptr<Abstract_MTree> newTree = std::make_shared<RANDOM_MTree>();
		return newTree;
	}

	virtual std::vector<double>
		eval(DataMap &dataMap, std::shared_ptr<ParametersTable> PT,
			const std::vector<std::vector<double>> &vectorData) override {
		return { (double)Global::update };
	}
	virtual void show(int indent = 0) override {
		std::cout << std::string(indent, '\t') << "** UPDATE" << std::endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual std::string getFormula() override {
		std::string args = "UPDATE";
		return args;
	}
	virtual std::string type() override { return "UPDATE"; }
	virtual std::vector<int> numBranches() override { return { 0 }; }
};

inline std::shared_ptr<Abstract_MTree>
stringToMTree(std::string formula) {
	// std::cout << "in MTree '" << formula << std::endl;

	std::shared_ptr<Abstract_MTree> newMTree;
	std::vector<std::shared_ptr<Abstract_MTree>> branches;
	char op = 'x';
	// setup /  define some things

	// make list of all ops so we can do lookups - this list needs to be updated
	// when new MTree classes are added
	std::vector<std::shared_ptr<Abstract_MTree>> allOps;
	allOps.push_back(std::make_shared<SUM_MTree>());
	allOps.push_back(std::make_shared<MULT_MTree>());
	allOps.push_back(std::make_shared<SUBTRACT_MTree>());
	allOps.push_back(std::make_shared<DIVIDE_MTree>());
	allOps.push_back(std::make_shared<POW_MTree>());
	allOps.push_back(std::make_shared<SIN_MTree>());
	allOps.push_back(std::make_shared<COS_MTree>());
	allOps.push_back(std::make_shared<MANY_MTree>());
	allOps.push_back(std::make_shared<VECT_MTree>());
	allOps.push_back(std::make_shared<RANDOM_MTree>());
	allOps.push_back(std::make_shared<IF_MTree>());
	allOps.push_back(std::make_shared<MIN_MTree>());
	allOps.push_back(std::make_shared<MAX_MTree>());
	allOps.push_back(std::make_shared<REMAP_MTree>());
	allOps.push_back(std::make_shared<SIGMOID_MTree>());
	allOps.push_back(std::make_shared<MOD_MTree>());
	allOps.push_back(std::make_shared<ABS_MTree>());
	// allOps.push_back(std::make_shared <CONST_MTree>());
	// allOps.push_back(std::make_shared <UPDATE_MTree>());

	for (auto op : allOps) { // first check to see if formula is just an op, if it
							 // is, just return an empty version of that type
		if (formula == op->type()) {
			return (op);
		}
	}

	// some ops are special (they can be represented by their symbol)
	char ops[] = { '+', '-', '*', '/', '^' }; // add %,|,&, etc)
											  // done with setup

	std::string currString = ""; // working string

	int index = 0; // where in the formula string are we looking?
	int formulaSize =
		(int)formula.size(); // size of the formula we are converting

	while (index < formulaSize) {
		// check to see if this formula is a const
		if (isdigit(formula[index]) || formula[index] == '.') {
			std::string constString = "";
			while (index < formulaSize &&
				(isdigit(formula[index]) || formula[index] == '.')) {
				constString.push_back(formula[index]);
				index++;
			}
			double constValue;
			if (convertString(constString, constValue)) {
				// std::cout << " make const " << constString << std::endl;
				// std::cout << "     constValue:" << constValue << std::endl;
				branches.push_back(std::make_shared<CONST_MTree>(constValue));
			}
		}
		// check to see if MTree is a DataMap lookup
		else if ((int)formula.size() > (index + 6) &&
			formula.substr(index, 6) == "DM_AVE") {
			// else if (formula.substr(index, 6) == "DM_AVE") {
			std::string argsString;     // = formula.substr(testType.size() + 1,
										// (formula.size() - testType.size()) - 2);
			index = index + 6 + 1; // move past 'DM_AVE['
			while (formula[index] != ']') {
				argsString.push_back(formula[index]);
				index++;
				if (index > formulaSize) {
					std::cout << "  In stringToMTree() :: while converting " << formula
						<< ", found unmatched '[' opening braket.\n  Exiting." << std::endl;
				}
			}
			index++; // move index to char after ']'
			branches.push_back(std::make_shared<fromDataMapAve_MTree>(argsString));
			// std::cout << "in DM_AVE['" << argsString << "'].  index = " << index <<
			// std::endl;
			// exit(1);
		}
		// check to see if MTree is a DataMap Sum lookup
		else if ((int)formula.size() > (index + 6) &&
			formula.substr(index, 6) == "DM_SUM") {
			// else if (formula.substr(index, 6) == "DM_SUM") {
			std::string argsString;     // = formula.substr(testType.size() + 1,
										// (formula.size() - testType.size()) - 2);
			index = index + 6 + 1; // move past 'DM_SUM['
			while (formula[index] != ']') {
				argsString.push_back(formula[index]);
				index++;
				if (index > formulaSize) {
					std::cout << "  In stringToMTree() :: while converting " << formula
						<< ", found unmatched '[' opening braket.\n  Exiting." << std::endl;
				}
			}
			index++; // move index to char after ']'
			branches.push_back(std::make_shared<fromDataMapSum_MTree>(argsString));
			// std::cout << "in DM['" << argsString << "'].  index = " << index << std::endl;
			// exit(1);
		}

		// check to see if MTree is a ParametersTable lookup
		else if (formula.substr(index, 2) == "PT") {
			std::cout << "PT MTree class is not implimented!" << std::endl;
			exit(1);
		}
		// check to see if MTree is UPDATE
		else if (formula.substr(index, 6) == "UPDATE") {
			branches.push_back(std::make_shared<UPDATE_MTree>());
			index = index + 6;
		}
		// check for'('
		else if (formula[index] == '(') {
			index++; // move past '('
			int nesting = 1;
			std::vector<std::string> argStrings;

			while (nesting > 0) {
				// std::cout << index << "  " << formula[index] << "  formula size: " <<
				// formulaSize << "  nesting: " << nesting << std::endl;
				if (index >= formulaSize) {
					std::cout << "  In stringToMTree() :: while converting " << formula
						<< ", found unmatched '(' opening parentheses.\n  Exiting."
						<< std::endl;
					exit(1);
				}
				if (formula[index] == '(') {
					nesting++;
				}
				if (formula[index] == ')') {
					nesting--;
				}
				currString += formula[index];
				index++;
			}
			currString.pop_back();
			branches.push_back(stringToMTree(currString));
			currString = "";
		}
		else if (index < formulaSize &&
			memchr(ops, formula[index], sizeof(ops))) { // if char at index
														// is an opperation
														// (+,-,*,/, etc)...
														//////std::cout << "found op: " << formula[index] << std::endl;
			if (op == 'x') {
				op = formula[index];
				//////std::cout << "    setting op" << op << std::endl;
			}
			else if (op != formula[index]) {
				std::cout << "  In stringToMTree() :: while converting " << formula
					<< ", found two diffrent operators! \"" << op << "\" and "
					<< formula[index]
					<< " in the same parentheses.\n  Please add parentheses to "
					"resolve oder of operations.\n  Exiting"
					<< std::endl;
				exit(1);
			}
			if (branches.size() == 0) {
				if (formula[index] == '-') {
					branches.push_back(std::make_shared<CONST_MTree>(0));
					index--;
				}
				else {
					std::cout << "  In stringToMTree() :: while converting " << formula
						<< ", found op \"" << op
						<< "\" with no precceding element.\n  Exiting" << std::endl;
					exit(1);
				}
			}
			index++;
		}
		else {
			// check to see if MTree is a MTree Class
			std::string testType = "";
			bool foundType = false;
			for (auto op : allOps) {
				testType = op->type();
				// std::cout << op->type() << "  -  " << formula.substr(index,
				// testType.size()) << std::endl;
				// std::cout << "   " << formula << "     " << index << std::endl;
				if (formula.substr(index, testType.size()) == testType) {
					// std::cout << "found type: '" << testType << "' in " << formula << std::endl;
					// get parameters, and build branches argument
					foundType = true;
					std::string argsString; // = formula.substr(testType.size() + 1,
											// (formula.size() - testType.size()) - 2);
					index = index + testType.size() + 1; // move past '['
					int nestingDepth = 0;
					int blockDepth = 0;
					while (formula[index] != ']' || nestingDepth != 0 ||
						blockDepth != 0) { // read args into a string till ']'
										   // std::cout << index << "  " << formula[index] << "  nestingDepth = " <<
										   // nestingDepth << "  blockDepth = " << blockDepth << std::endl;
						if (formula[index] == '(') {
							nestingDepth++;
						}
						else if (formula[index] == ')') {
							nestingDepth--;
							if (nestingDepth < 0) {
								std::cout << "  In stringToMTree() :: while converting " << formula
									<< ", found unmatched ')'.\n  Exiting." << std::endl;
								exit(1);
							}
						}
						if (formula[index] == '[') {
							blockDepth++;
						}
						else if (formula[index] == ']') {
							blockDepth--;
							if (blockDepth < 0) {
								std::cout << "  In stringToMTree() :: while converting " << formula
									<< ", found unmatched ']'.\n  Exiting." << std::endl;
								exit(1);
							}
						}
						argsString.push_back(formula[index]);
						index++;
						if (index > formulaSize) {
							if (nestingDepth != 0) {
								std::cout << "  In stringToMTree() :: while converting " << formula
									<< ", found unmatched '('.\n  Exiting." << std::endl;
							}
							else {
								std::cout << "  In stringToMTree() :: while converting " << formula
									<< ", found unmatched '[' opening braket.\n  Exiting."
									<< std::endl;
							}
							exit(1);
						}
					}
					index++; // move index to char after ']'
							 //////std::cout << argsString << std::endl;
					std::string arg;
					std::vector<std::shared_ptr<Abstract_MTree>> args;
					int argsIndex = 0;
					while (argsIndex <
						(int)argsString.size()) { // convert args string to MTree args
						if (argsString[argsIndex] == '(') {
							nestingDepth++;
						}
						else if (argsString[argsIndex] == ')') {
							nestingDepth--;
							if (nestingDepth < 0) {
								std::cout << "  In stringToMTree() :: while converting " << formula
									<< ", found unmatched ')'.\n  Exiting." << std::endl;
								exit(1);
							}
						}
						if (argsString[argsIndex] == '[') {
							blockDepth++;
						}
						else if (argsString[argsIndex] == ']') {
							blockDepth--;
							if (blockDepth < 0) {
								std::cout << "  In stringToMTree() :: while converting " << formula
									<< ", found unmatched ']'.\n  Exiting." << std::endl;
								exit(1);
							}
						}

						if (argsString[argsIndex] == ',' && nestingDepth == 0 &&
							blockDepth == 0) { // this is a ',' seperated list
							args.push_back(stringToMTree(arg));
							arg.clear();
						}
						else {
							arg.push_back(argsString[argsIndex]);
						}
						argsIndex++;
					}
					args.push_back(stringToMTree(arg));
					// op->show();
					branches.push_back(op->makeCopy(args));
					// index += testType.size() + argsString.size();
				}
			}
			if (!foundType) {
				std::cout << "  In MTree, not able to parse formula: '" << formula
					<< "'. exiting." << std::endl;
				exit(1);
			}
		}
	}

	if (op == 'x') { // no op, this must be a single term
		if (branches.size() > 1) {
			std::cout << "  In MTree, more then one argument provided with out an "
				"operator in formula: "
				<< formula << ". exiting." << std::endl;
			exit(1);
		}
		return branches[0];
	}
	else { // there is an op, convert all args and make MTree for arg
		if (op == '+') {
			auto oper = std::make_shared<SUM_MTree>(branches);
			return oper;
		}
		if (op == '-') {
			auto oper = std::make_shared<SUBTRACT_MTree>(branches);
			return oper;
		}
		if (op == '*') {
			auto oper = std::make_shared<MULT_MTree>(branches);
			return oper;
		}
		if (op == '/') {
			auto oper = std::make_shared<DIVIDE_MTree>(branches);
			return oper;
		}
		if (op == '^') {
			auto oper = std::make_shared<POW_MTree>(branches);
			return oper;
		}
	}

	std::cout << "can not convert formula: '" << formula << "' to MTree. Exiting."
		<< std::endl;
	exit(1);
}

