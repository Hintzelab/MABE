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

#include "../Archivist/DefaultArchivist.h"
#include "../Global.h"
#include "../Optimizer/AbstractOptimizer.h"
#include "../Organism/Organism.h"

using namespace std;

class Group {
 public:
	vector<shared_ptr<Organism>> population;
	shared_ptr<Organism> templateOrg;
	shared_ptr<DefaultArchivist> archivist;
	shared_ptr<AbstractOptimizer> optimizer;

	Group();
	Group(vector<shared_ptr<Organism>> _population, shared_ptr<AbstractOptimizer> _optimizer, shared_ptr<DefaultArchivist> _archivist);
	~Group();

	bool archive(int flush = 0);
	void optimize();
	void cleanup();
};

