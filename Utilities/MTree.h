//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__MTree__
#define __BasicMarkovBrainTemplate__MTree__

#include <cwctype>
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

using namespace std;

class Abstract_MTree {
public:

	shared_ptr<ParametersTable> PT;

	enum OPERATIONS {
		CONTAINER, MANY, CONST, fromDataMap, fromParameterTable, SUM, MULT, SUBTRACT, DIVIDE, SIN, COS
	};

	Abstract_MTree() = default;
	virtual ~Abstract_MTree() = default;

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT = nullptr) = 0;
	//virtual shared_ptr<Abstract_MTree> makeCopy() {};
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) = 0;
	virtual void show(int indent = 0) {
		cout << string(indent, '\t') << "show has not been defined for this MTree type!!" << endl;
	}
	virtual string getFormula() {
		return "**undefined**";
	}
	virtual string type() = 0;
};


class MANY_MTree : public Abstract_MTree {
public:
	vector<shared_ptr<Abstract_MTree>> branches;

	MANY_MTree() = default;
	MANY_MTree(vector<shared_ptr<Abstract_MTree>> _branches) : branches(_branches) {
	}
	virtual ~MANY_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<MANY_MTree>(_branches);
		return newTree;
	}
	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT = nullptr) override {
		vector<double> output;
		for (auto b : branches) {
			output.push_back(b->eval(dataMap)[0]);
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
};

class CONST_MTree : public Abstract_MTree {
public:
	double value;

	CONST_MTree() = default;
	CONST_MTree(double _value) : value(_value) {}
	virtual ~CONST_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) {
		shared_ptr<Abstract_MTree> newTree = make_shared<CONST_MTree>(value);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT = nullptr) override {
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
};

class fromDataMap_MTree : public Abstract_MTree {
public:

	string key;

	fromDataMap_MTree() = default;
	fromDataMap_MTree(string _key) : key(_key) {
	}
	virtual ~fromDataMap_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) {
		shared_ptr<Abstract_MTree> newTree = make_shared<fromDataMap_MTree>(key);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT = nullptr) override {
		vector<double> output;
		output.push_back(dataMap.GetAverage(key));
		return output;
	}
	virtual void show(int indent = 0) override {
		cout << string(indent, '\t') << "** fromDataMap\t\"" << key << "\"" << endl;
	}
	virtual string getFormula() override {
		return "DM[" + key + "]";
	}
	virtual string type() override {
		return "DM";
	}
};

class SUM_MTree : public Abstract_MTree {
public:
	vector<shared_ptr<Abstract_MTree>> branches;

	SUM_MTree() = default;
	SUM_MTree(vector<shared_ptr<Abstract_MTree>> _branches) : branches(_branches) {
	}
	virtual ~SUM_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<SUM_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT = nullptr) override {
		vector<double> output;
		output.push_back(0);
		for (auto b : branches) {
			output[0] += b->eval(dataMap)[0];
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
};

class MULT_MTree : public Abstract_MTree {
public:
	vector<shared_ptr<Abstract_MTree>> branches;

	MULT_MTree() = default;
	MULT_MTree(vector<shared_ptr<Abstract_MTree>> _branches) : branches(_branches) {
	}
	virtual ~MULT_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<MULT_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT = nullptr) override {
		vector<double> output;
		output.push_back(1);
		for (auto b : branches) {
			output[0] *= b->eval(dataMap)[0];
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
};


class SUBTRACT_MTree : public Abstract_MTree {
public:
	vector<shared_ptr<Abstract_MTree>> branches;

	SUBTRACT_MTree() = default;
	SUBTRACT_MTree(vector<shared_ptr<Abstract_MTree>> _branches) : branches(_branches) {
		if (branches.size() != 2) {
			cout << "  In DIVIDE_MTree::constructor - branches does not contain 2 elements!" << endl;
			exit(1);
		}
	}
	virtual ~SUBTRACT_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<SUBTRACT_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT = nullptr) override {
		vector<double> output;
		output.push_back(branches[0]->eval(dataMap)[0]);
		return{ branches[0]->eval(dataMap)[0] - branches[1]->eval(dataMap)[0] };
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
};

class DIVIDE_MTree : public Abstract_MTree {
public:
	vector<shared_ptr<Abstract_MTree>> branches;

	DIVIDE_MTree() = default;
	DIVIDE_MTree(vector<shared_ptr<Abstract_MTree>> _branches) : branches(_branches) {
		if (branches.size() != 2) {
			cout << "  In DIVIDE_MTree::constructor - branches does not contain 2 elements!" << endl;
			exit(1);
		}
	}
	virtual ~DIVIDE_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<DIVIDE_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT = nullptr) override {
		return{ branches[0]->eval(dataMap)[0] / branches[1]->eval(dataMap)[0] };
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
};


class SIN_MTree : public Abstract_MTree {
public:
	vector<shared_ptr<Abstract_MTree>> branches;

	SIN_MTree() = default;
	SIN_MTree(vector<shared_ptr<Abstract_MTree>> _branches) : branches(_branches) {
		if (branches.size() != 1) {
			cout << "  In SIN_MTree::constructor - branches does not contain 1 element!" << endl;
			exit(1);
		}
	}
	virtual ~SIN_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<SIN_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT = nullptr) override {
		vector<double> output;
		output.push_back(branches[0]->eval(dataMap)[0]);
		return{ sin(branches[0]->eval(dataMap)[0]) };
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
};


class COS_MTree : public Abstract_MTree {
public:
	vector<shared_ptr<Abstract_MTree>> branches;

	COS_MTree() = default;
	COS_MTree(vector<shared_ptr<Abstract_MTree>> _branches) : branches(_branches) {
		if (branches.size() != 1) {
			cout << "  In COS_MTree::constructor - branches does not contain 1 element!" << endl;
			exit(1);
		}
	}
	virtual ~COS_MTree() = default;
	virtual shared_ptr<Abstract_MTree> makeCopy(vector<shared_ptr<Abstract_MTree>> _branches = {}) {
		if (_branches.size() == 0) {
			for (auto b : branches) {
				_branches.push_back(b->makeCopy());
			}
		}
		shared_ptr<Abstract_MTree> newTree = make_shared<COS_MTree>(_branches);
		return newTree;
	}

	virtual vector<double> eval(DataMap& dataMap, shared_ptr<ParametersTable> PT = nullptr) override {
		vector<double> output;
		output.push_back(branches[0]->eval(dataMap)[0]);
		return{ cos(branches[0]->eval(dataMap)[0]) };
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
};


inline shared_ptr<Abstract_MTree> stringToMTree(string formula) {
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
	allOps.push_back(make_shared <SIN_MTree>());
	allOps.push_back(make_shared <COS_MTree>());
	allOps.push_back(make_shared <CONST_MTree>());
	allOps.push_back(make_shared <MANY_MTree>());

	// some ops are special (they can be represented by their symbol)
	char ops[] = { '+','-','*','/' }; // add %,^,|,&, etc)
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
				//////cout << " make const " << constString << endl;
				//////cout << "     constValue:" << constValue << endl;
				branches.push_back(make_shared<CONST_MTree>(constValue));
			}
		}
		// check to see if MTree is a DataMap lookup
		else if (formula.substr(index, 2) == "DM") {
			string argsString;// = formula.substr(testType.size() + 1, (formula.size() - testType.size()) - 2);
			index = index + 2 + 1; // move past 'DM['
			while (formula[index] != ']') {
				argsString.push_back(formula[index]);
				index++;
				if (index > formulaSize) {
					cout << "  In stringToMTree() :: while converting " << formula << ", found unmatched '[' opening braket.\n  Exiting." << endl;
				}
			}
			index++; // move index to char after ']'
			branches.push_back(make_shared<fromDataMap_MTree>(argsString));
			//cout << "in DM['" << argsString << "'].  index = " << index << endl;
			//exit(1);
		}

		// check to see if MTree is a ParametersTable lookup
		else if (formula.substr(index, 2) == "PT") {
			cout << "PT MTree class is not implimented!" << endl;
			exit(1);
		}
		// check for'('
		else if (formula[index] == '(') {
			index++; // move past '('
			int nesting = 1;
			vector<string> argStrings;

			while (nesting > 0) {
				//////cout << index << "  " << formula[index] << "  formula size: " << formulaSize << "  nesting: " << nesting << endl;
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
				cout << "  In stringToMTree() :: while converting " << formula << ", found op \"" << op << "\" with no precceding element.\n  Exiting" << endl;
				exit(1);
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
					//////cout << "found type: '" << testType << "' in " << formula << endl;
					//get parameters, and build branches argument
					foundType = true;
					string argsString;// = formula.substr(testType.size() + 1, (formula.size() - testType.size()) - 2);
					index = index + testType.size() + 1; // move past '['
					while (formula[index] != ']') {
						argsString.push_back(formula[index]);
						index++;
						if (index > formulaSize) {
							cout << "  In stringToMTree() :: while converting " << formula << ", found unmatched '[' opening braket.\n  Exiting." << endl;
						}
					}
					index++; // move index to char after ']'
					//////cout << argsString << endl;
					string arg;
					vector<shared_ptr<Abstract_MTree>> args;
					int argsIndex = 0;
					while (argsIndex < (int)argsString.size()) {

						if (argsString[argsIndex] == ',') {
							args.push_back(stringToMTree(arg));
							arg.clear();
						}
						else {
							arg.push_back(argsString[argsIndex]);
						}
						argsIndex++;
					}
					args.push_back(stringToMTree(arg));
					branches.push_back(op->makeCopy(args));
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
			return make_shared<SUM_MTree>(branches);
		}
		if (op == '-') {
			return make_shared<SUBTRACT_MTree>(branches);
		}
		if (op == '*') {
			return make_shared<MULT_MTree>(branches);
		}
		if (op == '/') {
			return make_shared<DIVIDE_MTree>(branches);
		}
	}

	cout << "can not convert formula: '" << formula << "' to MTree. Exiting." << endl;
	exit(1);
}


#endif /* defined(__BasicMarkovBrainTemplate__MTree__) */
