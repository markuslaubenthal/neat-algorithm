#ifndef __GENOTYPE__
#define __GENOTYPE__

#include <eigen3/Eigen/Dense>
#include <sstream>
#include <vector>

// namespace Evolution {
//   class GenoType;
// }

#include "HistoricalMarking.hpp"
#include "GenoConnection.hpp"
using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::ArrayXd;

namespace Evolution {
  class HistoricalMarking;
  struct GenoNode {
    int id;
    // double bias;

    GenoNode() {}

  };

  class GenoType {
    std::vector<HistoricalMarking> *markingHistory;
    std::vector<GenoNode *> nodes;
    int *innovNo;


    void setInputNodes(int amount) {
      inputNodes = std::vector<GenoNode>();
      for(int i = 0; i < amount; i++) {
        inputNodes.push_back(GenoNode());
      }
      mergeNodes();
    }

    void setOutputNodes(int amount) {
      outputNodes = std::vector<GenoNode>();
      for(int i = 0; i < amount; i++) {
        outputNodes.push_back(GenoNode());
      }
      mergeNodes();
    }

    public:
    std::vector<GenoConnection> connections;
    double fitness = 0;
    std::vector<GenoNode> hiddenNodes;
    std::vector<GenoNode> inputNodes;
    std::vector<GenoNode> outputNodes;
    GenoType(int *innovNo, std::vector<HistoricalMarking> *markingHistory)
      : innovNo(innovNo), markingHistory(markingHistory) {
      nodes = std::vector<GenoNode *>();
      inputNodes = std::vector<GenoNode>();
      outputNodes = std::vector<GenoNode>();
      hiddenNodes = std::vector<GenoNode>();
      connections = std::vector<GenoConnection>();
    }

    GenoType() {

    }

    std::string toString(bool disabled = true) {
      std::string line1 = "Number of Nodes: " + std::to_string(nodes.size());
      std::string s_connections = "";
      for(int i = 0; i < connections.size(); i++) {
        GenoConnection con = connections[i];
        if(con.enabled || !con.enabled && disabled) {
          s_connections += "Index: " + std::to_string(i) + "\n" + con.toString() + "\n--------\n";
        }
      }
      return line1 + "\n" + s_connections;
    }


    void addConnection(int inId, int outId, double weight, bool enabled) {
      GenoConnection con(inId, outId, weight, enabled, *innovNo);
      (*innovNo)++;
      connections.push_back(con);
    }

    void setConnection(int index, bool enabled, double weight) {
      connections[index].enabled = enabled;
      connections[index].weight = weight;
    }

    std::vector<GenoConnection> getConnections() {
      return connections;
    }
    std::vector<GenoNode *> getNodes() {
      return nodes;
    }
    std::vector<GenoNode> getInputNodes() {
      return inputNodes;
    }
    std::vector<GenoNode> getOutputNodes() {
      return outputNodes;
    }

    void mergeNodes() {
      nodes = std::vector<GenoNode *>();
      for(int i = 0; i < inputNodes.size(); i++) {
        nodes.push_back(&inputNodes[i]);
      }
      for(int i = 0; i < outputNodes.size(); i++) {
        nodes.push_back(&outputNodes[i]);
      }
      for(int i = 0; i < hiddenNodes.size(); i++) {
        nodes.push_back(&hiddenNodes[i]);
      }
    }

    void setInputAndOutputNodes(int input, int output) {
      setInputNodes(input);
      setOutputNodes(output);
      for(int i = 0; i < output; i++) {
        addConnection(-1, inputNodes.size() + i, randomWeight(), true);
      }
      for(int i = 0; i < input; i++) {
        for(int o = 0; o < output; o++) {
          addConnection(i, inputNodes.size() + o, randomWeight(), true);
        }
      }
    }

    bool connectionExists(int inId, int outId) {
      for(int i = 0; i < connections.size(); i++) {
        if(connections[i].inId == inId && connections[i].outId == outId) return true;
      }
      return false;
    }

    void createHiddenNode() {
      GenoNode node = GenoNode();
      hiddenNodes.push_back(node);
      mergeNodes();
      addConnection(-1, nodes.size() - 1, randomWeight(), true);
    }

    void sortConnections() {
      std::sort(connections.rbegin(), connections.rend(),
        [](auto const &a, auto const &b) { return a.innovNo < b.innovNo; });
    }

    void addNodeAtConnection(int connectionId);

    void addNodeAtConnection(int connectionId, HistoricalMarking *hm);

    void toggleConnection(int inId, int outId) {
      for(int i = 0; i < connections.size(); i++) {
        if(connections[i].inId == inId && connections[i].outId == outId) {
          connections[i].enabled = !connections[i].enabled;
          return;
        }
      }
    }

    void mutateConnection() {
      // Add Random connection
      int numberNonMutatableNodes = inputNodes.size() + outputNodes.size();
      int inId = rand() % (nodes.size());
      int outId = rand() % (nodes.size());
      bool conExists = connectionExists(inId, outId);

      if(outId >= inputNodes.size() && !conExists) {
        addConnection(inId, outId, randomWeight(), true);
      } else if(conExists) {
        toggleConnection(inId, outId);
      }
    }

    double randomWeight() {
      return ((double)rand() / RAND_MAX - 0.5) * 5;
    }

    bool historicalNodeMarkingExists(int conId);

    HistoricalMarking *getHistoricalNodeMarking(int conId);

    HistoricalMarking *getHistoricalMarking(int conId);

    void mutateAddNode() {
      if(connections.size() == 0) return;
      int conId = rand() % connections.size();
      if(connections[conId].enabled) {

        // TODO: If HistoricalMarking exists, create connection and rename Ids
        // Renaming causes Problems with array mapping. create hashmap which holds
        // Further Mapping for array <-> Id
        if(historicalNodeMarkingExists(conId)) {
          addNodeAtConnection(conId, getHistoricalNodeMarking(conId));
        } else {
          addNodeAtConnection(conId);
        }

      }
    }

    void mutateWeight() {

      double p = (double)rand() / RAND_MAX;

      if(connections.size() == 0) return;
      int conId = rand() % (connections.size());

      if(connections[conId].enabled) {
        if(p < 0.1) {
          connections[conId].weight = randomWeight();
        } else {
          connections[conId].weight *= (((double)rand() / RAND_MAX)) * 2;
        }
      }
    }

    void mutate() {
      double p;
      p = (double)rand() / RAND_MAX;
      if(p < 0.03) {
        mutateAddNode();
      }
      p = (double)rand() / RAND_MAX;
      if(p < 0.05) {
        mutateConnection();
      }
      p = (double)rand() / RAND_MAX;
      if(p < 0.8) {
        mutateWeight();
      }
    }

    std::vector<int> getOutputIdRange() {
      std::vector<int> idRange;
      idRange.push_back(inputNodes.size());
      idRange.push_back(inputNodes.size() + outputNodes.size());
      return idRange;
    }
  };
}




#endif
