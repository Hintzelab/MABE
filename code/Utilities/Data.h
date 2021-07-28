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

#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Utilities.h"

class FileManager {
private:
  static void openFile(const std::string &fileName, const std::string &header = ""); // open file and write header to file if file is new and header is provided
public:
  static std::map<std::string, std::vector<std::string>>
      fileColumns;                     // list of files (NAME,LIST OF COLUMNS)
  static std::map<std::string, std::ofstream> files;  // list of files (NAME,ofstream)
  static std::map<std::string, bool> fileStates; // list of files states (NAME,open?)

  static std::string outputPrefix;

  static const char separator = ',';

  /* opens file if not open, writes header if first file access, writes data if present, does not close file */
  static void openAndWriteToFile(const std::string &fileName, const std::string &data, const std::string &header = ""); // fileName, data, header - used when you want to output formatted data (i.e. genomes)
  static void writeToFile(const std::string &fileName, const std::string &data, const std::string &header = ""); // fileName, data, header - used when you want to output formatted data (i.e. genomes)
  static void closeFile(const std::string &fileName);   // close file
};

class DataMap {
public:
  enum outputBehaviors {
    LIST = 1,
    AVE = 2,
    SUM = 4,
    PROD = 8,
    STDERR = 16,
    FIRST = 32,
    VAR = 64,
	 NO_OUTPUT = 128
  };                               // 0 = do not save or default..?
  std::map<std::string, int> outputBehavior; // Defines how each element should be written
                                   // to file - if element not found, LIST
                                   // (write out list) is used.
  static std::map<std::string, int> knownOutputBehaviors;

private:
  enum dataMapType {
    NONE = 0,
    BOOL = 1,
    DOUBLE = 2,
    INT = 3,
    STRING = 4,
    BOOLSOLO = 11,
    DOUBLESOLO = 12,
    INTSOLO = 13,
    STRINGSOLO = 14
  }; // NONE = not found in this data map

  std::unordered_map<std::string, std::vector<bool>> boolData;
  std::unordered_map<std::string, std::vector<double>> doubleData;
  std::unordered_map<std::string, std::vector<int>> intData;
  std::unordered_map<std::string, std::vector<std::string>> stringData;

  std::map<std::string, dataMapType>
      inUse; // holds list of keys with type 1=bool, 2=double, 3=int, 4=string

public:
  DataMap() = default;

  // copy constructor
  DataMap(std::shared_ptr<DataMap> source);

  inline void setOutputBehavior(const std::string &key, int _outputBehavior) {
    outputBehavior[key] = _outputBehavior;
  }

  // find key in this data map and return type (NONE = not found)
  inline dataMapType findKeyInData(const std::string &key, bool printType = false) {
    if (printType) {
      std::cout << key << "is of type " << inUse[key] << std::endl;
    }
    if (inUse.find(key) != inUse.end()) {
      return inUse[key];
    } else {
      return NONE;
    }
  }

  // find key in this data map and return type (NONE = not found)
  inline bool isKeySolo(const std::string &key) {
    if (inUse.find(key) != inUse.end()) {
      return ((findKeyInData(key) == BOOLSOLO) ||
              (findKeyInData(key) == DOUBLESOLO) ||
              (findKeyInData(key) == INTSOLO) ||
              (findKeyInData(key) == STRINGSOLO));
    } else {
      std::cout << "  ERROR :: in DataMap::isKeySolo, key name " << key
           << " is not defined in DataMap. Exiting!" << std::endl;
      exit(1);
    }
  }

  // return a string of the type of key in this data map
  inline std::string lookupDataMapTypeName(dataMapType t) {
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
      std::cout << "  In lookupDataMapTypeName... type " << t
           << " is not defined. Exiting!" << std::endl;
      exit(1);
    }
  }

  // return vector of strings will all keys in this data map
  inline std::vector<std::string> getKeys() {
    std::vector<std::string> keys;
    for (auto e : inUse) {
		 if (outputBehavior[e.first] != NO_OUTPUT) keys.push_back(e.first); // just push back the whole key
    }
    return (keys);
  }

  // set functions (bool,double,int,string) that take a **single** value -
  // either make new map entry or replace existing
  inline void set(const std::string &key, const bool &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE || typeOfKey == BOOL ||
        typeOfKey ==
            BOOLSOLO) { // if key is unused or associates with correct type
      boolData[key] = std::vector<bool>({value});
      inUse[key] = BOOLSOLO; // since this is set with SET, it is a single value
    } else {
      std::cout << "  ERROR :: a call to DataMap::set was called where the key was "
              "already in use with another type."
           << std::endl;
      std::cout << "  function was called with : key = \"" << key << "\" value = \""
           << value << "\" where value is bool." << std::endl;
      std::cout << "  but ... key is already associated with type "
           << findKeyInData(key) << ". Exiting." << std::endl;
      exit(1);
    }
	 setOutputBehavior(key, FIRST);
  }
  inline void set(const std::string &key, const double &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE || typeOfKey == DOUBLE ||
        typeOfKey ==
            DOUBLESOLO) { // if key is unused or associates with correct type
      doubleData[key] = std::vector<double>({value});
      inUse[key] =
          DOUBLESOLO; // since this is set with SET, it is a single value
    } else {
      std::cout << "  ERROR :: a call to DataMap::set was called where the key was "
              "already in use with another type."
           << std::endl;
      std::cout << "  function was called with : key = \"" << key << "\" value = \""
           << value << "\" where value is double." << std::endl;
      std::cout << "  but ... key is already associated with type "
           << findKeyInData(key) << ". Exiting." << std::endl;
      exit(1);
    }
	 setOutputBehavior(key, FIRST);
  }
  inline void set(const std::string &key, const int &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE || typeOfKey == INT ||
        typeOfKey ==
            INTSOLO) { // if key is unused or associates with correct type
      intData[key] = std::vector<int>({value});
      inUse[key] = INTSOLO; // since this is set with SET, it is a single value
    } else {
      std::cout << "  ERROR :: a call to DataMap::set was called where the key was "
              "already in use with another type."
           << std::endl;
      std::cout << "  function was called with : key = \"" << key << "\" value = \""
           << value << "\" where value is int." << std::endl;
      std::cout << "  but ... key is already associated with type "
           << findKeyInData(key) << ". Exiting." << std::endl;
      exit(1);
    }
	 setOutputBehavior(key, FIRST);
  }
  inline void set(const std::string &key, const std::string &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE || typeOfKey == STRING ||
        typeOfKey ==
            STRINGSOLO) { // if key is unused or associates with correct type
      stringData[key] = std::vector<std::string>({value});
      inUse[key] = STRINGSOLO;
    } else {
      std::cout << "  ERROR :: a call to DataMap::set was called where the key was "
              "already in use with another type."
           << std::endl;
      std::cout << "  function was called with : key = \"" << key << "\" value = \""
           << value << "\" where value is string." << std::endl;
      std::cout << "  but ... key is already associated with type "
           << findKeyInData(key) << ". Exiting." << std::endl;
      exit(1);
    }
	 setOutputBehavior(key, FIRST);
  }

  // set functions (bool,double,int,string) that take a **vector** of value -
  // either make new map entry or replace existing
  // outputBehavior is set as though there was an append (i.e. list)
  inline void set(const std::string &key, const std::vector<bool> &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE || typeOfKey == BOOL ||
        typeOfKey ==
            BOOLSOLO) { // if key is unused or associates with correct type
      boolData[key] = value;
      inUse[key] = BOOL;
    } else {
      std::cout << "  ERROR :: a call to DataMap::set was called where the key was "
              "already in use with another type."
           << std::endl;
      std::cout << "  function was called with : key = \"" << key
           << "\" value is a vector of bool." << std::endl;
      std::cout << "  but ... key is already associated with type "
           << findKeyInData(key) << ". Exiting." << std::endl;
      exit(1);
    }
	setOutputBehavior(key, LIST | AVE);
  }
  inline void set(const std::string &key, const std::vector<double> &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE || typeOfKey == DOUBLE ||
        typeOfKey ==
            DOUBLESOLO) { // if key is unused or associates with correct type
      doubleData[key] = value;
      inUse[key] = DOUBLE;
    } else {
      std::cout << "  ERROR :: a call to DataMap::set was called where the key was "
              "already in use with another type."
           << std::endl;
      std::cout << "  function was called with : key = \"" << key
           << "\" value is a vector of double." << std::endl;
      std::cout << "  but ... key is already associated with type "
           << findKeyInData(key) << ". Exiting." << std::endl;
      exit(1);
    }
	setOutputBehavior(key, LIST | AVE);
  }
  inline void set(const std::string &key, const std::vector<int> &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE || typeOfKey == INT ||
        typeOfKey ==
            INTSOLO) { // if key is unused or associates with correct type
      intData[key] = value;
      inUse[key] = INT;
    } else {
      std::cout << "  ERROR :: a call to DataMap::set was called where the key was "
              "already in use with another type."
           << std::endl;
      std::cout << "  function was called with : key = \"" << key
           << "\" value is a vector of int." << std::endl;
      std::cout << "  but ... key is already associated with type "
           << findKeyInData(key) << ". Exiting." << std::endl;
      exit(1);
    }
	setOutputBehavior(key, LIST | AVE);
  }
  inline void set(const std::string &key, const std::vector<std::string> &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE || typeOfKey == STRING ||
        typeOfKey ==
            STRINGSOLO) { // if key is unused or associates with correct type
      stringData[key] = value;
      inUse[key] = STRING;
    } else {
      std::cout << "  ERROR :: a call to DataMap::set was called where the key was "
              "already in use with another type."
           << std::endl;
      std::cout << "  function was called with : key = \"" << key
           << "\" value is a vector of string." << std::endl;
      std::cout << "  but ... key is already associated with type "
           << findKeyInData(key) << ". Exiting." << std::endl;
      exit(1);
    }
	setOutputBehavior(key, LIST);
  }

  // append a value to the end of vector associated with key. If key is not
  // found, start a new vector for key
  inline void append(const std::string &key, const bool &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE) { // this key is not in data map, use Set.
      set(key, value);
      inUse[key] = BOOL; // set the in use to be a list rather then a solo
    } else if (typeOfKey == BOOL ||
               typeOfKey == BOOLSOLO) { // if this key is in data map as a
                                        // string, append to vector
      boolData[key].push_back(value);
      inUse[key] = BOOL;
    } else {
      std::cout << "  In DataMap::append :: attempt to append value \"" << value
           << "\" of type bool to \"" << key
           << "\" but this key is already associated with "
           << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << std::endl;
      exit(1);
    }
	setOutputBehavior(key, LIST | AVE);
  }
  inline void append(const std::string &key, const double &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE) { // this key is not in data map, use Set.
      set(key, value);
      inUse[key] = DOUBLE; // set the in use to be a list rather then a solo
    } else if (typeOfKey == DOUBLE ||
               typeOfKey == DOUBLESOLO) { // if this key is in data map as a
                                          // string, append to vector
      doubleData[key].push_back(value);
      inUse[key] = DOUBLE;
    } else {
      std::cout << "  In DataMap::append :: attempt to append value \"" << value
           << "\" of type double to \"" << key
           << "\" but this key is already associated with "
           << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << std::endl;
      exit(1);
    }
	setOutputBehavior(key, LIST | AVE);
  }
  inline void append(const std::string &key, const int &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE) { // this key is not in data map, use Set.
      set(key, value);
      inUse[key] = INT; // set the in use to be a list rather then a solo
    } else if (typeOfKey == INT ||
               typeOfKey == INTSOLO) { // if this key is in data map as a
                                       // string, append to vector
      intData[key].push_back(value);
      inUse[key] = INT; // set the in use to be a list rather then a solo
    } else {
      std::cout << "  In DataMap::append :: attempt to append value \"" << value
           << "\" of type int to \"" << key
           << "\" but this key is already associated with "
           << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << std::endl;
      exit(1);
    }
	setOutputBehavior(key, LIST | AVE);
  }
  inline void append(const std::string &key, const std::string &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE) { // this key is not in data map, use Set.
      set(key, value);
      inUse[key] = STRING; // set the in use to be a list rather then a solo
    } else if (typeOfKey == STRING ||
               typeOfKey == STRINGSOLO) { // if this key is in data map as a
                                          // string, append to vector
      stringData[key].push_back(value);
      inUse[key] = STRING; // set the in use to be a list rather then a solo
    } else {
      std::cout << "  In DataMap::append :: attempt to append value \"" << value
           << "\" of type string to \"" << key
           << "\" but this key is already associated with "
           << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << std::endl;
      exit(1);
    }
	setOutputBehavior(key, LIST);
  }

  // append a vector of values to the end of vector associated with key. If key
  // is not found, start a new vector for key
  inline void append(const std::string &key, const std::vector<bool> &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE) { // this key is not in data map, use Set.
      set(key, value);
    } else if (typeOfKey == BOOL ||
               typeOfKey == BOOLSOLO) { // if this key is in data map as a
                                        // string, append to vector
      boolData[key].insert(boolData[key].end(), value.begin(), value.end());
      inUse[key] = BOOL; // may have been solo - make sure it's list
    } else {
      std::cout << "  In DataMap::append :: attempt to append a vector of type bool "
              "to \""
           << key << "\" but this key is already associated with "
           << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << std::endl;
      exit(1);
    }
	setOutputBehavior(key, LIST | AVE);
  }
  inline void append(const std::string &key, const std::vector<double> &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE) { // this key is not in data map, use Set.
      set(key, value);
    } else if (typeOfKey == DOUBLE) { // if this key is in data map as a string,
                                      // append to vector
      doubleData[key].insert(doubleData[key].end(), value.begin(), value.end());
      inUse[key] = DOUBLE; // may have been solo - make sure it's list
    } else {
      std::cout << "  In DataMap::append :: attempt to append a vector of type "
              "double to \""
           << key << "\" but this key is already associated with "
           << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << std::endl;
      exit(1);
    }
	setOutputBehavior(key, LIST | AVE);
  }
  inline void append(const std::string &key, const std::vector<int> &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE) { // this key is not in data map, use Set.
      set(key, value);
    } else if (typeOfKey == INT) { // if this key is in data map as a string,
                                   // append to vector
      intData[key].insert(intData[key].end(), value.begin(), value.end());
      inUse[key] = INT; // may have been solo - make sure it's list
    } else {
      std::cout << "  In DataMap::append :: attempt to append a vector of type int "
              "to \""
           << key << "\" but this key is already associated with "
           << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << std::endl;
      exit(1);
    }
	setOutputBehavior(key, LIST | AVE);
  }
  inline void append(const std::string &key, const std::vector<std::string> &value) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE) { // this key is not in data map, use Set.
      set(key, value);
    } else if (typeOfKey == STRING) { // if this key is in data map as a string,
                                      // concat new string with existing value
		stringData[key] = { stringData[key][0] + value[0] };
      inUse[key] = STRING; // may have been solo - make sure it's list
    } else {
      std::cout << "  In DataMap::append :: attempt to append a vector of type "
              "string to \""
           << key << "\" but this key is already associated with "
           << lookupDataMapTypeName(typeOfKey) << ".\n  exiting." << std::endl;
      exit(1);
    }
	setOutputBehavior(key, LIST);
  }

  // merge contents of two data maps - if common keys are found behavior is determined by 'replace'
  // replace 0 = default behavior - if the same key exists in both maps, throw and error
  // replace 1 = keep current value - if the same key exists in both maps, keep the current value
  // replace 3 = keep the other value - if the same key exists in both maps, keep the other value
  // merge will attempt to merge outputBehavior
  inline void merge(DataMap otherDataMap, int replace = 0) {
	  for (std::string key : otherDataMap.getKeys()) {
		  dataMapType typeOfKey = findKeyInData(key);
		  dataMapType typeOfOtherKey = otherDataMap.findKeyInData(key);
		  if (replace == 0) { // no replacement allowed!
			  if (typeOfKey != NONE) { // make sure key is not in both data maps
				  std::cout << "  In DataMap::merge() - attempt to merge key: \"" << key
					  << "\" but key exists in both data maps and replace = 0!\n  Exiting." << std::endl;
			  }
		  }
		  // keep other either because:
		  //   rule is keep other (replace = 2)
		  //  or
		  //   rule is default (and test to make sure key is not in both passed) (replace 0)
		  //  or
		  //   rule is keep current, and this key is not already in this data map (replace = 1)
		  if (replace == 2 || replace == 0 || (replace == 1 && typeOfKey == NONE)) {
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
		  }
	  }
  }

  inline std::vector<bool> getBoolVector(
      const std::string &key) { // retrieve a double from a dataMap with "key"
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) {
      return boolData[key];
    } else {
      std::cout << "  in DataMap::getBoolVector :: attempt to use getBoolVector "
              "with key \""
           << key << "\" but this key is associated with type " << typeOfKey
           << "\n  exiting." << std::endl;
      std::cout << "  (if type is NONE, then the key was not found in dataMap)"
           << std::endl;
      exit(1);
    }
  }
  inline std::vector<double> getDoubleVector(
      const std::string &key) { // retrieve a double from a dataMap with "key"
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) {
      return doubleData[key];
    } else {
      std::cout << "  in DataMap::getDoubleVector :: attempt to use getDoubleVector "
              "with key \""
           << key << "\" but this key is associated with type " << typeOfKey
           << "\n  exiting." << std::endl;
      std::cout << "  (if type is NONE, then the key was not found in dataMap)"
           << std::endl;
      exit(1);
    }
  }
  inline std::vector<int> getIntVector(
      const std::string &key) { // retrieve a double from a dataMap with "key"
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == INT || typeOfKey == INTSOLO) {
      return intData[key];
    } else {
      std::cout << "  in DataMap::getIntVector :: attempt to use getIntVector with "
              "key \""
           << key << "\" but this key is associated with type " << typeOfKey
           << "\n  exiting." << std::endl;
      std::cout << "  (if type is NONE, then the key was not found in dataMap)"
           << std::endl;
      exit(1);
    }
  }
  inline std::vector<std::string> getStringVector(
      const std::string &key) { // retrieve a double from a dataMap with "key"
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == STRING || typeOfKey == STRINGSOLO) {
      return stringData[key];
    } else {
      std::cout << "  in DataMap::getStringVector :: attempt to use getStringVector "
              "with key \""
           << key << "\" but this key is associated with type " << typeOfKey
           << "\n  exiting." << std::endl;
      std::cout << "  (if type is NONE, then the key was not found in dataMap)"
           << std::endl;
      exit(1);
    }
  }

  inline std::string getStringOfVector(const std::string &key) { // retrieve a string from
                                                       // a dataMap with "key" -
                                                       // if not already string,
                                                       // will be converted
    std::string returnString = "";
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey == NONE) {
      std::cout << "  In DataMap::GetString() :: key \"" << key
           << "\" is not in data map!\n  exiting." << std::endl;
      exit(1);
    } else {
      if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) {
        for (auto e : boolData[key]) {
          returnString += to_string(e) + ",";
        }
      } else if (typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) {
        for (auto e : doubleData[key]) {
          returnString += std::to_string(e) + ",";
        }
      } else if (typeOfKey == INT || typeOfKey == INTSOLO) {
        for (auto e : intData[key]) {
          returnString += std::to_string(e) + ",";
        }
      } else if (typeOfKey == STRING || typeOfKey == STRINGSOLO) {
        for (auto e : stringData[key]) {
          returnString += e + ",";
        }
      }
    }
    if (returnString.size() > 2) { // if vector was not empty
      returnString.pop_back();     // remove trailing ","
    }
    returnString += "";
    return returnString;
  }

  // get ave of values in a vector - must be bool, double or, int
  inline double
  getAverage(std::string key) { // not ref, we may need to change to a "{LIST}" key
    dataMapType typeOfKey = findKeyInData(key);
    double returnValue = 0;
    if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) {
      for (auto e : boolData[key]) {
        returnValue += (double)e;
      }
      if (boolData[key].size() > 1) {
        returnValue /= boolData[key].size();
      } // else vector is  size 1, no div needed or vector is empty, returnValue
        // will be 0
    } else if (typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) {
      for (auto e : doubleData[key]) {
        returnValue += (double)e;
      }
      if (doubleData[key].size() > 1) {
        returnValue /= doubleData[key].size();
      } // else vector is  size 1, no div needed or vector is empty, returnValue
        // will be 0
    } else if (typeOfKey == INT || typeOfKey == INTSOLO) {
      for (auto e : intData[key]) {
        returnValue += (double)e;
      }
      if (intData[key].size() > 1) {
        returnValue /= intData[key].size();
      } // else vector is  size 1, no div needed or vector is empty, returnValue
        // will be 0
    } else if (typeOfKey == STRING || typeOfKey == STRINGSOLO) {
      std::cout << "  in DataMap::getAverage attempt to use with vector of type "
              "string associated key \""
           << key << "\".\n  Cannot average strings!\n  Exiting." << std::endl;
      exit(1);
    } else if (typeOfKey == NONE) {
      std::cout << "  in DataMap::getAverage attempt to get average from "
              "nonexistent key \""
           << key << "\".\n  Exiting." << std::endl;
      exit(1);
    }
    return returnValue;
  }

  inline double
  getVariance(std::string key) { // not ref, we may need to change to a "{LIST}" key
    dataMapType typeOfKey = findKeyInData(key);
    double averageValue(0);
    double varianceValue(0);
    if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) {
      for (auto e : boolData[key]) {
        averageValue += (double)e;
      }
      averageValue /= boolData[key].size();
      for (auto e : boolData[key]) {
        varianceValue +=
            ((double)e - averageValue) * ((double)e - averageValue);
      }
      if (boolData[key].size() > 0)
        varianceValue /= boolData[key].size() - 1;
      else
        varianceValue = 0;
    } else if (typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) {
      for (auto e : doubleData[key]) {
        averageValue += (double)e;
      }
      averageValue /= doubleData[key].size();
      for (auto e : doubleData[key]) {
        varianceValue +=
            ((double)e - averageValue) * ((double)e - averageValue);
      }
      if (doubleData[key].size() > 0)
        varianceValue /= doubleData[key].size() - 1;
      else
        varianceValue = 0;
    } else if (typeOfKey == INT || typeOfKey == INTSOLO) {
      for (auto e : intData[key]) {
        averageValue += (double)e;
      }
      averageValue /= intData[key].size();
      for (auto e : intData[key]) {
        varianceValue +=
            ((double)e - averageValue) * ((double)e - averageValue);
      }
      if (intData[key].size() > 0)
        varianceValue /= intData[key].size() - 1;
      else
        varianceValue = 0;
    } else if (typeOfKey == STRING || typeOfKey == STRINGSOLO) {
      std::cout << "  in DataMap::getVariance attempt to use with vector of type "
              "string associated key \""
           << key << "\".\n  Cannot average strings!\n  Exiting." << std::endl;
      exit(1);
    } else if (typeOfKey == NONE) {
      std::cout << "  in DataMap::getVariance attempt to get value from nonexistent "
              "key \""
           << key << "\".\n  Exiting." << std::endl;
      exit(1);
    }
    return varianceValue;
  }

  // get ave of values in a vector - must be bool, double or, int
  inline double
  getSum(std::string key) { // not ref, we may need to change to a "{LIST}" key
    dataMapType typeOfKey = findKeyInData(key);
    double returnValue = 0;
    if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) {
      for (auto e : boolData[key]) {
        returnValue += (double)e;
      }
      // if (boolData[key].size() > 1) {
      //	returnValue /= boolData[key].size();
      //} // else vector is  size 1, no div needed or vector is empty,
      //returnValue will be 0
    } else if (typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) {
      for (auto e : doubleData[key]) {
        returnValue += (double)e;
      }
      // if (doubleData[key].size() > 1) {
      //	returnValue /= doubleData[key].size();
      //} // else vector is  size 1, no div needed or vector is empty,
      //returnValue will be 0
    } else if (typeOfKey == INT || typeOfKey == INTSOLO) {
      for (auto e : intData[key]) {
        returnValue += (double)e;
      }
      // if (intData[key].size() > 1) {
      //	returnValue /= intData[key].size();
      //} // else vector is  size 1, no div needed or vector is empty,
      //returnValue will be 0
    } else if (typeOfKey == STRING || typeOfKey == STRINGSOLO) {
      std::cout << "  in DataMap::getAverage attempt to use with vector of type "
              "string associated key \""
           << key << "\".\n  Cannot average strings!\n  Exiting." << std::endl;
      exit(1);
    } else if (typeOfKey == NONE) {
      std::cout << "  in DataMap::getAverage attempt to get average from "
              "nonexistent key \""
           << key << "\".\n  Exiting." << std::endl;
      exit(1);
    }
    return returnValue;
  }

  // Clear a field in a DataMap
  inline void clear(const std::string &key) {
    dataMapType typeOfKey = findKeyInData(key);
    if (typeOfKey != NONE) {
      if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) { // data is bool
        boolData.erase(key);
      } else if (typeOfKey == DOUBLE ||
                 typeOfKey == DOUBLESOLO) { // data is double
        doubleData.erase(key);
      } else if (typeOfKey == INT || typeOfKey == INTSOLO) { // data is int
        intData.erase(key);
      } else if (typeOfKey == STRING ||
                 typeOfKey == STRINGSOLO) { // data is string
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

  inline bool
  fieldExists(const std::string &key) { // return true if a data map contains "key"
    return (findKeyInData(key) > 0);
  }

  // take two strings (header and data), and a list of keys, and whether or not
  // to save "{LIST}"s. convert data from data map to header and data strings
  void constructHeaderAndDataStrings(std::string &headerStr, std::string &dataStr,
                                     const std::vector<std::string> &keys,
                                     bool aveOnly = false);

  [[deprecated("Use openAndWriteToFile() instead.")]]
  inline void writeToFile(const std::string &fileName,
                          const std::vector<std::string> &keys = {},
                          bool aveOnly = false) {
    // Set("score{LIST}",10.0);

    if (FileManager::files.find(fileName) ==
        FileManager::files
            .end()) { // first make sure that the dataFile has been set up.
      if (keys.size() == 0) { // if no keys are given
        FileManager::fileColumns[fileName] = getKeys();
      } else {
        FileManager::fileColumns[fileName] = keys;
      }
    }
    std::string headerStr = "";
    std::string dataStr = "";

    constructHeaderAndDataStrings(headerStr, dataStr,
                                  FileManager::fileColumns[fileName],
                                  aveOnly); // if a list is given, use that.

    FileManager::openAndWriteToFile(fileName, dataStr, headerStr); // write the data to file!
  }
  inline void openAndWriteToFile(const std::string &fileName,
                          const std::vector<std::string> &keys = {},
                          bool aveOnly = false) {
    // Set("score{LIST}",10.0);

    if (FileManager::files.find(fileName) ==
        FileManager::files
            .end()) { // first make sure that the dataFile has been set up.
      if (keys.size() == 0) { // if no keys are given
        FileManager::fileColumns[fileName] = getKeys();
      } else {
        FileManager::fileColumns[fileName] = keys;
      }
    }
    std::string headerStr = "";
    std::string dataStr = "";

    constructHeaderAndDataStrings(headerStr, dataStr,
                                  FileManager::fileColumns[fileName],
                                  aveOnly); // if a list is given, use that.

    FileManager::openAndWriteToFile(fileName, dataStr,
                             headerStr); // write the data to file!
  }

  inline std::vector<std::string> getColumnNames() {
    std::vector<std::string> columnNames;

    for (auto element : inUse) {
      //if (outputBehavior.find(element.first) == outputBehavior.end()) {
      //  // this element has no defined output behavior, so it will be LIST
      //  // (default) or FIRST (if it's a solo value)
      //  if (element.second == BOOLSOLO || element.second == DOUBLESOLO ||
      //      element.second == INTSOLO || element.second == STRINGSOLO) {
      //    columnNames.push_back(element.first);
      //  } else {
      //    columnNames.push_back(element.first + "_LIST");
      //  }
      //} else { // there is an output behavior defined
		 {
        auto OB = outputBehavior[element.first];
        if (OB & AVE) {
          columnNames.push_back(element.first + "_AVE");
        }
        if (OB & FIRST) {
          columnNames.push_back(element.first);
        }
        if (OB & SUM) {
          std::cout << "  WARNING OUTPUT METHOD SUM IS HAS YET TO BE WRITTEN!"
               << std::endl;
        }
        if (OB & PROD) {
          std::cout << "  WARNING OUTPUT METHOD PROD IS HAS YET TO BE WRITTEN!"
               << std::endl;
        }
        if (OB & STDERR) {
          std::cout << "  WARNING OUTPUT METHOD STDERR IS HAS YET TO BE WRITTEN!"
               << std::endl;
        }
        if (OB & LIST) {
          columnNames.push_back(element.first + "_LIST");
        }
		  // if (OB & NO_OUTPUT) do nothing...
      }
    }
    return columnNames;
  }
  //	/*
  //	 * takes a vector of string with key value pairs. Calls set for each
  //pair.
  //	 */
  //	void SetMany(vector<string> dataPairs);

  inline DataMap remakeDataMapWithPrefix(std::string prefix, bool stringify = 0) {
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
    } else {
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
