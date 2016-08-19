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

class OldDataMap {
private:
	map<string, string> data;

public:

	OldDataMap() = default;

	OldDataMap(shared_ptr<OldDataMap> source) {
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

class DataMap {
public:

	enum outputBehaviors {
		LIST = 1, AVE = 2, SUM = 4, PROD = 8, STDERR = 16, FIRST = 32
	}; // 0 = do not save or default..?
	map<string, int> outputBehavior; // Defines how each element should be written to file - if element not found, LIST (write out list) is used.

private:
	enum dataMapType {
		NONE = 0, BOOL = 1, DOUBLE = 2, INT = 3, STRING = 4
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
		inUse = source->inUse;
		outputBehavior = source->outputBehavior;
	}

	inline void setOutputBehavior(const string& key, int _outputBehavior) {
		outputBehavior[key] = _outputBehavior;
	}

	// find key in this data map and return type (NONE = not found)
	inline dataMapType findKeyInData(const string& key) {
		if (inUse.find(key) != inUse.end()) {
			return inUse[key];
		} else {
			return NONE;
		}
	}

	// return a string of the type of key in this data map
	inline string lookupDataMapTypeName(dataMapType t) {
		if (t == NONE) {
			return "none";
		} else if (t == BOOL) {
			return "bool";
		} else if (t == DOUBLE) {
			return "double";
		} else if (t == INT) {
			return "int";
		} else if (t == STRING) {
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

	// set functions (bool,double,int,string) that take a single value - either make new map entry or replace existing
	inline void Set(const string& key, const bool& value) {
		if (findKeyInData(key) == NONE || findKeyInData(key) == BOOL) { // if key is unused or associates with correct type
			boolData[key] = vector<bool>( { value });
			inUse[key] = BOOL;
		} else {
			cout << "  ERROR :: a call to DataMap::Set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value = \"" << value << "\" where value is bool." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}
	inline void Set(const string& key, const double& value) {
		if (findKeyInData(key) == NONE || findKeyInData(key) == DOUBLE) { // if key is unused or associates with correct type
			doubleData[key] = vector<double>( { value });
			inUse[key] = DOUBLE;
		} else {
			cout << "  ERROR :: a call to DataMap::Set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value = \"" << value << "\" where value is double." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}
	inline void Set(const string& key, const int& value) {
		if (findKeyInData(key) == NONE || findKeyInData(key) == INT) { // if key is unused or associates with correct type
			intData[key] = vector<int>( { value });
			inUse[key] = INT;
		} else {
			cout << "  ERROR :: a call to DataMap::Set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value = \"" << value << "\" where value is int." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}
	inline void Set(const string& key, const string& value) {
		if (findKeyInData(key) == NONE || findKeyInData(key) == STRING) { // if key is unused or associates with correct type
			stringData[key] = vector<string>( { value });
			inUse[key] = STRING;
		} else {
			cout << "  ERROR :: a call to DataMap::Set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value = \"" << value << "\" where value is string." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}

	// set functions (bool,double,int,string) that take a vector of value - either make new map entry or replace existing
	inline void Set(const string& key, const vector<bool>& value) {
		if (findKeyInData(key) == NONE || findKeyInData(key) == BOOL) { // if key is unused or associates with correct type
			boolData[key] = value;
			inUse[key] = BOOL;
		} else {
			cout << "  ERROR :: a call to DataMap::Set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value is a vector of bool." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}
	inline void Set(const string& key, const vector<double>& value) {
		if (findKeyInData(key) == NONE || findKeyInData(key) == DOUBLE) { // if key is unused or associates with correct type
			doubleData[key] = value;
			inUse[key] = DOUBLE;
		} else {
			cout << "  ERROR :: a call to DataMap::Set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value is a vector of double." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}
	inline void Set(const string& key, const vector<int>& value) {
		if (findKeyInData(key) == NONE || findKeyInData(key) == INT) { // if key is unused or associates with correct type
			intData[key] = value;
			inUse[key] = INT;
		} else {
			cout << "  ERROR :: a call to DataMap::Set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value is a vector of int." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}
	inline void Set(const string& key, const vector<string>& value) {
		if (findKeyInData(key) == NONE || findKeyInData(key) == STRING) { // if key is unused or associates with correct type
			stringData[key] = value;
			inUse[key] = STRING;
		} else {
			cout << "  ERROR :: a call to DataMap::Set was called where the key was already in use with another type." << endl;
			cout << "  function was called with : key = \"" << key << "\" value is a vector of string." << endl;
			cout << "  but ... key is already associated with type " << findKeyInData(key) << ". Exiting." << endl;
			exit(1);
		}
	}

	// append a value to the end of vector associated with key. If key is not found, start a new vector for key
	inline void Append(const string& key, const bool& value) {
		int typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			Set(key, value);
		} else if (typeOfKey == BOOL) { // if this key is in data map as a string, append to vector
			boolData[key].push_back(value);
		} else {
			cout << "  In DataMap::Append :: attempt to append value \"" << value << "\" of type bool to \"" << key << "\" but this key is already associated with " << typeOfKey << ".\n  exiting." << endl;
		}
	}
	inline void Append(const string& key, const double& value) {
		int typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			Set(key, value);
		} else if (typeOfKey == DOUBLE) { // if this key is in data map as a string, append to vector
			doubleData[key].push_back(value);
		} else {
			cout << "  In DataMap::Append :: attempt to append value \"" << value << "\" of type double to \"" << key << "\" but this key is already associated with " << typeOfKey << ".\n  exiting." << endl;
		}
	}
	inline void Append(const string& key, const int& value) {
		int typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			Set(key, value);
		} else if (typeOfKey == INT) { // if this key is in data map as a string, append to vector
			intData[key].push_back(value);
		} else {
			cout << "  In DataMap::Append :: attempt to append value \"" << value << "\" of type int to \"" << key << "\" but this key is already associated with " << typeOfKey << ".\n  exiting." << endl;
		}
	}
	inline void Append(const string& key, const string& value) {
		int typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			Set(key, value);
		} else if (typeOfKey == STRING) { // if this key is in data map as a string, append to vector
			stringData[key].push_back(value);
		} else {
			cout << "  In DataMap::Append :: attempt to append value \"" << value << "\" of type string to \"" << key << "\" but this key is already associated with " << typeOfKey << ".\n  exiting." << endl;
		}
	}

	// append a vector of values to the end of vector associated with key. If key is not found, start a new vector for key
	inline void Append(const string& key, const vector<bool>& value) {
		int typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			Set(key, value);
		} else if (typeOfKey == BOOL) { // if this key is in data map as a string, append to vector
			boolData[key].insert(boolData[key].end(), value.begin(), value.end());
		} else {
			cout << "  In DataMap::Append :: attempt to append a vector of type bool to \"" << key << "\" but this key is already associated with " << typeOfKey << ".\n  exiting." << endl;
		}
	}
	inline void Append(const string& key, const vector<double>& value) {
		int typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			Set(key, value);
		} else if (typeOfKey == DOUBLE) { // if this key is in data map as a string, append to vector
			doubleData[key].insert(doubleData[key].end(), value.begin(), value.end());
		} else {
			cout << "  In DataMap::Append :: attempt to append a vector of type double to \"" << key << "\" but this key is already associated with " << typeOfKey << ".\n  exiting." << endl;
		}
	}
	inline void Append(const string& key, const vector<int>& value) {
		int typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			Set(key, value);
		} else if (typeOfKey == INT) { // if this key is in data map as a string, append to vector
			intData[key].insert(intData[key].end(), value.begin(), value.end());
		} else {
			cout << "  In DataMap::Append :: attempt to append a vector of type int to \"" << key << "\" but this key is already associated with " << typeOfKey << ".\n  exiting." << endl;
		}
	}
	inline void Append(const string& key, const vector<string>& value) {
		int typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) { // this key is not in data map, use Set.
			Set(key, value);
		} else if (typeOfKey == STRING) { // if this key is in data map as a string, append to vector
			stringData[key].insert(stringData[key].end(), value.begin(), value.end());
		} else {
			cout << "  In DataMap::Append :: attempt to append a vector of type string to \"" << key << "\" but this key is already associated with " << typeOfKey << ".\n  exiting." << endl;
		}
	}

	// merge contents of two data maps - if common keys are found, replace 1 = overwrite, replace 0 = append
	inline void Merge(DataMap& otherDataMap, bool replace = 0) {
		if (replace == true) {
			for (string key : otherDataMap.getKeys()) {
				int typeOfKey = findKeyInData(key);
				if (typeOfKey == BOOL) {
					Set(key, otherDataMap.GetBoolVector(key));
				}
				if (typeOfKey == DOUBLE) {
					Set(key, otherDataMap.GetDoubleVector(key));
				}
				if (typeOfKey == INT) {
					Set(key, otherDataMap.GetIntVector(key));
				}
				if (typeOfKey == STRING) {
					Set(key, otherDataMap.GetStringVector(key));
				}
			}
		} else { // replace != true
			for (string key : otherDataMap.getKeys()) {
				int typeOfKey = findKeyInData(key);
				if (typeOfKey == BOOL) {
					Append(key, otherDataMap.GetBoolVector(key));
				}
				if (typeOfKey == DOUBLE) {
					Append(key, otherDataMap.GetDoubleVector(key));
				}
				if (typeOfKey == INT) {
					Append(key, otherDataMap.GetIntVector(key));
				}
				if (typeOfKey == STRING) {
					Append(key, otherDataMap.GetStringVector(key));
				}
			}
		}
	}

	inline vector<bool> GetBoolVector(const string &key) { // retrieve a double from a dataMap with "key"
		int typeOfKey = findKeyInData(key);
		if (typeOfKey == BOOL) {
			return boolData[key];
		} else {
			cout << "  in DataMap::GetBoolVector :: attempt to use GetBoolVector with key \"" << key << "\" but this key is associated with type " << typeOfKey << "\n  exiting." << endl;
			cout << "  (if type is NONE, then the key was not found in dataMap)" << endl;
			exit(1);
		}
	}
	inline vector<double> GetDoubleVector(const string &key) { // retrieve a double from a dataMap with "key"
		int typeOfKey = findKeyInData(key);
		if (typeOfKey == DOUBLE) {
			return doubleData[key];
		} else {
			cout << "  in DataMap::GetDoubleVector :: attempt to use GetDoubleVector with key \"" << key << "\" but this key is associated with type " << typeOfKey << "\n  exiting." << endl;
			cout << "  (if type is NONE, then the key was not found in dataMap)" << endl;
			exit(1);
		}
	}
	inline vector<int> GetIntVector(const string &key) { // retrieve a double from a dataMap with "key"
		int typeOfKey = findKeyInData(key);
		if (typeOfKey == INT) {
			return intData[key];
		} else {
			cout << "  in DataMap::GetIntVector :: attempt to use GetIntVector with key \"" << key << "\" but this key is associated with type " << typeOfKey << "\n  exiting." << endl;
			cout << "  (if type is NONE, then the key was not found in dataMap)" << endl;
			exit(1);
		}
	}
	inline vector<string> GetStringVector(const string &key) { // retrieve a double from a dataMap with "key"
		int typeOfKey = findKeyInData(key);
		if (typeOfKey == STRING) {
			return stringData[key];
		} else {
			cout << "  in DataMap::GetStringVector :: attempt to use GetStringVector with key \"" << key << "\" but this key is associated with type " << typeOfKey << "\n  exiting." << endl;
			cout << "  (if type is NONE, then the key was not found in dataMap)" << endl;
			exit(1);
		}
	}

	inline string GetStringOfVector(const string &key) { // retrieve a string from a dataMap with "key" - if not already string, will be converted
		string returnString = "\"[";
		int typeOfKey = findKeyInData(key);
		if (typeOfKey == NONE) {
			cout << "  In DataMap::GetString() :: key \"" << key << "\" is not in data map!\n  exiting." << endl;
			exit(1);
		} else {
			if (typeOfKey == BOOL) {
				for (auto e : boolData[key]) {
					returnString += to_string(e) + ",";
				}
			} else if (typeOfKey == DOUBLE) {
				for (auto e : doubleData[key]) {
					returnString += to_string(e) + ",";
				}
			} else if (typeOfKey == INT) {
				for (auto e : intData[key]) {
					returnString += to_string(e) + ",";
				}
			} else if (typeOfKey == STRING) {
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
	inline double GetAverage(string key) { // not ref, we may need to change to a "{LIST}" key
		int typeOfKey = findKeyInData(key);
		if (typeOfKey == 0) { // maybe it's a "{LIST}" so also check that
			key += "{LIST}";
			typeOfKey = findKeyInData(key);
		}
		double returnValue = 0;
		if (typeOfKey == BOOL) {
			for (auto e : boolData[key]) {
				returnValue += (double) e;
			}
			if (boolData[key].size() > 1) {
				returnValue /= boolData[key].size();
			} // else vector is  size 1, no div needed or vector is empty, returnValue will be 0
		} else if (typeOfKey == DOUBLE) {
			for (auto e : doubleData[key]) {
				returnValue += (double) e;
			}
			if (doubleData[key].size() > 1) {
				returnValue /= doubleData[key].size();
			} // else vector is  size 1, no div needed or vector is empty, returnValue will be 0
		} else if (typeOfKey == INT) {
			for (auto e : intData[key]) {
				returnValue += (double) e;
			}
			if (intData[key].size() > 1) {
				returnValue /= intData[key].size();
			} // else vector is  size 1, no div needed or vector is empty, returnValue will be 0
		} else if (typeOfKey == STRING) {
			cout << "  in DataMap::GetAverage attempt to use with vector of type string associated key \"" << key << "\".\n  Cannot average strings!\n  Exiting." << endl;
			exit(1);
		}
		return returnValue;
	}

	// Clear a field in a DataMap
	inline void Clear(const string &key) {
		dataMapType typeOfKey = findKeyInData(key);
		if (typeOfKey != NONE) {
			if (typeOfKey == BOOL) { // data is bool
				boolData.erase(key);
			} else if (typeOfKey == DOUBLE) { // data is double
				doubleData.erase(key);
			} else if (typeOfKey == INT) { // data is int
				intData.erase(key);
			} else if (typeOfKey == STRING) { // data is string
				stringData.erase(key);
			}
			inUse.erase(key);
		}
	}

	// Clear all data in a DataMap
	inline void ClearMap() {
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
	inline void constructHeaderAndDataStrings(string& headerStr, string& dataStr, const vector<string>& keys, bool aveOnly = false) {
		headerStr = ""; // make sure the strings are clean
		dataStr = "";
		dataMapType typeOfKey;
		int OB; // holds output behavior so it can be over ridden for ave file output!
		if (keys.size() > 0) {  // if we keys is not empty
			for (auto i : keys) {
				typeOfKey = findKeyInData(i);
				//typeOfKeyLIST = findKeyInData(i + "{LIST}");

				if (typeOfKey == NONE) {
					cout << "  in DataMap::writeToFile() - key \"" << i << "\" can not be found in data map!\n  exiting." << endl;
					exit(1);
				}

				// the following code makes use of bit masks! in short, AVE,SUM,LIST,etc each use only one bit of an int.
				// therefore if we apply that mask the the outputBehavior, we can see if that type of output is needed.

				OB = outputBehavior[i];
				if (aveOnly) {
					cout << OB << " ";
					OB = OB & (AVE|FIRST); // if aveOnly, only output AVE on the entries that have been set for AVE
					cout << "here " << i << "  " << OB << endl;
				}

				if (OB & FIRST) { // save first (only?) element in vector with key as column name
					headerStr = headerStr + FileManager::separator + i;
					if (typeOfKey == BOOL){
						if (GetBoolVector(i).size()>0){
							dataStr = dataStr + FileManager::separator + to_string(GetBoolVector(i)[0]);
						} else {
							dataStr = dataStr + '0';
							cout << "  WARNING!! In DataMap::constructHeaderAndDataStrings :: while getting value for FIRST with key \"" << i << "\" vector is empty!" << endl;
						}
					}
					if (typeOfKey == DOUBLE){
						if (GetDoubleVector(i).size()>0){
							dataStr = dataStr + FileManager::separator + to_string(GetDoubleVector(i)[0]);
						} else {
							dataStr = dataStr + '0';
							cout << "  WARNING!! In DataMap::constructHeaderAndDataStrings :: while getting value for FIRST with key \"" << i << "\" vector is empty!" << endl;
						}
					}
					if (typeOfKey == INT){
						if (GetIntVector(i).size()>0){
							dataStr = dataStr + FileManager::separator + to_string(GetIntVector(i)[0]);
						} else {
							dataStr = dataStr + '0';
							cout << "  WARNING!! In DataMap::constructHeaderAndDataStrings :: while getting value for FIRST with key \"" << i << "\" vector is empty!" << endl;
						}
					}
					if (typeOfKey == STRING){
						if (GetStringVector(i).size()>0){
							dataStr = dataStr + FileManager::separator + to_string(GetStringVector(i)[0]);
						} else {
							dataStr = dataStr + '0';
							cout << "  WARNING!! In DataMap::constructHeaderAndDataStrings :: while getting value for FIRST with key \"" << i << "\" vector is empty!" << endl;
						}
					}

				}
				if (OB & AVE) { // key_AVE = ave of vector (will error if of type string!)
					headerStr = headerStr + FileManager::separator + i + "_AVE";
					dataStr = dataStr + FileManager::separator + to_string(GetAverage(i));
				}
				if (OB & SUM) { // key_SUM = sum of vector
					cout << "  WARNING OUTPUT METHOD SUM IS HAS YET TO BE WRITTEN!" << endl;
				}
				if (OB & PROD) { // key_PROD = product of vector
					cout << "  WARNING OUTPUT METHOD PROD IS HAS YET TO BE WRITTEN!" << endl;
				}
				if (OB & STDERR) { // key_STDERR = standard error of vector
					cout << "  WARNING OUTPUT METHOD STDERR IS HAS YET TO BE WRITTEN!" << endl;
				}
				if (OB & LIST) { //key_LIST = save all elements in vector in csv list format
					headerStr = headerStr + FileManager::separator + i + "_LIST";
					dataStr = dataStr + FileManager::separator + GetStringOfVector(i);
				}
			}
			headerStr.erase(headerStr.begin());  // clip off the leading separator
			dataStr.erase(dataStr.begin());  // clip off the leading separator
		}
	}

	inline void writeToFile(const string &fileName, const vector<string>& keys = { }, bool aveOnly = false) {
		//Set("score{LIST}",10.0);
		string headerStr = "";
		string dataStr = "";
		if (keys.size() == 0) { // if no keys are given
			constructHeaderAndDataStrings(headerStr, dataStr, getKeys(), aveOnly); // create strings with all keys (get keys, removing {LIST} if it's there.)
		} else {
			constructHeaderAndDataStrings(headerStr, dataStr, keys, aveOnly); // if a list is given, use that.
		}
		FileManager::writeToFile(fileName, dataStr, headerStr);  // write the data to file!
	}

//	/*
//	 * takes a vector of string with key value pairs. Calls set for each pair.
//	 */
//	void SetMany(vector<string> dataPairs);

};

#endif /* defined(__BasicMarkovBrainTemplate__Data__) */
