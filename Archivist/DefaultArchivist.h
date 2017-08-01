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

#include <algorithm>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "../Global.h"
#include "../Organism/Organism.h"
#include "../Utilities/MTree.h"

using namespace std;

class DefaultArchivist {
 public:



	/*shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_outputMethodStrPL = Parameters::register_parameter("ARCHIVIST_OUTPUT_METHOD-outputMethod", (string) "default", "output method, [default, LODwAP (Line of Decent with Aggressive Pruning), snapshot, SSwD (SnapShot with Delay)]");  // string parameter for outputMethod;

shared_ptr<ParameterLink<int>> DefaultArchivist::Arch_realtimeFilesIntervalPL = Parameters::register_parameter("ARCHIVIST-realtimeFilesInterval", 10, "How often to write to realtime data files");
shared_ptr<ParameterLink<bool>> DefaultArchivist::Arch_writePopFilePL = Parameters::register_parameter("ARCHIVIST-writePopFile", true, "Save data to average file?");
shared_ptr<ParameterLink<bool>> DefaultArchivist::Arch_writeMaxFilePL = Parameters::register_parameter("ARCHIVIST-writeMaxFile", true, "Save data to Max file?");
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_PopFileNamePL = Parameters::register_parameter("ARCHIVIST-popFileName", (string) "pop.csv", "name of average file (saves population averages)");
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_MaxFileNamePL = Parameters::register_parameter("ARCHIVIST-MaxFileName", (string) "Max.csv", "name of Max file (saves data on Max organism)");
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_DefaultPopFileColumnNamesPL = Parameters::register_parameter("ARCHIVIST-popFileColumns", (string) "[]", "data to be saved into average file (must be values that can generate an average). If empty, MABE will try to figure it out");
shared_ptr<ParameterLink<bool>> DefaultArchivist::Arch_MaxFileShowAllListsPL = Parameters::register_parameter("ARCHIVIST-MaxFileShowAllLists", true, "lists named 'all'* in data map will be averaged and added to file. if true, raw 'all'* lists will also be added to the file");
	 */

	static shared_ptr<ParameterLink<string>> Arch_outputMethodStrPL;  // string parameter for outputMethod;

	static shared_ptr<ParameterLink<bool>> Arch_writeMaxFilePL;  // if true, Max file will be created
	static shared_ptr<ParameterLink<bool>> Arch_writePopFilePL;  // if true, pop file will be created
	static shared_ptr<ParameterLink<string>> Arch_MaxFileNamePL;  // name of the Max file (all stats for best brain when file is writtne to)
	static shared_ptr<ParameterLink<string>> Arch_PopFileNamePL;  // name of the Population file (ave and var for population at each timepoint)
	static shared_ptr<ParameterLink<string>> Arch_DefaultPopFileColumnNamesPL;  // data to be saved into average file (must be values that can generate an average)

	static shared_ptr<ParameterLink<string>> Arch_realtimeSequencePL;  // how often to write out data
	static shared_ptr<ParameterLink<string>> SS_Arch_dataSequencePL;  // how often to save data
	static shared_ptr<ParameterLink<string>> SS_Arch_organismSequencePL;  // how often to save genomes

	static shared_ptr<ParameterLink<string>> SS_Arch_DataFilePrefixPL;  // name of the Data file
	static shared_ptr<ParameterLink<string>> SS_Arch_OrganismsFilePrefixPL;  // name of the Genome file (genomes on LOD)
	static shared_ptr<ParameterLink<bool>> SS_Arch_writeDataFilesPL;  // if true, write data file
	static shared_ptr<ParameterLink<bool>> SS_Arch_writeOrganismsFilesPL;  // if true, write genome file




	bool writeMaxFile;  // if true, Max file will be created
	bool writePopFile;  // if true, pop file will be created
	string MaxFileName;  // name of the Max file (all stats for best brain when file is writtne to)
	string PopFileName;  // name of the Population file (ave and var for population at each timepoint)
	string PopFileColumnNames;  // data to be saved into average file (must be values that can generate an average)
	shared_ptr<Abstract_MTree> maxFormula;  // what value will be used to determine which organism to write to max file


	vector<int> realtimeSequence;  // how often to write out data
	int realtimeSequenceIndex;

	vector<int> realtimeDataSequence;
	vector<int> realtimeOrganismSequence;
	int realtimeDataSeqIndex;
	int realtimeOrganismSeqIndex;


	string DataFilePrefix;  // name of the Data file
	string OrganismFilePrefix;  // name of the Genome file (genomes on LOD)
	bool writeSnapshotDataFiles;  // if true, write data file
	bool writeSnapshotGenomeFiles;  // if true, write genome file

	bool saveNewOrgs = false;

	string groupPrefix;

	map<string, vector<string>> files;  // list of files (NAME,LIST OF COLUMNS)
	vector<string> DefaultPopFileColumns;  // what columns will be written into the PopFile

	bool finished;  // if finished, then as far as the archivist is concerned, we can stop the run.

	const shared_ptr<ParametersTable> PT;

	DefaultArchivist(shared_ptr<ParametersTable> _PT = nullptr, string _groupPrefix = "");
	DefaultArchivist(vector<string> popFileColumns, shared_ptr<Abstract_MTree> _maxFormula = nullptr, shared_ptr<ParametersTable> _PT = nullptr, string _groupPrefix = "");
	virtual ~DefaultArchivist() = default;

	//save Max and average file data
	void writeRealTimeFiles(vector<shared_ptr<Organism>> &population);

	void saveSnapshotData(vector<shared_ptr<Organism>> population);

	//void saveSnapshotGenomes(vector<shared_ptr<Organism>> population);
	void saveSnapshotOrganisms(vector<shared_ptr<Organism>> population);

	// save data and manage in memory data
	// return true if next save will be > updates + terminate after
	virtual bool archive(vector<shared_ptr<Organism>> population, int flush = 0);

	//virtual void processAllLists(OldDataMap &dm);

	virtual bool isDataUpdate(int checkUpdate = -1);
	virtual bool isOrganismUpdate(int checkUpdate = -1);

	map<string, int> uniqueColumnNameToOutputBehaviors;

};
