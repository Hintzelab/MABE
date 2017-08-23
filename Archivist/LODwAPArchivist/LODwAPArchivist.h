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

#include "../DefaultArchivist.h"

using namespace std;

class LODwAPArchivist: public DefaultArchivist {  // Line of Decent with Active Pruning
public:

	static shared_ptr<ParameterLink<string>> LODwAP_Arch_dataSequencePL;  // how often to write out data
	static shared_ptr<ParameterLink<string>> LODwAP_Arch_organismSequencePL;  // how often to write out genomes
	static shared_ptr<ParameterLink<int>> LODwAP_Arch_pruneIntervalPL;  // how often to attempt to prune the LOD
	static shared_ptr<ParameterLink<int>> LODwAP_Arch_terminateAfterPL;  // how long to run after updates (to get better coalescence)
	static shared_ptr<ParameterLink<bool>> LODwAP_Arch_writeDataFilePL;  // if true, write data file
	static shared_ptr<ParameterLink<bool>> LODwAP_Arch_writeOrganismFilePL;  // if true, write genome file

	static shared_ptr<ParameterLink<string>> LODwAP_Arch_FilePrefixPL;

	vector<int> dataSequence;  // how often to write out data
	vector<int> organismSequence;  // how often to write out genomes
	int pruneInterval;  // how often to attempt to prune the LOD
	int terminateAfter;  // how long to run after updates (to get better coalescence)
	string DataFileName;  // name of the Data file
	string OrganismFileName;  // name of the Genome file (genomes on LOD)
	bool writeDataFile;  // if true, write data file
	bool writeOrganismFile;  // if true, write genome file

	int lastPrune;  // last time Genome was Pruned

	//// info about files under management
	int nextDataWrite;  // next time data files will be written to disk
	int nextOrganismWrite;  // next time a genome file will be written to genome.csv

	int dataSeqIndex;
	int organismSeqIndex;

	//int nextDataCheckPoint;  // next time data files contents need to be checkpointed (this is needed particularly to handle delay > interval)
	//int nextGenomeCheckPoint;  // next time a genome files contents need to be checkpointed (this is needed particularly to handle delay > interval)

	//unordered_map<int, vector<weak_ptr<Organism>>> checkpoints;  // used by SSwD only - this keeps lists of orgs that may be written (if they have living decendents)
	//// key is Global::nextGenomeWrite or Global::nextDataWrite
	LODwAPArchivist() = delete;
	LODwAPArchivist(vector<string> popFileColumns = { }, shared_ptr<Abstract_MTree> _maxFormula = nullptr, shared_ptr<ParametersTable> _PT = nullptr, string _groupPrefix ="");

	virtual ~LODwAPArchivist() = default;

	virtual bool archive(vector<shared_ptr<Organism>> population, int flush = 0) override;

	virtual bool isDataUpdate(int checkUpdate = -1) override;
	virtual bool isOrganismUpdate(int checkUpdate = -1) override;
};

