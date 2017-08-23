//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

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

void Organism::initOrganism(shared_ptr<ParametersTable> _PT) {
	PT = _PT;
	ID = registerOrganism();
	alive = true;
	offspringCount = 0;  // because it's alive;
	timeOfBirth = Global::update;  // happy birthday!
	timeOfDeath = -1;  // still alive
	dataMap.set("ID", ID);
	dataMap.set("alive", alive);
	dataMap.set("timeOfBirth", timeOfBirth);
}


/*
 * create an empty organism - it must be filled somewhere else.
 * parents is left empty (this is organism has no parents!)
 */
Organism::Organism(shared_ptr<ParametersTable> _PT) {
	initOrganism(_PT);
	ancestors.insert(ID);  // it is it's own Ancestor for data tracking purposes
	snapshotAncestors.insert(ID);
}

/*
* create a new organism given genomes and brains - the grnome and brains passed with be installed as is (i.e. NOT copied)
* it is assumed that either this organism will never by used (it will serve as a template), or the brains have already been built elsewhere
* parents is set left unset/nullptr (no parents), and ancestor is set to self (this organism is the result of adigigenesis!)
*/
Organism::Organism(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes, unordered_map<string, shared_ptr<AbstractBrain>>& _brains, shared_ptr<ParametersTable> _PT) {
	initOrganism(_PT);
	
	genomes = _genomes;

	for (auto genome : genomes) { // collect stats from genomes
		string prefix;
		(genome.first == "root::") ? prefix = "" : prefix = genome.first;
		dataMap.merge(genome.second->getStats(prefix));
	}

	brains = _brains;

	for (auto brain : _brains) { // collect stats from brains
		string prefix;
		(brain.first == "root::") ? prefix = "" : prefix = brain.first;
		dataMap.merge(brain.second->getStats(prefix));
	}

	if (genomes.count("root::") == 0) {
		genome = nullptr;
	}
	else {
		genome = genomes["root::"];
	}

	if (brains.count("root::") == 0) {
		brain = nullptr;
	}
	else {
		brain = brains["root::"];
	}

	ancestors.insert(ID);  // it is it's own Ancestor for data tracking purposes
	snapshotAncestors.insert(ID);
}

/*
* create a new organism given a single parent, genomes and brains - the grnome and brains passed with be installed as is (i.e. NOT copied)
* it is assumed that either this organism will never by used (it will serve as a template), or the brains have already been built elsewhere
*/
Organism::Organism(shared_ptr<Organism> from, unordered_map<string, shared_ptr<AbstractGenome>>& _genomes, unordered_map<string, shared_ptr<AbstractBrain>>& _brains, shared_ptr<ParametersTable> _PT) {
	initOrganism(_PT);

	genomes = _genomes;

	for (auto genome : genomes) { // collect stats from genomes
		string prefix;
		(genome.first == "root::") ? prefix = "" : prefix = genome.first;
		dataMap.merge(genome.second->getStats(prefix));
	}

	brains = _brains;

	for (auto brain : _brains) { // collect stats from brains
		string prefix;
		(brain.first == "root::") ? prefix = "" : prefix = brain.first;
		dataMap.merge(brain.second->getStats(prefix));
	}

	if (genomes.count("root::") == 0) {
		genome = nullptr;
	}
	else {
		genome = genomes["root::"];
	}

	if (brains.count("root::") == 0) {
		brain = nullptr;
	}
	else {
		brain = brains["root::"];
	}

	parents.push_back(from);
	from->offspringCount++;  // this parent has an(other) offspring
	for (auto ancestorID : from->ancestors) {
		ancestors.insert(ancestorID);  // union all parents ancestors into this organisms ancestor set.
	}
	for (auto ancestorID : from->snapshotAncestors) {
		snapshotAncestors.insert(ancestorID);  // union all parents ancestors into this organisms ancestor set.
	}
}

/*
* create a new organism given a list of parents, genomes and brains - the grnome and brains passed with be installed as is (i.e. NOT copied)
* it is assumed that either this organism will never by used (it will serve as a template), or the brains have already been built elsewhere
*/
Organism::Organism(vector<shared_ptr<Organism>> from, unordered_map<string, shared_ptr<AbstractGenome>>& _genomes, unordered_map<string, shared_ptr<AbstractBrain>>& _brains, shared_ptr<ParametersTable> _PT) {
	initOrganism(_PT);

	genomes = _genomes;

	for (auto genome : genomes) { // collect stats from genomes
		string prefix;
		(genome.first == "root::") ? prefix = "" : prefix = genome.first;
		dataMap.merge(genome.second->getStats(prefix));
	}

	brains = _brains;

	for (auto brain : _brains) { // collect stats from brains
		string prefix;
		(brain.first == "root::") ? prefix = "" : prefix = brain.first;
		dataMap.merge(brain.second->getStats(prefix));
	}

	if (genomes.count("root::") == 0) {
		genome = nullptr;
	}
	else {
		genome = genomes["root::"];
	}

	if (brains.count("root::") == 0) {
		brain = nullptr;
	}
	else {
		brain = brains["root::"];
	}

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
}

// this function provides a unique ID value for every org
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
	timeOfDeath = Global::update;
	if (!trackOrganism) { // if the archivist is not tracking is organism, we can clear it's genomes and brains.
		genome = nullptr;
		brain = nullptr;
		genomes.clear();
		brains.clear();
	}
}

shared_ptr<Organism> Organism::makeMutatedOffspringFrom(shared_ptr<Organism> from) {

	unordered_map<string, shared_ptr<AbstractGenome>> newGenomes;
	unordered_map<string, shared_ptr<AbstractBrain>> newBrains;

	for (auto genome : from->genomes) {
		newGenomes[genome.first] = genome.second->makeMutatedGenomeFrom(genome.second);
	}

	for (auto brain : from->brains) {
		newBrains[brain.first] = brain.second->makeBrainFrom(brain.second,newGenomes);
		newBrains[brain.first]->mutate();
	}
	
	return make_shared<Organism>(from, newGenomes, newBrains, PT);
}

shared_ptr<Organism> Organism::makeMutatedOffspringFromMany(vector<shared_ptr<Organism>> from) {

	unordered_map<string, shared_ptr<AbstractGenome>> newGenomes;
	unordered_map<string, shared_ptr<AbstractBrain>> newBrains;

	for (auto genome : from[0]->genomes) {
		vector<shared_ptr<AbstractGenome>> parentGenomes; // make a list of parents genomes
		for (auto p : from) {
			parentGenomes.push_back(p->genomes[genome.first]);
		}
		newGenomes[genome.first] = genome.second->makeMutatedGenomeFromMany(parentGenomes);


	}

	for (auto brain : from[0]->brains) {
		vector<shared_ptr<AbstractBrain>> parentBrains; // make a list of parents genomes
		for (auto p : from) {
			parentBrains.push_back(p->brains[brain.first]);
		}

		newBrains[brain.first] = brain.second->makeBrainFromMany(parentBrains, newGenomes);
		newBrains[brain.first]->mutate();
	}

	return make_shared<Organism>(from, newGenomes, newBrains, PT);
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


shared_ptr<Organism> Organism::makeCopy(shared_ptr<ParametersTable> _PT) {
	auto newOrg = make_shared<Organism>(_PT);
	for (auto genome : genomes) {
		newOrg->genomes[genome.first] = genome.second->makeCopy(genome.second->PT);
	}
	for (auto brain : brains) {
		newOrg->brains[brain.first] = brain.second->makeCopy(brain.second->PT);
	}

	if (newOrg->genomes.count("root::") == 0) {
		newOrg->genome = nullptr;
	}
	else {
		newOrg->genome = newOrg->genomes["root::"];
	}

	if (newOrg->brains.count("root::") == 0) {
		newOrg->brain = nullptr;
	}
	else {
		newOrg->brain = newOrg->brains["root::"];
	}

	newOrg->dataMap = dataMap;
	newOrg->snapShotDataMaps = snapShotDataMaps;
	newOrg->offspringCount = offspringCount;
	newOrg->parents = parents;
	for (auto parent : parents) {
		parent->offspringCount++;
	}
	newOrg->ancestors = ancestors;
	newOrg->timeOfBirth = timeOfBirth;
	newOrg->timeOfDeath = timeOfDeath;
	newOrg->alive = alive;
	return newOrg;
}
