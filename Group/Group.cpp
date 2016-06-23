//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#include "../Group/Group.h"

Group::Group() {  // create an empty Group
}

Group::Group(vector<shared_ptr<Organism>> _population, shared_ptr<AbstractOptimizer> _optimizer, shared_ptr<DefaultArchivist> _archivist) {
	population = _population;
	optimizer = _optimizer;
	archivist = _archivist;
}

Group::~Group() {
}

bool Group::archive(int flush) {
	return archivist->archive(population, flush);
}

void Group::optimize() {
	optimizer->makeNextGeneration(population);
}

