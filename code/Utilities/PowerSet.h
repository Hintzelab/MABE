#pragma once
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cmath>
#include <unordered_set>
#include "Utilities.h"

class PowerSet {
    // a static container that holds all the power sets we have made so far
    // map < size
    //   map < maxSetSize
    //     powerSet ( vector<vector<int>> ) >>
    
    static std::unordered_set<std::string> knownPowerSets; // [size,maxSetSize,Reflected]

    static std::unordered_map<int, std::unordered_map<int, std::vector<std::vector<int>>>> PowerSetCollection;
    static std::unordered_map<int, std::unordered_map<int, std::vector<std::vector<int>>>> ReflectedPowerSetCollection;



    // create a power set from a vector of elements
    void createPowerSet(std::vector<std::vector<int>>& powerSet, std::vector<int> start, std::vector<int> current = std::vector<int>(), int index = -1);
public:

    // given size, return the power set for the vector [0,size-1]
    // this function uses PowerSetCollection so it will not recrate power sets, but rather, just return the prior result
    const std::vector<std::vector<int>>& getPowerSet(int size, int maxSetSize = -1, bool reflected = false);

    // given an incompete powerset, for every set, add the set with all elements except these.
    // also, add the set with all elements.
    const std::vector<std::vector<int>>& reflectPowerSet(std::vector<std::vector<int>> ps, int size);
};
