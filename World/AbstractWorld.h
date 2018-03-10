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

#include <stdlib.h>
#include <thread>
#include <vector>

#include "../Group/Group.h"
#include "../Utilities/Utilities.h"
#include "../Utilities/Data.h"
#include "../Utilities/Parameters.h"

using namespace std;

class AbstractWorld {
public:
	static shared_ptr<ParameterLink<bool>> debugPL;
	static shared_ptr<ParameterLink<string>> worldTypePL;
	
	const shared_ptr<ParametersTable> PT;

	int requiredInputs = 0;
	int requiredOutputs = 0;

	vector<string> popFileColumns;

	AbstractWorld(shared_ptr<ParametersTable> _PT) :
			PT(_PT) {
	}
	virtual ~AbstractWorld() = default;

	virtual unordered_map<string, unordered_set<string>> requiredGroups() = 0;// {
	//	string groupName = "root::";
	//	string brainName = "root::";
	//	return { { groupName,{"B:"+ brainName+","+to_string(requiredInputs)+","+to_string(requiredOutputs)}} }; // default requires a root group and a brain (in root namespace) and no genome 
	//}

	virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyze = 0, int visualize = 0, int debug = 0) {
		cout << "  chosen world does not define evaluate()! Exiting." << endl;
		exit(1);
	};
	virtual void evaluateSolo(shared_ptr<Organism> org, int analyze, int visualize, int debug) {
		cout << "  chosen world does not define evaluateSolo()! Exiting." << endl;
		exit(1);
	};
};
