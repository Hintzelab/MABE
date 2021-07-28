#include "entropy.h"

double ENT::Entropy(const TS::intTimeSeries& X) {
	std::set<std::vector<int>> symbols{ X.begin(),X.end() }; // used to get unique symbols
	std::vector<std::vector<int>> symbolIndices(symbols.begin(), symbols.end()); // "map" (index in vector is key) to keep track of which symbol is which
	std::vector<int> frequencyTable(symbols.size(), 0); // count of how many times each symbol shows up

	for (auto symbol : X) {
		frequencyTable[std::find(symbolIndices.begin(), symbolIndices.end(), symbol) - symbolIndices.begin()]++;
	}

	double ent = 0;
	double temp;
	for (int index = 0; index < symbols.size(); index++) {
		temp = (1.0 / X.size()) * frequencyTable[index];
		ent += (temp * std::log2(temp)); // p log(p)
	}
	return std::abs(ent);
}

double ENT::MutualEntropy(const TS::intTimeSeries& X, const TS::intTimeSeries& Y) {
	return (Entropy(X) + Entropy(Y)) - Entropy(TS::Join(X, Y));
}

double ENT::ConditionalEntropy(const TS::intTimeSeries& X, const TS::intTimeSeries& Y) {
	return Entropy(X) - MutualEntropy(X, Y);
}

double ENT::ConditionalMutualEntropy(const TS::intTimeSeries& X, const TS::intTimeSeries& Y, const TS::intTimeSeries& Z) {
	return Entropy(TS::Join(X, Z)) + Entropy(TS::Join(Y, Z)) - (Entropy(Z) + Entropy(TS::Join({ X, Y, Z })));
}

