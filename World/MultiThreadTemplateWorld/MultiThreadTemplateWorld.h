//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__MultiThreadTemplateWorld__
#define __BasicMarkovBrainTemplate__MultiThreadTemplateWorld__

#include "../AbstractWorld.h"

#include <stdlib.h>
#include <thread>
#include <vector>
#include <pthread.h>




using namespace std;

struct thread_data{
    int thread_id;
    int chunkBegin,chunkEnd;
    int analyse;
    int visualize;
    int debug;
    vector<shared_ptr<Organism>> *population;
};



class MultiThreadTemplateWorld : public AbstractWorld {

public:
    
    static shared_ptr<ParameterLink<int>> nThreadsPL;
    int nThreads;

	MultiThreadTemplateWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~MultiThreadTemplateWorld() = default;

    virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyse = 0, int visualize = 0, int debug = 0) override;
	static void *multiThreadEvaluate(void *threadArg);
    
    static void multiThreadEvaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug);

	virtual int requiredInputs() override;
	virtual int requiredOutputs() override;

};

#endif /* defined(__BasicMarkovBrainTemplate__MultiThreadTemplateWorld__) */
