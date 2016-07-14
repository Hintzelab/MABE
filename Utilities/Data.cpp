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
set<string> FileManager::dataFilesCreated;

string FileManager::outputDirectory = "./";

/*
 * takes a vector of string with key value pairs. Calls set for each pair.
 */
void DataMap::SetMany(vector<string> dataPairs) {
	if (dataPairs.size() % 2 == 1) {
		cout << "  In DataMap::SetMany : ERROR! dataPairs vector has an odd number of elements.\n  Exiting...\n";
		exit(1);
	}
	for (auto i = 0; i < (int) dataPairs.size(); i += 2) {
		Set(dataPairs[i], dataPairs[i + 1]);
	}
}

/*
 * returns a string value for "key" given a map<string,string>
 */
string DataMap::Get(const string& key) {
	return data[key];
}

bool DataMap::fieldExists(const string& key) {
	return (data.find(key) != data.end());
}

void DataMap::writeToFile(const string &fileName, const vector<string> &keys) {

	string headerStr = "";
	string dataStr = "";
	if (keys.size() > 0) {  // if we keys is not empty
		for (auto i : keys) {
			headerStr = headerStr + FileManager::separator + i;  // make the header string from keys
			dataStr = dataStr + FileManager::separator + data[i];  // make the data string from data[keys]
		}
		headerStr.erase(headerStr.begin());  // clip off the leading separator
		dataStr.erase(dataStr.begin());  // clip off the leading separator
	} else {  // if keys is empty
		for (auto i : data) {  // for every element in data...
			headerStr = headerStr + FileManager::separator + i.first;  // make the header string from all of the keys in data
			dataStr = dataStr + FileManager::separator + i.second;  // make the data string from all of the data
		}
		if (headerStr.size() > 0) {  // if the header string is not empty
			headerStr.erase(headerStr.begin());  // clip off the leading separator
			dataStr.erase(dataStr.begin());  // clip off the leading separator
		} else {  // if the header string is empty, print a warning!
			cout << "  In DataMap::writeToFile(): This DataMap contains no keys. Writing a blank line to file: " << fileName << "\n";
		}
	}
	FileManager::writeToFile(fileName, dataStr, headerStr);  // write the data to file!
}

vector<string> DataMap::getKeys() {
	vector<string> keys;
	for (auto element : data) {
		keys.push_back(element.first);
	}
	return (keys);
}


//void DataMap::clear() {
//	data.clear();
//}

void FileManager::writeToFile(const string& fileName, const string& data, const string& header) {
	ofstream FILE;
	bool fileClosed = true;
	if (FileManager::dataFilesCreated.find(fileName) == FileManager::dataFilesCreated.end()) {  // if file has not be initialized yet
		FileManager::dataFilesCreated.insert(fileName);  // make a note that file exists
		FILE.open(outputDirectory + fileName);  // clear file contents and open in write mode
		fileClosed = false;
		if (header != "") {
			FILE << header << "\n";
		}
	}
	if (fileClosed) {
		FILE.open(outputDirectory + fileName, ios::out | ios::app);  // open file in append mode
	}
	FILE << data << "\n";
    FILE.close();
}

///////////////////////////////////////
// need to add support for output prefix directory
// need to add support for population file name prefixes
///////////////////////////////////////
