// A C++ program to find single source longest distances 
// in a DAG 
#include <iostream> 
#include <limits.h> 
#include <list> 
#include <unordered_map>
#include <string>
#include <stack> 
#include <vector>

#include <sstream>

#define NINF INT_MIN 
using namespace std; 

// Graph is represented using adjacency list. Every 
// node of adjacency list contains vertex number of 
// the vertex to which edge connects. It also 
// contains weight of the edge 
class AdjListNode { 
	int v; 
	int weight; 
	//int node_weight;

public: 
	AdjListNode(int _v, int _w) 
	{ 
		v = _v; 
		weight = _w;
		
	} 
	int getV() { return v; } 
	int getWeight() { return weight; }
	//int getNWeight() { return n_weight; }
}; 
	
// Class to represent a graph using adjacency list 
// representation 
class Graph { 
	int V; // No. of vertices' 
	vector<int> nodeWeights; // node values
	// Pointer to an array containing adjacency lists 
	list<AdjListNode>* adj; 
	
	// A function used by longestPath 
	void topologicalSortUtil(int v, bool visited[], 
							stack<int>& Stack); 
	
public: 
	Graph(int V, vector<int> nodeWeights); // Constructor 1
	Graph(int V, vector<int> nodeWeights, unordered_map<string, int> edgeWeights); // Constructor 2
	~Graph(); // Destructor 

	// function to add an edge to graph 
	void addEdge(int u, int v, int weight); 
	
	// Finds longest distances from given source vertex 
	vector<int> longestPath(int s); 
}; 