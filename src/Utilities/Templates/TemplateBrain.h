//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once					// directive to insure that this .h file is only included one time

#include <World/AbstractWorld.h> 


// If your brain is (or maybe) constructed using a genome, you must include AbstractGenome.h
#include <Genome/AbstractGenome.h>

// AbstractBrain defines all the basic function templates for brains
#include <Brain/AbstractBrain.h>

using namespace std;

class {{MODULE_NAME}}Brain : public AbstractBrain {
public:
	// If you are not familiar with parameters, parameters links
	// and MTree, you can find examples of these in TestWorld_h
	// and TestWorld_cpp

	// Create a PL to indicate the genome to be used to build this brain.
	// If your brain is a directly encoded brain, then you don't need this.
	// see the cpp file for more
	static shared_ptr<ParameterLink<string>> genomeNamePL;


  {{MODULE_NAME}}Brain() = delete;
  {{MODULE_NAME}}Brain(int /*_nrInNodes*/, int /*_nrOutNodes*/, shared_ptr<ParametersTable> /*PT_*/);

	virtual ~{{MODULE_NAME}}Brain() = default;

	// the update function converts inputs into outputs.
	// more on this in the cpp file
	virtual auto update() -> void override;

	// makeBrain() is a function which creates a brain.
	// more on this in the cpp file
	virtual auto makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& /*_genomes*/) -> shared_ptr<AbstractBrain> override;
	
	// make a copy of this brain (sort of, see cpp file)
	virtual auto makeCopy(shared_ptr<ParametersTable> PT_ = nullptr) -> shared_ptr<AbstractBrain> override;

	// makeMutatedBrainFrom and makeMutatedBrainFromMany are used
	// to produce offspring by direct encoded brains (brains encoded
	// from genomes are built from mutated genomes)
  virtual auto mutate() -> void override;

	// description returns a short description of this brain
	virtual auto description() -> string override;

	// getStats collects data on this brain
	virtual auto getStats(string& prefix) -> DataMap override;
	
	// return the type of this brain
	virtual auto getType() -> string override { return "{{MODULE_NAME}}"; }

	// clear all temporary values
	virtual auto resetBrain() -> void override;

	// set up inital genomes before building brains
	virtual auto initializeGenomes(unordered_map<string, shared_ptr<AbstractGenome>>& /*_genomes*/) -> void;

	// provide a list of genomes that this brain needs
	virtual auto requiredGenomes() -> unordered_set<string> override;

};

// This factory function exists to allow for the creation of an exampleBrain
// it is required in order for modules.h to function and provides a standard
// function format which can be created easily with MBuild.py
// Inside of this function, we now know the type of brain being constructed,
// and so can take addtional actions which may be required before this
// brains constructor can be called (in the case of multiGenome, this
// includes creating chromosomes which will be passed into the genome
// constructor. In Markov Brains, this function creates a GateListBuilder
// (a tool used to build markov brain gates) which is passed to the
// Markov Brain constructor.
inline shared_ptr<AbstractBrain> {{MODULE_NAME}}Brain_brainFactory(int ins, int outs, shared_ptr<ParametersTable> PT = Parameters::root) {
	return make_shared<{{MODULE_NAME}}Brain>(ins, outs, PT);
}
