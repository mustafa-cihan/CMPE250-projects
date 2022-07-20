#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>

/*
*	https://konaeakira.github.io/posts/using-the-shortest-path-faster-algorithm-to-find-negative-cycles.html
*	credit for some algorithms thanks man good job :)
*/

/*Graph class
* V vertex count
* vector for storing vertices. Element of each vertex is its edges and their costs.
* *pre previous vertex.
* getCycle it is the reverse cycle.
* 
* addEdge for adding edge  from vertex u, to vertex v, weight is w.  
* SPFA_early_terminate 
* getting the maximum total weight
*/

class MCMF_Graph {
private:
	int V;
	std::vector<std::pair<int, int>> *adj;
	int *pre;
	std::vector<int> getCycle(const int &trig);
	bool detectCycle(const int &trig);
public:
	MCMF_Graph(int V);
	~MCMF_Graph();
	void addEdge(const int &u, const int &v, const int &w);
	bool SPFA_early_terminate();
	int getTotalWeight() const;
};

// constructor for graph 
MCMF_Graph::MCMF_Graph(int V) : V(V), adj(new std::vector<std::pair<int, int>>[V]), pre(new int[V]) {	}

MCMF_Graph::~MCMF_Graph() {
	delete [] adj;
	delete [] pre;
}
/*
*	addEgde simply adds edge wow
*/
void MCMF_Graph::addEdge(const int &u, const int &v, const int &w) {
	adj[u].emplace_back(v, w);
}

std::vector<int> MCMF_Graph::getCycle(const int &trig) {
	bool visited[V] = {false}, on_stack[V] = {false};
	std::vector<int> neg_cycle_vec;
	for(const auto &vis : visited)
		if(!vis) {
			std::vector<int> vec;
			for(int j{pre[trig]};~j;j = pre[j]) {
				if(visited[j]) {
					if(on_stack[j]) {
						bool flag{true};
						int prev{j};
						do {
							neg_cycle_vec.emplace_back(prev);
							prev = pre[prev];
							flag = std::all_of(neg_cycle_vec.cbegin(), neg_cycle_vec.cend(), [&prev](const int &neg_cycle){ return neg_cycle != prev; });
						} while(flag);
						neg_cycle_vec.emplace_back(j);
						return neg_cycle_vec;
					}
					break;
				}
				visited[j] = on_stack[j] = true;
				vec.emplace_back(j);
			}
			for(const auto &j : vec)
				on_stack[j] = false;
		}
	return neg_cycle_vec;
}

bool MCMF_Graph::detectCycle(const int &trig) {
	const auto &neg_cycle = getCycle(trig);
	if(neg_cycle.empty())
		return false;
	for(auto cyc_it = neg_cycle.crbegin();cyc_it+1 != neg_cycle.crend();++cyc_it)
		for(auto edge_it = adj[*cyc_it].cbegin();edge_it != adj[*cyc_it].cend();++edge_it)
			if(edge_it->first == *(cyc_it+1)) {
				adj[*(cyc_it+1)].emplace_back(*cyc_it, -edge_it->second);
				adj[*cyc_it].erase(edge_it);
				break;
			}
	return true;
}

bool MCMF_Graph::SPFA_early_terminate() {
	int dis[V] = {0};	bool in_queue[V];
	std::fill(pre, pre + V, -1);
	std::fill(in_queue, in_queue + V, true);
	std::queue<int> queue;
	for(int i{0};i<V;queue.push(i++));
	int trig{-1}, iter{0};
	do {
		const auto &u = queue.front();
		queue.pop();
		in_queue[u] = false;
		for(const auto &edge : adj[u]) {
			const auto &v = edge.first, &w = edge.second;
			if(dis[u] + w < dis[v]) {
				pre[v] = u;
				dis[v] = dis[u] + w;
				trig = v;
				if(++iter == V) {
					iter = 0;
					if(detectCycle(trig))
						return true;
				}
				if(!in_queue[v]) {
					queue.push(v);
					in_queue[v] = true;
				}
			}
		}
	} while(!queue.empty());
	return detectCycle(trig);
}

/*
* gets the total weight from cycle.
* only weights bigger than zero
*/
int MCMF_Graph::getTotalWeight() const {
	int tot_w{0};
	for(int i{0};i<V;++i)
		for(const auto &adj_i : adj[i])
			if(adj_i.second > 0)
				tot_w += adj_i.second;
	return tot_w;
}
/*
*	reads the input file 
*	T number of testcases
*	N number of ports
*	cost cost of edges
*
*	it reads every element of input file and initializes them
*	then calls SPFA_early_terminate to find negative cycles.
*	program simply does that until there are no negative cycles
*	if there is no negative cycle, program has best cycle for maximum voltage
*	cost of smallest negative cycle multiplied by mines 1 means that we have largest cost
*	
*/
int main(int argc, char* argv[]) {
	std::ifstream input_file(argv[1]);
	int T, N, cost;
	input_file >> T;
	int Vmax[T];
	for(auto &Vmax_i : Vmax) {
		input_file >> N;
		auto graph = MCMF_Graph(2*N);
		for(int i{0};i<N;++i)
			for(int j{0};j<N;++j) {
				input_file >> cost;
				i == j	?	graph.addEdge(j+N, i, cost)		:	graph.addEdge(i, j+N, -cost);
			}
		while(graph.SPFA_early_terminate());
		Vmax_i = graph.getTotalWeight();
	}
	input_file.close();
	std::ofstream output_file(argv[2]);
	for(const auto &Vmax_i : Vmax)
		output_file << Vmax_i << std::endl;
	output_file.close();
}
