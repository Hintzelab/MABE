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

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <unordered_set>
#include <vector>

#include "../../Genome/AbstractGenome.h"

#include "../../Utilities/Random.h"

#include "../AbstractBrain.h"

// define the wire states
using namespace std;

class WireBrain: public AbstractBrain {

	static shared_ptr<ParameterLink<int>> bitsPerCodonPL;

	static shared_ptr<ParameterLink<bool>> allowNegativeChargePL;
	static shared_ptr<ParameterLink<int>> defaultWidthPL;
	static shared_ptr<ParameterLink<int>> defaultHeightPL;
	static shared_ptr<ParameterLink<int>> defaultDepthPL;
	static shared_ptr<ParameterLink<int>> worldConnectionsSeparationPL;
	static shared_ptr<ParameterLink<int>> overchargeThresholdPL;
	static shared_ptr<ParameterLink<int>> decayDurationPL;
	static shared_ptr<ParameterLink<int>> chargeUpdatesPerUpdatePL;
	static shared_ptr<ParameterLink<bool>> constantInputsPL;
	static shared_ptr<ParameterLink<bool>> cacheResultsPL;
	static shared_ptr<ParameterLink<int>> cacheResultsCountPL;

	static shared_ptr<ParameterLink<string>> genomeDecodingMethodPL;  // "bitmap" = convert genome directly, "wiregenes" = genes defined by start codeons, location, direction and location
	static shared_ptr<ParameterLink<int>> wiregenesInitialGeneCountPL;
	static shared_ptr<ParameterLink<double>> bitmapInitialFillRatioPL;

	static shared_ptr<ParameterLink<bool>> wiregenesAllowSimpleWiresPL;
	static shared_ptr<ParameterLink<int>> wiregenesSimpleWireMaxLengthPL;
	static shared_ptr<ParameterLink<string>> wiregenesSimpleWireDirectionsPL;

	static shared_ptr<ParameterLink<bool>> wiregenesAllowWormholesPL;
	static shared_ptr<ParameterLink<int>> wiregenesWormholesBidirectionalPL;

	static shared_ptr<ParameterLink<bool>> wiregenesAllowSquiggleWiresPL;
	static shared_ptr<ParameterLink<int>> wiregenesSquiggleWireMinLengthPL;
	static shared_ptr<ParameterLink<int>> wiregenesSquiggleWireMaxLengthPL;
	static shared_ptr<ParameterLink<string>> wiregenesSquiggleWireDirectionsPL;

	static shared_ptr<ParameterLink<int>> hiddenValuesPL;

	bool allowNegativeCharge;
	int defaultWidth;
	int defaultHeight;
	int defaultDepth;
	int worldConnectionsSeparation;
	int overchargeThreshold;
	int decayDuration;
	int chargeUpdatesPerUpdate;
	bool constantInputs;
	bool cacheResults;
	int cacheResultsCount;

	string genomeDecodingMethod;  // "bitmap" = convert genome directly, "wiregenes" = genes defined by start codeons, location, direction and location
	int wiregenesInitialGeneCount;
	double bitmapInitialFillRatio;

	bool wiregenesAllowSimpleWires;
	int wiregenesSimpleWireMaxLength;
	string wiregenesSimpleWireDirections;

	bool wiregenesAllowWormholes;
	int wiregenesWormholesBidirectional;

	bool wiregenesAllowSquiggleWires;
	int wiregenesSquiggleWireMinLength;
	int wiregenesSquiggleWireMaxLength;
	string wiregenesSquiggleWireDirections;

	int nrHiddenValues;

	vector<double> nodes;
	vector<double> nextNodes;

	int CHARGE;  // = 2 + *decayDuration;
	int NEGCHARGE;  // = CHARGE * -1;

	const int WIRE = 1;
	const int HOLLOW = 0;

	const int START_CODE = 0;
	const int LOCATION_CODE = 1;
	const int DIRECTION_CODE = 2;
	const int LENGTH_CODE = 3;
	const int DESTINATION_CODE = 4;

public:

	int width, depth, height;

	vector<int> nodesAddresses, nodesNextAddresses;  // where the nodes connect to the brain

	vector<int> allCells, nextAllCells;  // list of all cells in this brain
	vector<vector<int>> neighbors;  // for every cell list of wired neighbors (most will be empty)
	vector<int> wireAddresses;  // list of addresses for all cells which are wireAddresses (uncharged, charged and decay)

	vector<vector<long>> inputLookUpTable;  // table that contains output for a given input
	vector<int> inputCount;  // table that contains a count of the number of times we have seen a given input

	static shared_ptr<ParameterLink<string>> genomeNamePL;
	string genomeName;

	int connectionsCount;

	int nrValues;

	WireBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = nullptr);
	WireBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes, int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = nullptr);
	WireBrain(const vector<bool> &genome, int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~WireBrain() = default;

	virtual void initialize();
	virtual void connectPruneAndSetPopColumns(vector<pair<int, int>> wormholeList);
	virtual shared_ptr<AbstractBrain> makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) override;
	virtual shared_ptr<AbstractBrain> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;

	virtual void chargeUpdate();
	virtual void chargeUpdateTrit();
	virtual void update() override;
	virtual void SaveBrainState(string fileName);
	virtual void displayBrainState();
	virtual string description() override;
	virtual DataMap getStats(string& prefix) override;
	virtual string getType() override {
		return "Wire";
	}

	virtual void initializeGenomes(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) override;
	virtual unordered_set<string> requiredGenomes() override {
		return { genomeName };
	}

};

inline shared_ptr<AbstractBrain> WireBrain_brainFactory(int ins, int outs, shared_ptr<ParametersTable> PT) {
	return make_shared<WireBrain>(ins, outs, PT);
}

