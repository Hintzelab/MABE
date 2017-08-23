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

#include <cwctype>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <unordered_set>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

#include "Utilities.h"

using namespace std;



class FileManager {
public:
	static map<string, vector<string>> fileColumns;  // list of files (NAME,LIST OF COLUMNS)
	static map<string, ofstream> files; // list of files (NAME,ofstream)
	static map<string, bool> fileStates; // list of files states (NAME,open?)

	static string outputDirectory;

	static const char separator = ',';

	static void writeToFile(const string& fileName, const string& data, const string& header = "");  //fileName, data, header - used when you want to output formatted data (i.e. genomes)
	static void openFile(const string& fileName, const string& header = "");  // open file and write header to file if file is new and header is provided
	static void closeFile(const string& fileName); // close file
};

class DataMap {
public:

	enum outputBehaviors {
		LIST = 1, AVE = 2, SUM = 4, PROD = 8, STDERR = 16, FIRST = 32, VAR = 64
	}; // 0 = do not save or default..?
	map<string, int> outputBehavior; // Defines how each element should be written to file - if element not found, LIST (write out list) is used.
	static map<string,int> knownOutputBehaviors;

private:
	enum dataMapType {
		NONE = 0, BOOL = 1, DOUBLE = 2, INT = 3, STRING = 4, BOOLSOLO = 11, DOUBLESOLO = 12, INTSOLO = 13, STRINGSOLO = 14
	}; // NONE = not found in this data map

	unordered_map<string, vector<bool>> boolData;
	unordered_map<string, vector<double>> doubleData;
	unordered_map<string, vector<int>> intData;
	unordered_map<string, vector<string>> stringData;

	map<string, dataMapType> inUse; // holds list of keys with type 1=bool, 2=double, 3=int, 4=string

public:

	DataMap() = default;

	// copy constructor
	DataMap(shared_ptr<DataMap> source) {
		boolData = source->boolData;
		doubleData = source->doubleData;
		intData = source->intData;
		stringData = source->stringData;
		for (auto entry : source->inUse) {
			inUse[entry.first] = entry.second;
		}
		//inUse = source->inUse; // replaced with for loop.
		outputBehavior = source->outputBehavior;
	}

	inline void setOutputBehavior(const string& key, int _outputBehavior) {
		outputBehavior[key] = _outputBehavior;
	}

	// find key in this data map and return type (NONE = not found)
	inline dataMapType findKeyInData(const string& key, bool printType = false) {
		if (printType){
			cout << key << "is of type " << inUse[key] << endl;
		}
		if (inUse.find(key) != inUse.end()) {
			return inUse[key];
		} else {
			return NONE;
		}
	}

	// find key in this data map and return type (NONE = not found)
	inline bool isKeySolo(const string& key) {
		if (inUse.find(key) != inUse.end()) {
			return ((findKeyInData(key) == BOOLSOLO) || (findKeyInData(key) == DOUBLESOLO) || (findKeyInData(key) == INTSOLO) || (findKeyInData(key) == STRINGSOLO));
		} else {
			cout << "  ERROR :: in DataMap::isKeySolo, key name " << key << " is not defined in DataMap. Exiting!" << endl;
			exit(1);
		}
	}

	// return a string of the type of key in this data map
	inline string lookupDataMapTypeName(dataMapType t) {
		if (t == NONE) {
			return "none";
		} else if (t == BOOL || t == BOOLSOLO) {
			return "bool";
		} else if (t == DOUBLE || t == DOUBLESOLO) {
			return "double";
		} else if (t == INT || t == INTSOLO) {
			return "int";
		} else if (t == STRING || t == STRINGSOLO) {
			return "string";
		} else {
			cout << "  In lookupDataMapTypeName... type " << t << " is not defined. Exiting!" << endl;
			exit(1);
		}
	}

	// return vector of strings will all keys in this data map
	inline vector<string> getKeys() {
		vector<string> keys;
		for (auto e : inUse) {
			keys.push_back(e.first); // just push back the whole key
		}
		return (keys);
	}

	// set functions (bool,double,int,string) that take a **single** value - either make new map entry or replace existing
	inline void set(const string& key, const bool& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE || typeOfKey == BOOL || typeOfKey == BOOLSOLO) { // if key is unused or associates with correct type
			boolData[key] = vector<bool>( { value });
			inUse[key] = BOOLSOLO; // since this is set with SET, it is a single value
		} else {
			cout << "  ERROR :: a call to DataMap::set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value = \"" << value << "\" where value is bool." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}
	inline void set(const string& key, const double& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE || typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) { // if key is unused or associates with correct type
			doubleData[key] = vector<double>( { value });
			inUse[key] = DOUBLESOLO; // since this is set with SET, it is a single value
		} else {
			cout << "  ERROR :: a call to DataMap::set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value = \"" << value << "\" where value is double." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}
	inline void set(const string& key, const int& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE || typeOfKey == INT || typeOfKey == INTSOLO) { // if key is unused or associates with correct type
			intData[key] = vector<int>( { value });
			inUse[key] = INTSOLO; // since this is set with SET, it is a single value
		} else {
			cout << "  ERROR :: a call to DataMap::set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value = \"" << value << "\" where value is int." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}
	inline void set(const string& key, const string& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE || typeOfKey == STRING || typeOfKey == STRINGSOLO) { // if key is unused or associates with correct type
			stringData[key] = vector<string>( { value });
			inUse[key] = STRINGSOLO;
		} else {
			cout << "  ERROR :: a call to DataMap::set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value = \"" << value << "\" where value is string." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}

	// set functions (bool,double,int,string) that take a **vector** of value - either make new map entry or replace existing
	inline void set(const string& key, const vector<bool>& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE || typeOfKey == BOOL || typeOfKey == BOOLSOLO) { // if key is unused or associates with correct type
			boolData[key] = value;
			inUse[key] = BOOL;
		} else {
			cout << "  ERROR :: a call to DataMap::set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value is a vector of bool." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}
	inline void set(const string& key, const vector<double>& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE || typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) { // if key is unused or associates with correct type
			doubleData[key] = value;
			inUse[key] = DOUBLE;
		} else {
			cout << "  ERROR :: a call to DataMap::set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value is a vector of double." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}
	inline void set(const string& key, const vector<int>& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE || typeOfKey == INT || typeOfKey == INTSOLO) { // if key is unused or associates with correct type
			intData[key] = value;
			inUse[key] = INT;
		} else {
			cout << "  ERROR :: a call to DataMap::set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value is a vector of int." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}
	inline void set(const string& key, const vector<string>& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE || typeOfKey == STRING || typeOfKey == STRINGSOLO) { // if key is unused or associates with correct type
			stringData[key] = value;
			inUse[key] = STRING;
		} else {
			cout << "  ERROR :: a call to DataMap::set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value is a vector of string." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}

	// append a value to the end of vector associated with key. If key is not found, start a new vector for key
	inline void append(const string& key, const bool& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			set(key, value);
			inUse[key] = BOOL; // set the in use to be a list rather then a solo
		} else if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) { // if this key is in data map as a string, append to vector
			boolData[key].push_back(value);
			inUse[key] = BOOL;
		} else {
			cout << "  In DataMap::append :: attempt to append value \"" << value << "\" of type bool to \"" << key << "\" but this key is already associated with " << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << endl;
			exit(1);
		}
	}
	inline void append(const string& key, const double& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			set(key, value);
			inUse[key] = DOUBLE; // set the in use to be a list rather then a solo
		} else if (typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) { // if this key is in data map as a string, append to vector
			doubleData[key].push_back(value);
			inUse[key] = DOUBLE;
		} else {
			cout << "  In DataMap::append :: attempt to append value \"" << value << "\" of type double to \"" << key << "\" but this key is already associated with " << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << endl;
			exit(1);
		}
	}
	inline void append(const string& key, const int& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			set(key, value);
			inUse[key] = INT; // set the in use to be a list rather then a solo
		} else if (typeOfKey == INT || typeOfKey == INTSOLO) { // if this key is in data map as a string, append to vector
			intData[key].push_back(value);
			inUse[key] = INT; // set the in use to be a list rather then a solo
		} else {
			cout << "  In DataMap::append :: attempt to append value \"" << value << "\" of type int to \"" << key << "\" but this key is already associated with " << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << endl;
			exit(1);
		}
	}
	inline void append(const string& key, const string& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			set(key, value);
			inUse[key] = STRING; // set the in use to be a list rather then a solo
		} else if (typeOfKey == STRING || typeOfKey == STRINGSOLO) { // if this key is in data map as a string, append to vector
			stringData[key].push_back(value);
			inUse[key] = STRING; // set the in use to be a list rather then a solo
		} else {
			cout << "  In DataMap::append :: attempt to append value \"" << value << "\" of type string to \"" << key << "\" but this key is already associated with " << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << endl;
			exit(1);
		}
	}

	// append a vector of values to the end of vector associated with key. If key is not found, start a new vector for key
	inline void append(const string& key, const vector<bool>& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			set(key, value);
		} else if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) { // if this key is in data map as a string, append to vector
			boolData[key].insert(boolData[key].end(), value.begin(), value.end());
			inUse[key] = BOOL; // may have been solo - make sure it's list
		} else {
			cout << "  In DataMap::append :: attempt to append a vector of type bool to \"" << key << "\" but this key is already associated with " << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << endl;
			exit(1);
		}
	}
	inline void append(const string& key, const vector<double>& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			set(key, value);
		} else if (typeOfKey == DOUBLE) { // if this key is in data map as a string, append to vector
			doubleData[key].insert(doubleData[key].end(), value.begin(), value.end());
			inUse[key] = DOUBLE; // may have been solo - make sure it's list
		} else {
			cout << "  In DataMap::append :: attempt to append a vector of type double to \"" << key << "\" but this key is already associated with " << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << endl;
			exit(1);
		}
	}
	inline void append(const string& key, const vector<int>& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			set(key, value);
		} else if (typeOfKey == INT) { // if this key is in data map as a string, append to vector
			intData[key].insert(intData[key].end(), value.begin(), value.end());
			inUse[key] = INT; // may have been solo - make sure it's list
		} else {
			cout << "  In DataMap::append :: attempt to append a vector of type int to \"" << key << "\" but this key is already associated with " << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << endl;
			exit(1);
		}
	}
	inline void append(const string& key, const vector<string>& value) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			set(key, value);
		} else if (typeOfKey == STRING) { // if this key is in data map as a string, append to vector
			stringData[key].insert(stringData[key].end(), value.begin(), value.end());
			inUse[key] = STRING; // may have been solo - make sure it's list
		} else {
			cout << "  In DataMap::append :: attempt to append a vector of type string to \"" << key << "\" but this key is already associated with " << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << endl;
			exit(1);
		}
	}

	// merge contents of two data maps - if common keys are found, replace 1 = overwrite, replace 0 = append
	inline void merge(DataMap otherDataMap, bool replace = 0) {
		if (replace == true) {
			for (string key : otherDataMap.getKeys()) {
				dataMapType typeOfKey = findKeyInData(key);
				dataMapType typeOfOtherKey = otherDataMap.findKeyInData(key);
				if (typeOfOtherKey == typeOfKey || typeOfKey == NONE) {
					if (typeOfOtherKey == BOOL || typeOfOtherKey == BOOLSOLO) {
						set(key, otherDataMap.getBoolVector(key));
						outputBehavior[key] = otherDataMap.outputBehavior[key];
					}
					if (typeOfOtherKey == DOUBLE || typeOfOtherKey == DOUBLESOLO) {
						set(key, otherDataMap.getDoubleVector(key));
						outputBehavior[key] = otherDataMap.outputBehavior[key];
					}
					if (typeOfOtherKey == INT || typeOfOtherKey == INTSOLO) {
						set(key, otherDataMap.getIntVector(key));
						outputBehavior[key] = otherDataMap.outputBehavior[key];
					}
					if (typeOfOtherKey == STRING || typeOfOtherKey == STRINGSOLO) {
						set(key, otherDataMap.getStringVector(key));
						outputBehavior[key] = otherDataMap.outputBehavior[key];
					}
				} else {
					cout << "  In DataMap::merge() - attempt to merge key: \"" << key << "\" but types do not match!\n  Exiting." << endl;
				}
			}
		} else { // replace != true
			for (string key : otherDataMap.getKeys()) {
				dataMapType typeOfKey = findKeyInData(key);
				dataMapType typeOfOtherKey = otherDataMap.findKeyInData(key);
				if (typeOfOtherKey == typeOfKey || typeOfKey == NONE) {
					if (typeOfOtherKey == BOOL || typeOfOtherKey == BOOLSOLO) {
						append(key, otherDataMap.getBoolVector(key));
						if (boolData[key].size() == 1 && typeOfOtherKey == BOOLSOLO) {
							inUse[key] = BOOLSOLO;
						} else {
							inUse[key] = BOOL;
						}
						outputBehavior[key] = outputBehavior[key] | otherDataMap.outputBehavior[key];
					}
					if (typeOfOtherKey == DOUBLE || typeOfOtherKey == DOUBLESOLO) {
						append(key, otherDataMap.getDoubleVector(key));
						if (doubleData[key].size() == 1 && typeOfOtherKey == DOUBLESOLO) {
							inUse[key] = DOUBLESOLO;
						} else {
							inUse[key] = DOUBLE;
						}
						outputBehavior[key] = outputBehavior[key] | otherDataMap.outputBehavior[key];
					}
					if (typeOfOtherKey == INT || typeOfOtherKey == INTSOLO) {
						append(key, otherDataMap.getIntVector(key));
						if (intData[key].size() == 1 && typeOfOtherKey == INTSOLO) {
							inUse[key] = INTSOLO;
						} else {
							inUse[key] = INT;
						}
						outputBehavior[key] = outputBehavior[key] | otherDataMap.outputBehavior[key];
					}
					if (typeOfOtherKey == STRING || typeOfOtherKey == STRINGSOLO) {
						append(key, otherDataMap.getStringVector(key));
						if (stringData[key].size() == 1 && typeOfOtherKey == STRINGSOLO) {
							inUse[key] = STRINGSOLO;
						} else {
							inUse[key] = STRING;
						}
						outputBehavior[key] = outputBehavior[key] | otherDataMap.outputBehavior[key];
					}
				} else {
					cout << "  In DataMap::merge() - attempt to merge key: \"" << key << "\" but types do not match!\n  Exiting." << endl;
				}
			}
		}
	}

	inline vector<bool> getBoolVector(const string &key) { // retrieve a double from a dataMap with "key"
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) {
			return boolData[key];
		} else {
			cout << "  in DataMap::getBoolVector :: attempt to use getBoolVector with key \"" << key << "\" but this key is associated with type " << typeOfKey << "\n  exiting." << endl;
			cout << "  (if type is NONE, then the key was not found in dataMap)" << endl;
			exit(1);
		}
	}
	inline vector<double> getDoubleVector(const string &key) { // retrieve a double from a dataMap with "key"
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) {
			return doubleData[key];
		} else {
			cout << "  in DataMap::getDoubleVector :: attempt to use getDoubleVector with key \"" << key << "\" but this key is associated with type " << typeOfKey << "\n  exiting." << endl;
			cout << "  (if type is NONE, then the key was not found in dataMap)" << endl;
			exit(1);
		}
	}
	inline vector<int> getIntVector(const string &key) { // retrieve a double from a dataMap with "key"
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == INT || typeOfKey == INTSOLO) {
			return intData[key];
		} else {
			cout << "  in DataMap::getIntVector :: attempt to use getIntVector with key \"" << key << "\" but this key is associated with type " << typeOfKey << "\n  exiting." << endl;
			cout << "  (if type is NONE, then the key was not found in dataMap)" << endl;
			exit(1);
		}
	}
	inline vector<string> getStringVector(const string &key) { // retrieve a double from a dataMap with "key"
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == STRING || typeOfKey == STRINGSOLO) {
			return stringData[key];
		} else {
			cout << "  in DataMap::getStringVector :: attempt to use getStringVector with key \"" << key << "\" but this key is associated with type " << typeOfKey << "\n  exiting." << endl;
			cout << "  (if type is NONE, then the key was not found in dataMap)" << endl;
			exit(1);
		}
	}

	inline string getStringOfVector(const string &key) { // retrieve a string from a dataMap with "key" - if not already string, will be converted
		string returnString = "\"[";
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) {
			cout << "  In DataMap::GetString() :: key \"" << key << "\" is not in data map!\n  exiting." << endl;
			exit(1);
		} else {
			if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) {
				for (auto e : boolData[key]) {
					returnString += to_string(e) + ",";
				}
			} else if (typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) {
				for (auto e : doubleData[key]) {
					returnString += to_string(e) + ",";
				}
			} else if (typeOfKey == INT || typeOfKey == INTSOLO) {
				for (auto e : intData[key]) {
					returnString += to_string(e) + ",";
				}
			} else if (typeOfKey == STRING || typeOfKey == STRINGSOLO) {
				for (auto e : stringData[key]) {
					returnString += e + ",";
				}
			}
		}
		if (returnString.size() > 2) { // if vector was not empty
			returnString.pop_back(); // remove trailing ","
		}
		returnString += "]\"";
		return returnString;
	}

	// get ave of values in a vector - must be bool, double or, int
	inline double getAverage(string key) { // not ref, we may need to change to a "{LIST}" key
		dataMapType typeOfKey = findKeyInData(key);
		double returnValue = 0;
		if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) {
			for (auto e : boolData[key]) {
				returnValue += (double) e;
			}
			if (boolData[key].size() > 1) {
				returnValue /= boolData[key].size();
			} // else vector is  size 1, no div needed or vector is empty, returnValue will be 0
		} else if (typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) {
			for (auto e : doubleData[key]) {
				returnValue += (double) e;
			}
			if (doubleData[key].size() > 1) {
				returnValue /= doubleData[key].size();
			} // else vector is  size 1, no div needed or vector is empty, returnValue will be 0
		} else if (typeOfKey == INT || typeOfKey == INTSOLO) {
			for (auto e : intData[key]) {
				returnValue += (double) e;
			}
			if (intData[key].size() > 1) {
				returnValue /= intData[key].size();
			} // else vector is  size 1, no div needed or vector is empty, returnValue will be 0
		} else if (typeOfKey == STRING || typeOfKey == STRINGSOLO) {
			cout << "  in DataMap::getAverage attempt to use with vector of type string associated key \"" << key << "\".\n  Cannot average strings!\n  Exiting." << endl;
			exit(1);
		} else if (typeOfKey == NONE) {
			cout << "  in DataMap::getAverage attempt to get average from nonexistent key \"" << key << "\".\n  Exiting." << endl;
			exit(1);
		}
		return returnValue;
	}

	inline double getVariance(string key) { // not ref, we may need to change to a "{LIST}" key
		dataMapType typeOfKey = findKeyInData(key);
		double averageValue(0);
		double varianceValue(0);
		if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) {
			for (auto e : boolData[key]) {
				averageValue += (double) e;
			}
			averageValue /= boolData[key].size();
			for (auto e : boolData[key]) {
				varianceValue += ((double) e - averageValue)*((double) e - averageValue);
			}
			if (boolData[key].size() > 0)
				varianceValue /= boolData[key].size()-1;
			else
				varianceValue = 0;
		} else if (typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) {
			for (auto e : doubleData[key]) {
				averageValue += (double) e;
			}
			averageValue /= doubleData[key].size();
			for (auto e : doubleData[key]) {
				varianceValue += ((double) e - averageValue)*((double) e - averageValue);
			}
			if (doubleData[key].size() > 0)
				varianceValue /= doubleData[key].size()-1;
			else
				varianceValue = 0;
		} else if (typeOfKey == INT || typeOfKey == INTSOLO) {
			for (auto e : intData[key]) {
				averageValue += (double) e;
			}
			averageValue /= intData[key].size();
			for (auto e : intData[key]) {
				varianceValue += ((double) e - averageValue)*((double) e - averageValue);
			}
			if (intData[key].size() > 0)
				varianceValue /= intData[key].size()-1;
			else
				varianceValue = 0;
		} else if (typeOfKey == STRING || typeOfKey == STRINGSOLO) {
			cout << "  in DataMap::getVariance attempt to use with vector of type string associated key \"" << key << "\".\n  Cannot average strings!\n  Exiting." << endl;
			exit(1);
		} else if (typeOfKey == NONE) {
			cout << "  in DataMap::getVariance attempt to get value from nonexistent key \"" << key << "\".\n  Exiting." << endl;
			exit(1);
		}
		return varianceValue;
	}

	// get ave of values in a vector - must be bool, double or, int
	inline double getSum(string key) { // not ref, we may need to change to a "{LIST}" key
		dataMapType typeOfKey = findKeyInData(key);
		double returnValue = 0;
		if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) {
			for (auto e : boolData[key]) {
				returnValue += (double)e;
			}
			//if (boolData[key].size() > 1) {
			//	returnValue /= boolData[key].size();
			//} // else vector is  size 1, no div needed or vector is empty, returnValue will be 0
		}
		else if (typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) {
			for (auto e : doubleData[key]) {
				returnValue += (double)e;
			}
			//if (doubleData[key].size() > 1) {
			//	returnValue /= doubleData[key].size();
			//} // else vector is  size 1, no div needed or vector is empty, returnValue will be 0
		}
		else if (typeOfKey == INT || typeOfKey == INTSOLO) {
			for (auto e : intData[key]) {
				returnValue += (double)e;
			}
			//if (intData[key].size() > 1) {
			//	returnValue /= intData[key].size();
			//} // else vector is  size 1, no div needed or vector is empty, returnValue will be 0
		}
		else if (typeOfKey == STRING || typeOfKey == STRINGSOLO) {
			cout << "  in DataMap::getAverage attempt to use with vector of type string associated key \"" << key << "\".\n  Cannot average strings!\n  Exiting." << endl;
			exit(1);
		}
		else if (typeOfKey == NONE) {
			cout << "  in DataMap::getAverage attempt to get average from nonexistent key \"" << key << "\".\n  Exiting." << endl;
			exit(1);
		}
		return returnValue;
	}

	// Clear a field in a DataMap
	inline void clear(const string &key) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey != NONE) {
			if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) { // data is bool
				boolData.erase(key);
			} else if (typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) { // data is double
				doubleData.erase(key);
			} else if (typeOfKey == INT || typeOfKey == INTSOLO) { // data is int
				intData.erase(key);
			} else if (typeOfKey == STRING || typeOfKey == STRINGSOLO) { // data is string
				stringData.erase(key);
			}
			inUse.erase(key);
		}
	}

	// Clear all data in a DataMap
	inline void clearMap() {
		boolData.clear();
		doubleData.clear();
		intData.clear();
		stringData.clear();
		inUse.clear();
	}

	inline bool fieldExists(const string &key) {  // return true if a data map contains "key"
		return (findKeyInData(key) > 0);
	}

	// take two strings (header and data), and a list of keys, and whether or not to save "{LIST}"s. convert data from data map to header and data strings
	void constructHeaderAndDataStrings(string& headerStr, string& dataStr, const vector<string>& keys, bool aveOnly = false);

	inline void writeToFile(const string &fileName, const vector<string>& keys = { }, bool aveOnly = false) {
		//Set("score{LIST}",10.0);

		if (FileManager::files.find(fileName) == FileManager::files.end()) {  // first make sure that the dataFile has been set up.
			if (keys.size() == 0) { // if no keys are given
				FileManager::fileColumns[fileName] = getKeys();
			} else {
				FileManager::fileColumns[fileName] = keys;
			}
		}
		string headerStr = "";
		string dataStr = "";

		constructHeaderAndDataStrings(headerStr, dataStr, FileManager::fileColumns[fileName], aveOnly); // if a list is given, use that.

		FileManager::writeToFile(fileName, dataStr, headerStr);  // write the data to file!
	}


	inline vector<string> getColumnNames(){
		vector<string> columnNames;

		for (auto element:inUse){
			if (outputBehavior.find(element.first) == outputBehavior.end()){
				// this element has no defined output behavior, so it will be LIST (default) or FIRST (if it's a solo value)
				if (element.second == BOOLSOLO || element.second == DOUBLESOLO || element.second == INTSOLO || element.second == STRINGSOLO){
					columnNames.push_back(element.first);
				} else {
					columnNames.push_back(element.first + "_LIST");
				}
			} else { // there is an output behavior defined
				auto OB = outputBehavior[element.first];
				if (OB & AVE) {
					columnNames.push_back(element.first + "_AVE");
				}
				if (OB & SUM) {
					cout << "  WARNING OUTPUT METHOD SUM IS HAS YET TO BE WRITTEN!" << endl;
				}
				if (OB & PROD) {
					cout << "  WARNING OUTPUT METHOD PROD IS HAS YET TO BE WRITTEN!" << endl;
				}
				if (OB & STDERR) {
					cout << "  WARNING OUTPUT METHOD STDERR IS HAS YET TO BE WRITTEN!" << endl;
				}
				if (OB & LIST) {
					columnNames.push_back(element.first + "_LIST");
				}

			}
		}
		return columnNames;
	}
//	/*
//	 * takes a vector of string with key value pairs. Calls set for each pair.
//	 */
//	void SetMany(vector<string> dataPairs);
	

	inline DataMap remakeDataMapWithPrefix(string prefix, bool stringify = 0) {
		DataMap copyDataMap;
		dataMapType entryType;
		if (!stringify) {
			for (auto key : getKeys()) {
				entryType = findKeyInData(key);
				if (entryType == BOOL || entryType == BOOLSOLO) {
					copyDataMap.set(prefix + "_" + key, getBoolVector(key));
				}
				if (entryType == STRING || entryType == STRINGSOLO) {
					copyDataMap.set(prefix + "_" + key, getStringVector(key));
				}
				if (entryType == INT || entryType == INTSOLO) {
					copyDataMap.set(prefix + "_" + key, getIntVector(key));
				}
				if (entryType == DOUBLE || entryType == DOUBLESOLO) {
					copyDataMap.set(prefix + "_" + key, getDoubleVector(key));
				}
				copyDataMap.setOutputBehavior(prefix + "_" + key, outputBehavior[key]);
			}
		}
		else {
			for (auto key : getKeys()) {
				entryType = findKeyInData(key);
				if (entryType == BOOL || entryType == BOOLSOLO) {
					copyDataMap.set(prefix + "_" + key, getBoolVector(key));
				}
				if (entryType == STRING || entryType == STRINGSOLO) {
					copyDataMap.set(prefix + "_" + key, getStringVector(key));
				}
				if (entryType == INT || entryType == INTSOLO) {
					copyDataMap.set(prefix + "_" + key, getIntVector(key));
				}
				if (entryType == DOUBLE || entryType == DOUBLESOLO) {
					copyDataMap.set(prefix + "_" + key, getDoubleVector(key));
				}
			}
		}
		return copyDataMap;
	}

};