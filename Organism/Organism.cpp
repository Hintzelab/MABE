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

void Organism::initOrganism(shared_ptr<ParametersTable> _PT){
	PT = _PT;
	ID = registerOrganism();
	alive = true;
	offspringCount = 0;  // because it's alive;
	timeOfBirth = Global::update;  // happy birthday!
	timeOfDeath = -1;  // still alive
	dataMap.Set("ID", ID);
	dataMap.Set("alive", alive);
	dataMap.Set("timeOfBirth", timeOfBirth);

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
 * create a new organism given only a genome - since we do not know the type of brain we are using, we can not make the brain yet
 * parents is left empty (this is organism has no parents!)
 */
Organism::Organism(shared_ptr<AbstractGenome> _genome, shared_ptr<ParametersTable> _PT) {
	initOrganism(_PT);
	if (_genome != nullptr){ // if an actual genome is passed
		genome = _genome;
		hasGenome = true;
		dataMap.Merge(genome->getStats());
	} // else hasGenome will be false
	ancestors.insert(ID);  // it is it's own Ancestor for data tracking purposes
	snapshotAncestors.insert(ID);
}

/*
 * create a new organism given only a brain - the brain passed will be installed as is (this constructor is mostly for brain->buildFromGenome is false
 * parents is left empty (this is organism has no parents!)
 * This constructor can also be used if you need to test a brain and don't want to bother with the genome!
 */
Organism::Organism(shared_ptr<AbstractBrain> _brain, shared_ptr<ParametersTable> _PT) {
	initOrganism(_PT);
	if (_brain != nullptr){ // if an actual brain was passed
		brain = _brain;
		hasBrain = true;
		dataMap.Merge(brain->getStats());
	} // else hasBrain will remain false and there will be no brain
	ancestors.insert(ID);  // it is it's own Ancestor for data tracking purposes
	snapshotAncestors.insert(ID);
}

/*
 * create a new organism given a genome and a brain - this brain severs as a template which allows us to convert the genome into a brain
 * parents is left empty (this is organism has no parents!)
 */
Organism::Organism(shared_ptr<AbstractGenome> _genome, shared_ptr<AbstractBrain> _brain, shared_ptr<ParametersTable> _PT) {
	initOrganism(_PT);
	if (_genome != nullptr){ // if an actual genome is passed
		genome = _genome;
		hasGenome = true;
		dataMap.Merge(genome->getStats());
	}
	if (brain!=nullptr && !brain->buildFromGenome){ // if this orgs brain is not built from genome, then install the brain that was passed
		brain = _brain;
		hasBrain = true;
		dataMap.Merge(brain->getStats());
	} else if (_brain!=nullptr && _genome != nullptr){ // ... else, build a brain from genome using passes brain as a template
		brain = _brain->makeBrainFromGenome(genome);
		hasBrain = true;
		dataMap.Merge(brain->getStats());
	} // else either brain is nullprt (results in no brain) or brain is buildFromGenome, and genome is nullptr (also results in no brain)
	ancestors.insert(ID);  // it is it's own Ancestor for data tracking purposes
	snapshotAncestors.insert(ID);
}

/*
 * given a genome, and a parent, create an organism with one parent
 * a brain is build from the genome using the parents brain as a template
 */
Organism::Organism(shared_ptr<Organism> from, shared_ptr<AbstractGenome> _genome, shared_ptr<ParametersTable> _PT) {
	initOrganism(_PT);
	if (_genome != nullptr){ // if an actual genome is passed
		genome = _genome;
		hasGenome = true;
		dataMap.Merge(genome->getStats());
	} // else hasGenome will remain false and there will be no genome
	if (from->hasBrain && from->brain->buildFromGenome){ // if parent has a brain and that brain is built from genome
		brain = from->brain->makeBrainFromGenome(genome); // build brain from genome using parents brain as template
		hasBrain = true;
		dataMap.Merge(brain->getStats());
	} else if (from->hasBrain && !from->brain->buildFromGenome){
		cout << "  in Orgainsm constructor (from(parent), genome, PT) :: brain in parent is !buildFromGenome and no brain is provided to this function so Organism can not be created. Exiting." << endl;
		exit(1);
	} // else hasBrain will remain false and there will be no brain
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
 * given a genome, a brain, and a parent, create an organism with one parent
 * the brain and genome are installed as passed
 */
Organism::Organism(shared_ptr<Organism> from, shared_ptr<AbstractGenome> _genome, shared_ptr<AbstractBrain> _brain, shared_ptr<ParametersTable> _PT) {
	initOrganism(_PT);
	if (_genome != nullptr){ // if an actual genome is passed
		genome = _genome;
		hasGenome = true;
		dataMap.Merge(genome->getStats());
	} // else hasGenome will remain false and there will be no brain
	if (_brain != nullptr){ // if an actual brain is passed
		brain = _brain;
		hasBrain = true;
		dataMap.Merge(brain->getStats());
	} // else hasBrain will remain false and there will be no brain
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
 * create an organism that has more than one parent
 * in this case the parent pointer is not used, and ancestor* sets are used instead to track lineage
 * a brain is created using the first parents brain as a template
 * this function assumes that all parent organisms use the same type of brain (this is not checked!)
 */
Organism::Organism(const vector<shared_ptr<Organism>> from, shared_ptr<AbstractGenome> _genome, shared_ptr<ParametersTable> _PT) {
	initOrganism(_PT);
	if (genome != nullptr){ // if an actual genome is passed
		genome = _genome;
		hasGenome = true;
		dataMap.Merge(genome->getStats());
	} // else hasGenome will remain false and there will be no brain
	if (from[0]->brain != nullptr and from[0]->brain->buildFromGenome){ // if parent[0] has a brain and that brain is built from genome
		brain = from[0]->brain->makeBrainFromGenome(genome); // build brain from genome using parent[0]s brain as template
		hasBrain = true;
		dataMap.Merge(brain->getStats());
	} else if (from[0]->hasBrain && !from[0]->brain->buildFromGenome){
		cout << "  in Orgainsm constructor (from(parents), genome, PT) :: brain in parents is !buildFromGenome and no brain is provided to this function so Organism can not be created. Exiting." << endl;
		exit(1);
	} //else hasBrain will remain false and there will be no brain
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

/*
 * create an organism that has more than one parent
 * in this case the parent pointer is not used, and ancestor* sets are used instead to track lineage
 * a genome and brain are passed to this organism, and installed directly
 */
Organism::Organism(const vector<shared_ptr<Organism>> from, shared_ptr<AbstractGenome> _genome, shared_ptr<AbstractBrain> _brain, shared_ptr<ParametersTable> _PT) {
	initOrganism(_PT);
	//PT = _PT;
	if (genome != nullptr){ // if an actual genome is passed
		genome = _genome;
		hasGenome = true;
		dataMap.Merge(genome->getStats());
	} // else hasGenome will remain false and there will be no brain
	if (_brain != nullptr){ // if an actual brain is passed
		brain = _brain;
		hasBrain = true;
		dataMap.Merge(brain->getStats());
	} // else hasBrain will remain false and there will be no brain
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
	if (!trackGenome) {
		genome = nullptr;
	}
}

shared_ptr<Organism> Organism::makeMutatedOffspringFrom(shared_ptr<Organism> parent) {
	shared_ptr<Organism> newOrg;
	if(parent->hasGenome && parent->hasBrain && parent->brain->buildFromGenome){
		// if parent has genome and brain which is built from genome, then make a new organism with mutated genome and brain template from parent
		newOrg = make_shared<Organism>(parent, parent->genome->makeMutatedGenomeFrom(parent->genome),PT);
	} else if(parent->hasGenome && parent->hasBrain){
		// if org has genome and brain, and brain is NOT built from genome, then make new organism with mutated genome and mutated brain
		newOrg = make_shared<Organism>(parent, parent->genome->makeMutatedGenomeFrom(parent->genome), parent->brain);//->makeMutatedBrainFrom(brain),PT);
	} else if (!parent->hasGenome && parent->hasBrain && !parent->brain->buildFromGenome){
		// if parent has no genome, but has a brain which is not built from genome then make new organism with a mutated brain
		newOrg = make_shared<Organism>(parent, nullptr, parent->brain);//->makeMutatedBrainFrom(brain),PT);
	} else {
		cout << "  in makeMutatedOffspringFrom() :: attempt to build new organism where brain->buildFromGenome = true, but parent has no genome. Exiting." << endl;
		exit(1);
	}
	return newOrg;
}

shared_ptr<Organism> Organism::makeMutatedOffspringFromMany(vector<shared_ptr<Organism>> from) {
	//cout << "In Organism::makeMutatedOffspringFromMany(vector<shared_ptr<Organism>> from)"<<endl;
	vector<shared_ptr<AbstractGenome>> parentGenomes;
	for (auto p : from) {
		parentGenomes.push_back(p->genome);
	}
	shared_ptr<Organism> newOrg = make_shared<Organism>(from, genome->makeMutatedGenomeFromMany(parentGenomes));

	//cout << "  leaving Organism::makeMutatedOffspringFromMany(vector<shared_ptr<Organism>> from)"<<endl;
	return newOrg;
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
