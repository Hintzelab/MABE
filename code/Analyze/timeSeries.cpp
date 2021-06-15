#include "timeSeries.h"

std::string TS::TimeSeriesSampleToString(const std::vector<int>& sample, const std::string& sep) {
	std::string returnStr = "";
	for (int i = 0; i < sample.size(); i++) {
		returnStr += std::to_string(sample[i]);
		if (i < sample.size() - 1) {
			returnStr += sep;
		}
	}
	return returnStr;
}

std::string TS::TimeSeriesSampleToString(const std::vector<double>& sample, const std::string& sep) {
	std::string returnStr = "";
	for (int i = 0; i < sample.size(); i++) {
		returnStr += std::to_string(sample[i]);
		if (i < sample.size() - 1) {
			returnStr += sep;
		}
	}
	return returnStr;
}

std::vector<std::string> TS::TimeSeriesToVectString(const intTimeSeries& X, const std::string& sep) {
	std::vector<std::string> returnVect;
	for (auto s : X) {
		returnVect.push_back(TimeSeriesSampleToString(s, sep));
	}
	return returnVect;
}

std::vector<std::string> TS::TimeSeriesToVectString(const TimeSeries& X, const std::string& sep) {
	std::vector<std::string> returnVect;
	for (auto s : X) {
		returnVect.push_back(TimeSeriesSampleToString(s, sep));
	}
	return returnVect;
}

std::string TS::TimeSeriesToString(const intTimeSeries & X, const std::string& sepElement, const std::string& sepLine) {
	std::string returnStr = "";
	for (int i = 0; i < X.size(); i++) {
		returnStr += "[" + TimeSeriesSampleToString(X[i], sepElement) + "]";
		if (i < X.size() - 1) {
			returnStr += sepLine;
		}
	}
	return returnStr;
}

std::string TS::TimeSeriesToString(const TimeSeries& X, const std::string& sepElement, const std::string& sepLine) {
	std::string returnStr = "";
	for (int i = 0; i < X.size(); i++) {
		returnStr += "[" + TimeSeriesSampleToString(X[i], sepElement) + "]";
		if (i < X.size() - 1) {
			returnStr += sepLine;
		}
	}
	return returnStr;
}

TS::intTimeSeries TS::subSetTimeSeries(const intTimeSeries& X, const std::vector<int>& indices) {
	intTimeSeries returnTimeSeries(X.size(),std::vector<int>(indices.size()));
	for (size_t s = 0; s < X.size(); s++) {
		for (size_t i = 0; i < indices.size(); i++) {
			returnTimeSeries[s][i] = X[s][indices[i]];
		}
	}
	return returnTimeSeries;
}

std::vector<TS::intTimeSeries> TS::deconstructTimeSeries(const intTimeSeries& X) {
	std::vector<intTimeSeries> returnTS(X[0].size());
	for (int i = 0; i < X[0].size(); i++) {
		for (int j = 0; j < X.size(); j++) {
			returnTS[i].push_back({ X[j][i] });
		}
	}
	return returnTS;
}

TS::intTimeSeries TS::Join(const intTimeSeries& X, const intTimeSeries& Y){
	if (X.size() != Y.size()) {
		std::cout << "in Join(X,Y) :: X and Y are not of the same size. exiting...";
		exit(1);
	}
	auto returnTS = X;
	for (int i = 0; i < X.size(); i++) {
		returnTS[i].insert(returnTS[i].end(), Y[i].begin(), Y[i].end());
	}
	return returnTS;
}

TS::intTimeSeries TS::Join(const std::vector<intTimeSeries>& data) {
	for (int d = 1; d < data.size(); d++) {
		if (data[d].size() != data[0].size()) {
			std::cout << "in Join({X,Y,...}) :: the data sets are not of the same size. exiting...";
			exit(1);
		}
	}
	auto returnTS = data[0];
	for (int i = 0; i < data[0].size(); i++) {
		returnTS[i].reserve(data.size() * data[0].size());
		for (int d = 1; d < data.size(); d++) {
			returnTS[i].insert(returnTS[i].end(), data[d][i].begin(), data[d][i].end());
		}
	}
	return returnTS;
}

TS::intTimeSeries TS::trimTimeSeries(const intTimeSeries& experience, const std::pair<double, double>& range, const std::vector<int>& lifeTimes) {
	intTimeSeries returnTS;
	int birthTime = 0; // updated to the birth time of each life time
	for (auto lifeTime : lifeTimes) { // itterate over all life times
		size_t ignoreBefore = ceil((lifeTime-1.0) * range.first);
		size_t ignoreAfter = (lifeTime-1) * range.second;
		returnTS.insert(returnTS.end(),
			experience.begin() + birthTime + ignoreBefore,
			experience.begin() + birthTime + ignoreAfter + 1
		);
		birthTime += lifeTime;
	}
	return returnTS;
}

TS::intTimeSeries TS::trimTimeSeries(const intTimeSeries& experience, const std::pair<double, double>& range, size_t lives) {
	if (experience.size() % lives != 0) {
		std::cout << "  in trimTimeSeries :: while attempting to trimTimeSeries, experiance is not divisable by lives. exiting..." << std::endl;
		exit(1);
	}
	int lifeTime = experience.size() / lives;
	std::vector<int> lifeTimes(lives, lifeTime);
	return trimTimeSeries(experience, range, lifeTimes);
}

TS::intTimeSeries TS::trimTimeSeries(const intTimeSeries& experience, const Position& removeWhich, const std::vector<int>& lifeTimes, int n) {
	std::vector<int> localLifeTimes;
	int totalLifeTime = std::accumulate(lifeTimes.begin(), lifeTimes.end(), 0);
	if (totalLifeTime + lifeTimes.size() == experience.size()) {
		localLifeTimes = updateLifeTimes(lifeTimes, 1);
		totalLifeTime += lifeTimes.size();
	}
	else {
		localLifeTimes = lifeTimes;
	}
	
	if (totalLifeTime != experience.size()) {
		std::cout << "  in trimTimeSeries :: either sum(lifeTimes) must equal experiance.size(),\n"<<
			"    or sum(lifeTimes) + lifeTimes.size() must equal experience.size() (in the case of a bloated TimeSeries), but it does not. \n" << 
			"    I can not determine where lives start and end!\n" <<
			"    exiting..." << std::endl;
		exit(1);
	}

	intTimeSeries returnTS;
	int birthTime = 0;
	for (auto lifeTime : localLifeTimes) { // itterate over all life times
		if (removeWhich == Position::FIRST) {
			returnTS.insert(returnTS.end(), experience.begin() + birthTime + n, experience.begin() + birthTime + lifeTime);
		}
		else if (removeWhich == Position::LAST) {
			returnTS.insert(returnTS.end(), experience.begin() + birthTime, experience.begin() + birthTime + (lifeTime - n));
		}
		birthTime += lifeTime;
	}
	return returnTS;
}

TS::intTimeSeries TS::trimTimeSeries(const intTimeSeries& experience, const Position& removeWhich, size_t lives, int n) {
	if (experience.size() % lives != 0) {
		std::cout << "  in trimTimeSeries :: while attempting to trimTimeSeries, experiance is not divisable by lives. exiting..." << std::endl;
		exit(1);
	}
	int lifeTime = experience.size() / lives;
	std::vector<int> lifeTimes(lives, lifeTime);
	return trimTimeSeries(experience, removeWhich, lifeTimes, n);
}

TS::intTimeSeries TS::extendTimeSeries(const intTimeSeries& X, const std::vector<int>& lifeTimes, const std::vector<int> Y, Position addWhere, int n) {
	intTimeSeries returnTS;
	int birthTime = 0;
	for (auto l : lifeTimes) {
		if (addWhere == Position::FIRST) {
			for (int i = 0; i < n; i++) {
				returnTS.push_back(Y);
			}
		}
		returnTS.insert(returnTS.end(), X.begin() + birthTime, X.begin() + birthTime + l);
		if (addWhere == Position::LAST) {
			for (int i = 0; i < n; i++) {
				returnTS.push_back(Y);
			}
		}
		birthTime += l;
	}
	return returnTS;
}

TS::intTimeSeries TS::extendTimeSeries(const intTimeSeries& X, const size_t lives, const std::vector<int> Y, Position addWhere, int n) {
	if (X.size() % lives != 0) {
		std::cout << "  in extendTimeSeries :: while attempting to extendTimeSeries, X is not divisable by lives. exiting..." << std::endl;
		exit(1);
	}
	int lifeTime = X.size() / lives;
	std::vector<int> lifeTimes(lives, lifeTime);
	return extendTimeSeries(X, lifeTimes, Y, addWhere, n);
}

std::vector<int> TS::updateLifeTimes(const std::vector<int>& lifeTimes, int n) {
	std::vector<int> newLifeTimes;
	for (auto l : lifeTimes) {
		newLifeTimes.push_back(l + n);
	}
	return newLifeTimes;
}

TS::intTimeSeries TS::remapToIntTimeSeries(const TS::TimeSeries& X, TS::RemapRules rule, std::vector<double> ruleParameter) {
	//RemapRules { INT, BIT, TRIT, NEAREST_BIT, NEAREST_TRIT, MEDIAN };
	TS::intTimeSeries returnTS(X.size());
	if (rule == TS::RemapRules::INT) {
		for (int i = 0; i < X.size(); i++) {
			for (int j = 0; j < X[i].size(); j++) {
				returnTS[i].push_back((int)X[i][j]);
			}
		}
	}
	else if (rule == TS::RemapRules::BIT) {
		for (int i = 0; i < X.size(); i++) {
			for (int j = 0; j < X[i].size(); j++) {
				returnTS[i].push_back(Bit(X[i][j]));
			}
		}
	}
	else if (rule == TS::RemapRules::TRIT) {
		for (int i = 0; i < X.size(); i++) {
			for (int j = 0; j < X[i].size(); j++) {
				returnTS[i].push_back(Trit(X[i][j]));
			}
		}
	}
	else if (rule == TS::RemapRules::NEAREST_INT) {
		for (int i = 0; i < X.size(); i++) {
			for (int j = 0; j < X[i].size(); j++) {
				returnTS[i].push_back((int)(X[i][j] + .5));
			}
		}
	}
	else if (rule == TS::RemapRules::NEAREST_BIT) {
		for (int i = 0; i < X.size(); i++) {
			for (int j = 0; j < X[i].size(); j++) {
				returnTS[i].push_back(Bit(X[i][j] + .5));
			}
		}
	}
	else if (rule == TS::RemapRules::NEAREST_TRIT) {
		for (int i = 0; i < X.size(); i++) {
			for (int j = 0; j < X[i].size(); j++) {
				if (X[i][j] > .5) { returnTS[i].push_back(1); }
				else if (X[i][j] < -.5) { returnTS[i].push_back(-1); }
				else { returnTS[i].push_back(0); }
			}
		}
	}
	else if (rule == TS::RemapRules::MEDIAN) {
		std::vector<std::vector<double>> values;
		for (int row = 0; row < X.size(); row++) {
			for (int column = 0; column < X[row].size(); column++) {
				if (values.size() < column + 1) {
					values.push_back({});
				}
				values[column].push_back(X[row][column]);
			}
		}
		for (int column = 0; column < values.size(); column++) {
			std::sort(values[column].begin(), values[column].end());
		}
		int subSetCount = (int)ruleParameter[0];
		if (subSetCount == -1) {
			subSetCount = 2;
		}

		std::vector<std::vector<double>> cutPoints(values.size(), std::vector<double>(subSetCount - 1));
		for (int column = 0; column < values.size(); column++) {
			for (int cutPointIndex = 0; cutPointIndex < (subSetCount - 1); cutPointIndex++) {
				cutPoints[column][cutPointIndex] = (double)values[column][((double)(1 + cutPointIndex)* (double)values[column].size()) / (double)subSetCount];
			}
		}

		for (int row = 0; row < X.size(); row++) {
			for (int column = 0; column < X[row].size(); column++) {
				int s = 0;
				while (s < cutPoints[column].size() && X[row][column] > cutPoints[column][s]) {
 					s++;
				}
				returnTS[row].push_back(s);
			}
		}
	}
	else if (rule == TS::RemapRules::UNIQUE) {
		std::vector<std::vector<double>> uniqueSymbols; // this will hold the unique values in each column
		for (int row = 0; row < X.size(); row++) {
			for (int column = 0; column < X[row].size(); column++) {

				//std::cout << row << "  symbol: " << X[row][column] << " ";

				if (uniqueSymbols.size() < column + 1) {
					//std::cout << "NEW COLUMN (" << uniqueSymbols.size() << "):: ";
					uniqueSymbols.push_back({});
				}
				// see if the current value (i.e. symbol is in uniqueSymbols for this column already.
				auto symbolAddress = std::find(uniqueSymbols[column].begin(), uniqueSymbols[column].end(),X[row][column]);

				// if not found, it's new
				if (symbolAddress == uniqueSymbols[column].end()) { // this is a new symbol for this column
					//std::cout << "NEW SYMBOL :: VALUE = " << uniqueSymbols[column].size() << std::endl;
					returnTS[row].push_back(uniqueSymbols[column].size());
					uniqueSymbols[column].push_back(X[row][column]);
				}
				else { // it's not new, add the index (i.e. pointer - pointer to start of vector
					//std::cout << "VALUE = " << symbolAddress - uniqueSymbols[column].begin() << std::endl;
					returnTS[row].push_back(symbolAddress - uniqueSymbols[column].begin());
				}
			}
		}

	}

	return returnTS;
}
