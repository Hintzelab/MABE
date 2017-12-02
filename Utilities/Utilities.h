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
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <sstream>
#include <set>
#include <vector>

using namespace std;


template<typename T1, typename T2>
bool HaveSameType(T1, T2) {
	return is_same<T1, T2>();
}

inline string get_var_typename(const bool&) {
	return "bool";
}
inline string get_var_typename(const string&) {
	return "string";
}
inline string get_var_typename(const int&) {
	return "int";
}
inline string get_var_typename(const double&) {
	return "double";
}

inline vector<string> nameSpaceToNameParts(const string& nameSpace) {
	string localNameSpace = nameSpace;
	vector<string> nameParts;
	bool done = (nameSpace.size() == 0);
	bool nameSpaceValid = true;
	while (!done) {
		if (localNameSpace.size() > 2) {  // name space name must end with :: so must have more then 2 characters
			int i = 0;
			while ((localNameSpace[i] != ':' || localNameSpace[i + 1] != ':') && (i < (int) localNameSpace.size() - 2)) {
				i++;
			}
			if (!(localNameSpace[i] == ':' && localNameSpace[i + 1] == ':')) {  // if there is not "::" at the end of the name space part
				nameSpaceValid = false;
				done = true;
			} else {  // found "::"
				nameParts.push_back(localNameSpace.substr(0, i + 2));
				localNameSpace = localNameSpace.substr(i + 2, localNameSpace.size());
				if (localNameSpace.size() == 0) {  // if after cutting of part there is nothing left
					done = true;
				}
			}
		} else {  // if the name space name part is less then 3 characters... ie, must be at least "*::"
			nameSpaceValid = false;
			done = true;
		}
	}
	if (!nameSpaceValid) {
		cout << "  Error::in nameSpaceToNameParts(const string& nameSpace). name space is invalid.\n  Can't parse \"" << localNameSpace << "\"\n   Parameter name space must end in ::\nExiting." << endl;
		exit(1);
	}
	return nameParts;
}


/*
 * return x % y were (-1 % y) = (y - 1)
 * (-2 % y) = (y - 2), etc.
 */
inline int loopMod(const int numerator, const int denominator) {
	return ((numerator % denominator) + denominator) % denominator;
}


inline double loopModDouble(const double numerator, const double denominator) {
	double n = numerator;
	while (n < 0){
		n+=denominator;
	}
	while (n >= denominator){
		n-=denominator;
	}
	return n;
}

// returns 1 if "d" is greater than 0, else return 0
template<typename Type>
inline int Bit(Type d) {
	return d > 0.0;
}

// returns 1 if "d" is greater than 0
//         0 if "d" is equal to 0
//        -1 if "d" is less than 0
template<typename Type>
inline int Trit(Type d) {
	if (d < 0) {
		return -1;
	} else {
		return d > 0;
	}
}

inline vector<string> parseCSVLine(string rawLine, const char separator = ',') {
	vector<string> dataLine;
	string dataEntry;
	bool inQuote = false;

	dataEntry.clear();
	for (auto c : rawLine) {
		if (inQuote) {  // if inQuote
			if (c == '\"') {  // found the closed quote, we are at the end of the quote
				inQuote = false;
			} else {
				dataEntry.push_back(c);  // else just append c to the back of dataEntry
			}
		} else {  // if not in quote
			if (!isspace(c)) {
				if (c == '\"') {  // we have just entered some quoted text, just read everything until the next quote
					inQuote = true;
				} else {  // we are not inQuote and this c is not whitespace
					if (c == separator) {  // if we are at a separator
						dataLine.push_back(dataEntry);  // add dataEntry to dataLine
						dataEntry.clear();  // ... and clear out dataEntry to make room for the next value
					} else {
						dataEntry.push_back(c);  // look it's just text, add it to dataEntry
					}
				}

				// read non " non whitespace stuff (i.e. look for separators and entries);

			} else {  // it is whitespace. there should be no whitespace in this file. Error.
				cout << "In parseCSVLine(string rawLine)\nWhile attempting to read csv file, encountered whitespace. This file should have no whitespace. Exiting.\n";
				exit(1);
			}
		}
	}
	dataLine.push_back(dataEntry);  // we are at the end of the line. add dataEntry to dataLine
	dataEntry.clear();  // ... and clear out dataEntry to make room for the next value
	return dataLine;
}

// reads a csv file with header and converts to a map of string,vector<string>
inline map<string, vector<string>> readFromCSVFile(const string& fileName, const char separator = ',') {
	std::ifstream FILE(fileName);
	map<string, vector<string>> data;  // the final map
	vector<string> dataLine;  // one line of data, converted from a single csv string to a vector of strings
	string rawLine;
	bool firstLine = true;
	vector<string> lookUpTable;

	if (FILE.is_open())  // if the file named by configFileName can be opened
	{
		dataLine.clear();
		while (getline(FILE, rawLine))  // keep loading one line from the file at a time into "line" until we get to the end of the file
		{
			dataLine = parseCSVLine(rawLine, separator);
			if (firstLine) {  // this is the first line, dataLine contains the keys... use them to build the map and loopUpTable
				for (auto key : dataLine) {
					if (isspace(key.back())){
						key.pop_back();
					}
					lookUpTable.push_back(key);  // add the key so we can make sure we assign the right values to the right columns
					data[key] = {};  // add an empty vector for each key into the map data
				}
				firstLine = false;
			} else {  // we are not in the first line, dataLine has values
				for (size_t i = 0; i < dataLine.size(); i++) {  // for each entry in dataLine
					data[lookUpTable[i]].push_back(dataLine[i]);
				}
			}
		}
	}
	return data;
}

// extract a value from a map<string,vector<string>>
// given a value from one vector, return the value in another vector at the same index
inline string CSVLookUp(map<string, vector<string>> CSV_Table, const string& lookupKey, const string& lookupValue, const string& returnKey) {

	int lookupIndex = -1;
	size_t i = 0;
	string temp;

	// check to make sure that CSV_Table has both the lookup and return keys
	// throw errors if either key is not found
	auto iter = CSV_Table.find(lookupKey);
	if (iter == CSV_Table.end()) {
		throw std::invalid_argument("CSVLookup could not find requested lookup key\n");
	}

	iter = CSV_Table.find(returnKey);
	if (iter == CSV_Table.end()) {
		throw std::invalid_argument("CSVLookup could not find requested return key\n");
	}

	// if the lookup and return keys exist, look for the look up value in the lookup keys vector
	while (i < CSV_Table[lookupKey].size() && lookupIndex == -1) {
		if (lookupValue == CSV_Table[lookupKey][i]) {
			lookupIndex = i;
		}
		i++;
	}

	// if the lookup value is was not found, throw an error
	if (lookupIndex == -1) {
		throw std::invalid_argument("CSVLookup could not find requested lookup value.\n");
	}

	return CSV_Table[returnKey][lookupIndex];
}

// Put an arbitrary value to the target variable, return false on conversion failure
template<class T>
static bool load_value(const string& value, T& target) {
	std::stringstream ss(value);
	ss >> target;
	if (ss.fail()) {
		return false;
	} else {
		string remaining;
		ss >> remaining;
		// stream failure means nothing left in stream, which is what we want
		return ss.fail();
	}
}

// Put an arbitrary value to the target variable, return false on conversion failure (COPIES FUNCTION OF load_value()!)
template<class T>
static bool stringToValue(const string& source, T& target) {
	std::stringstream ss(source);
	ss >> target;
	if (ss.fail()) {
		return false;
	} else {
		string remaining;
		ss >> remaining;
		// stream failure means nothing left in stream, which is what we want
		return ss.fail();
	}
}

// converts a vector of string to a vector of type of returnData
template<class T>
void convertCSVListToVector(string stringData, vector<T> &returnData, const char separator = ',') {
	returnData.clear();
	if (stringData[0] == '\"') {
		stringData = stringData.substr(1, stringData.size() - 2);  // strip off leading and trailing quotes
	}
	if (stringData[0] == '[') {
		stringData = stringData.substr(1, stringData.size() - 2);  // strip off leading and trailing square brackets
	}
	if (stringData.size() != 0) {  // this is not an empty list... if it is an empty list, we don't need to do anything else
		vector<string> dataLine = parseCSVLine(stringData, separator);

		T tempValue;
		for (auto s : dataLine) {
			bool success = load_value(s, tempValue);
			if (!success) {
				cout << " --- while parsing: " << stringData << " .... " << endl;
				cout << " In convertCSVListToVector() attempt to convert string to value failed\n" << endl;
				exit(1);
			} else {
				returnData.push_back(tempValue);
			}
		}
	}
}

// this is here so we can use to string and it will work even if we give it a string as input
inline string to_string(string str) {
	return (str);
}

/*
 * getBestInVector(vector<T> vec)
 * given W (a list of scores), return the index of the highest score
 * if more then one genome has max score, return the first one.
 */
template<typename Type>
inline int findGreatestInVector(vector<Type> vec) {
	return distance(vec.begin(), max_element(vec.begin(), vec.end()));
}

// takes a vector of template values and a vector of indices.
// each indicated value is converted to a bit and the resulting bit string is packed into an int.
// if reverseOrder, than the last index is read first
//
// useful to generate values for lookups
// reverseOrder is useful for gate lookups as it maintains meaning even if the number of inputs to a gate changes
// i.e. since the first input is read last, 1, 10 and 100 (etc.) all return 1. (01)
// and 01, 010, 0100 (etc.) all return 2 (10)
// and 11, 110, 1100 (etc.) all return 3 (11)
// etc... hopefully you see the pattern

template<typename Type>
inline int vectorToBitToInt(const vector<Type> &nodes, const vector<int> &nodeAddresses, bool reverseOrder = false) {
	int result = 0;
	if (reverseOrder) {

		for (int i = (int) nodeAddresses.size() - 1; i >= 0; i--) {
			result = (result << 1) + Bit(nodes[nodeAddresses[i]]);
		}
	} else {
		for (int i = 0; i < (int) nodeAddresses.size(); i++) {
			result = (result << 1) + Bit(nodes[nodeAddresses[i]]);
		}
	}
	return result;
}

// see vectorToBitToInt, same process, but for Trits
template<typename Type>
inline int vectorToTritToInt(const vector<Type> &nodes, const vector<int> &nodeAddresses, bool reverseOrder = false) {
	int result = 0;
	if (reverseOrder) {
		for (int i = (int) nodeAddresses.size() - 1; i >= 0; i--) {
			result = (result * 3) + (Trit(nodes[nodeAddresses[i]]) + 1);
		}
	} else {
		for (int i = 0; i < (int) nodeAddresses.size(); i++) {
			result = (result * 3) + (Trit(nodes[nodeAddresses[i]]) + 1);
		}
	}
	return result;
}


// converts a ',' separated formatted string to sequence with duplicates removed
// single number -> just add that number
// x-y -> add from x to y
// x-y:z -> add from x to y on z
// x:z -> from x to defaultMax on z (if defaultMax is not defined, error... see below)
// :z -? from 0 to defaultMax on z (if defaultMax is not defined, error... see below)
//
// on error (see x:z and :z or if the str is not formatted correctly), return an empty vector (size 0)
//
// addZero true will insure that 0 is in the sequence
//
// function works by first parsing seqStr into a vector where each element if a vector with {start,end,step}
// next, each list is added to a new vector. This vector is convered to a set and back to a vector (which both
// remove duplicates, and sorts)
//
// example input   /   output
// 4                                      /   4
// 10-20                                  /   10,11,12,13,14,15,16,17,18,19,20
// 10-20:3                                /   10,13,16,19
// 5:5 (with defaultMax = 20)             /   5,10,15,20
// :3 (with defaultMax = 20)              /   0,3,6,9,12,15,18
// 4,10-15,30:2 ((with defaultMax = 40)   /   4,10,11,12,13,14,15,30,32,34,36,38,40


inline vector<int> seq(const string seqStr, int defaultMax = -1, bool addZero = false) {
	stringstream ss(seqStr);
	int n;
	char c;
	bool error = (seqStr.size() == 0) ? true : false;  // detect empty string

	vector<vector<int>> seqDefinitions;

	int currentDef = 0;

	while (!ss.fail() && !error) {  // while not at end of string and no error has been detected
		seqDefinitions.resize(seqDefinitions.size() + 1);
		ss >> n;
		if (ss.fail()) {  // if first element read was not an int
			ss.clear();
			ss >> c;
			if (c == ':') {  // this may be a definition of format ':x', that is a range from 0
				ss >> n;
				if (ss.fail()) {  // we were expecting an int!
					error = true;
				} else {  // set up seq definition 0 > defaultMax : n
					if (defaultMax == -1) {
						cout << "Error in seq(const string seqStr, int defaultMax = -1)\n  attempt to use simple range (':x'), but defaultMax was not set\n  will return empty sequence."<<endl;
						error = true;
					}
					seqDefinitions[currentDef].push_back(0);
					seqDefinitions[currentDef].push_back(defaultMax);
					seqDefinitions[currentDef].push_back(n);
					ss >> c;
					if (c != ',' && !ss.fail()) {
						error = true;
					}
				}
			} else {  // first element was neither an int or ':'
				error = true;
			}
		} else {  // first element was an int
			seqDefinitions[currentDef].push_back(n);  // push back first number read
			ss >> c;
			if (c == ',' || ss.fail()) {
				// if next char is a ',' or end of string, we have a single value or a ':x' definition
				// do nothing...
			} else if (c == '-') {
				ss >> n;
				seqDefinitions[currentDef].push_back(n);
				if (ss.fail()) {
					error = true;
				}
				ss >> c;
				if (c == ':') {
					ss >> n;
					seqDefinitions[currentDef].push_back(n);
					ss >> c;
					if (c != ',' && !ss.fail()) {
						error = true;
					}
				} else if (c == ',' || ss.fail()) {
					//we are at the end of this definition (either comma, get read to read more, or end of string)
					seqDefinitions[currentDef].push_back(1); // no step was provided
				} else {
					error = true;  // we did not find a comma or end of string at the end of a definition
				}
			} else if (c == ':') {  // this maybe a definition of format x : y
				ss >> n;
				if (ss.fail()) {  // we were expecting an int!
					error = true;
				} else {  // set up seq definition 0 > defaultMax : n
					if (defaultMax == -1) {
						cout << "ERROR :: in seq(const string seqStr, int defaultMax = -1)\n  attempt to define unbound range ('x:y'), but defaultMax was not set\n  will return empty sequence."<<endl;
						error = true;
					}
					seqDefinitions[currentDef].push_back(defaultMax);  // first is alreay pushed
					seqDefinitions[currentDef].push_back(n);
					ss >> c;
					if (c != ',' && !ss.fail()) {
						error = true;
					}
				}
			} else {
				error = true;  // we needed to find either '>' or ',' and did not!
			}
		}
		currentDef++;
	}

	vector<int> seq;

	if (error) {
		seq.resize(0);
		return seq;
	}

	for (auto seqDef : seqDefinitions) {
		if (seqDef.size() == 1) {
			seq.push_back(seqDef[0]);
		} else {
			int current = seqDef[0];
			int end = seqDef[1];
			int step = seqDef[2];

			if (current < end) {
				for (; current <= end; current += step) {
					seq.push_back(current);
				}
			} else {
				for (; current >= end; current -= step) {
					seq.push_back(current);
				}
			}
		}
	}

	if (addZero){
		seq.push_back(0); // may result in duplicate which will be corrected in the next step
	}
	set<int> converterSet(seq.begin(), seq.end());
	seq.assign(converterSet.begin(), converterSet.end());

	return seq;
}

// load a line from FILE. IF the line is empty or a comment (starts with #), skip line.
// if the line is not empty/comment, clean ss and load line.
// rawLine is the string version of the same data as ss
inline bool loadLineToSS(ifstream& FILE, string& rawLine, stringstream& ss) {
	rawLine.resize(0);
	if (FILE.is_open() && !FILE.eof()) {
		while ((rawLine.size() == 0 || rawLine[0] == '#') && !FILE.eof()) {
			getline(FILE, rawLine);
		}
		ss.clear();
		ss.str(string());
		ss << rawLine;
	} else if (!FILE.eof()) {
		cout << "in loadSS, FILE is not open!\n  Exiting." << endl;
		exit(1);
	}
	//cout << "from file:  " << rawLine << endl;
	return FILE.eof();
}



// load all genomes from a file
inline void loadIndexedCSVFile(const string& fileName, unordered_map<int,unordered_map<string,string>>& data, string& indexName) {
	data.clear();
	std::ifstream FILE(fileName);
	string rawLine;
	vector<string> keys,tempValues;
	int numKeys;
	if (FILE.is_open()) {  // if the file named by configFileName can be opened
		getline(FILE, rawLine);
		// extract header
		rawLine = "[" + rawLine + "]";
		convertCSVListToVector(rawLine, keys);
		numKeys = keys.size();
		// find indexIndex
		auto index = find(keys.begin(), keys.end(), indexName) - keys.begin();
		while (getline(FILE, rawLine)) {
			rawLine = "[" + rawLine + "]";
			convertCSVListToVector(rawLine, tempValues);
			int mapKey;
			stringToValue(tempValues[index], mapKey);
			for (int i = 0; i < numKeys; i++) {
				data[mapKey][keys[i]] = tempValues[i];
				//cout << keys[i] << " = " << tempValues[i] << "   --  " << flush;
			}
			//cout << endl;
		}
	}
	else {
		cout << "\n\nERROR: In MultiGenome::loadGenomeFile, unable to open file \"" << fileName << "\"\n\nExiting\n" << endl;
		exit(1);
	}
}

/*
// must add vector<T> to CSVString function
inline string GetStringOfVector(const string &key) { // retrieve a string from a dataMap with "key" - if not already string, will be converted
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
*/
