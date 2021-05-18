//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once					// directive to insure that this .h file is only included one time

#include <World/AbstractWorld.h> // AbstractWorld defines all the basic function templates for worlds
#include <string>
#include <memory> // shared_ptr
#include <map>
#include <Analyze/timeSeries.h>

#include <Global.h>
#include <module_factories.h>

using std::shared_ptr;
using std::string;
using std::map;
using std::unordered_map;
using std::unordered_set;
using std::to_string;

class TPM_GENERATORWorld : public AbstractWorld {

public:
	// parameters for group and brain namespaces
	static shared_ptr<ParameterLink<int>> numberOfHiddenPL;
	static shared_ptr<ParameterLink<int>> numberOfSamplesPL;
	static shared_ptr<ParameterLink<bool>> outputJoinedStatesPL;
	static shared_ptr<ParameterLink<std::string>> outputModePL;
	static shared_ptr<ParameterLink<std::string>> worldNamePL;
	static shared_ptr<ParameterLink<int>> missingSymbolPL;
	static shared_ptr<ParameterLink<std::string>> beforeStatesEndianPL;

	static shared_ptr<ParameterLink<std::string>> discretizeRuleInputPL;
	static shared_ptr<ParameterLink<std::string>> discretizeRuleOutputPL;
	static shared_ptr<ParameterLink<std::string>> discretizeRuleHiddenPL;

	static shared_ptr<ParameterLink<std::string>> symbolsListInputPL;
	static shared_ptr<ParameterLink<std::string>> symbolsListOutputPL;
	static shared_ptr<ParameterLink<std::string>> symbolsListHiddenPL;

	static shared_ptr<ParameterLink<bool>> saveS2SPL;
	static shared_ptr<ParameterLink<bool>> saveBrainStructurePL;

	int nrIn = -1;
	int nrOut = -1;
	int nrHid = -1;
	int missingSymbol = -1;
	bool beforeStatesLittleEndian = true;
	std::string outputMode = "raw";

	bool saveS2S = false;
	bool saveBrainStructure = false;

	std::string discretizeRuleInputName = "BIT";
	std::string discretizeRuleOutputName = "BIT";
	std::string discretizeRuleHiddenName = "BIT";

	TS::RemapRules discretizeRuleInput = TS::RemapRules::BIT;
	TS::RemapRules discretizeRuleOutput = TS::RemapRules::BIT;
	TS::RemapRules discretizeRuleHidden = TS::RemapRules::BIT;

	std::vector<double> symbolsListInput = { 0.0,1.0 };
	std::vector<double> symbolsListOutput = { 0.0,1.0 };
	std::vector<double> symbolsListHidden = { 0.0,1.0 };

    TPM_GENERATORWorld(shared_ptr<ParametersTable> PT_);
	virtual ~TPM_GENERATORWorld() = default;

	virtual auto evaluate(map<string, shared_ptr<Group>>& /*groups*/, int /*analyze*/, int /*visualize*/, int /*debug*/) -> void override;

	virtual auto requiredGroups() -> unordered_map<string,unordered_set<string>> override;
};

