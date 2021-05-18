#include "smearedness.h"

double SMR::getAtomicR(size_t whichConcept, size_t whichBrainNode, const TS::intTimeSeries& inputStates, const TS::intTimeSeries& worldStates, const TS::intTimeSeries& brainStates) {
	TS::intTimeSeries input_and_world_concept = TS::Join(inputStates, TS::subSetTimeSeries(worldStates, { (int)whichConcept }));
	TS::intTimeSeries input_and_brain_node = TS::Join(inputStates, TS::subSetTimeSeries(brainStates, { (int)whichBrainNode }));
	TS::intTimeSeries input_and_world_concept_and_brain_node = TS::Join({ 
		inputStates,
		TS::subSetTimeSeries(worldStates, { (int)whichConcept }), 
		TS::subSetTimeSeries(brainStates, { (int)whichBrainNode }) 
		});

	// Calculate final equation 
	// r = H(S,E) + H(S,M) - H(S) - H(E,M,S)
	return ENT::Entropy(input_and_world_concept) + ENT::Entropy(input_and_brain_node) - ENT::Entropy(inputStates) - ENT::Entropy(input_and_world_concept_and_brain_node);
}


std::vector<std::vector<double>> SMR::getAtomicRArray(const TS::intTimeSeries& inputStates, const TS::intTimeSeries& worldStates, const TS::intTimeSeries& brainStates){
	double sensorEntropy = ENT::Entropy(inputStates);
	std::vector<double> environmentSensorEntropies(worldStates[0].size(), 0.0);
	std::vector<double> memorySensorEntropies(brainStates[0].size(), 0.0);
	std::vector <std::vector<double>> totalEntropies(worldStates[0].size(), std::vector<double>(brainStates[0].size(), 0.0));
	for (int ii = 0; ii < worldStates[0].size(); ii++) {
		environmentSensorEntropies[ii] = ENT::Entropy(TS::Join(inputStates, TS::subSetTimeSeries(worldStates, { ii })));
		for (int jj = 0; jj < brainStates[0].size(); jj++) {
			totalEntropies[ii][jj] = ENT::Entropy(TS::Join({ inputStates, TS::subSetTimeSeries(worldStates, { ii }) , TS::subSetTimeSeries(brainStates, { jj }) }));
		}
	}
	for (int jj = 0; jj < brainStates[0].size(); jj++) {
		memorySensorEntropies[jj] = ENT::Entropy(TS::Join( inputStates,TS::subSetTimeSeries(brainStates, { jj }) ));
	}

	std::vector<std::vector<double>> m_array;
	std::vector<double> m_row;
	for (int ii = 0; ii < worldStates[0].size(); ii++) {
		m_row = {};
		for (int jj = 0; jj < brainStates[0].size(); jj++) {
			m_row.push_back(environmentSensorEntropies[ii] + memorySensorEntropies[jj] - sensorEntropy - totalEntropies[ii][jj]);
		}
		m_array.push_back(m_row);
	}
	return m_array;
}


double SMR::getSmearednessOfConcepts(const TS::intTimeSeries& inputStates, const TS::intTimeSeries& worldStates, const TS::intTimeSeries& brainStates) {
	std::vector < std::vector < double > > atomicRValues = getAtomicRArray(inputStates, worldStates, brainStates);
	double smearedness = 0.0;
	for (int ii = 0; ii < brainStates[0].size(); ii++) {
		for (int jj = 0; jj < worldStates[0].size() - 1; jj++) {
			for (int kk = jj + 1; kk < worldStates[0].size(); kk++) {
				smearedness += std::min(atomicRValues[jj][ii], atomicRValues[kk][ii]);
			}
		}
	}
	return smearedness;
}

double SMR::getSmearednessOfNodes(const TS::intTimeSeries& inputStates, const TS::intTimeSeries& worldStates, const TS::intTimeSeries& brainStates) {
	std::vector < std::vector < double > > atomicRValues = getAtomicRArray(inputStates, worldStates, brainStates);
	double smearedness = 0.0;
	for (int ii = 0; ii < worldStates[0].size(); ii++) {
		for (int jj = 0; jj < brainStates[0].size() - 1; jj++) {
			for (int kk = jj + 1; kk < brainStates[0].size(); kk++) {
				smearedness += std::min(atomicRValues[ii][jj], atomicRValues[ii][kk]);
			}
		}
	}
	return smearedness;

}

std::pair<double, double> SMR::getSmearednessConceptsNodesPair(const TS::intTimeSeries& inputStates, const TS::intTimeSeries& worldStates, const TS::intTimeSeries& brainStates) {
	std::vector < std::vector < double > > atomicRValues = getAtomicRArray(inputStates, worldStates, brainStates);
	double smearednessConcepts = 0.0;
	for (int ii = 0; ii < brainStates[0].size(); ii++) {
		for (int jj = 0; jj < worldStates[0].size() - 1; jj++) {
			for (int kk = jj + 1; kk < worldStates[0].size(); kk++) {
				smearednessConcepts += std::min(atomicRValues[jj][ii], atomicRValues[kk][ii]);
			}
		}
	}
	double smearednessNodes = 0.0;
	for (int ii = 0; ii < worldStates[0].size(); ii++) {
		for (int jj = 0; jj < brainStates[0].size() - 1; jj++) {
			for (int kk = jj + 1; kk < brainStates[0].size(); kk++) {
				smearednessNodes += std::min(atomicRValues[ii][jj], atomicRValues[ii][kk]);
			}
		}
	}
	return { smearednessConcepts, smearednessNodes };
}
