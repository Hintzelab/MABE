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

void Organism::initOrganism(shared_ptr<ParametersTable> _PT) {
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

///*
// * create a new organism given only a genome - since we do not know the type of brain we are using, we can not make the brain yet
// * parents is left empty (this is organism has no parents!)
// */
//Organism::Organism(shared_ptr<AbstractGenome> _genome, shared_ptr<ParametersTable> _PT) {
//	initOrganism(_PT);
//	if (_genome != nullptr) { // if an actual genome is passed
//		genome = _genome;
//		dataMap.Merge(genome->getStats());
//	} // else hasGenome will be false
//	ancestors.insert(ID);  // it is it's own Ancestor for data tracking purposes
//	snapshotAncestors.insert(ID);
//}

///*
// * create a new organism given only a brain - the brain passed will be installed as is (this constructor is mostly for brain->buildFromGenome is false
// * parents is left empty (this is organism has no parents!)
// * This constructor can also be used if you need to test a brain and don't want to bother with the genome!
// */
//Organism::Organism(shared_ptr<AbstractBrain> _brain, shared_ptr<ParametersTable> _PT) {
//	initOrganism(_PT);
//	if (_brain != nullptr) { // if an actual brain was passed
//		brain = _brain;
//		dataMap.Merge(brain->getStats());
//	} // else hasBrain will remain false and there will be no brain
//	ancestors.insert(ID);  // it is it's own Ancestor for data tracking purposes
//	snapshotAncestors.insert(ID);
//}

/*
 * create a new organism given a genome and a brain - this brain severs as a template which allows us to convert the genome into a brain
 * parents is left empty (this is organism has no parents!)
 */
Organism::Organism(shared_ptr<AbstractGenome> _genome, shared_ptr<AbstractBrain> _brain, shared_ptr<ParametersTable> _PT) {
	initOrganism(_PT);
	genome = _genome;
	if (genome != nullptr) { // if an actual genome is passed
		dataMap.Merge(genome->getStats());
	}
	if ((_brain != nullptr && !_brain->buildFromGenome) || (_brain!=nullptr && genome==nullptr)) {
		// if a brain is passed and brain is not buildFromGenome
		// or a brain is passes which is buildFromGenome, but there is no Genome
		// then install the passed brain.
		brain = _brain;
		dataMap.Merge(brain->getStats());
	} else if (_brain != nullptr && genome != nullptr) { // else, if we were passed a genome and a brain, and brain is buildFromGenome...
		brain = _brain->makeBrainFromGenome(genome); // ... build a brain from genome using passed brain as a template
		dataMap.Merge(brain->getStats());
	}
	ancestors.insert(ID);  // it is it's own Ancestor for data tracking purposes
	snapshotAncestors.insert(ID);
}

///*
// * given a genome, and a parent, create an organism with one parent
// * a brain is build from the genome using the parents brain as a template
// */
//Organism::Organism(shared_ptr<Organism> from, shared_ptr<AbstractGenome> _genome, shared_ptr<ParametersTable> _PT) {
//	cout << "in Organism(shared_ptr<Organism> from, shared_ptr<AbstractGenome> _genome, shared_ptr<ParametersTable> _PT)" << endl;
//	initOrganism(_PT);
//	if (_genome != nullptr) { // if an actual genome is passed
//		cout << " passed Genome != nullptr" << endl;
//		genome = _genome;
//		dataMap.Merge(genome->getStats());
//		cout << "got stats..." << endl;
//		cout << from << endl;
//		if (from->hasBrain() && from->brain->buildFromGenome) {// if parent has a brain and that brain is built from genome
//			cout << " build brain from genome!" << endl;
//			brain = from->brain->makeBrainFromGenome(genome); // build brain from genome using parents brain as template
//			dataMap.Merge(brain->getStats());
//		}
//		else if (from->hasBrain()) { // parent has genome and brain, but brain is not built from genome (this function will not assume you want to mutated brain)
//			cout << "  in Orgainsm constructor (from(parent), genome, PT) :: brain in parent is !buildFromGenome and no brain is provided to this function so Organism can not be created." << endl;
//			cout << "  - if you want to make a mutated an offspring with a mutated brain, you must use makeMutatedOffspringFrom()." << endl;
//			cout << "  - if you want a perfect copy of this organism, use the makeCopy() function." << endl;
//			cout << "    exiting." << endl;
//			exit(1);
//		} // else parent has no brain, brain here also = nullptr
//	} else { // parent has genome and brain, but brain is not built from genome (this function will not assume you want to mutated brain)
//		cout << "  in Orgainsm constructor (from(parent), genome, PT) :: genome passed is nullptr, I need a genome." << endl;
//		cout << "    exiting." << endl;
//		exit(1);
//	}
//	parents.push_back(from);
//	from->offspringCount++;  // this parent has an(other) offspring
//	for (auto ancestorID : from->ancestors) {
//		ancestors.insert(ancestorID);  // union all parents ancestors into this organisms ancestor set.
//	}
//	for (auto ancestorID : from->snapshotAncestors) {
//		snapshotAncestors.insert(ancestorID);  // union all parents ancestors into this organisms ancestor set.
//	}
//	cout << "... done" << endl;
//}

/*
 * given a genome, a brain, and a parent, create an organism with one parent
 *
 * if only genome is passed...
 *    and parent has no brain, then this brain will be nullptr
 *    and parent has brain, this brain will be built using parents brain and passed genome
 *
 * if only brain is passed...
 *    passed brain will be installed as is (it is possible to have a brain that is buildFromGenome, and NO genome)
 *
 * if brain and genome are passed...
 *    if brain is buildFromGenome, passed brain will use genome to build this brain
 *    if brain is not buildFromGenome, passed brain will be installed as is
 */
Organism::Organism(shared_ptr<Organism> from, shared_ptr<AbstractGenome> _genome, shared_ptr<AbstractBrain> _brain, shared_ptr<ParametersTable> _PT) {
	initOrganism(_PT);
	if (_genome != nullptr) { // if an actual genome is passed
		//cout << "    has genome" << endl;
		genome = _genome;
		dataMap.Merge(genome->getStats());
		if (_brain == nullptr && from->brain != nullptr && from->brain->buildFromGenome) { // we were not passed a brain, so a brain will be build using parent as template
			brain = from->brain->makeBrainFromGenome(genome); // ... build a brain from genome using passed brain as a template
			dataMap.Merge(brain->getStats());
		}
		else if (_brain != nullptr && _brain->buildFromGenome) {
			brain = _brain->makeBrainFromGenome(genome); // ... build a brain from genome using passed brain as a template
			dataMap.Merge(brain->getStats());
		}
	}

	if ((_brain != nullptr && !_brain->buildFromGenome) || (_brain != nullptr && genome == nullptr)) {
		// if a brain is passed and brain is not buildFromGenome
		// or a brain is passes which is buildFromGenome, but no Genome is passed
		// then install the passed brain.
		brain = _brain;
		dataMap.Merge(brain->getStats());
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

///*
// * create an organism that has more than one parent
// * in this case the parent pointer is not used, and ancestor* sets are used instead to track lineage
// * a brain is created using the first parents brain as a template
// * this function assumes that all parent organisms use the same type of brain (this is not checked!)
// */
//Organism::Organism(const vector<shared_ptr<Organism>> from, shared_ptr<AbstractGenome> _genome, shared_ptr<ParametersTable> _PT) {
//	initOrganism(_PT);
//	if (genome != nullptr) { // if an actual genome is passed
//		genome = _genome;
//		dataMap.Merge(genome->getStats());
//		if (from[0]->brain != nullptr && from[0]->brain->buildFromGenome) { // if parent[0] has a brain and that brain is built from genome
//			brain = from[0]->brain->makeBrainFromGenome(genome); // build brain from genome using parent[0]s brain as template
//			dataMap.Merge(brain->getStats());
//		} else if (from[0]->hasBrain() && !from[0]->brain->buildFromGenome) {
//			cout << "  in Orgainsm constructor (from(parents), genome, PT) :: brain in parents is !buildFromGenome and no brain is provided to this function so Organism can not be created." << endl;
//			cout << "  - if you want to make a mutated/crossed an offspring with a mutated/crossed brains, you must use makeMutatedOffspringFromMany()." << endl;
//			cout << "    exiting." << endl;
//			exit(1);
//		} // else parent has no brain, brain here also = nullptr
//	} // else no genome was provided. genome and brain will be nullptr... which is probably a bad thing... maybe should throw error here.
//	for (auto parent : from) {
//		parents.push_back(parent);  // add this parent to the parents set
//		parent->offspringCount++;  // this parent has an(other) offspring
//		for (auto ancestorID : parent->ancestors) {
//			ancestors.insert(ancestorID);  // union all parents ancestors into this organisms ancestor set
//		}
//		for (auto ancestorID : parent->snapshotAncestors) {
//			snapshotAncestors.insert(ancestorID);  // union all parents ancestors into this organisms ancestor set.
//		}
//	}
//}

/*
 * create an organism that has more than one parent
 * in this case the parent pointer is not used, and ancestor* sets are used instead to track lineage
 *
 * if only genome is passed...
 *    and parent[0] has no brain, then this brain will be nullptr
 *    and parent[0] has brain, this brain will be built using parent[0]s brain and passed genome
 *
 * if only brain is passed...
 *    passed brain will be installed as is (it is possible to have a brain that is buildFromGenome, and NO genome)
 *
 * if brain and genome are passed...
 *    if brain is buildFromGenome, passed brain will use genome to build this brain
 *    if brain is not buildFromGenome, passed brain will be installed as is
 */
Organism::Organism(const vector<shared_ptr<Organism>> from, shared_ptr<AbstractGenome> _genome, shared_ptr<AbstractBrain> _brain, shared_ptr<ParametersTable> _PT) {
	//cout << "  in Organism(const vector<shared_ptr<Organism>> from, shared_ptr<AbstractGenome> _genome, shared_ptr<AbstractBrain> _brain, shared_ptr<ParametersTable> _PT)" << endl;
	initOrganism(_PT);
	if (_genome != nullptr) { // if an actual genome is passed
		//cout << "has genome..." << endl;
		genome = _genome;
		dataMap.Merge(genome->getStats());
		if (_brain == nullptr && from[0]->brain != nullptr && from[0]->brain->buildFromGenome) { // we were not passed a brain, so a brain will be build using parent as template
			brain = from[0]->brain->makeBrainFromGenome(genome); // ... build a brain from genome using passed brain as a template
			dataMap.Merge(brain->getStats());
		}
	}
	if ((_brain != nullptr && !_brain->buildFromGenome) || (_brain != nullptr && genome == nullptr)) {
		// if a brain is passed and brain is not buildFromGenome
		// or a brain is passes which is buildFromGenome, but there is no Genome
		// then install the passed brain.
		brain = _brain;
		dataMap.Merge(brain->getStats());
	}
	else if (_brain != nullptr && genome != nullptr) { // else, if we were passed a genome and a brain, and brain is buildFromGenome...
		brain = _brain->makeBrainFromGenome(genome); // ... build a brain from genome using passed brain as a template
		dataMap.Merge(brain->getStats());
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
	if (!trackGenome) {
		genome = nullptr;
	}
}

shared_ptr<Organism> Organism::makeMutatedOffspringFrom(shared_ptr<Organism> from) {
	//cout << "In makeMutatedOffspringFrom" << endl;
	//cout << from->ID << "  genome? " << from->hasGenome << "  brain? " << from->hasBrain << endl;
	//if (from->hasBrain) {
	//	cout << "  buildFromGenome? " << from->brain->buildFromGenome << endl;
	//}
	shared_ptr<Organism> newOrg;
	if (from->hasGenome() && from->hasBrain() && from->brain->buildFromGenome) {
		//cout << "from->hasGenome && from->hasBrain && from->brain->buildFromGenome" << endl;
		// if parent has genome and brain which is built from genome, then make a new organism with mutated genome and brain template from parent
		newOrg = make_shared<Organism>(from, from->genome->makeMutatedGenomeFrom(from->genome), nullptr, PT);
	} else if (from->hasGenome() && from->hasBrain()) {
		//cout << "from->hasGenome && from->hasBrain" << endl;
		// if org has genome and brain, and brain is NOT built from genome, then make new organism with mutated genome and mutated brain
		newOrg = make_shared<Organism>(from, from->genome->makeMutatedGenomeFrom(from->genome), from->brain->makeMutatedBrainFrom(brain), PT);
	} else if (!from->hasGenome() && from->hasBrain() && !from->brain->buildFromGenome) {
		//cout << "!from->hasGenome && from->hasBrain && !from->brain->buildFromGenome" << endl;
		// if parent has no genome, but has a brain which is not built from genome then make new organism with a mutated brain
		newOrg = make_shared<Organism>(from, nullptr, from->brain->makeMutatedBrainFrom(brain), PT);
	} else {
		cout << "  in makeMutatedOffspringFrom() :: attempt to build new organism where brain->buildFromGenome = true, but parent has no genome. Exiting." << endl;
		exit(1);
	}

	if (!from->hasGenome() && !from->hasBrain()){
		cout << "  in Organism::makeMutatedOffspringFrom :: attempt to make offspring from parent which have neither genomes nor brains. Exiting." << endl;
		exit(1);
	}

	return newOrg;
}

shared_ptr<Organism> Organism::makeMutatedOffspringFromMany(vector<shared_ptr<Organism>> from) {
	//cout << "In Organism::makeMutatedOffspringFromMany(vector<shared_ptr<Organism>> from)"<<endl;
	shared_ptr<Organism> newOrg;

	if (from[0]->hasGenome()) { // if parents have genomes
		vector<shared_ptr<AbstractGenome>> parentGenomes; // make a list of parents genomes
		for (auto p : from) {
			parentGenomes.push_back(p->genome);
		}

		if (from[0]->hasBrain() && from[0]->brain->buildFromGenome){ // if parents have genome and brains and those brains are buildFromGenome
			newOrg = make_shared<Organism>(from, from[0]->genome->makeMutatedGenomeFromMany(parentGenomes),nullptr,PT);
		} else if (from[0]->hasBrain() && !from[0]->brain->buildFromGenome) { // if parents have genomes and brains and those brains are not buildFromGenome
			vector<shared_ptr<AbstractBrain>> parentBrains; // make a list of parents brains
			for (auto p : from) {
				parentBrains.push_back(p->brain);
			}

			newOrg = make_shared<Organism>(from, from[0]->genome->makeMutatedGenomeFromMany(parentGenomes), from[0]->brain->makeMutatedBrainFromMany(parentBrains), PT);

		} else if (!from[0]->hasBrain()){ // else if there is no brain, create org with mutated genome and no brain (nullptr)
			newOrg = make_shared<Organism>(from, from[0]->genome->makeMutatedGenomeFromMany(parentGenomes), nullptr, PT);
		}
	} else { // parents do not have genomes
		if (from[0]->hasBrain() && from[0]->brain->buildFromGenome){
			cout << "  in Organism::makeMutatedOffspringFromMany :: attempt to make offspring from parents with brains which are buildFromGenome, but parents do not have genomes. Exiting." << endl;
			exit(1);
		} else if (from[0]->hasBrain()){
			vector<shared_ptr<AbstractBrain>> parentBrains; // make a list of parents brains
			for (auto p : from) {
				parentBrains.push_back(p->brain);
			}
			//cout << "oooooooooooooooOOOOOOOOOOOOOOooooooooOOOOOooooooooooOoOoooooooooooooooOOOOOOOOOOooooooooooooooooooooooooooooooooooooooooooooooooooooooo" << endl;
			newOrg = make_shared<Organism>(from, nullptr, from[0]->brain->makeMutatedBrainFromMany(parentBrains), PT);
			//cout << " 0000000000000000000000000000000 " << endl;
			//cout << from[0]->brain->description() << endl;
			//cout << "bgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtgbgttgtg" << endl;
			//cout << " :: " << from[0]->brain->makeMutatedBrainFromMany(parentBrains)->description() << endl;
			//cout << "newOrg: " << newOrg << endl;
			//cout << "newOrg->brain: " << newOrg->brain << endl;
			//cout << newOrg->brain->description() << endl;
		}
	}
	if (!from[0]->hasGenome() && !from[0]->hasBrain()){
		cout << "  in Organism::makeMutatedOffspringFromMany :: attempt to make offspring from parents which have neither genomes nor brains. Exiting." << endl;
		exit(1);
	}

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
