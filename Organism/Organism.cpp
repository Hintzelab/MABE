//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include <algorithm>

#include "../Global.h"
#include "../Organism/Organism.h"

#include "../Genome/AbstractGenome.h"
#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

/* Organism class (the one we expect to be used most of the time
 * has a genome, a brain, tools for lineage and ancestor tracking (for snapshot data saving method)
 */

int Organism::organismIDCounter = -1;  // every organism will get a unique ID

// this is used to hold the most recent common ancestor

/*
 * create an empty organism - it must be filled somewhere else.
 * parents is left empty (this is organism has no parents!)
 */
Organism::Organism(shared_ptr<ParametersTable> _PT) {
	PT = _PT; 
	genome = nullptr;
	brain = nullptr;
	ID = registerOrganism();
	alive = true;
	offspringCount = 0;  // because it's alive;
	//genomeAncestors.insert(ID);  // it is it's own Ancestor for genome tracking purposes
	ancestors.insert(ID);  // it is it's own Ancestor for data tracking purposes
	snapshotAncestors.insert(ID);
	timeOfBirth = Global::update;  // happy birthday!
	timeOfDeath = -1;  // still alive
	dataMap.Set("ID", ID);
	dataMap.Set("alive", alive);
	dataMap.Set("timeOfBirth", timeOfBirth);
}

/*
 * create a new organism given only a genome - since we do not know the type of brain we are using, we can not make the brain yet
 * parents is left empty (this is organism has no parents!)
 */
Organism::Organism(shared_ptr<AbstractGenome> _genome, shared_ptr<ParametersTable> _PT) {
	PT = _PT;
	genome = _genome;
	brain = nullptr;
	ID = registerOrganism();
	alive = true;
	offspringCount = 0;  // because it's alive;
	ancestors.insert(ID);  // it is it's own Ancestor for data tracking purposes
	snapshotAncestors.insert(ID);
	timeOfBirth = Global::update;  // happy birthday!
	timeOfDeath = -1;  // still alive
	dataMap.Set("ID", ID);
	dataMap.Set("alive", alive);
	dataMap.Set("timeOfBirth", timeOfBirth);
	dataMap.SetMany(genome->getStats());
}

Organism::Organism(shared_ptr<AbstractGenome> _genome, shared_ptr<AbstractBrain> _brain, shared_ptr<ParametersTable> _PT) {
	PT = _PT;
	genome = _genome;
	//cout << "in Organism::Organism(shared_ptr<AbstractGenome> _genome, shared_ptr<AbstractBrain> _brain)\n\tabout to make brain from genome"<<endl;
	brain = _brain->makeBrainFromGenome(genome);
	//cout << "\tmade brain from genome"<<endl;
	ID = registerOrganism();
	alive = true;
	offspringCount = 0;  // because it's alive;
	ancestors.insert(ID);  // it is it's own Ancestor for data tracking purposes
	snapshotAncestors.insert(ID);
	timeOfBirth = Global::update;  // happy birthday!
	timeOfDeath = -1;  // still alive
	dataMap.Set("ID", ID);
	dataMap.Set("alive", alive);
	dataMap.Set("timeOfBirth", timeOfBirth);
	dataMap.SetMany(genome->getStats());
	dataMap.SetMany(brain->getStats());
}

/*
 * create an organism with one parent
 * a brain is created with the assumption that the new brain should be of the same type as the parents brain
 */
Organism::Organism(shared_ptr<Organism> from, shared_ptr<AbstractGenome> _genome, shared_ptr<ParametersTable> _PT) {
	PT = _PT;
	genome = _genome;
	brain = from->brain->makeBrainFromGenome(genome);
	ID = registerOrganism();
	alive = true;
	offspringCount = 0;
	parents.push_back(from);
	from->offspringCount++;  // this parent has an(other) offspring
	for (auto ancestorID : from->ancestors) {
		ancestors.insert(ancestorID);  // union all parents ancestors into this organisms ancestor set.
	}
	for (auto ancestorID : from->snapshotAncestors) {
		snapshotAncestors.insert(ancestorID);  // union all parents ancestors into this organisms ancestor set.
	}
	timeOfBirth = Global::update;  // happy birthday!
	timeOfDeath = -1;  // still alive
	dataMap.Set("ID", ID);
	dataMap.Set("alive", alive);
	dataMap.Set("timeOfBirth", timeOfBirth);
	dataMap.SetMany(genome->getStats());
	dataMap.SetMany(brain->getStats());
}

/*
 * create an organism that has more than one parent
 * in this case the parent pointer is not used, and ancestor* sets are used instead to track lineage
 * a brain is created with the assumption that:
 * a) all organisms in from are the same
 * b) and the new brain should be of the same type as the parents brain
 */
Organism::Organism(const vector<shared_ptr<Organism>> from, shared_ptr<AbstractGenome> _genome, shared_ptr<ParametersTable> _PT) {
	PT = _PT;
	genome = _genome;
	brain = from[0]->brain->makeBrainFromGenome(genome);
	ID = registerOrganism();
	alive = true;
	offspringCount = 0;  // because it's alive;
	for (auto parent : from) {
		parents.push_back(parent);  // add this parent to the parents set
		parent->offspringCount++;  // this parent has an(other) offspring
		for (auto ancestorID : parent->ancestors) {
			ancestors.insert(ancestorID);  // union all parents ancestors into this organisms ancestor set
		}
		for (auto ancestorID : parent->snapshotAncestors) {
			snapshotAncestors.insert(ancestorID);  // union all parents ancestors into this organisms ancestor set.
		}
	}
	timeOfBirth = Global::update;  // happy birthday!
	timeOfDeath = -1;  // still alive
	dataMap.Set("ID", ID);
	dataMap.Set("alive", alive);
	dataMap.Set("timeOfBirth", timeOfBirth);
	dataMap.SetMany(genome->getStats());
	dataMap.SetMany(brain->getStats());
}

int Organism::registerOrganism() {
	return organismIDCounter++;;
}

Organism::~Organism() {
	for (auto parent : parents) {
		parent->offspringCount--;  // this parent has one less child in memory
	}
	parents.clear();
}

/*
 * called to kill an organism. Set alive to false
 */
void Organism::kill() {
	alive = false;
}

shared_ptr<Organism> Organism::makeMutatedOffspringFrom(shared_ptr<Organism> parent) {
	shared_ptr<Organism> newOrg = make_shared<Organism>(parent, genome->makeMutatedGenomeFrom(genome));
	return newOrg;
}

shared_ptr<Organism> Organism::makeMutatedOffspringFromMany(vector<shared_ptr<Organism>> from) {
	//cout << "In Organism::makeMutatedOffspringFromMany(vector<shared_ptr<Organism>> from)\n";
	vector<shared_ptr<AbstractGenome>> parentGenomes;
	for (auto p : from) {
		parentGenomes.push_back(p->genome);
	}
	shared_ptr<Organism> newOrg = make_shared<Organism>(from, genome->makeMutatedGenomeFromMany(parentGenomes));

	//cout << "  leaving Organism::makeMutatedOffspringFromMany(vector<shared_ptr<Organism>> from)\n";
	return newOrg;
}

/*
 * Given a genome and a key(to data that has been saved into "dataMap"
 * return a list of the value for key for genome and all genomes ancestors ordered oldest first
 */
vector<string> Organism::GetLODItem(string key, shared_ptr<Organism> org) {
	vector<string> list;
	list.insert(list.begin(), org->dataMap.Get(key));  // add this organisms data to the front of the list
	while (org->parents.size() == 1) {  // while the current org has one and only one parent
		org = org->parents[0];  // move to the next ancestor (since there is only one parent it is the element in the first position).
		list.insert(list.begin(), org->dataMap.Get(key));  // add that ancestors data to the front of the list
	}
	if (org->parents.size() > 1) {  // if more than one parent we have a problem!
		cout << "  In Organism::GetLODItem :: ERROR! an Organism has more than one parent! Can not establish LOD (do not use getLOD with Sexual Populations).\n  If you are using LODwAP, try using SSwD instead.\n  Exiting...\n";
		exit(1);
	}
	return list;
}

/*
 * Given a genome return a list of Organisms containing this Organism and all if this Organisms ancestors ordered oldest first
 * it will fail if any organism in the LOD has more then one parent. (!not for sexual reproduction!)
 */
vector<shared_ptr<Organism>> Organism::getLOD(shared_ptr<Organism> org) {
	vector<shared_ptr<Organism>> list;

	list.push_back(org);  // add this organism to the front of the LOD list
	while (org->parents.size() == 1) {  // while the current org has one and only one parent
		org = org->parents[0];  // move to the next ancestor (since there is only one parent it is the element in the first position).
		list.push_back(org);  // add that ancestor to the front of the LOD list
	}
	reverse(list.begin(), list.end());
	for (size_t i = 0; i < list.size(); i++) {
	}
	if (org->parents.size() > 1) {  // if more than one parent we have a problem!
		cout << "In Organism::getLOD(shared_ptr<Organism> org)\n Looks like you have enabled sexual reproduction.\nLOD only works with asexual populations. i.e. an offspring may have at most one parent.\nExiting!\n";
		exit(1);
	}
	return list;
}

/*
 * find the Most Recent Common Ancestor
 * uses getLOD to get a list of ancestors (oldest first). seaches the list for the first ancestor with a referenceCounter > 1
 * that is the first reference counter with more then one offspring.
 * If none are found, then return "from"
 * Note: a currently active genome has a referenceCounter = 1 (it has not reproduced yet, it only has 1 for it's self)
 *       a dead Organism with a referenceCounter = 0 will not be in the LOD (it has no offspring and will have been pruned)
 *       a dead Organism with a referenceCounter = 1 has only one offspring.
 *       a dead Organism with a referenceCounter > 1 has more then one spring with surviving lines of decent.
 */
shared_ptr<Organism> Organism::getMostRecentCommonAncestor(shared_ptr<Organism> org) {
	vector<shared_ptr<Organism>> LOD = getLOD(org);  // get line of decent parent "parent"
	for (auto org : LOD) {  // starting at the oldest parent, moving to the youngest
		if (org->offspringCount > 1)  // the first (oldest) ancestor with more then one surviving offspring
			return org;
	}
	return org;  // a currently active genome will have referenceCounter = 1 but may be the Most Recent Common Ancestor
}
shared_ptr<Organism> Organism::getMostRecentCommonAncestor(vector<shared_ptr<Organism>> LOD) {
	for (auto org : LOD) {  // starting at the oldest parent, moving to the youngest
		if (org->offspringCount > 1)  // the first (oldest) ancestor with more then one surviving offspring
			return org;
	}
	return LOD.back();  // a currently active genome will have referenceCounter = 1 but may be the Most Recent Common Ancestor
}

// clear all historical data (used when only saving real time data)
void Organism::clearHistory() {
	parents.clear();
}

shared_ptr<Organism> Organism::makeCopy(shared_ptr<ParametersTable> _PT) {
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newOrg = make_shared<Organism>(_PT); 
	newOrg->brain = brain->makeCopy(); 
	newOrg->genome = genome->makeCopy(); 
	newOrg->dataMap = dataMap; 
	newOrg->snapShotDataMaps = snapShotDataMaps;
	newOrg->score = score;
	newOrg->offspringCount = offspringCount;
	newOrg->parents = parents;
	for (auto parent : parents) {
		parent->offspringCount++; 
	}
	newOrg->ancestors = ancestors;
	newOrg->timeOfBirth = timeOfBirth;
	newOrg->timeOfDeath = timeOfDeath;
	newOrg->alive = alive;
	newOrg->ID = registerOrganism();
	return newOrg; 
}
