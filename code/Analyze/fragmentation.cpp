#include "fragmentation.h"

int FRAG::getFragmentation(const TS::intTimeSeries& feature, const TS::intTimeSeries& predictor, double threshold, const std::string& compareTo, int maxPartitionSize, bool reflectPartitions) {

	if (predictor.size() != feature.size()) {
		std::cout << "in entropy.h Fragmentation(...) :: the predictor and feature are not of the same size. exiting...";
		exit(1);
	}

	double featureEntropy = ENT::Entropy(feature);
	double predictorEntropy = ENT::Entropy(predictor);
	double maxJointEntropy = ENT::Entropy(TS::Join(feature,predictor)); // this is the entropy of the feature and predictor together
	double maxSharedEntropy = ENT::MutualEntropy(feature,predictor); // this is the max known by the predictor about the feature

	double maxEntropyRatio = 0;
	if (featureEntropy > 0) {
		maxEntropyRatio = maxSharedEntropy / featureEntropy;
	}
	else {
		return -1; // there is no entropy in feature, so we can just stop now
	}

	// get power set for all combinations of predictor (partitions)
	if (maxPartitionSize == -1 || maxPartitionSize > predictor[0].size()) {
		maxPartitionSize = predictor[0].size();
	}
	PowerSet ps;
	auto indexSets = ps.getPowerSet(predictor[0].size(), maxPartitionSize,reflectPartitions);

	// test each power set until we find a powerset with suffect shared entropy
	for (const auto& indexSet : indexSets) {
		TS::intTimeSeries partition = TS::subSetTimeSeries(predictor, indexSet);
		double partitionEntropy = ENT::Entropy(partition);
		double jointEntropy = ENT::Entropy(TS::Join(feature,partition));

		if (compareTo == "feature") {
			if ((partitionEntropy + featureEntropy) - jointEntropy >= threshold * featureEntropy) { // if what we have left after we remove joint entorpy is = feature entropy then...
				return indexSet.size();
			}
		}
		else if (compareTo == "shared") { // i.e.  to maxSharedEntropy
			if ((partitionEntropy + featureEntropy) - jointEntropy >= threshold * maxSharedEntropy) { // if what this brain partition knows about everything the brain knows about the feature then...
				return indexSet.size();
			}
		}
		else {
			std::cout << "in entropy.h Fragmentation(...) :: compairTo is not \"feature\" or \"shared\". exiting...";
			exit(1);
		}
	}

	// if we don't find a good partition...
	return -1;
}

std::vector<int> FRAG::getFragmentationSet(const std::vector<TS::intTimeSeries>& features, const TS::intTimeSeries& predictor, double threshold, const std::string& compareTo, int maxPartitionSize, bool reflectPartitions) {
	std::vector<int> returnVect;
	for (auto feature : features) {
		returnVect.push_back(getFragmentation(feature, predictor, threshold, compareTo, maxPartitionSize,reflectPartitions));
	}
	return returnVect;
}

std::vector<int> FRAG::getFragmentationSet(const TS::intTimeSeries& features, const TS::intTimeSeries& predictor, double threshold, const std::string& compareTo, int maxPartitionSize, bool reflectPartitions) {
	return getFragmentationSet(TS::deconstructTimeSeries(features), predictor, threshold, compareTo, maxPartitionSize, reflectPartitions);
}


std::pair<std::vector<std::vector<int>>, std::vector<std::vector<double>>> FRAG::getFragmentationMatrix(const std::vector<TS::intTimeSeries>& features, const TS::intTimeSeries& predictor, const std::string& compareTo, int maxPartitionSize, bool reflectPartitions) {
	for (int f = 0; f < features.size(); f++) {
		if (features[f].size() != predictor.size()) {
			std::cout << "in entropy.h fragmentationMatrix() :: the features sets are not of the same size as the predictor. exiting...";
			exit(1);
		}
	}

	std::cout << "  in getFragmentationMatrix: maxPartitionSize before correction : " << maxPartitionSize;

	// get power set for all combinations of predictor (partitions)
	if (maxPartitionSize == -1 || maxPartitionSize > predictor[0].size()) {
		maxPartitionSize = predictor[0].size();
	}
	std::cout << "   ... after correction : " << maxPartitionSize << std::endl;

	PowerSet ps;
	auto indexSets = ps.getPowerSet(predictor[0].size(), maxPartitionSize, reflectPartitions);

	std::cout << "    " << indexSets.size() << " partitions need to be evaluated for each feature..." << std::endl;

	//std::cout << TS::TimeSeriesToString(indexSets) << std::endl;

	std::vector<std::vector<double>> fragMatrix; // a matrix used to how the shared info for each partition and feature

	double predictorEntropy = ENT::Entropy(predictor);
	std::vector<double> partitionEntropies;
	std::vector<TS::intTimeSeries> partitions;
	for (size_t i = 0; i < indexSets.size(); i++) {
		partitions.push_back(TS::subSetTimeSeries(predictor, indexSets[i]));
		partitionEntropies.push_back(ENT::Entropy(partitions.back()));
	}

	int feature_count = 0;
	for (auto feature : features) {

		double featureEntropy = ENT::Entropy(feature);
		double maxJointEntropy = ENT::Entropy(TS::Join(feature, predictor)); // this is the entropy of this feature and predictor together
		double maxSharedEntropy = ENT::MutualEntropy(feature, predictor); // this is the max known by the predictor about this feature

		std::cout << "  feature " << feature_count++ <<
			" : featureEntropy = " << featureEntropy << "  maxSharedEntropy between feature and predictor = " << maxSharedEntropy;
		if (std::abs(featureEntropy - maxSharedEntropy) < .0000001) { // a hack because of rounding error!
			std::cout << " **";
		}
		std::cout << std::endl;

		if (maxSharedEntropy == 0) { // there is no infomation in any fragments, add an empty row to fragMap for this feature
			fragMatrix.push_back(std::vector<double>(indexSets.size(),0.0));
			fragMatrix.back().push_back(featureEntropy);
			fragMatrix.back().push_back(0.0); // maxSharedEntropy

		}
		else {
			fragMatrix.push_back({});

			for (size_t i = 0; i < indexSets.size(); i++) {
				double jointEntropy = ENT::Entropy(TS::Join(feature, partitions[i]));

				if (compareTo == "none") { // add mutual entropy without normalizing
					fragMatrix.back().push_back((partitionEntropies[i] + featureEntropy) - jointEntropy);
				}
				else if (compareTo == "feature") { // add mutual entropy nomalized by featureEntropy
					fragMatrix.back().push_back(((partitionEntropies[i] + featureEntropy) - jointEntropy) / featureEntropy);
				}
				else if (compareTo == "shared") { // add mutual entropy nomalized by maxSharedEntropy
					fragMatrix.back().push_back(((partitionEntropies[i] + featureEntropy) - jointEntropy) / maxSharedEntropy);
				}
				else {
					std::cout << "  In fragmentation::fragmentationMatrix(...) recived bad compairTo method: " << compareTo << " must be \"none\", \"feature\", or \"shared\". exiting..." << std::endl;
					exit(1);
				}
			}
			fragMatrix.back().push_back(featureEntropy);
			fragMatrix.back().push_back(maxSharedEntropy);
		}
	}
	fragMatrix.push_back(partitionEntropies);
	fragMatrix.back().push_back(0);
	fragMatrix.back().push_back(0);

	std::pair<std::vector<std::vector<int>>, std::vector<std::vector<double>>> result({ indexSets, fragMatrix });
	return result;

}

std::pair<std::vector<std::vector<int>>, std::vector<std::vector<double>>> FRAG::getFragmentationMatrix(const TS::intTimeSeries& features, const TS::intTimeSeries& predictor, const std::string& compareTo, int maxPartitionSize, bool reflectPartitions) {
	return getFragmentationMatrix(TS::deconstructTimeSeries(features), predictor, compareTo, maxPartitionSize,reflectPartitions);
}


// save a collection of fragmentation matrices derived from a set of time ranges
void FRAG::saveFragMatrixSet(const TS::intTimeSeries& features, const TS::intTimeSeries& predictor, const std::vector<int>& lifeTimes, const std::vector<std::pair<double, double>>& lifeTimeRanges, const std::string& fileName, const std::string& compareTo, int maxPartitionSize, bool reflectPartitions) {
	std::string outStr;
	// save predition information
	// this is written here as flow (i.e. data flow) as in the fragmentation matrix of (input + hidden) predictions of (output + hidden)
	auto flowStatesBefore = predictor;
	auto flowStatesAfter = features;
	for (int r = 0; r < lifeTimeRanges.size(); r++) {
		double i = lifeTimeRanges[r].first;
		double j = lifeTimeRanges[r].second;

		std::cout << "\n    Frag Matrix range: " << std::to_string(int(i * 100)) + "_" + std::to_string(int(j * 100)) << std::endl;
		auto fm = getFragmentationMatrix(TS::trimTimeSeries(flowStatesAfter, { i,j }, lifeTimes), TS::trimTimeSeries(flowStatesBefore, { i,j }, lifeTimes), "shared", maxPartitionSize, reflectPartitions);
		outStr = "";
		if (r == 0) {
			outStr += "flowPartitions = [\n";
			for (auto p : fm.first) {
				outStr += "[";
				for (auto e : p) {
					outStr += std::to_string(e) + ",";
				}
				outStr += "],\n";
			}
			outStr += "]\n\n";
		}

		outStr += "flowMap_" + std::to_string(int(i * 100)) + "_" + std::to_string(int(j * 100)) + " = [\n";
		for (auto p : fm.second) {
			outStr += "[";
			for (auto e : p) {
				outStr += std::to_string(e) + ",";
			}
			outStr += "],\n";
		}
		outStr += "]\n\n";

		FileManager::writeToFile(fileName, outStr);
	}
}

void FRAG::saveFragMatrix(const TS::intTimeSeries& features, const TS::intTimeSeries& predictor, const std::string& fileName, const std::string& compareTo, std::vector<std::string> featureNames, int maxPartitionSize, bool reflectPartitions) {
	auto fm = getFragmentationMatrix(features, predictor, compareTo, maxPartitionSize, reflectPartitions);
	std::string outStr = "fragPartitions = [\n";
	for (auto p : fm.first) {
		outStr += "[";
		for (auto e : p) {
			outStr += std::to_string(e) + ",";
		}
		outStr += "],\n";
	}
	outStr += "]\n\n";

	outStr += "fragMap = [\n";
	for (auto p : fm.second) {
		outStr += "[";
		for (auto e : p) {
			outStr += std::to_string(e) + ",";
		}
		outStr += "],\n";
	}
	outStr += "]\n\n";

	outStr += "featureNames = [";
	for (auto f : featureNames) {
		outStr += "\"" + f + "\",";
	}

	outStr += "]\n\n";

	FileManager::writeToFile(fileName, outStr);
}


/*
void testEntropy() {
	{
		std::cout << "\n\ntesting Join(X,Y)" << std::endl;

		std::vector<std::vector<int>> X, Y, Z, result;
		X = { {1,2,3},{0,0,0},{0,0,0},{0,0,0} };
		Y = { {2,3,4},{1,1,1},{0,0,0},{0,0,0} };
		Z = { {123,2},{5,43,2},{0,0,0},{0,0,0} };

		std::cout << "X:\n" << TimeSeriesToString(X) << std::endl;
		std::cout << "Y:\n" << TimeSeriesToString(Y) << std::endl;
		std::cout << "Z:\n" << TimeSeriesToString(Z) << std::endl;
		result = Join(X, Y);
		std::cout << "Join(X, Y):\n" << TimeSeriesToString(result) << std::endl;
		result = Join({ X, Y, Z });
		std::cout << "Join({X, Y, Z}):\n" << TimeSeriesToString(result) << std::endl;

		std::cout << "Entropy(Join({ X, Y, Z })) = " << Entropy(Join({ X, Y, Z })) << std::endl;

		std::cout << "\n\ntrimSetState(X,{1,2}):\n" << TimeSeriesToString(subSetTimeSeries(X, { 1,2 })) << std::endl;
		std::cout << "\n\ntrimSetState(Y,{2}):\n" << TimeSeriesToString(subSetTimeSeries(Y, { 2 })) << std::endl;
		std::cout << "\n\ntrimSetState(Z,{1,2}):\n" << TimeSeriesToString(subSetTimeSeries(Z, { 1,2 })) << std::endl;

	}


	{
		std::cout << "\n\ntesting Entropy(X)" << std::endl;

		std::vector<std::pair<double, std::vector<std::vector<int>>>> tests;
		tests = {
			{1.0, { {0},{1} }},
			{0.0, { {0,1},{0,1} }},
			{1.0, { {0},{1},{1},{0} }},
			{.9182, { {0},{1},{1} }},
			{.8112, { {0},{1},{1},{1} }},
			{.7219, { {0},{1},{1},{1},{1} }},
			{.7219, { {0,2,3,5},{1,1,1},{1,1,1},{1,1,1},{1,1,1} }},
		};

		for (auto test : tests) {
			std::cout << test.first << "... ? " << Entropy(test.second) << std::endl;
		}
	}

	{
		std::cout << "\n\ntesting the other stuff" << std::endl;

		std::vector<std::vector<int>> X, Y, Z, Z_and, Z_or, Z_xor ;
		X = { {0},{0},{1},{1} };
		Y = { {0},{1},{0},{1} };
		Z_and = { {0},{0},{0},{1} };
		Z_or = { {0},{1},{1},{1} };
		Z_xor = { {0},{1},{1},{0} };

		std::cout << "X:\n" << TimeSeriesToString(X) << std::endl;
		std::cout << "Y:\n" << TimeSeriesToString(Y) << std::endl;
		std::cout << "Z_and:\n" << TimeSeriesToString(Z_and) << std::endl;
		std::cout << "Z_or:\n" << TimeSeriesToString(Z_or) << std::endl;
		std::cout << "Z_xor:\n" << TimeSeriesToString(Z_xor) << std::endl;

		std::cout << std::endl;

		std::cout << "X(" << Entropy(X) << ") and Y(" << Entropy(Y) << ") = Z_and(" << Entropy(Z_and) << ")    MutualEntropy(X,Z_and) = " << MutualEntropy(X, Z_and) << std::endl;
		std::cout << "X(" << Entropy(X) << ") and Y(" << Entropy(Y) << ") = Z_or(" << Entropy(Z_or) << ")    MutualEntropy(X,Z_or) = " << MutualEntropy(X, Z_or) << std::endl;
		std::cout << "X(" << Entropy(X) << ") and Y(" << Entropy(Y) << ") = Z_xor(" << Entropy(Z_xor) << ")    MutualEntropy(X,Z_xor) = " << MutualEntropy(X, Z_xor) << std::endl;

		std::cout << std::endl;

		std::cout << "X(" << Entropy(X) << ") and Y(" << Entropy(Y) << ") = Z_and(" << Entropy(Z_and) << ")    MutualEntropy(Join(X,Y),Z_and) = " << MutualEntropy(Join(X, Y), Z_and) << std::endl;
		std::cout << "X(" << Entropy(X) << ") and Y(" << Entropy(Y) << ") = Z_or(" << Entropy(Z_or) << ")    MutualEntropy(Join(X,Y),Z_or) = " << MutualEntropy(Join(X, Y), Z_or) << std::endl;
		std::cout << "X(" << Entropy(X) << ") and Y(" << Entropy(Y) << ") = Z_xor(" << Entropy(Z_xor) << ")    MutualEntropy(Join(X,Y),Z_xor) = " << MutualEntropy(Join(X, Y), Z_xor) << std::endl;

		std::cout << std::endl;

		std::cout << "X(" << Entropy(X) << ") and Y(" << Entropy(Y) << ") = Z_and(" << Entropy(Z_and) << ")    ConditionalEntropy(X,Z_and) = " << ConditionalEntropy(X, Z_and) << std::endl;
		std::cout << "X(" << Entropy(X) << ") and Y(" << Entropy(Y) << ") = Z_or(" << Entropy(Z_or) << ")    ConditionalEntropy(X,Z_or) = " << ConditionalEntropy(X, Z_or) << std::endl;
		std::cout << "X(" << Entropy(X) << ") and Y(" << Entropy(Y) << ") = Z_xor(" << Entropy(Z_xor) << ")    ConditionalEntropy(X,Z_xor) = " << ConditionalEntropy(X, Z_xor) << std::endl;

		std::cout << std::endl;

		std::cout << "X(" << Entropy(X) << ") and Y(" << Entropy(Y) << ") = Z_and(" << Entropy(Z_and) << ")    ConditionalMutualEntropy(X, Y, Z_and) = " << ConditionalMutualEntropy(X, Y, Z_and) << std::endl;
		std::cout << "X(" << Entropy(X) << ") and Y(" << Entropy(Y) << ") = Z_or(" << Entropy(Z_or) << ")    ConditionalMutualEntropy(X, Y, Z_or) = " << ConditionalMutualEntropy(X, Y, Z_or) << std::endl;
		std::cout << "X(" << Entropy(X) << ") and Y(" << Entropy(Y) << ") = Z_xor(" << Entropy(Z_xor) << ")    ConditionalMutualEntropy(X, Y, Z_xor) = " << ConditionalMutualEntropy(X, Y, Z_xor) << std::endl;

		std::cout << std::endl;

		std::cout << "X(" << Entropy(X) << ") and Y(" << Entropy(Y) << ") = Z_and(" << Entropy(Z_and) << ")    ConditionalMutualEntropy(X, Z_and, Y) = " << ConditionalMutualEntropy(X, Z_and, Y) << std::endl;
		std::cout << "X(" << Entropy(X) << ") and Y(" << Entropy(Y) << ") = Z_or(" << Entropy(Z_or) << ")    ConditionalMutualEntropy(X, Z_or, Y) = " << ConditionalMutualEntropy(X, Z_or, Y) << std::endl;
		std::cout << "X(" << Entropy(X) << ") and Y(" << Entropy(Y) << ") = Z_xor(" << Entropy(Z_xor) << ")    ConditionalMutualEntropy(X, Z_xor, Y) = " << ConditionalMutualEntropy(X, Z_xor, Y) << std::endl;
	}

	{
		std::cout << "\n\ntesting fragmentation" << std::endl;

		intTimeSeries X, Y, Z, Z_and, Z_or, Z_xor;
		X = { {0},{0},{1},{1} };
		Y = { {0},{1},{0},{1} };
		Z_and = { {0},{0},{0},{1} };
		Z_or = { {0},{1},{1},{1} };
		Z_xor = { {0},{1},{1},{0} };



		auto fm_none = getFragmentationMatrix({ Z_and,Z_or,Z_xor }, Join(X, Y),"none");
		auto fm_feature = getFragmentationMatrix({ Z_and,Z_or,Z_xor }, Join(X, Y));
		auto fm_shared = getFragmentationMatrix({ Z_and,Z_or,Z_xor }, Join(X, Y),"shared");

		for (auto fm : { fm_none, fm_feature, fm_shared }) {
			std::cout << "-----------------------------------------------------" << std::endl;
			for (auto r : fm.second) {
				for (auto e : r) {
					std::cout << e << " ";
				}
				std::cout << std::endl;
			}
			std::cout << "-----------------------------------------------------" << std::endl;
		}
	}

	{
		std::cout << "\n\ntesting trim" << std::endl;
		intTimeSeries X = { {0},{1},{2},{3},{4},{5},{6},{7},{8},{9} };
		std::cout << TimeSeriesToString(X) << std::endl;

		std::cout << "\ntrim(0,1):" << std::endl;
		std::cout << TimeSeriesToString(trimTimeSeries(X, { 0,1 }, 2)) << std::endl;
		std::cout << "\ntrim(0,.5):" << std::endl;
		std::cout << TimeSeriesToString(trimTimeSeries(X, { 0,.5 }, 2)) << std::endl;
		std::cout << "\ntrim(.51,1):" << std::endl;
		std::cout << TimeSeriesToString(trimTimeSeries(X, { .51,1 }, 2)) << std::endl;
		std::cout << "\ntrim(0,.332):" << std::endl;
		std::cout << TimeSeriesToString(trimTimeSeries(X, { 0,.332 }, 2)) << std::endl;
		std::cout << "\ntrim(.333,.666):" << std::endl;
		std::cout << TimeSeriesToString(trimTimeSeries(X, { .333,.666 }, 2)) << std::endl;
		std::cout << "\ntrim(.667,1):" << std::endl;
		std::cout << TimeSeriesToString(trimTimeSeries(X, { .667,1 }, 2)) << std::endl;

		std::cout << "\ntrim(0,.49) lifeTimes(4,6):" << std::endl;
		std::cout << TimeSeriesToString(trimTimeSeries(X, { 0,.49 }, { 4,6 })) << std::endl;
		std::cout << "\ntrim(0,.5) lifeTimes(4,6):" << std::endl;
		std::cout << TimeSeriesToString(trimTimeSeries(X, { 0,.5 }, { 4,6 })) << std::endl;
		std::cout << "\ntrim(.51,1) lifeTimes(4,6):" << std::endl;
		std::cout << TimeSeriesToString(trimTimeSeries(X, { .51,1 }, { 4,6 })) << std::endl;



		std::cout << "\n\ntesting trim with FIRST/LAST" << std::endl;
		X = { {10},{11},{12},{13},{14},{15},{20},{21},{22},{23},{24},{25} };
		std::cout << TimeSeriesToString(X) << std::endl;

		std::cout << "\ntrim first:" << std::endl;
		std::cout << TimeSeriesToString(trimTimeSeries(X, FIRST, 2)) << std::endl;
		std::cout << "\ntrim last:" << std::endl;
		std::cout << TimeSeriesToString(trimTimeSeries(X, LAST, 2)) << std::endl;

		std::cout << "\n\ntesting trim with FIRST/LAST" << std::endl;
		X = { {10},{11},{12},{13},{20},{21},{22},{23},{24},{25},{26},{27} };
		std::cout << TimeSeriesToString(X) << std::endl;
		std::cout << "\ntrim first (3,7):" << std::endl;
		std::cout << TimeSeriesToString(trimTimeSeries(X, FIRST, { 3,7 })) << std::endl;
		std::cout << "\ntrim last (3,7):" << std::endl;
		std::cout << TimeSeriesToString(trimTimeSeries(X, LAST, { 3,7 })) << std::endl;

	}
	exit(1);
}
*/
