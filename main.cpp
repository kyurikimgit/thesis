#include <boost/graph/adjacency_list.hpp>
#include <iostream>

int main() {
    using namespace boost;
    typedef adjacency_list<vecS, vecS, undirectedS> Graph;
    Graph g(5);

    add_edge(0, 1, g);
    add_edge(1, 2, g);
    add_edge(2, 3, g);

    std::cout << "Number of vertices: " << num_vertices(g) << std::endl;
    std::cout << "Number of edges: " << num_edges(g) << std::endl;

    return 0;
}
