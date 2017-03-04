//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "MultiThreadTemplateWorld.h"

shared_ptr<ParameterLink<int>> MultiThreadTemplateWorld::nThreadsPL = Parameters::register_parameter("WORLD_MultiThreadTemplate-nThreads", 4, "number of threads to be used, WARNING: if you use more threads than cores you will seriously compromise speed!");


MultiThreadTemplateWorld::MultiThreadTemplateWorld(shared_ptr<ParametersTable> _PT) : AbstractWorld(_PT) {
    nThreads = (PT == nullptr) ? nThreadsPL->lookup() : PT->lookupInt("WORLD_MultiThreadTemplate-nThreads");
	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("score");
}

void MultiThreadTemplateWorld::evaluate(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
    int popSize = (int)groups["default"]->population.size();
    {
//        evaluateSolo(groups["default"]->population[i], analyse, visualize, debug);
        int chunksize=(int)groups["default"]->population.size()/nThreads;
        if(popSize%nThreads!=0){
            printf("Dude you try an uneven division of population size to threads!\n");
            exit(0);
        }
        
        pthread_t threads[nThreads];
        pthread_attr_t attr;
        void *status;
        
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        vector<thread_data*> tdCollector;
        for (int threadid=0; threadid < nThreads; threadid++){
            thread_data *td=new thread_data;
            tdCollector.push_back(td);
            td->chunkBegin=chunksize*threadid;
            td->chunkEnd=(chunksize*threadid)+chunksize;
            td->population=&groups["default"]->population;
            td->analyse=analyse;
            td->debug=debug;
            td->visualize=visualize;
            int rc = pthread_create(&threads[threadid],
                                    NULL,
                                    MultiThreadTemplateWorld::multiThreadEvaluate,
                                    (void *)(td));
            if (rc){
                cout << "Error:unable to create thread," << rc << endl;
                exit(-1);
            }
        }
        
        pthread_attr_destroy(&attr);
        for(int i=0; i < nThreads; i++ ){
            int rc = pthread_join(threads[i], &status);
            if (rc){
                cout << "Error:could not join thread" << rc << endl;
                exit(-1);
            }
        }
        for(int i=0;i<tdCollector.size();i++){
            delete tdCollector[i];
        }
        printf("Threads: %i ",nThreads);
    }
}

void *MultiThreadTemplateWorld::multiThreadEvaluate(void *threadArg){
    struct thread_data *myData;
    myData = (struct thread_data *) threadArg;
    for (int chunk_index=myData->chunkBegin; chunk_index<myData->chunkEnd; chunk_index++) {
        //chunk index is the organism to evaluate thus use: myData->population[chunk_index]
        multiThreadEvaluateSolo(myData->population->at(chunk_index), myData->analyse, myData->visualize, myData->debug);
    }
    pthread_exit(NULL);
}


void MultiThreadTemplateWorld::multiThreadEvaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
    for(int i=0;i<10000;i++)
        org->dataMap.Set("score",Random::getDouble(0.0,10.0));
    org->dataMap.Set("update", Global::update);
}

int MultiThreadTemplateWorld::requiredInputs() {
	return 1;
}
int MultiThreadTemplateWorld::requiredOutputs() {
	return 400;
}
