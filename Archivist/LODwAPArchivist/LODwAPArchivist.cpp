//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "LODwAPArchivist.h"

shared_ptr<ParameterLink<string>> LODwAPArchivist::LODwAP_Arch_dataSequencePL = Parameters::register_parameter("ARCHIVIST_LODWAP-dataSequence", (string)":100", "How often to write to data file. (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
shared_ptr<ParameterLink<string>> LODwAPArchivist::LODwAP_Arch_genomeSequencePL = Parameters::register_parameter("ARCHIVIST_LODWAP-genomeSequence", (string)":1000", "How often to write genome file. (format: x = single value, x-y = x to y, x-y:z = x to y on x, :z = from 0 to updates on z, x:z = from x to 'updates' on z) e.g. '1-100:10, 200, 300:100'");
shared_ptr<ParameterLink<int>> LODwAPArchivist::LODwAP_Arch_pruneIntervalPL = Parameters::register_parameter("ARCHIVIST_LODWAP-pruneInterval", 100, "How often to attempt to prune LOD and actually write out to files");
shared_ptr<ParameterLink<int>> LODwAPArchivist::LODwAP_Arch_terminateAfterPL = Parameters::register_parameter("ARCHIVIST_LODWAP-terminateAfter", 100, "how long to run after updates (to get allow time for coalescence)");
shared_ptr<ParameterLink<string>> LODwAPArchivist::LODwAP_Arch_DataFileNamePL = Parameters::register_parameter("ARCHIVIST_LODWAP-dataFileName", (string) "data.csv", "name of genome file (stores genomes for line of decent)");
shared_ptr<ParameterLink<string>> LODwAPArchivist::LODwAP_Arch_GenomeFileNamePL = Parameters::register_parameter("ARCHIVIST_LODWAP-genomeFileName", (string) "genome.csv", "name of data file (stores everything but genomes)");
shared_ptr<ParameterLink<bool>> LODwAPArchivist::LODwAP_Arch_writeDataFilePL = Parameters::register_parameter("ARCHIVIST_LODWAP-writeDataFile", true, "if true, a data file will be written");
shared_ptr<ParameterLink<bool>> LODwAPArchivist::LODwAP_Arch_writeGenomeFilePL = Parameters::register_parameter("ARCHIVIST_LODWAP-writeGenomeFile", true, "if true, a genome file will be written");
shared_ptr<ParameterLink<bool>> LODwAPArchivist::LODwAP_Arch_dataFileShowAllListsPL = Parameters::register_parameter("ARCHIVIST_LODWAP-dataFileShowAllLists", true, "if true, lists named 'all'* in data map will be saved");
shared_ptr<ParameterLink<bool>> LODwAPArchivist::LODwAP_Arch_dataFileConvertAllListsPL = Parameters::register_parameter("ARCHIVIST_LODWAP-dataFileConvertAllLists", true, "if true, lists named 'all'* in data map will be averaged and added to file");

LODwAPArchivist::LODwAPArchivist(vector<string> aveFileColumns, shared_ptr<ParametersTable> _PT) :
		DefaultArchivist(aveFileColumns, _PT) {

	pruneInterval = (PT == nullptr) ? LODwAP_Arch_pruneIntervalPL->lookup() : PT->lookupInt("ARCHIVIST_LODWAP-pruneInterval");
	terminateAfter = (PT == nullptr) ? LODwAP_Arch_terminateAfterPL->lookup() : PT->lookupInt("ARCHIVIST_LODWAP-terminateAfter");
	DataFileName = (PT == nullptr) ? LODwAP_Arch_DataFileNamePL->lookup() : PT->lookupString("ARCHIVIST_LODWAP-dataFileName");
	GenomeFileName = (PT == nullptr) ? LODwAP_Arch_GenomeFileNamePL->lookup() : PT->lookupString("ARCHIVIST_LODWAP-genomeFileName");
	writeDataFile = (PT == nullptr) ? LODwAP_Arch_writeDataFilePL->lookup() : PT->lookupBool("ARCHIVIST_LODWAP-writeDataFile");
	writeGenomeFile = (PT == nullptr) ? LODwAP_Arch_writeGenomeFilePL->lookup() : PT->lookupBool("ARCHIVIST_LODWAP-writeGenomeFile");
	dataFileShowAllLists = (PT == nullptr) ? LODwAP_Arch_dataFileShowAllListsPL->lookup() : PT->lookupBool("ARCHIVIST_LODWAP-dataFileShowAllLists");
	dataFileConvertAllLists = (PT == nullptr) ? LODwAP_Arch_dataFileConvertAllListsPL->lookup() : PT->lookupBool("ARCHIVIST_LODWAP-dataFileConvertAllLists");

	dataSequence.push_back(0);
	genomeSequence.push_back(0);

	string dataSequenceStr = (PT == nullptr) ? LODwAP_Arch_dataSequencePL->lookup() : PT->lookupString("ARCHIVIST_LODWAP-dataSequence");
	string genomeSequenceStr = (PT == nullptr) ? LODwAP_Arch_genomeSequencePL->lookup() : PT->lookupString("ARCHIVIST_LODWAP-genomeSequence");

	dataSequence = seq(dataSequenceStr, Global::updatesPL->lookup(), true);
	if (dataSequence.size() == 0) {
		cout << "unable to translate ARCHIVIST_LODWAP-dataSequence \"" << dataSequenceStr << "\".\nExiting." << endl;
		exit(1);
	}
	genomeSequence = seq(genomeSequenceStr, Global::updatesPL->lookup(), true);
	if (genomeSequence.size() == 0) {
		cout << "unable to translate ARCHIVIST_LODWAP-genomeSequence \"" << genomeSequenceStr << "\".\nExiting." << endl;
		exit(1);
	}

	if (writeDataFile != false) {
		dataSequence.clear();
		dataSequence = seq(dataSequenceStr, Global::updatesPL->lookup(), true);
		if (dataSequence.size() == 0) {
			cout << "unable to translate ARCHIVIST_SSWD-dataSequence \"" << dataSequenceStr << "\".\nExiting." << endl;
			exit(1);
		}
	}

	if (writeGenomeFile != false) {
		genomeSequence = seq(genomeSequenceStr, Global::updatesPL->lookup(), true);
		if (genomeSequence.size() == 0) {
			cout << "unable to translate ARCHIVIST_SSWD-genomeSequence \"" << genomeSequenceStr << "\".\nExiting." << endl;
			exit(1);
		}
	}

	dataSeqIndex = 0;
	genomeSeqIndex = 0;
	nextDataWrite = dataSequence[dataSeqIndex];
	nextGenomeWrite = genomeSequence[genomeSeqIndex];

	lastPrune = 0;

}

bool LODwAPArchivist::archive(vector<shared_ptr<Organism>> population, int flush) {

	if ((Global::update == realtimeSequence[realtimeSequenceIndex]) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
		writeRealTimeFiles(population);  // write to dominant and average files
		if (realtimeSequenceIndex + 1 < (int)realtimeSequence.size()){
			realtimeSequenceIndex++;
		}
	}

	if ((Global::update == realtimeDataSequence[realtimeDataSeqIndex]) && (flush == 0) && writeSnapshotDataFiles) {  // do not write files on flush - these organisms have not been evaluated!
		saveSnapshotData(population);
		if (realtimeDataSeqIndex + 1 < (int)realtimeDataSequence.size()){
			realtimeDataSeqIndex++;
		}
	}
	if ((Global::update == realtimeGenomeSequence[realtimeGenomeSeqIndex]) && (flush == 0) && writeSnapshotGenomeFiles) {  // do not write files on flush - these organisms have not been evaluated!
		saveSnapshotGenomes(population);
		if (realtimeGenomeSeqIndex + 1 < (int)realtimeGenomeSequence.size()){
			realtimeGenomeSeqIndex++;
		}
	}

	if (writeGenomeFile && find (genomeSequence.begin(), genomeSequence.end(), Global::update) != genomeSequence.end()) {
		for (auto org : population) {  // if this update is in the genome sequence, turn on genome tracking.
			org->trackGenome = true;
		}

	}
	if ((Global::update % pruneInterval == 0) || (flush == 1)) {

		if (files.find(DataFileName) == files.end()) {  // if file has not be initialized yet
			if (dataFileConvertAllLists) {
				DataMap TempMap;
				for (auto key : population[0]->dataMap.getKeys()) {
					if (key[0] == 'a' && key[1] == 'l' && key[2] == 'l') {
						double temp = 0;
						vector<double> values;
						convertCSVListToVector(population[0]->dataMap.Get(key), values);
						for (auto v : values) {
							temp += v;
						}
						temp /= (double) values.size();
						TempMap.Set(key.substr(3, key.size() - 1), temp);
						if (dataFileShowAllLists) {
							TempMap.Set(key, population[0]->dataMap.Get(key));
						}
					} else {
						TempMap.Set(key, population[0]->dataMap.Get(key));
					}
				}
				files[DataFileName] = TempMap.getKeys();  // store keys from data map associated with file name
			} else {
				files[DataFileName] = population[0]->dataMap.getKeys();  // store keys from data map associated with file name
			}
			files[DataFileName].push_back("update");
		}

		// get the MRCA
		vector<shared_ptr<Organism>> LOD = population[0]->getLOD(population[0]);  // get line of decent
		shared_ptr<Organism> effective_MRCA;
		if (flush) {  // if flush then we don't care about coalescence
			cout << "flushing LODwAP: using population[0] as Most Recent Common Ancestor (MRCA)" << endl;
			effective_MRCA = population[0]->parents[0];  // this assumes that a population was created, but not tested at the end of the evolution loop!
		} else {
			effective_MRCA = population[0]->getMostRecentCommonAncestor(LOD);  // find the convergance point in the LOD.
		}

		// Save Data
		if (writeDataFile) {
			while ((effective_MRCA->timeOfBirth >= nextDataWrite) && (nextDataWrite <= Global::updatesPL->lookup())) {  // if there is convergence before the next data interval
				shared_ptr<Organism> current = LOD[nextDataWrite - lastPrune];
				current->dataMap.Set("update", nextDataWrite);
				if (dataFileConvertAllLists) {
					DataMap TempMap;
					for (auto key : current->dataMap.getKeys()) {
						if (key[0] == 'a' && key[1] == 'l' && key[2] == 'l') {
							double temp = 0;
							vector<double> values;
							convertCSVListToVector(current->dataMap.Get(key), values);
							for (auto v : values) {
								temp += v;
							}
							temp /= (double) values.size();
							TempMap.Set(key.substr(3, key.size() - 1), temp);
							if (dataFileShowAllLists) {
								TempMap.Set(key, current->dataMap.Get(key));
							}
						} else {
							TempMap.Set(key, current->dataMap.Get(key));
						}
					}
//					for (auto file : files) {  // for each file in files
//						TempMap.writeToFile(file.first, file.second);  // append new data to the file
//					}
					TempMap.writeToFile(DataFileName, files[DataFileName]);
				} else {
//					for (auto file : files) {  // for each file in files
					current->dataMap.writeToFile(DataFileName, files[DataFileName]);  // append new data to the file
//					}
				}
				if ((int)dataSequence.size() > dataSeqIndex + 1) {
					dataSeqIndex++;
					nextDataWrite = dataSequence[dataSeqIndex];
				} else {
					nextDataWrite = Global::updatesPL->lookup() + terminateAfter + 1;
				}
			}
		}

		//Save Genomes
		if (writeGenomeFile) {

			while ((effective_MRCA->timeOfBirth >= nextGenomeWrite) && (nextGenomeWrite <= Global::updatesPL->lookup())) {  // if there is convergence before the next data interval
				shared_ptr<Organism> current = LOD[nextGenomeWrite - lastPrune];
				//string dataString = to_string(nextGenomeWrite) + FileManager::separator + "\"[" + current->genome->genomeToStr() + "]\"";  // add write update and padding to genome string
				//FileManager::writeToFile(GenomeFileName, dataString, "update,genome");  // write data to file
				current->genome->dataMap.Set("sites", current->genome->genomeToStr());
				current->genome->dataMap.Set("update", nextGenomeWrite);
				current->genome->dataMap.Set("ID", current->dataMap.Get("ID"));
				//current->genome->dataMap.writeToFile(GenomeFileName, current->genome->dataMap.getKeys());  // append new data to the file
				current->genome->dataMap.writeToFile(GenomeFileName, current->genome->genomeFileColumns);  // append new data to the file
				if ((int)genomeSequence.size() > genomeSeqIndex + 1) {
					genomeSeqIndex++;
					nextGenomeWrite = genomeSequence[genomeSeqIndex];
				} else {
					nextGenomeWrite = Global::updatesPL->lookup() + terminateAfter + 1;
				}
			}
		}
		// data and genomes have now been written out up till the MRCA
		// so all data and genomes from before the MRCA can be deleted
		effective_MRCA->parents.clear();
		lastPrune = effective_MRCA->timeOfBirth;  // this will hold the time of the oldest genome in RAM
	}

	// if we have reached the end of time OR we have pruned past updates (i.e. written out all data up to updates), then we ae done!
	return (Global::update >= Global::updatesPL->lookup() + terminateAfter || lastPrune >= Global::updatesPL->lookup());
}

bool LODwAPArchivist::isDataUpdate(int checkUpdate){
	if (checkUpdate == -1) {
		checkUpdate = Global::update;
	}
	bool check = DefaultArchivist::isDataUpdate(checkUpdate);
	check = check || find(dataSequence.begin(),dataSequence.end(),checkUpdate) != dataSequence.end();
	return check;
}

bool LODwAPArchivist::isGenomeUpdate(int checkUpdate){
	if (checkUpdate == -1) {
		checkUpdate = Global::update;
	}
	bool check = DefaultArchivist::isGenomeUpdate(checkUpdate);
	check = check || find(genomeSequence.begin(),genomeSequence.end(),checkUpdate) != genomeSequence.end();
	return check;
}
