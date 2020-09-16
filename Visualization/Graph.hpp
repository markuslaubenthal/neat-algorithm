#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <map>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/unordered_set.hpp>

#include "../Evolution/Genes/GenoType.hpp"
#include "../Evolution/Genes/GenoConnection.hpp"
namespace Visualization {
  class Graph {
  public:
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS> BGraph;
    typedef std::pair<int, int> Edge;

    Graph() {}
    ~Graph() {}

    static void write(BGraph g, std::string filename) {
      std::ofstream file;
      file.open(filename);

      if (file.is_open()) {
      	boost::write_graphviz(file, g);
      }
      file.close();
    }

    static void write(Evolution::GenoType geno, std::string filename) {
      std::vector<Evolution::GenoConnection> connections = geno.getConnections();
      boost::unordered_set<int> set;
      std::vector<Edge> edges;

      std::map<int, int> renamingMap;
      int renameCounter = 0;

      for(int i = 0; i < connections.size(); i++) {
        Evolution::GenoConnection con = connections[i];
        if(con.enabled) {
          if(!renamingMap.contains(con.inId)) {
            renamingMap[con.inId] = renameCounter;
            renameCounter++;
          }
        }
      }

      for(int i = 0; i < connections.size(); i++) {
        Evolution::GenoConnection con = connections[i];
        if(con.enabled) {
          if(!renamingMap.contains(con.outId)) {
            renamingMap[con.outId] = renameCounter;
            renameCounter++;
          }
        }
      }

      for(int i = 0; i < connections.size(); i++) {
        Evolution::GenoConnection con = connections[i];
        if(con.enabled) {
          set.insert(con.inId + 1);
          set.insert(con.outId + 1);
          Edge edge(con.inId + 1, con.outId + 1);
          edges.push_back(edge);
        }
      }

      const int num_vertices = set.size();
      const int num_edges = edges.size();

      BGraph g(num_vertices);

      // add the edges to the graph object
      for (int i = 0; i < num_edges; ++i) {
        add_edge(edges[i].first, edges[i].second, g);
      }

      write(g, filename);
    }
  };

}
