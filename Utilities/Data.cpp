//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include <cstring>
#include <memory>
#include <iostream>

#include <fstream>
#include <map>

#include "Data.h"

//global variables that should be accessible to all
//set<string> FileManager::dataFilesCreated;

string FileManager::outputDirectory = "./";
map<string, vector<string>> FileManager::fileColumns;
map<string, ofstream> FileManager::files; // list of files (NAME,ofstream)
map<string, bool> FileManager::fileStates; // list of files states (NAME,open?)

void FileManager::writeToFile(const string& fileName, const string& data, const string& header) {
	openFile(fileName, header); // make sure that the file is open and ready to be written to
	files[fileName] << data << "\n" << flush;
}

void FileManager::openFile(const string& fileName, const string& header) {
	if (files.find(fileName) == files.end()) {  // if file has not be initialized yet
		files.emplace(make_pair(fileName, ofstream())); // make an ofstream for the new file and place in FileManager::files
		files[fileName].open((string)outputDirectory + (string)"/" + fileName);  // clear file contents and open in write mode
		fileStates[fileName] = true; // this file is now open
		if (header != "") {  // if there is a header string, write this to the new file
			files[fileName] << header << "\n";
		}
	}
	if (fileStates[fileName] == false) { // if file is closed ...
		files[fileName].open((string)outputDirectory + (string)"/" + fileName, ios::out | ios::app);  // open file in append mode
	}

}

void FileManager::closeFile(const string& fileName) {
	if (files.find(fileName) == files.end()) {
		cout << "  In FileManager::closeFile :: ERROR, attempt to close file '" << fileName << "' but this file has not been opened or created! Exiting." << endl;
		exit(1);
	}
	files[fileName].close();
	fileStates[fileName] = false; // make a note that this file is closed
}



///////////////////////////////////////
// need to add support for output prefix directory
// need to add support for population file name prefixes
///////////////////////////////////////
