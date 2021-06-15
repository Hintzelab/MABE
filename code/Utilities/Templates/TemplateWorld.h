//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once    // directive to insure that this .h file is only included one time

#include <World/AbstractWorld.h> // AbstractWorld defines all the basic function templates for worlds
#include <string>
#include <memory> // shared_ptr
#include <map>

using std::shared_ptr;
using std::string;
using std::map;
using std::unordered_map;
using std::unordered_set;
using std::to_string;

class {{MODULE_NAME}}World : public AbstractWorld {

public:
    // parameters for group and brain namespaces
    static shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;
    
    // a local variable used for faster access to the ParameterLink value
    int evaluationsPerGeneration;
    
    std::string groupName = "root::";
    std::string brainName = "root::";
    
    {{MODULE_NAME}}World(shared_ptr<ParametersTable> PT);
    virtual ~{{MODULE_NAME}}World() = default;

    virtual auto evaluate(map<string, shared_ptr<Group>>& /*groups*/, int /*analyze*/, int /*visualize*/, int /*debug*/) -> void override;

    virtual auto requiredGroups() -> unordered_map<string,unordered_set<string>> override;
};

