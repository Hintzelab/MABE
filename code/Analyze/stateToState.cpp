#include "stateToState.h"

// given two list of intTimeSeries, nodes and edges, and a list of lifetimes save stateToState diagram (graphviz) for each node->edge->node trasition
// if there is more then one lifeTime then the node->node between lifetimes is excluded
// nodes and edges are labeled, and edges labels are followed by "(# of times this edge appears)"
// in addtion, edge thickness is representative of relitive frequency (2.0 + .5 * std::sqrt(counts)) - need parameter here
// finally, edges are colored to represent the average time that the edge occures relitive to lifetimes RED->GREEN->BLUE = EARLY->LATE
void S2S::saveStateToState(const std::vector<TS::intTimeSeries>& nodesList, const std::vector<TS::intTimeSeries>& edgesList, std::vector<int> lifeTimes, std::string fileName) {

	if (nodesList[0].size() != edgesList[0].size()+lifeTimes.size()) {
		std::cout << "  in saveStateToState :: nodesList[0].size() != linksList[0].size() + lifeTimes.size(). exiting. " << std::endl;
		exit(1);
	}

	// create containers to hold the string "names" for nodes and edges
	std::vector<std::string> nodes(nodesList[0].size(), "");
	std::vector<std::string> edges(edgesList[0].size(), "");

	// for each list in the nodes lists
	for (int i = 0; i < nodesList.size(); i++) {
		if (nodesList[i].size() != nodesList[0].size()) {
			std::cout << "  in saveStateToState :: not all nodes lists have the same size. exiting. " << std::endl;
			exit(1);
		}
		// for each element in each time list in nodesList, append to the back of the new nodes strings list
		for (int j = 0; j < nodesList[i].size(); j++) {
			nodes[j] += TS::TimeSeriesSampleToString(nodesList[i][j],",");
			if (i < nodesList.size() - 1) {
				nodes[j] += "_";
			}
		}
	}

	// do the same thing for the edges
	for (int i = 0; i < edgesList.size(); i++) {
		if (edgesList[i].size() != edgesList[0].size()) {
			std::cout << "  in saveStateToState :: not all edge lists have the same size. exiting. " << std::endl;
			exit(1);
		}
		for (int j = 0; j < edgesList[i].size(); j++) {
			edges[j] += TS::TimeSeriesSampleToString(edgesList[i][j],",");
			if (i < edgesList.size() - 1) {
				edges[j] += "_";
			}
		}
	}

	// counters used to itterate over nodes and edges
	int currentNodeID = 0;
	int currentEdgeID = 0;

	std::vector<std::string> linkStrings; // node->(edge)->node only one instance of each node->(edge)->node will appear in this vector 
	std::vector<int> linkCounts; // number of times each element in linkStrings appears (in the same order as linkStrings)
	std::vector<std::vector<double>> linkTimes; // for each element in linkStrings (same order), a list of time relitive to life times when node->(edge)->node occurs
	int maxLinkCount = 1;

	std::string fileData = "digraph G {\n\n";

	// this code adds a legend
	/*
	fileData += "  subgraph cluster_key{\n";
	fileData += "    node[style = filled]\n";
	fileData += "    start_node[label = \"start\", fillcolor = pink]\n";
	fileData += "    middle_node[label = \"\", fillcolor = white]\n";
	fileData += "    end_node[label = \"end\", fillcolor = lightblue]\n";
	fileData += "    start_node->middle_node[label = \"  early(count) \" color = "0,1,.8" penwidth = 5]\n";
	fileData += "    middle_node->end_node[label = \"  late(count) \" color = ".7,1,.8" penwidth = 5]\n";
	fileData += "    label = \"LEGEND\"\n";
	fileData += "    color = black\n";
	fileData += "  }\n\n";
	*/

	// keep track of which nodes appear at the starts of lifetimes and then ends
	std::unordered_set<std::string> startNodes;
	std::unordered_set<std::string> endNodes;

	//create node->node text (we will add more info, like edge color, below), also keep track of number of times and when each link occures
	for (int life = 0; life < lifeTimes.size(); life++) {
		for (int t = 0; t < lifeTimes[life]; t++) {
			if (t == 0) { // on the first update of each lifetime, jump nodes ahead by 1 so we don't link across lifetimes
				currentNodeID ++;
				startNodes.insert(nodes[currentNodeID - 1]); // mark this node as a start node
			}

			std::string thisLink = "\"" + nodes[currentNodeID - 1] + "\"->\"" + nodes[currentNodeID] + "\"[label = \"" + edges[currentEdgeID];
			auto linkIter = std::find(linkStrings.begin(), linkStrings.end(), thisLink);
			if (linkIter == linkStrings.end()) { // this is a link we have not seen yet
				linkStrings.push_back(thisLink);
				linkCounts.push_back(1);
				linkTimes.push_back({ (double)t/(double)lifeTimes[life] });
			} else { // not a new link
				int thisIndex = linkIter - linkStrings.begin();
				linkCounts[thisIndex]++;
				maxLinkCount = std::max(maxLinkCount, linkCounts[thisIndex]);
				linkTimes[thisIndex].push_back((double)t / (double)lifeTimes[life]);
			}
			currentNodeID++;
			currentEdgeID++;
		}
		endNodes.insert(nodes[currentNodeID - 1]); // mark this node as a start node
	}
	
	for (auto n : startNodes) {
		fileData += "  \"" + n + "\"[style=filled,fillcolor = pink]\n";
	}
	for (auto n : endNodes) {
		fileData += "  \"" + n + "\"[style=filled,fillcolor = lightblue]\n";
	}
	for (int i = 0; i < linkStrings.size(); i++) {
		double linkTimeAve = std::accumulate(linkTimes[i].begin(), linkTimes[i].end(), 0.0)/ (double)linkTimes[i].size();
		
		/*
		// getting an RGB color
		int r = 255 * std::max(0.0, 1.0 - (2.0 * (double)linkTimeAve));
		int g = 255 * (1.0 - (std::abs(1.0 - (2.0 * (double)linkTimeAve))));
		int b = 255 * std::max(0.0, (2.0 * (double)linkTimeAve) - 1.0);
		int r1 = r / 16;
		int r2 = r - r1 * 16;
		int g1 = g / 16;
		int g2 = g - g1 * 16;
		int b1 = b / 16;
		int b2 = b - b1 * 16;
		std::stringstream rgb_color_ss;
		rgb_color_ss << "\"#" << std::hex << r1 << r2 << g1 << g2 << b1 << b2 << "\"";
		std::string rgb_color = rgb_color_ss.str();
		*/

		// getting an hsv_color
		std::string hsv_color = "\"" + std::to_string(linkTimeAve * .7) + ",1,.8\"";

		fileData += "  " +  linkStrings[i] + "(" + std::to_string(linkCounts[i]) + ")\"" +
			" color = " + hsv_color +
			" penwidth = " + std::to_string (1.0 + 2.0 * ((double)linkCounts[i] / (double)maxLinkCount)) + "]\n";
	}
	FileManager::writeToFile(fileName, fileData + "}\n");

}

