//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "Data.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>


// global variables that should be accessible to all
// set<string> FileManager::dataFilesCreated;

std::string FileManager::outputPrefix;
std::map<std::string, std::vector<std::string>> FileManager::fileColumns;
std::map<std::string, std::ofstream>
    FileManager::files; // list of files (NAME,ofstream)
std::map<std::string, bool>
    FileManager::fileStates; // list of files states (NAME,open?)
std::map<std::string, int> DataMap::knownOutputBehaviors = {
    {"LIST", LIST},     {"AVE", AVE},     {"SUM", SUM}, {"PROD", PROD},
    {"STDERR", STDERR}, {"FIRST", FIRST}, {"VAR", VAR}};

void FileManager::openAndWriteToFile(const std::string &fileName, const std::string &data, const std::string &header) {
  openFile( fileName, header); // make sure that the file is open and ready to be written to
  if (!data.empty()) {files[fileName] << data << "\n" << std::flush;}
}

[[deprecated("Use openAndWriteToFile() instead.")]]
void FileManager::writeToFile(const std::string &fileName, const std::string &data, const std::string &header) {
  openFile( fileName, header); // make sure that the file is open and ready to be written to
  if (!data.empty()) {files[fileName] << data << "\n" << std::flush;}
}

void FileManager::openFile(const std::string &fileName, const std::string &header) {
  if (files.find(fileName) == files.end()) { // if file has not be initialized yet
    files.emplace(make_pair(fileName, std::ofstream())); // make an ofstream for the new file and place in FileManager::files
    files[fileName].open(std::string(outputPrefix) + fileName); // clear file contents and open in write mode
    fileStates[fileName] = true; // this file is now open
    if (!header.empty()) { // if there is a header string, write this to the new file
      files[fileName] << header << "\n";
    }
  }
  if (fileStates[fileName] == false) { // if file is closed ...
    files[fileName].open(std::string(outputPrefix) + fileName, std::ios::out | std::ios::app); // open file in append mode
  }
}

void FileManager::closeFile(const std::string &fileName) {
  if (files.find(fileName) == files.end()) {
    std::cout << "  In FileManager::closeFile :: ERROR, attempt to close file '" << fileName << "' but this file has not been opened or created! Exiting." << std::endl;
    exit(1);
  }
  files[fileName].close();
  fileStates[fileName] = false; // make a note that this file is closed
}

// copy constructor
DataMap::DataMap(std::shared_ptr<DataMap> source) {
  boolData = source->boolData;
  doubleData = source->doubleData;
  intData = source->intData;
  stringData = source->stringData;
  for (auto entry : source->inUse) {
    inUse[entry.first] = entry.second;
  }
  // inUse = source->inUse; // replaced with for loop.
  outputBehavior = source->outputBehavior;
}


// take two strings (header and data), and a list of keys, and whether or not to
// save "{LIST}"s. convert data from data map to header and data strings
void DataMap::constructHeaderAndDataStrings(std::string &headerStr, std::string &dataStr,
                                            const std::vector<std::string> &keys,
                                            bool aveOnly) {
  headerStr = ""; // make sure the strings are clean
  dataStr = "";
  dataMapType typeOfKey;
  unsigned int OB; // holds output behavior so it can be over ridden for ave file output!
  if (!keys.empty()) { // if keys is not empty
    for (auto const &i : keys) {
      typeOfKey = findKeyInData(i);
      if (typeOfKey == NONE) {
        std::cout << "  in DataMap::writeToFile() - key \"" << i
             << "\" can not be found in data map!\n  exiting." << std::endl;
        exit(1);
      }

      // the following code makes use of bit masks! in short, AVE,SUM,LIST,etc
      // each use only one bit of an int.
      // therefore if we apply that mask the the outputBehavior, we can see if
      // that type of output is needed.

      OB = outputBehavior[i];

      if (typeOfKey == STRING || typeOfKey == STRINGSOLO) {
        if (!(OB == LIST || OB == FIRST || OB == NO_OUTPUT)) {
			std::cout << std::endl << OB << std::endl;
          std::cout << "  in constructHeaderAndDataStrings :: attempt to write "
                  "string not in either LIST or FIRST formatte. This is not "
                  "allowed! Key was '" << i << "'. Exiting..."
               << std::endl;
          exit(1);
        }
      } 

      if (aveOnly) {
		  if (typeOfKey == STRING || typeOfKey == STRINGSOLO) {
			  OB = NO_OUTPUT;
		  }
		  else {
			  OB &= (AVE | FIRST); // if aveOnly, only output AVE on the entries
								 // that have been set for AVE
		  }
      }

      if (OB & FIRST) { // save first (only?) element in vector with key as
                        // column name
        headerStr += FileManager::separator + i;
        if (typeOfKey == BOOL || typeOfKey == BOOLSOLO) {
          if (!getBoolVector(i).empty()) {
            dataStr += FileManager::separator + to_string(getBoolVector(i)[0]);
          } else {
            dataStr += '0';
            std::cout << "  WARNING!! In DataMap::constructHeaderAndDataStrings :: "
                    "while getting value for FIRST with key \""
                 << i << "\" vector is empty!" << std::endl;
          }
        }
        if (typeOfKey == DOUBLE || typeOfKey == DOUBLESOLO) {
          if (!getDoubleVector(i).empty()) {
            dataStr +=
                FileManager::separator + std::to_string(getDoubleVector(i)[0]);
          } else {
            dataStr += '0';
            std::cout << "  WARNING!! In DataMap::constructHeaderAndDataStrings :: "
                    "while getting value for FIRST with key \""
                 << i << "\" vector is empty!" << std::endl;
          }
        }
        if (typeOfKey == INT || typeOfKey == INTSOLO) {
          if (!getIntVector(i).empty()) {
            dataStr +=
                FileManager::separator + std::to_string(getIntVector(i)[0]);
          } else {
            dataStr += '0';
            std::cout << "  WARNING!! In DataMap::constructHeaderAndDataStrings :: "
                    "while getting value for FIRST with key \""
                 << i << "\" vector is empty!" << std::endl;
          }
        }
        if (typeOfKey == STRING || typeOfKey == STRINGSOLO) {
          if (!getStringVector(i).empty()) {
            dataStr += FileManager::separator + (std::string)"\"" + getStringVector(i)[0] + (std::string)"\"";
          } else {
            dataStr += (std::string)"\"0\"";
            std::cout << "  WARNING!! In DataMap::constructHeaderAndDataStrings :: "
                    "while getting value for FIRST with key \""
                 << i << "\" vector is empty!" << std::endl;
          }
        }
      }
      if (OB & AVE) { // key_AVE = ave of vector (will error if of type string!)
        headerStr += FileManager::separator + i + "_AVE";
        dataStr += FileManager::separator + std::to_string(getAverage(i));
      }
      if (OB & VAR) { // key_VAR = variance of vector (will error if of type string!)
        headerStr += FileManager::separator + i + "_VAR";
        dataStr += FileManager::separator + std::to_string(getVariance(i));
      }
      if (OB & SUM) { // key_SUM = sum of vector
        headerStr += FileManager::separator + i + "_SUM";
        dataStr += FileManager::separator + std::to_string(getSum(i));
      }
      if (OB & PROD) { // key_PROD = product of vector
        std::cout << "  WARNING OUTPUT METHOD PROD IS HAS YET TO BE WRITTEN!"
             << std::endl;
      }
      if (OB & STDERR) { // key_STDERR = standard error of vector
        std::cout << "  WARNING OUTPUT METHOD STDERR IS HAS YET TO BE WRITTEN!"
             << std::endl;
      }
      if (OB & LIST) { // key_LIST = save all elements in vector in csv list format
        headerStr += FileManager::separator + i + "_LIST";
        dataStr += FileManager::separator + (std::string)"\"" + getStringOfVector(i) + (std::string)"\"";
      }
    }
    headerStr.erase(headerStr.begin()); // clip off the leading separator
    dataStr.erase(dataStr.begin());     // clip off the leading separator
  }
}
///////////////////////////////////////
// need to add support for output prefix directory
// need to add support for population file name prefixes
///////////////////////////////////////
