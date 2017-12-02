//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

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
map<string, int> DataMap::knownOutputBehaviors = { {"LIST",LIST}, {"AVE",AVE}, {"SUM",SUM}, {"PROD",PROD}, {"STDERR",STDERR}, {"FIRST",FIRST}, {"VAR",VAR} };

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




// take two strings (header and data), and a list of keys, and whether or not to save "{LIST}"s. convert data from data map to header and data strings
void DataMap::constructHeaderAndDataStrings(string& headerStr, string& dataStr, const vector<string>& keys, bool aveOnly) {
	headerStr = ""; // make sure the strings are clean
	dataStr = "";
	dataMapType typeOfKey;
	int OB; // holds output behavior so it can be over ridden for ave file output!
	if (keys.size() > 0) {  // if keys is not empty
		for (int n = 0; n < int(keys.size()); n++) {
			string i = keys[n];
			typeOfKey = findKeyInData(i);
			if (typeOfKey == NONE) {
				cout << "  in DataMap::writeToFile() - key \"" << i << "\" can not be found in data map!\n  exiting." << endl;
				exit(1);
			}

			// the following code makes use of bit masks! in short, AVE,SUM,LIST,etc each use only one bit of an int.
			// therefore if we apply that mask the the outputBehavior, we can see if that type of output is needed.

			OB = outputBehavior[i];

			if (typeOfKey == STRING || typeOfKey == STRINGSOLO) {
				if (OB == NONE && typeOfKey == STRING) {
					OB = LIST; // if no output behavior is assigned, make it list.
				}
				else if (OB == NONE && typeOfKey == STRINGSOLO) {
					OB = FIRST; // unless the value is a solo value (i.e. it was set up with a Set(value) function), then make it first.
				}
				else if (!(OB == LIST || OB == FIRST)) {
					cout << "  in constructHeaderAndDataStrings :: attempt to write string not in either LIST or FIRST formatte. This is not allowed! Exiting..." << endl;
					exit(1);
				}
			}
			else if (OB == NONE) { // this is not a string or stringsolo...
				OB = LIST | AVE; // if no output behavior is assigned, make it list and also record the average.
				if (typeOfKey == BOOLSOLO || typeOfKey == DOUBLESOLO || typeOfKey == INTSOLO) {
					OB = FIRST; // unless the value is a solo value (i.e. it was set up with a Set(value) function), then make it first.
				}
			}

			if (aveOnly) {
				OB = OB & (AVE | FIRST); // if aveOnly, only output AVE on the entries that have been set for AVE
			}

			if (OB & FIRST) { // save first (only?) element in vector with key as column name
				headerStr = headerStr + FileManager::separator + i;
				if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) {
					if (getBoolVector(i).size() > 0) {
						dataStr = dataStr + FileManager::separator + to_string(getBoolVector(i)[0]);
					}
					else {
						dataStr = dataStr + '0';
						cout << "  WARNING!! In DataMap::constructHeaderAndDataStrings :: while getting value for FIRST with key \"" << i << "\" vector is empty!" << endl;
					}
				}
				if (typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) {
					if (getDoubleVector(i).size() > 0) {
						dataStr = dataStr + FileManager::separator + to_string(getDoubleVector(i)[0]);
					}
					else {
						dataStr = dataStr + '0';
						cout << "  WARNING!! In DataMap::constructHeaderAndDataStrings :: while getting value for FIRST with key \"" << i << "\" vector is empty!" << endl;
					}
				}
				if (typeOfKey == INT || typeOfKey == INTSOLO) {
					if (getIntVector(i).size() > 0) {
						dataStr = dataStr + FileManager::separator + to_string(getIntVector(i)[0]);
					}
					else {
						dataStr = dataStr + '0';
						cout << "  WARNING!! In DataMap::constructHeaderAndDataStrings :: while getting value for FIRST with key \"" << i << "\" vector is empty!" << endl;
					}
				}
				if (typeOfKey == STRING || typeOfKey == STRINGSOLO) {
					if (getStringVector(i).size() > 0) {
						dataStr = dataStr + FileManager::separator + getStringVector(i)[0];
					}
					else {
						dataStr = dataStr + '0';
						cout << "  WARNING!! In DataMap::constructHeaderAndDataStrings :: while getting value for FIRST with key \"" << i << "\" vector is empty!" << endl;
					}
				}

			}
			if (OB & AVE) { // key_AVE = ave of vector (will error if of type string!)
				headerStr = headerStr + FileManager::separator + i + "_AVE";
				dataStr = dataStr + FileManager::separator + to_string(getAverage(i));
			}
			if (OB & VAR) { // key_VAR = variance of vector (will error if of type string!)
				headerStr = headerStr + FileManager::separator + i + "_VAR";
				dataStr = dataStr + FileManager::separator + to_string(getVariance(i));
			}
			if (OB & SUM) { // key_SUM = sum of vector
				headerStr = headerStr + FileManager::separator + i + "_SUM";
				dataStr = dataStr + FileManager::separator + to_string(getSum(i));
			}
			if (OB & PROD) { // key_PROD = product of vector
				cout << "  WARNING OUTPUT METHOD PROD IS HAS YET TO BE WRITTEN!" << endl;
			}
			if (OB & STDERR) { // key_STDERR = standard error of vector
				cout << "  WARNING OUTPUT METHOD STDERR IS HAS YET TO BE WRITTEN!" << endl;
			}
			if (OB & LIST) { //key_LIST = save all elements in vector in csv list format
				headerStr = headerStr + FileManager::separator + i + "_LIST";
				dataStr = dataStr + FileManager::separator + getStringOfVector(i);
			}
		}
		headerStr.erase(headerStr.begin());  // clip off the leading separator
		dataStr.erase(dataStr.begin());  // clip off the leading separator
	}
}
///////////////////////////////////////
// need to add support for output prefix directory
// need to add support for population file name prefixes
///////////////////////////////////////
