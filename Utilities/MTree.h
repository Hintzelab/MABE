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
#include <stdlib.h>
#include <cstring>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

#include "Utilities.h"
#include "Parameters.h"
#include "Data.h"

#include "../Global.h"

using namespace std;



/*

class EXAMPLE_MTree : public Abstract_MTree {
public:

	const vector<int> requiredBranches = { XXXX }; // set this value based on the needs of the function.
												// -x for variable number of branches >= x
												// empty vector = no requirement / any number of elements is fine

	virtual string type() override {
	return "EXAMPLE";				// SET TYPE NAME HERE //
	}

	EXAMPLE_MTree() = default;
	EXAMPLE_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (requiredBranches.size() != 0 && find(requiredBranches.begin(), requiredBranches.end(), (int)branches.size()) == requiredBranches.end()) {
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
				cout << "  In " << type() << "_MTree::constructor - branches does not contain a legal number of element(s)!" << endl;
				cout << "    " << branches.size() << " elements were provided, but function requires : ";
				for (auto n : requiredBranches) {
					cout << n;
					if (n != requiredBranches.back()) {
						cout << " or ";
					}
				}
				cout << "  NOTE: values < 0 indicate requirement > abs(value)." << endl;
				exit(1);
			}
		}
	}

	virtual ~EXAMPLE_MTree() = default;

	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
	// make copy is needed to support brain (must perform a deep copy)
	// copy any local data as well as all branches
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<EXAMPLE_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		double returnValue = branches[0]->eval(dataMap, PT, vectorData)[0]; // this will get value in first branch. '[0]' at end is because MTrees return vector<int>
		////////////////// 
		// DO MATH HERE //
		//////////////////
		return {returnValue}; // return vector with one element
	}

	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** "<< type() << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = type()+"[";
		for (auto b : branches) {
			args += b->getFormula();
			if (b != branches.back()){
				args += ",";
			}
		}
		args += "]";
		return args;
	}
	virtual vector<int> numBranches() override {
		return requiredBranches;
	}
};

*/




class Abstract_MTree {
public:

	shared_ptr<ParametersTable> PT;
	vector<shared_ptr<Abstract_MTree>> branches;
	shared_ptr<Abstract_MTree> parent;

	Abstract_MTree() = default;
	virtual ~Abstract_MTree() = default;


	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) = 0;
	virtual vector<double> eval(DataMap& dataMap) {
		vector<vector<double>> placeholder = {};
		return eval(dataMap, nullptr, placeholder);
	}
	virtual vector<double> eval(shared_ptr<ParametersTable> PT) {
		DataMap dataMap;
		vector<vector<double>> placeholder = {};
		return eval(dataMap, PT, placeholder);
	}
	virtual vector<double> eval(vector<vector<double>>& vectorData) {
		DataMap dataMap;
		return eval(dataMap, nullptr, vectorData);
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT) {
		vector<vector<double>> placeholder = {};
		return eval(dataMap, PT, placeholder);
	}
	virtual vector<double> eval(DataMap& dataMap, const vector<vector<double>>& vectorData) {
		return eval(dataMap, nullptr, vectorData);
	}
	virtual vector<double> eval(shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) {
		DataMap dataMap;
		return eval(dataMap, PT, vectorData);
	}

	//virtual shared_ptr<Abstract_MTree> makeCopy() {};
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) = 0;
	virtual void show(int indent = 0) {
		cout << string(indent, '\t') << "show has not been defined for this MTree type!!" << endl;
	}
	virtual string getFormula() {
		return "**undefined**";
	}
	virtual string type() = 0;
	virtual vector<int> numBranches() = 0;

	// return a vector of shared pointers to each node
	virtual void explode(shared_ptr<Abstract_MTree> tree, vector<shared_ptr<Abstract_MTree>>& nodeList) {
		nodeList.push_back(tree);
		for (auto b : tree->branches) {
			explode(b, nodeList);
		}
	}

};



// return (int) branches[0] % (int) branches[1]
class MOD_MTree : public Abstract_MTree {
public:

	const vector<int> requiredBranches = { 2 }; // set this value based on the needs of the function.
												// -x for variable number of branches >= x
												// empty vector = no requirement / any number of elements is fine

	virtual string type() override {
	return "MOD";				// SET TYPE NAME HERE //
	}

	MOD_MTree() = default;
	MOD_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (requiredBranches.size() != 0 && find(requiredBranches.begin(), requiredBranches.end(), (int)branches.size()) == requiredBranches.end()) {
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
				cout << "  In " << type() << "_MTree::constructor - branches does not contain a legal number of element(s)!" << endl;
				cout << "    " << branches.size() << " elements were provided, but function requires : ";
				for (auto n : requiredBranches) {
					cout << n;
					if (n != requiredBranches.back()) {
						cout << " or ";
					}
				}
				cout << "  NOTE: values < 0 indicate requirement > abs(value)." << endl;
				exit(1);
			}
		}
	}

	virtual ~MOD_MTree() = default;

	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
	// make copy is needed to support brain (must perform a deep copy)
	// copy any local data as well as all branches
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<MOD_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		int temp = ((int)branches[1]->eval(dataMap, PT, vectorData)[0] == 0) ? (int)1 : (int)branches[1]->eval(dataMap, PT, vectorData)[0];
		double returnValue = (int) branches[0]->eval(dataMap, PT, vectorData)[0] % temp;
		return { returnValue }; // return vector with one element
	}

	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** "<< type() << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = type()+"[";
		for (auto b : branches) {
			args += b->getFormula();
			if (b != branches.back()){
				args += ",";
			}
		}
		args += "]";
		return args;
	}
	virtual vector<int> numBranches() override {
		return requiredBranches;
	}
};



// return absolute value of first branch
class ABS_MTree : public Abstract_MTree {
public:

	const vector<int> requiredBranches = { 1 }; // set this value based on the needs of the function.
												   // -x for variable number of branches >= x
												   // empty vector = no requirement / any number of elements is fine

	virtual string type() override {
		return "ABS";				// SET TYPE NAME HERE //
	}

	ABS_MTree() = default;
	ABS_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (requiredBranches.size() != 0 && find(requiredBranches.begin(), requiredBranches.end(), (int)branches.size()) == requiredBranches.end()) {
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
				cout << "  In " << type() << "_MTree::constructor - branches does not contain a legal number of element(s)!" << endl;
				cout << "    " << branches.size() << " elements were provided, but function requires : ";
				for (auto n : requiredBranches) {
					cout << n;
					if (n != requiredBranches.back()) {
						cout << " or ";
					}
				}
				cout << "  NOTE: values < 0 indicate requirement > abs(value)." << endl;
				exit(1);
			}
		}
	}

	virtual ~ABS_MTree() = default;

	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		// make copy is needed to support brain (must perform a deep copy)
		// copy any local data as well as all branches
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<ABS_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		return { abs(branches[0]->eval(dataMap, PT, vectorData)[0]) }; // return vector with one element
	}

	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** " << type() << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = type() + "[";
		for (auto b : branches) {
			args += b->getFormula();
			if (b != branches.back()) {
				args += ",";
			}
		}
		args += "]";
		return args;
	}
	virtual vector<int> numBranches() override {
		return requiredBranches;
	}
};



// if first branch > 0 then second branch, else third branch
class IF_MTree : public Abstract_MTree {
public:

	const vector<int> requiredBranches = { 3 }; // set this value based on the needs of the function.
												// -x for variable number of branches >= x
												// empty vector = no requirement / any number of elements is fine

	virtual string type() override {
		return "IF";				// SET TYPE NAME HERE //
	}

	IF_MTree() = default;
	IF_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (requiredBranches.size() != 0 && find(requiredBranches.begin(), requiredBranches.end(), (int)branches.size()) == requiredBranches.end()) {
			cout << "  In " << type() << "_MTree::constructor - branches does not contain a legal number of element(s)!" << endl;
			cout << "    " << branches.size() << " elements were provided, but function requires : ";
			for (auto n : requiredBranches) {
				cout << n;
				if (n != requiredBranches.back()) {
					cout << " or ";
				}
			}
			cout << "  NOTE: values < 0 indicate requirement > abs(value)." << endl;
			exit(1);
		}
	}
	virtual ~IF_MTree() = default;

	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		// make copy is needed to support brain (must perform a deep copy)
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<IF_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		return { (branches[0]->eval(dataMap, PT, vectorData)[0] > 0) ? branches[1]->eval(dataMap, PT, vectorData)[0] : branches[2]->eval(dataMap, PT, vectorData)[0] };
	}

	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** " << type() << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = type() + "[";
		for (auto b : branches) {
			args += b->getFormula();
			if (b != branches.back()) {
				args += ",";
			}
		}
		args += "]";
		return args;
	}
	virtual vector<int> numBranches() override {
		return requiredBranches;
	}
};





// return min of all banches
class MIN_MTree : public Abstract_MTree {
public:

	const vector<int> requiredBranches = { -2 }; // set this value based on the needs of the function. -1 for variable number of branches

	virtual string type() override {
		return "MIN";				// SET TYPE NAME HERE //
	}

	MIN_MTree() = default;
	MIN_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (requiredBranches.size() != 0 && find(requiredBranches.begin(), requiredBranches.end(), (int)branches.size()) == requiredBranches.end()) {
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
				cout << "  In " << type() << "_MTree::constructor - branches does not contain a legal number of element(s)!" << endl;
				cout << "    " << branches.size() << " elements were provided, but function requires : ";
				for (auto n : requiredBranches) {
					cout << n;
					if (n != requiredBranches.back()) {
						cout << " or ";
					}
				}
				cout << endl;
				exit(1);
			}
		}
	}
	virtual ~MIN_MTree() = default;

	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		// make copy is needed to support brain (must perform a deep copy)
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<MIN_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		double minVal = branches[0]->eval(dataMap, PT, vectorData)[0]; // set min to first branch value
		for (int i = 1; i < branches.size(); i++) {
			minVal = min(minVal, branches[i]->eval(dataMap, PT, vectorData)[0]);
		}
		return { minVal };
	}

	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** " << type() << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = type() + "[";
		for (auto b : branches) {
			args += b->getFormula();
			if (b != branches.back()) {
				args += ",";
			}
		}
		args += "]";
		return args;
	}
	virtual vector<int> numBranches() override {
		return requiredBranches;
	}
};


// return max of all banches
class MAX_MTree : public Abstract_MTree {
public:

	const vector<int> requiredBranches = { -2 }; // set this value based on the needs of the function. -1 for variable number of branches

	virtual string type() override {
		return "MAX";				// SET TYPE NAME HERE //
	}

	MAX_MTree() = default;
	MAX_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (requiredBranches.size() != 0 && find(requiredBranches.begin(), requiredBranches.end(), (int)branches.size()) == requiredBranches.end()) {
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
				cout << "  In " << type() << "_MTree::constructor - branches does not contain a legal number of element(s)!" << endl;
				cout << "    " << branches.size() << " elements were provided, but function requires : ";
				for (auto n : requiredBranches) {
					cout << n;
					if (n != requiredBranches.back()) {
						cout << " or ";
					}
				}
				cout << endl;
				exit(1);
			}
		}
	}
	virtual ~MAX_MTree() = default;

	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		// make copy is needed to support brain (must perform a deep copy)
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<MAX_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		double maxVal = branches[0]->eval(dataMap, PT, vectorData)[0]; // set min to first branch value
		for (int i = 1; i < branches.size(); i++) {
			maxVal = max(maxVal, branches[i]->eval(dataMap, PT, vectorData)[0]);
		}
		return { maxVal };
	}

	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** " << type() << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = type() + "[";
		for (auto b : branches) {
			args += b->getFormula();
			if (b != branches.back()) {
				args += ",";
			}
		}
		args += "]";
		return args;
	}
	virtual vector<int> numBranches() override {
		return requiredBranches;
	}
};



// return remaped first branch
// if one branch just return branch clamped at 0,1
// if three branches, return first branch clamped between [branch[1],branch[2]] and scaled to [0,1]
// if five brances, return first branch clamped as above and then scaled into [branch[3],branch[4]]
class REMAP_MTree : public Abstract_MTree {
public:

	const vector<int> requiredBranches = { 1,3,5 }; // set this value based on the needs of the function. -1 for variable number of branches

	virtual string type() override {
		return "REMAP";				// SET TYPE NAME HERE //
	}

	REMAP_MTree() = default;
	REMAP_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (requiredBranches.size() != 0 && find(requiredBranches.begin(), requiredBranches.end(), (int)branches.size()) == requiredBranches.end()) {
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
				cout << "  In " << type() << "_MTree::constructor - branches does not contain a legal number of element(s)!" << endl;
				cout << "    " << branches.size() << " elements were provided, but function requires : ";
				for (auto n : requiredBranches) {
					cout << n;
					if (n != requiredBranches.back()) {
						cout << " or ";
					}
				}
				cout << endl;
				exit(1);
			}
		}
	}
	virtual ~REMAP_MTree() = default;

	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		// make copy is needed to support brain (must perform a deep copy)
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<REMAP_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		auto v = branches[0]->eval(dataMap, PT, vectorData)[0];
		
		auto oldMin = (branches.size() > 2) ? branches[1]->eval(dataMap, PT, vectorData)[0] : 0;
		auto oldMax = (branches.size() > 2) ? branches[2]->eval(dataMap, PT, vectorData)[0] : 1;

		auto newMin = (branches.size() > 4) ? branches[3]->eval(dataMap, PT, vectorData)[0] : 0;
		auto newMax = (branches.size() > 4) ? branches[4]->eval(dataMap, PT, vectorData)[0] : 1;
		
		return { ((max(min(v, oldMax), oldMin) - oldMin) * (1 / (oldMax - oldMin)) * (newMax - newMin)) + newMin };
	}

	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** " << type() << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = type() + "[";
		for (auto b : branches) {
			args += b->getFormula();
			if (b != branches.back()) {
				args += ",";
			}
		}
		args += "]";
		return args;
	}
	virtual vector<int> numBranches() override {
		return requiredBranches;
	}
};


// return value in [0,1] which is remaped first branch to an ease in ease out function, with the strength of funciton based on second branch
// with two branches, remapping will be for values between [0,1], values < lower bound return 0, values > upper bound return 1
// with four branches, bounds will be [branch[2],branch[3]]
class SIGMOID_MTree : public Abstract_MTree {
public:

	const vector<int> requiredBranches = { 2,4 }; // set this value based on the needs of the function. -1 for variable number of branches

	virtual string type() override {
		return "SIGMOID";				// SET TYPE NAME HERE //
	}

	SIGMOID_MTree() = default;
	SIGMOID_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (requiredBranches.size() != 0 && find(requiredBranches.begin(), requiredBranches.end(), (int)branches.size()) == requiredBranches.end()) {
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
				cout << "  In " << type() << "_MTree::constructor - branches does not contain a legal number of element(s)!" << endl;
				cout << "    " << branches.size() << " elements were provided, but function requires : ";
				for (auto n : requiredBranches) {
					cout << n;
					if (n != requiredBranches.back()) {
						cout << " or ";
					}
				}
				cout << endl;
				exit(1);
			}
		}
	}
	virtual ~SIGMOID_MTree() = default;

	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		// make copy is needed to support brain (must perform a deep copy)
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<SIGMOID_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		auto v = branches[0]->eval(dataMap, PT, vectorData)[0];
		auto e = branches[1]->eval(dataMap, PT, vectorData)[0];
		if (branches.size() > 2) { // if oldMin/oldMax are provided, use them
			auto oldMin = branches[2]->eval(dataMap, PT, vectorData)[0];
			auto oldMax = branches[3]->eval(dataMap, PT, vectorData)[0];
			v = ((max(min(v, oldMax), oldMin)) - oldMin) * (1 / (oldMax - oldMin));
		}
		else { // if not, clamp to [0,1]
			v = max(min(v, 1.0), 0.0);
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
		cout << string(indent, '\t') << "** " << type() << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = type() + "[";
		for (auto b : branches) {
			args += b->getFormula();
			if (b != branches.back()) {
				args += ",";
			}
		}
		args += "]";
		return args;
	}
	virtual vector<int> numBranches() override {
		return requiredBranches;
	}
};

class MANY_MTree : public Abstract_MTree {
public:

	MANY_MTree() = default;
	MANY_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
	}
	virtual ~MANY_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<MANY_MTree>(_branches);
		return newTree;
	}
	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		vector<double> output;
		for (auto b : branches) {
			output.push_back(b->eval(dataMap, PT, vectorData)[0]);
		}
		return output;
	}
	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** MANY\n" << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = "MANY(";
		for (auto b : branches) {
			args += b->getFormula();
			args += ",";
		}
		args.pop_back();
		args += ")";
		return args;
	}
	virtual string type() override {
		return "MANY";
	}
	virtual vector<int> numBranches() override {
		return { -1 }; // at least 1 branch
	}
};

class CONST_MTree : public Abstract_MTree {
public:
	double value;

	CONST_MTree() {
		value = 0;
	}
	CONST_MTree(double _value, shared_ptr<Abstract_MTree> _parent = nullptr){
		value = _value;
		parent = _parent;
		//cout << "made Const: " << value << endl;
	}
	virtual ~CONST_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		shared_ptr<Abstract_MTree> newTree = make_shared<CONST_MTree>(value);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		vector<double> output;
		output.push_back(value);
		return output;
	}
	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** CONST\t" << value << endl;
	}
	virtual string getFormula() override {
		return to_string(value);
	}
	virtual string type() override {
		return "CONST";
	}
	virtual vector<int> numBranches() override {
		return { 0 };
	}
};

class fromDataMapAve_MTree : public Abstract_MTree {
public:

	string key;

	fromDataMapAve_MTree(shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
	}
	fromDataMapAve_MTree(string _key) : key(_key) {
	}
	virtual ~fromDataMapAve_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		shared_ptr<Abstract_MTree> newTree = make_shared<fromDataMapAve_MTree>(key);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		vector<double> output;
		output.push_back(dataMap.getAverage(key));
		return output;
	}
	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** fromDataMapAve_MTree\t\"" << key << "\"" << endl;
	}
	virtual string getFormula() override {
		return "DM_AVE[" + key + "]";
	}
	virtual string type() override {
		return "DM_AVE";
	}
	virtual vector<int> numBranches() override {
		return { 0 };
	}
};


class fromDataMapSum_MTree : public Abstract_MTree {
public:

	string key;

	fromDataMapSum_MTree(shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
	}
	fromDataMapSum_MTree(string _key) : key(_key) {
	}
	virtual ~fromDataMapSum_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		shared_ptr<Abstract_MTree> newTree = make_shared<fromDataMapSum_MTree>(key);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		vector<double> output;
		output.push_back(dataMap.getSum(key));
		return output;
	}
	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** fromDataMapSum_MTree\t\"" << key << "\"" << endl;
	}
	virtual string getFormula() override {
		return "DM_SUM[" + key + "]";
	}
	virtual string type() override {
		return "DM_SUM";
	}
	virtual vector<int> numBranches() override {
		return { 0 };
	}
};

class SUM_MTree : public Abstract_MTree {
public:

	SUM_MTree() = default;
	SUM_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (branches.size() < 2) {
			cout << "  In SUM_MTree::constructor - branches does not contain at least 2 elements!" << endl;
			exit(1);
		}
	}
	virtual ~SUM_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<SUM_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		vector<double> output;
		output.push_back(0);
		for (auto b : branches) {
			output[0] += b->eval(dataMap, PT, vectorData)[0];
		}
		return output;
	}
	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** SUM" << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = "(";
		for (auto b : branches) {
			args += b->getFormula();
			args += "+";
		}
		args.pop_back();
		args += ")";
		return args;
	}
	virtual string type() override {
		return "SUM";
	}
	virtual vector<int> numBranches() override {
		return { -2 }; // one or more
	}
};

class MULT_MTree : public Abstract_MTree {
public:

	MULT_MTree() = default;
	MULT_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (branches.size() < 2) {
			cout << "  In MULT_MTree::constructor - branches does not contain at least 2 elements!" << endl;
			exit(1);
		}

	}
	virtual ~MULT_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<MULT_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		vector<double> output;
		output.push_back(1);
		for (auto b : branches) {
			output[0] *= b->eval(dataMap, PT, vectorData)[0];
		}
		return output;
	}
	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** MULT" << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = "(";
		for (auto b : branches) {
			args += b->getFormula();
			args += "*";
		}
		args.pop_back();
		args += ")";
		return args;
	}
	virtual string type() override {
		return "MULT";
	}
	virtual vector<int> numBranches() override {
		return { -2 }; // one or more
	}
};


class SUBTRACT_MTree : public Abstract_MTree {
public:

	SUBTRACT_MTree() = default;
	SUBTRACT_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (branches.size() != 2) {
			cout << "  In DIVIDE_MTree::constructor - branches does not contain 2 elements!" << endl;
			exit(1);
		}
	}
	virtual ~SUBTRACT_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<SUBTRACT_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		return{ branches[0]->eval(dataMap, PT, vectorData)[0] - branches[1]->eval(dataMap, PT, vectorData)[0] };
	}
	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** SUBTRACT" << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = "(" + branches[0]->getFormula() + "-" + branches[1]->getFormula() + ")";
		return args;
	}
	virtual string type() override {
		return "SUBTRACT";
	}
	virtual vector<int> numBranches() override {
		return { 2 };
	}
};

class DIVIDE_MTree : public Abstract_MTree {
public:

	DIVIDE_MTree() = default;
	DIVIDE_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (branches.size() != 2) {
			cout << "  In DIVIDE_MTree::constructor - branches does not contain 2 elements!" << endl;
			exit(1);
		}
	}
	virtual ~DIVIDE_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<DIVIDE_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		double denominator = branches[1]->eval(dataMap, PT, vectorData)[0];
		if (denominator == 0) {
			return { 0 };
		}
		else {
			return { branches[0]->eval(dataMap, PT, vectorData)[0] / branches[1]->eval(dataMap, PT, vectorData)[0] };
		}
	}
	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** DIVIDE" << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = "(" + branches[0]->getFormula() + "/" + branches[1]->getFormula() + ")";
		return args;
	}
	virtual string type() override {
		return "DIVIDE";
	}
	virtual vector<int> numBranches() override {
		return { 2 };
	}
};

class POW_MTree : public Abstract_MTree {
public:

	POW_MTree() = default;
	POW_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (branches.size() != 2) {
			cout << "  In POW_MTree::constructor - branches does not contain 2 elements!" << endl;
			exit(1);
		}
	}
	virtual ~POW_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<POW_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		return { pow( branches[0]->eval(dataMap, PT, vectorData)[0] , branches[1]->eval(dataMap, PT, vectorData)[0] ) };
	}
	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** POW" << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = "(" + branches[0]->getFormula() + "^" + branches[1]->getFormula() + ")";
		return args;
	}
	virtual string type() override {
		return "POW";
	}
	virtual vector<int> numBranches() override {
		return { 2 };
	}
};

class SIN_MTree : public Abstract_MTree {
public:

	SIN_MTree() = default;
	SIN_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (branches.size() != 1) {
			cout << "  In SIN_MTree::constructor - branches does not contain 1 element!" << endl;
			exit(1);
		}
	}
	virtual ~SIN_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<SIN_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		return{ sin(branches[0]->eval(dataMap, PT, vectorData)[0]) };
	}
	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** SIN" << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = "SIN[" + branches[0]->getFormula() + "]";
		return args;
	}
	virtual string type() override {
		return "SIN";
	}
	virtual vector<int> numBranches() override {
		return { 1 };
	}
};


class COS_MTree : public Abstract_MTree {
public:

	COS_MTree() = default;
	COS_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (branches.size() != 1) {
			cout << "  In COS_MTree::constructor - branches does not contain 1 element!" << endl;
			exit(1);
		}
	}
	virtual ~COS_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<COS_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		return{ cos(branches[0]->eval(dataMap, PT, vectorData)[0]) };
	}
	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** COS" << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = "COS[" + branches[0]->getFormula() + "]";
		return args;
	}
	virtual string type() override {
		return "COS";
	}
	virtual vector<int> numBranches() override {
		return { 1 };
	}
};

class VECT_MTree : public Abstract_MTree {
public:

	VECT_MTree() = default;
	VECT_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
	}
	virtual ~VECT_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<VECT_MTree>(_branches);
		return newTree;
	}
	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		int whichVect = max(0, (int)((branches[0]->eval(dataMap, PT, vectorData))[0]) % (int)vectorData.size());
		//int whichVect = 99909;
		int whichVal = max(0,(int)branches[1]->eval(dataMap, PT, vectorData)[0]%(int)vectorData[whichVect].size());
		if (Global::update > 759) {
		//	cout << "  In VECT::eval    whichVect: " << whichVect << "  whichVal: " << whichVal << endl;
		//	cout << "   ";
		//	for (auto a : vectorData) {
		//		for (auto b : a) {
		//			cout << b << " ";
		//		}
		//		cout << endl;
		//	}
		}
		return {vectorData[whichVect][whichVal]};
	}

	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** VECT" << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = "VECT[";
		for (auto b : branches) {
			args += b->getFormula();
			args += ",";
		}
		args.pop_back();
		args += "]";
		return args;
	}
	virtual string type() override {
		return "VECT";
	}
	virtual vector<int> numBranches() override {
		return { 2 };
	}
};

class RANDOM_MTree : public Abstract_MTree {
public:

	RANDOM_MTree() = default;
	RANDOM_MTree(vector<shared_ptr<Abstract_MTree>> _branches, shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
		branches = _branches;
		if (branches.size() != 2) {
			cout << "  In RANDOM_MTree::constructor - branches does not contain 2 elements!" << endl;
			exit(1);
		}
	}
	virtual ~RANDOM_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<RANDOM_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		double min = branches[0]->eval(dataMap, PT, vectorData)[0];
		double max = branches[1]->eval(dataMap, PT, vectorData)[0];
		return { Random::getDouble(branches[0]->eval(dataMap, PT, vectorData)[0], branches[1]->eval(dataMap, PT, vectorData)[0]) };
	}
	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** RANDOM" << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = "RANDOM[" + branches[0]->getFormula() + "," + branches[1]->getFormula() + "]";
		return args;
	}
	virtual string type() override {
		return "RANDOM";
	}
	virtual vector<int> numBranches() override {
		return { 2 };
	}
};

class UPDATE_MTree : public Abstract_MTree {
public:

	UPDATE_MTree(shared_ptr<Abstract_MTree> _parent = nullptr) {
		parent = _parent;
	}
	virtual ~UPDATE_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) override {
		shared_ptr<Abstract_MTree> newTree = make_shared<RANDOM_MTree>();
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT, const vector<vector<double>>& vectorData) override {
		return { (double)Global::update };
	}
	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** UPDATE" << endl;
		indent++;
		for (auto b : branches) {
			b->show(indent);
		}
	}
	virtual string getFormula() override {
		string args = "UPDATE";
		return args;
	}
	virtual string type() override {
		return "UPDATE";
	}
	virtual vector<int> numBranches() override {
		return { 0 };
	}
};

inline shared_ptr<Abstract_MTree> stringToMTree(string formula, shared_ptr<Abstract_MTree> parent = nullptr) {
	//cout << "in MTree '" << formula << endl;

	shared_ptr<Abstract_MTree> newMTree;
	vector<shared_ptr<Abstract_MTree>> branches;
	char op = 'x';
	// setup /  define some things

	// make list of all ops so we can do lookups - this list needs to be updated when new MTree classes are added
	vector<shared_ptr<Abstract_MTree>> allOps;
	allOps.push_back(make_shared <SUM_MTree>());
	allOps.push_back(make_shared <MULT_MTree>());
	allOps.push_back(make_shared <SUBTRACT_MTree>());
	allOps.push_back(make_shared <DIVIDE_MTree>());
	allOps.push_back(make_shared <POW_MTree>());
	allOps.push_back(make_shared <SIN_MTree>());
	allOps.push_back(make_shared <COS_MTree>());
	allOps.push_back(make_shared <MANY_MTree>());
	allOps.push_back(make_shared <VECT_MTree>());
	allOps.push_back(make_shared <RANDOM_MTree>());
	allOps.push_back(make_shared <IF_MTree>());
	allOps.push_back(make_shared <MIN_MTree>());
	allOps.push_back(make_shared <MAX_MTree>());
	allOps.push_back(make_shared <REMAP_MTree>());
	allOps.push_back(make_shared <SIGMOID_MTree>());
	allOps.push_back(make_shared <MOD_MTree>());
	allOps.push_back(make_shared <ABS_MTree>());
	//allOps.push_back(make_shared <CONST_MTree>());
	//allOps.push_back(make_shared <UPDATE_MTree>());

	for (auto op : allOps) { // first check to see if formula is just an op, if it is, just return an empty version of that type
		if (formula == op->type()) {
			return(op);
		}
	}

	// some ops are special (they can be represented by their symbol)
	char ops[] = { '+','-','*','/','^' }; // add %,|,&, etc)
	// done with setup

	string currString = ""; // working string

	int index = 0; // where in the formula string are we looking?
	int formulaSize = (int)formula.size(); // size of the formula we are converting

	while (index < formulaSize) {
		// check to see if this formula is a const
		if (isdigit(formula[index]) || formula[index] == '.') {
			string constString = "";
			while (index < formulaSize && (isdigit(formula[index]) || formula[index] == '.')) {
				constString.push_back(formula[index]);
				index++;
			}
			double constValue;
			if (stringToValue(constString, constValue)) {
				//cout << " make const " << constString << endl;
				//cout << "     constValue:" << constValue << endl;
				branches.push_back(make_shared<CONST_MTree>(constValue));
			}
		}
		// check to see if MTree is a DataMap lookup
		else if ((int)formula.size() > (index + 6) && formula.substr(index, 6) == "DM_AVE") {
		//else if (formula.substr(index, 6) == "DM_AVE") {
			string argsString;// = formula.substr(testType.size() + 1, (formula.size() - testType.size()) - 2);
			index = index + 6 + 1; // move past 'DM_AVE['
			while (formula[index] != ']') {
				argsString.push_back(formula[index]);
				index++;
				if (index > formulaSize) {
					cout << "  In stringToMTree() :: while converting " << formula << ", found unmatched '[' opening braket.\n  Exiting." << endl;
				}
			}
			index++; // move index to char after ']'
			branches.push_back(make_shared<fromDataMapAve_MTree>(argsString));
			//cout << "in DM_AVE['" << argsString << "'].  index = " << index << endl;
			//exit(1);
		}
		// check to see if MTree is a DataMap Sum lookup
		else if ((int)formula.size() > (index + 6) && formula.substr(index, 6) == "DM_SUM") {
		//else if (formula.substr(index, 6) == "DM_SUM") {
			string argsString;// = formula.substr(testType.size() + 1, (formula.size() - testType.size()) - 2);
			index = index + 6 + 1; // move past 'DM_SUM['
			while (formula[index] != ']') {
				argsString.push_back(formula[index]);
				index++;
				if (index > formulaSize) {
					cout << "  In stringToMTree() :: while converting " << formula << ", found unmatched '[' opening braket.\n  Exiting." << endl;
				}
			}
			index++; // move index to char after ']'
			branches.push_back(make_shared<fromDataMapSum_MTree>(argsString));
			//cout << "in DM['" << argsString << "'].  index = " << index << endl;
			//exit(1);
		}

		// check to see if MTree is a ParametersTable lookup
		else if (formula.substr(index, 2) == "PT") {
			cout << "PT MTree class is not implimented!" << endl;
			exit(1);
		}
		// check to see if MTree is UPDATE
		else if (formula.substr(index, 6) == "UPDATE") {
			branches.push_back(make_shared<UPDATE_MTree>());
			index = index + 6;
		}
		// check for'('
		else if (formula[index] == '(') {
			index++; // move past '('
			int nesting = 1;
			vector<string> argStrings;

			while (nesting > 0) {
				//cout << index << "  " << formula[index] << "  formula size: " << formulaSize << "  nesting: " << nesting << endl;
				if (index >= formulaSize) {
					cout << "  In stringToMTree() :: while converting " << formula << ", found unmatched '(' opening parentheses.\n  Exiting." << endl;
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
		else if (index < formulaSize && memchr(ops, formula[index], sizeof(ops))) { // if char at index is an opperation (+,-,*,/, etc)...
			//////cout << "found op: " << formula[index] << endl;
			if (op == 'x') {
				op = formula[index];
				//////cout << "    setting op" << op << endl;
			}
			else if (op != formula[index]) {
				cout << "  In stringToMTree() :: while converting " << formula << ", found two diffrent operators! \"" << op << "\" and " << formula[index] << " in the same parentheses.\n  Please add parentheses to resolve oder of operations.\n  Exiting" << endl;
				exit(1);
			}
			if (branches.size() == 0) {
				if (formula[index] == '-') {
					branches.push_back(make_shared<CONST_MTree>(0));
					index--;
				}
				else {
					cout << "  In stringToMTree() :: while converting " << formula << ", found op \"" << op << "\" with no precceding element.\n  Exiting" << endl;
					exit(1);
				}
			}
			index++;
		}
		else {
			// check to see if MTree is a MTree Class
			string testType = "";
			bool foundType = false;
			for (auto op : allOps) {
				testType = op->type();
				//cout << op->type() << "  -  " << formula.substr(index, testType.size()) << endl;
				//cout << "   " << formula << "     " << index << endl;
				if (formula.substr(index, testType.size()) == testType) {
					//cout << "found type: '" << testType << "' in " << formula << endl;
					//get parameters, and build branches argument
					foundType = true;
					string argsString;// = formula.substr(testType.size() + 1, (formula.size() - testType.size()) - 2);
					index = index + testType.size() + 1; // move past '['
					int nestingDepth = 0;
					int blockDepth = 0;
					while (formula[index] != ']' || nestingDepth != 0 || blockDepth != 0) { // read args into a string till ']'
						//cout << index << "  " << formula[index] << "  nestingDepth = " << nestingDepth << "  blockDepth = " << blockDepth << endl;
						if (formula[index] == '(') {
							nestingDepth++;
						}
						else if (formula[index] == ')') {
							nestingDepth--;
							if (nestingDepth < 0) {
								cout << "  In stringToMTree() :: while converting " << formula << ", found unmatched ')'.\n  Exiting." << endl;
								exit(1);
							}
						}
						if (formula[index] == '[') {
							blockDepth++;
						}
						else if (formula[index] == ']') {
							blockDepth--;
							if (blockDepth < 0) {
								cout << "  In stringToMTree() :: while converting " << formula << ", found unmatched ']'.\n  Exiting." << endl;
								exit(1);
							}
						}
						argsString.push_back(formula[index]);
						index++;
						if (index > formulaSize) {
							if (nestingDepth != 0) {
								cout << "  In stringToMTree() :: while converting " << formula << ", found unmatched '('.\n  Exiting." << endl;
							}
							else {
								cout << "  In stringToMTree() :: while converting " << formula << ", found unmatched '[' opening braket.\n  Exiting." << endl;
							}
							exit(1);
						}
					}
					index++; // move index to char after ']'
					//////cout << argsString << endl;
					string arg;
					vector<shared_ptr<Abstract_MTree>> args;
					int argsIndex = 0;
					while (argsIndex < (int)argsString.size()) { // convert args string to MTree args
						if (argsString[argsIndex] == '(') {
							nestingDepth++;
						}
						else if (argsString[argsIndex] == ')') {
							nestingDepth--;
							if (nestingDepth < 0) {
								cout << "  In stringToMTree() :: while converting " << formula << ", found unmatched ')'.\n  Exiting." << endl;
								exit(1);
							}
						}
						if (argsString[argsIndex] == '[') {
							blockDepth++;
						}
						else if (argsString[argsIndex] == ']') {
							blockDepth--;
							if (blockDepth < 0) {
								cout << "  In stringToMTree() :: while converting " << formula << ", found unmatched ']'.\n  Exiting." << endl;
								exit(1);
							}
						}

						if (argsString[argsIndex] == ',' && nestingDepth == 0 && blockDepth == 0) { // this is a ',' seperated list
							args.push_back(stringToMTree(arg));
							arg.clear();
						}
						else {
							arg.push_back(argsString[argsIndex]);
						}
						argsIndex++;
					}
					args.push_back(stringToMTree(arg));
					//op->show();
					branches.push_back(op->makeCopy(args));
					op->parent = parent;
					for (auto b : op->branches) {
						b->parent = op;
					}
					//index += testType.size() + argsString.size();
				}
			}
			if(!foundType){
				cout << "  In MTree, not able to parse formula: '" << formula << "'. exiting." << endl;
				exit(1);
			}
		}
	}

	if (op == 'x') { // no op, this must be a single term
		if (branches.size() > 1) {
			cout << "  In MTree, more then one argument provided with out an operator in formula: " << formula << ". exiting." << endl;
			exit(1);
		}
		return branches[0];
	}
	else { // there is an op, convert all args and make MTree for arg
		vector<shared_ptr<Abstract_MTree>> args;
		if (op == '+') {
			auto oper = make_shared<SUM_MTree>(branches,parent);
			for (auto b : oper->branches) {
				b->parent = oper;
			}
			return oper;
		}
		if (op == '-') {
			auto oper = make_shared<SUBTRACT_MTree>(branches, parent);
			for (auto b : oper->branches) {
				b->parent = oper;
			}
			return oper;
		}
		if (op == '*') {
			auto oper = make_shared<MULT_MTree>(branches, parent);
			for (auto b : oper->branches) {
				b->parent = oper;
			}
			return oper;
		}
		if (op == '/') {
			auto oper = make_shared<DIVIDE_MTree>(branches, parent);
			for (auto b : oper->branches) {
				b->parent = oper;
			}
			return oper;
		}
		if (op == '^') {
			auto oper = make_shared<POW_MTree>(branches, parent);
			for (auto b : oper->branches) {
				b->parent = oper;
			}
			return oper;
		}
	}

	cout << "can not convert formula: '" << formula << "' to MTree. Exiting." << endl;
	exit(1);
}

