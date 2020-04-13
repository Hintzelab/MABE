#include "Graph.h"
	
Graph::Graph(int V, vector<vector<double>> nodeWeights) // Constructor 
{ 
	this->V = V; 
	this->nodeWeights = nodeWeights;
	adj = new list<AdjListNode>[V];
	
} 
Graph::Graph(int V, vector<vector<double>> nodeWeights, unordered_map<string, double> edgeWeights, vector<vector<double>> bwMat) {
	this->V = V;
	this->nodeWeights = nodeWeights;
	this->bwMat = bwMat;
	this->preds = vector<vector<int>>(V);

	//cout << "Assigned all the members" << endl;
	//this->proc_map = proc_map;
	adj = new list<AdjListNode>[V];

	//cout << "assigned the adj lists" << endl;
	int weight_iter = 0;
	
	this->rankus = vector<double> (V, -1);
	//cout << "assigned the ranku vector" << endl;
	//compute the average bw rate between available procs
	double avgbw = 0.0;
	//int bwcount = 0;
	for(int i=0;i<bwMat.size();i++) {
		for(int j=0;j<bwMat.size();j++) {
			avgbw += bwMat[i][j];
		}
	}

	//Init events
	for(int i =0; i<V; i++) {
		ScheduleEvent sched = {};
		sched.v = -1;
		sched.start = -1;
		sched.end = -1;
		sched.proc = -1;
		events.push_back(sched);
	}
	/*
	for(int i =0; i<V; i++) {
		cout << "ScheduleEvent: " << events[i].v << events[i].start << endl;
	}
	*/
	//cout << "Passed the average bw loop" << endl;
	//cout << "BW mat size: " << bwMat.size() << endl;
	//cout << "AvgBW: " << avgbw << endl;
	this->avgBW = avgbw / ((bwMat.size() * bwMat.size()) - bwMat.size());

	//Assign edge weights (data size)
	for(pair<string, double> el : edgeWeights) {
		vector<string> tokens;
		cout << "el.first:" << el.first << endl;
		stringstream st(el.first);
		string tok;
		while(getline(st, tok, ':')) {
			//std::cout << tok << std::endl;
			tokens.push_back(tok);
		}
		//cout << "el.second: " << el.second << endl;
		addEdge(atoi(tokens[0].c_str()), atoi(tokens[1].c_str()), el.second);
	}
	//cout << "passed the edge loop" << endl;
}
Graph::~Graph() // Destructor 
{ 
	delete [] adj; 
}
void Graph::setProcMap(vector<int> pmap) {
	this->proc_map = pmap;
}
//Create a reverse mapping for the data dependencies, i.e, create a predecessor map... 
void Graph::compPreds() {
	for (int i=0; i < this->V; i++){
		//Get the adj list of the current node
		list<AdjListNode>::iterator j;
		for(j=adj[i].begin(); j!=adj[i].end();++j) {
			AdjListNode pred = *j;
			this->preds[pred.getV()].push_back(i);		
		}
	}

	for(int i=0;i<preds.size();i++) {
		for(int j=0;j<preds[i].size();j++) {
			cout << preds[i][j] << " ";
		}
		cout << endl;
	}
}
/*
ScheduleEvent Graph::computeEFT(int v, int proc) {
	
}
*/
// Computes the ranku of all nodes using BFS
double Graph::ranku() {
	//Initialize the terminal node's ranku with its avg comp cost
	int term_node = V;
	for(int i=0; i < V; i++) {
		if(adj[i].empty()) {
			term_node = i;
		}
	}
	cout << "Term Node: " << term_node << endl;
	cout << "Avg BW: " << avgBW << endl;
	double term_ranku = accumulate(nodeWeights[term_node].begin(), nodeWeights[term_node].end(), 0.0) / nodeWeights[term_node].size(); 
    rankus[term_node] = term_ranku;
    //cout << "Ranku/TERMNODE: " << rankus[term_node] << endl;
	vector<int> visit_queue =  {};

	visit_queue = this->preds[term_node];			//initialize the processing queue
	//for(int q : preds[term_node]){
	//	visit_queue.push_back(q);
	//}

	while (!visit_queue.empty()) {
		int cur_node = visit_queue[visit_queue.size()-1];
		visit_queue.pop_back();
		while(!this->canProcessNode(cur_node)) {
			int temp_node = visit_queue[visit_queue.size()-1];
			visit_queue.pop_back();
			visit_queue.push_back(cur_node);	//re-insert for future evaluation
			cur_node = temp_node; 			// iterate
		}

		// Get the max ranku among successors
		double max_succ_ranku = -1.0;
		double temp_ranku = 0.0;
		list<AdjListNode>::iterator i;
		for (i = adj[cur_node].begin(); i != adj[cur_node].end(); i++) {
			AdjListNode succ_node = *i;
			//cout << "NodeID: " << i->getV() << " ";
			//string edge_str = to_string(cur_node) + "-" + to_string(succ_id);
			temp_ranku = succ_node.getWeight()/avgBW + rankus[succ_node.getV()];			//here succ_node.getWeight() returns the edge weight
																					// between cur_node and succ_node
			//cout << "Temp Ranku: " << temp_ranku << " ";
			//Update the max ranku if necessary
			if(temp_ranku > max_succ_ranku) {
				max_succ_ranku = temp_ranku;
			}

			//Update the node's ranku value
			double avgNodeW = 0.0;
			for(int j=0; j<nodeWeights[cur_node].size(); j++) {
				avgNodeW += nodeWeights[cur_node][j];
			}
			avgNodeW = avgNodeW / nodeWeights[cur_node].size();
			rankus[cur_node] = max_succ_ranku + avgNodeW;
			//rankus[cur_node] = max_succ_ranku + accumulate(nodeWeights[cur_node].begin(), nodeWeights[cur_node].end(), 0.0) / nodeWeights[cur_node].size();
			//nodes[cur_node].setRanku(max_succ_ranku + nodeWeights[cur_node]);
		}
		//Insert the predeccessors of cur_node to visit_queue
		
		for(int pred : preds[cur_node]) {
			if( std::find(visit_queue.begin(),visit_queue.end(),pred) == visit_queue.end()) {
				visit_queue.insert(visit_queue.begin(), pred);
			}
		}
		
	}
	//Check the ranku values
	for (int r =0; r < rankus.size(); r++) {
		cout << "Ranku " << r << ": " << rankus[r] << endl;
	}	
}


// Determines whether a node can be processed for ranku computation or not. A node can be processed only after it successor's ranku value is determined
bool Graph::canProcessNode(int node) {
	bool return_flag = true;
	list<AdjListNode>::iterator i;
	for (i = adj[node].begin(); i != adj[node].end(); i++) {
		AdjListNode succ_node = *i;
		if (rankus[succ_node.getV()] == -1) {
			return_flag = false;
			break;
		}
	}
	return return_flag;
}
void Graph::addEdge(int u, int v, double weight) 
{ 
	//cout << "Entered addEdge" << endl;
	AdjListNode node(v, weight);

	//this->edgeWeights[to_string(u) + "-" + to_string(v)] = weight;
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
vector<double> Graph::longestPath(int s, vector<int> taskMapping) 
{ 
	//cout << "Entered longestPath" << endl;
	stack<int> Stack; 
	//int dist[V];
	vector<double> dist (V); 
	
	// Mark all the vertices as not visited 
	bool* visited = new bool[V]; 
	for (int i = 0; i < V; i++) 
		visited[i] = false; 

	//cout << "Initialized visited vector" << endl;
	// Call the recursive helper function to store Topological 
	// Sort starting from all vertices one by one 
	for (int i = 0; i < V; i++) 
		if (visited[i] == false) 
			topologicalSortUtil(i, visited, Stack); 

	//cout << "Completed topological sort" << endl;
	// Initialize distances to all vertices as infinite and 
	// distance to source as 0 
	for (int i = 0; i < V; i++) 
		dist[i] = NINF;

	dist[s] = nodeWeights[s][taskMapping[s]]; 
	//cout << "finished dist vector" << endl;
	/*
	for(int i=0;i < taskMapping.size(); i++) {
		cout << taskMapping[i] << endl;
	}
	*/
	//cout << "BW Matrix: " << endl;
	/*
	for(int i=0;i < bwMat.size(); i++) {
		for(int j=0; j < bwMat[i].size(); j++) {
			cout << "BW" << i << ":" << j << ": " << bwMat[i][j] << endl;
		}
	}
	*/
	// Process vertices in topological order 
	while (Stack.empty() == false) { 
		// Get the next vertex from topological order 
		int u = Stack.top(); 
		Stack.pop(); 
		//cout << "Popped stack" << endl;
		// Update distances of all adjacent vertices 
		list<AdjListNode>::iterator i; 
		if (dist[u] != NINF) { 
			for (i = adj[u].begin(); i != adj[u].end(); ++i) {
				//If the two tasks are assigned to different processors
				//cout << "distcur" << dist[i->getV()] << endl;
				if(taskMapping[u] == taskMapping[i->getV()]) {
					if (dist[i->getV()] < dist[u] + nodeWeights[i->getV()][taskMapping[i->getV()]] ){ 
						dist[i->getV()] = dist[u] + nodeWeights[i->getV()][taskMapping[i->getV()]]; 
						//cout << "NoProblem" << endl;
					}
				}
				else {
					//cout << "Abov" << endl;
					if (dist[i->getV()] < dist[u] + nodeWeights[i->getV()][taskMapping[i->getV()]] + i->getWeight()/bwMat[taskMapping[u]][taskMapping[i->getV()]]){ 
						dist[i->getV()] = dist[u] + nodeWeights[i->getV()][taskMapping[i->getV()]] + i->getWeight()/bwMat[taskMapping[u]][taskMapping[i->getV()]]; 
					}
				}
				
			}
		} 
	} 
	//cout << "are you out?" << endl;
	// Print the calculated longest distances 
	/*
	for (int i = 0; i < V; i++) 
		(dist[i] == NINF) ? cout << "INF " : cout << dist[i] << " "; 
	*/
	delete [] visited; 

	return dist;
}


double Graph::scheduleLength() {
	events = {};
	for(int i =0; i<V; i++) {
		ScheduleEvent sched = {};
		sched.v = -1;
		sched.start = -1;
		sched.end = -1;
		sched.proc = -1;
		events.push_back(sched);
	}
	proc_sch = {};
	double s_len = 0.0;
	if(proc_sch.empty()) {
		for(int i=0;i<nodeWeights[i].size();i++) {
			proc_sch.push_back({});
		}
	}
	
	//save the ranku indices
	vector<int> indices (V);
	int x = 0;
	std::iota(indices.begin(),indices.end(),x++);
	//rankus[0] = 0.1;
	sort(indices.begin(), indices.end(), [&](int i, int j){return rankus[i]>rankus[j];}); //get the sorted node ids

	for(auto test : indices) {
		//cout << "Node" << test << endl;
		if (events[test].v != -1) {
			continue;
		}
		//cout << "Are u ok?" << endl;
		ScheduleEvent minTaskSchedule = {};
		minTaskSchedule.v = test;
		minTaskSchedule.start = INF;
		minTaskSchedule.end = INF;
		minTaskSchedule.proc = -1;
		//cout << "Init mintaskschedule" << endl;

		//cout << "ProcMap[test]" << proc_map[test] << endl;
		minTaskSchedule = computeEFT(test, proc_map[test]);

		events[test] = minTaskSchedule;
		proc_sch[proc_map[test]].push_back(minTaskSchedule);
		sort(proc_sch[proc_map[test]].begin(),proc_sch[proc_map[test]].end(), [&](ScheduleEvent a, ScheduleEvent b){return a.end < b.end;});
	}

	//cout << events[V-1].end;
	return events[V-1].end;
}

Graph::ScheduleEvent Graph::computeEFT(int node, int proc) {
	//cout << "Entered EFT" << endl;
	double ready_time = 0.0;
	double comm_time = 0.0;
	ScheduleEvent minSchedule = {};
	if(!preds[node].empty()) {
		//cout << "Entered pred loop" << endl;
		for(int pred : preds[node]) {
			double temp_time = 0.0;
			ScheduleEvent pred_event = events[pred];
			if (proc_map[pred] == proc_map[node]) {
				temp_time = pred_event.end;
			}
			else {
				
				for(auto j=adj[pred].begin(); j!=adj[pred].end(); j++) {
					if(j->getV() == node) {
						comm_time = j->getWeight() / bwMat[proc_map[pred]][proc_map[node]];
					}
				}
				temp_time = pred_event.end + comm_time;
			}
			if (temp_time > ready_time) {
				ready_time = temp_time;
			}
		}
	}
	//cout << "Ready Time: " << ready_time << endl;
	vector<ScheduleEvent> ev_list = proc_sch[proc];
	double comp_time = nodeWeights[node][proc];
	bool loop_flag = true;
	for (int i=0;i<ev_list.size();i++) {
		ScheduleEvent prev_ev = ev_list[i];
		if(i==0){
			if(prev_ev.start - comp_time - ready_time > 0.0) {
				double ev_start = ready_time;
				
				minSchedule.v = node;
				minSchedule.start = ev_start;
				minSchedule.end = ev_start + comp_time;
				minSchedule.proc = proc;
				loop_flag = false;
				break; 
			}
		}
		if(i==ev_list.size()-1) {
			double max_temp_t = ready_time;
			if(prev_ev.end > max_temp_t) {
				max_temp_t = prev_ev.end;
			}
			
			minSchedule.v = node;
			minSchedule.start = max_temp_t;
			minSchedule.end = max_temp_t + comp_time;
			minSchedule.proc = proc;
			loop_flag = false;
			break;
		}
		ScheduleEvent next_ev = ev_list[i+1];
		double max_temp_t = ready_time;
		if(prev_ev.end > max_temp_t) {
			max_temp_t = prev_ev.end;
		}
		if(next_ev.start - comp_time - max_temp_t >= 0.0) {
			
			minSchedule.v = node;
			minSchedule.start = max_temp_t;
			minSchedule.end = max_temp_t + comp_time;
			minSchedule.proc = proc;
			loop_flag = false;
			break;
		}
	}
	if(loop_flag) {
		minSchedule.v = node;
		minSchedule.start = ready_time;
		minSchedule.end = ready_time + comp_time;
		minSchedule.proc = proc;
	}
	return minSchedule;
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