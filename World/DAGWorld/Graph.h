// A C++ program to find single source longest distances 
// in a DAG 
#include <iostream> 
#include <limits.h> 
#include <list> 
#include <unordered_map>
#include <string>
#include <stack> 
#include <vector>
#include <numeric>
#include <functional>
#include <sstream>

#define NINF INT_MIN 
#define INF INT_MAX
using namespace std; 

// Graph is represented using adjacency list. Every 
// node of adjacency list contains vertex number of 
// the vertex to which edge connects. It also 
// contains weight of the edge 
class AdjListNode { 
	int v; 
	double weight;
	// double ranku; 
	//int node_weight;

public: 
	AdjListNode(int _v, double _w) 
	{ 
		v = _v; 
		weight = _w;
		// ranku = 0.0	//rank-u of each node
		
	}
	
	
	
	int getV() { return v; } 
	double getWeight() { return weight; }
	//double getRanku() { return ranku; }
	//void setRanku(double _ranku) { ranku = _ranku; }

	
	//int getNWeight() { return n_weight; }
}; 
	
// Class to represent a graph using adjacency list 
// representation 
class Graph { 
	int V; // No. of vertices'
	int procs; //no. of procs 
	vector<vector<double>> nodeWeights; // node values
	vector<double> rankus; // contains ranku values for each node in the DAG
	vector<int> proc_map; // contains the task-proc mapping
	vector<vector<double>> bwMat;
	// vector<AdjListNode> nodes; // AdjListNode vector containing all the nodes for quicker access (may need to improve later -- too much memory -- ) -- fixed above
	// Pointer to an array containing adjacency lists 
	list<AdjListNode>* adj; 
	vector<vector<int>> preds;
	double avgBW;	//Average comm bandwidth between available processors

	struct ScheduleEvent{
		int v;
		int proc;
		double start;
		double end;
		
	};
	vector<ScheduleEvent> events;
	vector<vector<ScheduleEvent>> proc_sch;
	// unordered_map<string, double> edgeWeights;

	//int term_node;
	int root_node;
	// A function used by longestPath 
	void topologicalSortUtil(int v, bool visited[], 
							stack<int>& Stack); 
	
public: 
	Graph(int V, vector<vector<double>> nodeWeights); // Constructor 1
	Graph(int V, vector<vector<double>> nodeWeights, unordered_map<string, double> edgeWeights, vector<vector<double>> bwMat); // Constructor 2
	~Graph(); // Destructor 

	// function to add an edge to graph 
	void addEdge(int u, int v, double weight); 
	//function to compute the predeccessors
	void compPreds();
	//print function
	void print_node(int v) {
		cout << "Node: " << v << "/Ranku: " << rankus[v] << endl;
	}
	//Sets up the processor mapping
	void setProcMap(vector<int> pmap);
	void setProcs(int _procs) { procs=_procs;}
	//function to compute the earliest finish time
	//ScheduleEvent computeEFT(int v, int proc);
	//compute rankus 
	double ranku();
	//determine if a node's ranku is ready for computation
	bool canProcessNode(int node);
	// Finds longest distances from given source vertex 
	vector<double> longestPath(int s, vector<int> taskMapping); 

	double scheduleLength();

	ScheduleEvent computeEFT(int node, int proc);
	void printPrcSchd();
	string printEvent(ScheduleEvent event);
	int get_v(){return this->V;}
	//double compAvgBW();
}; 