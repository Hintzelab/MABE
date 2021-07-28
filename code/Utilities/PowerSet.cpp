#include "PowerSet.h"

// instantiate the static power set collection
std::unordered_set<std::string> PowerSet::knownPowerSets; // [size,maxSetSize,Reflected]
std::unordered_map<int, std::unordered_map<int, std::vector<std::vector<int>>>> PowerSet::PowerSetCollection;
std::unordered_map<int, std::unordered_map<int, std::vector<std::vector<int>>>> PowerSet::ReflectedPowerSetCollection;

// create a power set recursively from elements in start
void PowerSet::createPowerSet(std::vector<std::vector<int>>& powerSet, std::vector<int> start, std::vector<int> current, int index) {
    if (index == start.size()) { // termination case... do nothing now
        return;
    }

    if (current.size()) { // if current is not empty
        powerSet.push_back(std::vector<int>(current));
    }

    for (int i = index + 1; i < start.size(); i++) { // recursion magic...
        current.push_back(start[i]);
        createPowerSet(powerSet, start, current, i);

        current.pop_back();
    }

    return;
}

// given size, return the power set for the vector [0,size-1]
// this function uses PowerSetCollection so it will not recrate power sets, but rather, just return the prior result
//
// check to see if powerset has been created, if not, create it, sort it and add to PowerSetCollection
// then, return powerset
// note: the empty set is not included in the result
const std::vector<std::vector<int>>& PowerSet::getPowerSet(int size, int maxSetSize, bool reflected) {

    //std::cout << "getPowerSet::  size: " << size << "  maxSetSize: " << maxSetSize << "  reflected: " << reflected << std::endl;

    if (maxSetSize == -1 || maxSetSize > size) {
        maxSetSize = size;
    }

    std::string psKey = std::to_string(size) + "," + std::to_string(maxSetSize);
    if (knownPowerSets.find(psKey + "," + std::to_string(int(reflected))) != knownPowerSets.end()) { // we already know this one
        if (!reflected) {
            //std::cout << " I know this PS..." << std::endl;
            return PowerSetCollection[size][maxSetSize];
        }
        else {
            //std::cout << " I know this reflected PS..." << std::endl;
            return ReflectedPowerSetCollection[size][maxSetSize];
        }
    }
    else{ // we need to make the powerset

        //std::cout << "  need to make a powerset" << std::endl;

        int psElements = size;
        int psMaxSetsize = maxSetSize;

        std::vector<std::vector<int>> ps;

        for (int setSize = 1; setSize <= psMaxSetsize; setSize++) {
            
            //std::cout << setSize << "  ";

            std::vector<int> thisSet(setSize);
            std::iota(thisSet.begin(), thisSet.end(), 0); // make a vector with elements [0,1,...,size-2,size-1]
            bool done = false;
            while (!done) {
                ps.push_back(thisSet);
                int b = setSize - 1;
                while ((b >= 0) && (thisSet[b] == psElements - (setSize - b))) {
                    b--;
                }
                if (b == -1) {
                    done = true;
                }
                else {
                    thisSet[b]++;
                    for (int i = b + 1; i < setSize; i++) {
                        thisSet[i] = thisSet[i - 1] + 1;
                    }
                }
            }
        }

        //std::cout << "\n  done" << std::endl;

        PowerSetCollection[size][maxSetSize] = ps;
        knownPowerSets.insert(psKey+",0"); // now we know this key

        if (reflected) {
            //std::cout << "    reflecting" << std::endl;

            ps = reflectPowerSet(ps, size);
            ReflectedPowerSetCollection[size][maxSetSize] = ps;
            knownPowerSets.insert(psKey+",1"); // now we know this key
            return ReflectedPowerSetCollection[size][maxSetSize];
        }


        //std::cout << PowerSetCollection[size][maxSetSize].size() << std::endl;

        return PowerSetCollection[size][maxSetSize];
    }
}

const std::vector<std::vector<int>>& PowerSet::reflectPowerSet(std::vector<std::vector<int>> ps, int size) {
    if (ps.size() < size) {
        std::cout << "  PowerSet::reflectPowerSet :: attempt was made to reflect a power set with size < number of elements. I don't know what to do.\n  exiting!" << std::endl;
        exit(1);
    }
    if (ps.size() >= std::pow(2, size) - 2) {
        std::cout << "  PowerSet::reflectPowerSet :: attempt was made to reflect a more then 1/2 full power set.\n reflectPowerSet will simply return a full power set (excluding {})" << std::endl;
        return(getPowerSet(size));
    }
    else {
        int psSize = ps.size();
        for (int i = psSize - 1; i >= 0; i--) {
            ps.push_back({});
            for (int elem = 0; elem < size; elem++) {
                bool foundElem = false;
                for (int j = 0; j < ps[i].size(); j++) {
                    if (ps[i][j] == elem) {
                        foundElem = true;
                    }
                }
                if (!foundElem) {
                    ps.back().push_back(elem);
                }
            }
        }
        // add the full set
        ps.push_back({});
        for (int elem = 0; elem < size; elem++) {
            ps.back().push_back(elem);
        }
        const std::vector<std::vector<int>>& newPowerSet(ps);
        return newPowerSet;
    }
}
