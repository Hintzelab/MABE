//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

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
	optimizer->optimize(population);
}

void Group::cleanup() {
	optimizer->cleanup(population);
}

