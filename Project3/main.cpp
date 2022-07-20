#include <iostream>
#include <fstream>
#include <forward_list>
#include <deque>
using namespace std;

//limits for isEularian if vertex count is in this range, program uses different algorithm to find is eularian or not.
constexpr unsigned int MAX_LIMIT = 10000, MIN_LIMIT = 9000;

//Grap class for storing data of vertices and applying methods. 
class Graph {
private:
	unsigned int V;												//vertex number
	deque<unsigned int> *out;									//out vertices of a vertex
	size_t *in_degree;											//in edges of vertex
	bool isEulerian() const;									//whether graph is eularian or not. 
public:
	Graph(const unsigned int &V);
	~Graph();
	void addEdge(const unsigned int &v1, const unsigned int &v2);	//adds edge to vertex
	bool hasMoreInsThanOuts(const unsigned int &v_count) const;		// checks in degree and out degree equality for eularian circuit.
	forward_list<unsigned int> getCircuit(unsigned int &v);			// Apllies Hierholzer's Algorithm to circuit.
};


Graph::Graph(const unsigned int &V) : V(V), out(new deque<unsigned int>[V]), in_degree(new size_t[V]{0}) {	}
Graph::~Graph() {
	delete [] out;
	delete [] in_degree;
}
//creates egde between two given vertices as parameters.
void Graph::addEdge(const unsigned int &v1, const unsigned int &v2) {
	out[v1].emplace_back(v2);
	++in_degree[v2];
}
//checks vertexes in degree and out degree. if a vertices out degree is not equal to in degree, graph cannot be eularian.
bool Graph::hasMoreInsThanOuts(const unsigned int &v_count) const {
	for(unsigned int v{v_count-1};v!=WINT_MAX;--v)
		if(out[v].size() < in_degree[v])
			return true;
	return false;
}
//checks all vertices in degree and out degree for eularian circuit.  
bool Graph::isEulerian() const {
	for(unsigned int i{0};i<V;++i)
		if(out[i].size() != in_degree[i])
			return false;
	return true;
}
//Applies hierholzers algorith to graph. uses forward_list for storing queue because it is fast.
//takes the begin vertex as parameter and starts to find paths.
//gets vertices from graph classes out deque.
forward_list<unsigned int> Graph::getCircuit(unsigned int &v) {
	if(!isEulerian()){
		return forward_list<unsigned int>();
	}		
	forward_list<unsigned int> circuit(1, v);
	for(auto circuit_it = circuit.cbegin(); true; v = *circuit_it) {
		deque<unsigned int> tour;
		while(!out[v].empty()) {
			const auto u = out[v].front();
			out[v].pop_front();
			tour.emplace_back(v = u);
		}
		circuit.insert_after(circuit_it, tour.begin(), tour.end());
		for( circuit_it = circuit.begin(); circuit_it!=circuit.end(); circuit_it++){
			if(!out[*circuit_it].empty())
				break;
		}
		if(circuit_it == circuit.cend()){
			return circuit;
		}	
	}
}

int main(int argc, char* argv[]) {

	ifstream input_file(argv[1]);
	unsigned int V, beg_v;										//variables for vertex count and begginnig vertex
	bool has_no_path = false;									//boolean for path exists or not
	input_file >> V;
	Graph graph(V);												//creates a graph with given vertice count


	//if vertice count is in the range checks isEularian while taking input.
	if(MIN_LIMIT<=V && V<=MAX_LIMIT) {
		unsigned int i{0}, v1, edge_count;
		for(;i<MIN_LIMIT;++i) {
			input_file >> v1 >> edge_count;
			for(unsigned int j{0}, v2;j<edge_count;++j) {
				input_file >> v2;
				graph.addEdge(v1, v2);
			}
		}
		while(i<V) {
			input_file >> v1 >> edge_count;
			for(unsigned int j{0}, v2;j<edge_count;++j) {
				input_file >> v2;
				graph.addEdge(v1, v2);
			}
			if(graph.hasMoreInsThanOuts(++i)) {
				has_no_path = true;
				break;
			}
		}
	}

	//if count is not in range simply reads all vertices and then checks for isEularian.
	else
		for(unsigned int i{0}, v1, edge_count;i<V;++i) {
			input_file >> v1 >> edge_count;
			for(unsigned int j{0}, v2;j<edge_count;++j) {
				input_file >> v2;
				graph.addEdge(v1, v2);
			}
		}
	//reads the beginning vertex
	input_file >> beg_v;
	input_file.close();
	
	ofstream output_file(argv[2]);
	//if graph has no path prints "no path"
	if(has_no_path){
		output_file << "no path";
	}				
	//else applies hierholzers algorithm to the graph and returns the circuit.
	//if the circuit is empty prints no path.
	//else prints the circuit.
	else {
		const auto &circuit = graph.getCircuit(beg_v);
		if(circuit.empty()){
			output_file << "no path";
		}		
		
		else{
			for(const auto &v : circuit)	output_file << v << " ";
		}					
	}
	output_file.close();

	return 0;
}
