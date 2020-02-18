#include "Graph.h"
	
Graph::Graph(int V, vector<int> nodeWeights ) // Constructor 
{ 
	this->V = V; 
	this->nodeWeights = nodeWeights;
	adj = new list<AdjListNode>[V]; 
} 
Graph::Graph(int V, vector<int> nodeWeights, unordered_map<string, int> edgeWeights) {
	this->V = V;
	this->nodeWeights = nodeWeights;
	adj = new list<AdjListNode>[V];

	for(pair<string, int> el : edgeWeights) {
		vector<string> tokens;
		stringstream st(el.first);
		string tok;
		while(getline(st, tok, ':')) {
			//std::cout <<  << std::endl;
			tokens.push_back(tok);
		}
		addEdge(atoi(tokens[0].c_str()), atoi(tokens[1].c_str()), el.second);
	}
}
Graph::~Graph() // Destructor 
{ 
	delete [] adj; 
} 


void Graph::addEdge(int u, int v, int weight) 
{ 
	AdjListNode node(v, weight); 
	adj[u].push_back(node); // Add v to u's list 
} 
	
// A recursive function used by longestPath. See below 
// link for details 
// https:// www.geeksforgeeks.org/topological-sorting/ 
void Graph::topologicalSortUtil(int v, bool visited[], 
								stack<int>& Stack) 
{ 
	// Mark the current node as visited 
	visited[v] = true; 
	
	// Recur for all the vertices adjacent to this vertex 
	list<AdjListNode>::iterator i; 
	for (i = adj[v].begin(); i != adj[v].end(); ++i) { 
		AdjListNode node = *i; 
		if (!visited[node.getV()]) 
			topologicalSortUtil(node.getV(), visited, Stack); 
	} 
	
	// Push current vertex to stack which stores topological 
	// sort 
	Stack.push(v); 
} 
	
// The function to find longest distances from a given vertex. 
// It uses recursive topologicalSortUtil() to get topological 
// sorting. 
vector<int> Graph::longestPath(int s, vector<int> taskMapping) 
{ 
	stack<int> Stack; 
	//int dist[V];
	vector<int> dist (V); 
	
	// Mark all the vertices as not visited 
	bool* visited = new bool[V]; 
	for (int i = 0; i < V; i++) 
		visited[i] = false; 
	
	// Call the recursive helper function to store Topological 
	// Sort starting from all vertices one by one 
	for (int i = 0; i < V; i++) 
		if (visited[i] == false) 
			topologicalSortUtil(i, visited, Stack); 
	
	// Initialize distances to all vertices as infinite and 
	// distance to source as 0 
	for (int i = 0; i < V; i++) 
		dist[i] = NINF; 
	dist[s] = nodeWeights[s]; 
	
	// Process vertices in topological order 
	while (Stack.empty() == false) { 
		// Get the next vertex from topological order 
		int u = Stack.top(); 
		Stack.pop(); 
	
		// Update distances of all adjacent vertices 
		list<AdjListNode>::iterator i; 
		if (dist[u] != NINF) { 
			for (i = adj[u].begin(); i != adj[u].end(); ++i) {
				//If the two tasks are assigned to different processors
				if(taskMapping[u] == taskMapping[i->getV()]) {
					if (dist[i->getV()] < dist[u] + nodeWeights[i->getV()] ){ 
						dist[i->getV()] = dist[u] + nodeWeights[i->getV()]; 
					}
				}
				else {
					if (dist[i->getV()] < dist[u] + nodeWeights[i->getV()] + i->getWeight()){ 
						dist[i->getV()] = dist[u] + nodeWeights[i->getV()] + i->getWeight(); 
					}
				}
				
			}
		} 
	} 
	
	// Print the calculated longest distances 
	for (int i = 0; i < V; i++) 
		(dist[i] == NINF) ? cout << "INF " : cout << dist[i] << " "; 
	
	delete [] visited; 

	return dist;
} 
/*
// Driver program to test above functions 
int main() 
{ 
	// Create a graph given in the above diagram. 
	// Here vertex numbers are 0, 1, 2, 3, 4, 5 with 
	// following mappings: 
	// 0=r, 1=s, 2=t, 3=x, 4=y, 5=z 
	vector<int> test_node_weights;

	for(int i=0; i < 6; ++i) {
		test_node_weights.push_back(i); // = i;
		cout << "Test ET" << endl;
	}
	
	unordered_map<string, int> test_edge_weights;

	test_edge_weights["0:1"] = 5;
	test_edge_weights["0:2"] = 3;
	test_edge_weights["1:3"] = 6;
	test_edge_weights["1:2"] = 2;
	test_edge_weights["2:4"] = 4;
	test_edge_weights["2:5"] = 2;
	test_edge_weights["2:3"] = 7;
	test_edge_weights["3:5"] = 1;
	test_edge_weights["3:4"] = -1;
	test_edge_weights["4:5"] = -2;

	Graph g(6, test_node_weights, test_edge_weights);
	/*
	g.addEdge(0, 1, 5); 
	g.addEdge(0, 2, 3); 
	g.addEdge(1, 3, 6); 
	g.addEdge(1, 2, 2); 
	g.addEdge(2, 4, 4); 
	g.addEdge(2, 5, 2); 
	g.addEdge(2, 3, 7); 
	g.addEdge(3, 5, 1); 
	g.addEdge(3, 4, -1); 
	g.addEdge(4, 5, -2); 
	
	int s = 0; 
	cout << "Following are longest distances from "
			"source vertex "
		<< s << " \n"; 
	vector<int> result = g.longestPath(s); 
	
	for(int i=0; i < result.size(); ++i) {
		(result[i] == NINF) ? cout << "INF " : cout << result[i] << " ";
	}
	return 0; 
} 
*/