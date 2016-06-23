//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__Data__
#define __BasicMarkovBrainTemplate__Data__

#include <cwctype>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>

#include "Utilities.h"

using namespace std;

class DataMap {
 private:
	map<string, string> data;

 public:

	DataMap() = default;

	DataMap(shared_ptr<DataMap> source){
		data = source->data;
	}

	/*
	 * sets a value at "key" to a DataMap.data
	 * Template Functions must be in a header
	 */
	template<typename Type>
	void Set(const string &key, const Type& value) {  // sets a value in a DataMap with "key"
		data[key] = to_string(value);
	}

	// Clear a field in a DataMap
	void Clear(const string &key) {
		data.erase(key);
	}

	// Clear add data in a DataMap
	void ClearMap() {
		data.clear();
	}
	string Get(const string &key);  // retrieve the string from a dataMap with "key"
	bool fieldExists(const string &key);  // return true if a data map contains "key"

	void writeToFile(const string &fileName, const vector<string>& keys = { });  // write from this DataMap to fileName the data associated with keys
	vector<string> getKeys();
	//void clear();

	/*
	 * takes a vector of string with key value pairs. Calls set for each pair.
	 */
	void SetMany(vector<string> dataPairs);

	/*
	 * appends a value at "key" to a DataMap.data
	 * Template Functions must be in a header
	 */
	template<typename Type>
	void Append(const string &key, const Type &value) {
		// is this value in the DataMap already?
		if (data.find(key) == data.end()) {  // if this key is not already in the DataMap
			data[key] = "\"[" + to_string(value) + "]\"";
		} else {  // the key already exists
			if ((data[key][0] == '\"') && (data[key][1] = '[')) {  // if this key is already associated with a list
				string workingString = data[key];
				workingString.pop_back();  // strip off trailing ']"'
				workingString.pop_back();  // strip off trailing ']"'
				workingString = workingString + "," + to_string(value) + "]\"";  // add ",value]"
				data[key] = workingString;  // put new list back in DataMap
			} else {  // this key exists, but the associated element is not a list
				cout << "  In DataMap::Append(key, value)\n  ERROR: attempted to append to a non list DataMap entry. \n    Exiting!\n";
				exit(1);
			}
		}
	}

};

class FileManager {
 public:
	static string outputDirectory;
	static set<string> dataFilesCreated;  // list of files, this allows us to track if headers must be written
	static const char separator = ',';
	static void writeToFile(const string& fileName, const string& data, const string& header = "");  //fileName, data, header - used when you want to output formatted data (i.e. genomes)
};

#endif /* defined(__BasicMarkovBrainTemplate__Data__) */
