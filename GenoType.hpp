#ifndef __GENOTYPE__
#define __GENOTYPE__

#include <eigen3/Eigen/Dense>
#include <sstream>

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::ArrayXd;

namespace Evolution {
  struct GenoNode {
    double bias;
  };

  struct GenoConnection {
    int inId;
    int outId;
    double weight;
    bool enabled;
    int innovNo;
    GenoConnection(int inId, int outId, double weight, bool enabled, int innovNo) :
      inId(inId), outId(outId), weight(weight), enabled(enabled), innovNo(innovNo) {
    }

    std::string toString() {
      std::stringstream ss;
      return "In:  " + std::to_string(inId) + "\n" + "Out: " + std::to_string(outId) + "\n" + "w:   " + std::to_string(weight);
    }

  };

  class GenoType {
    std::vector<GenoNode> nodes;
    int *innovNo;

    void setInputNodes(int amount) {
      inputNodes = std::vector<GenoNode>(amount);
      mergeNodes();
    }

    void setOutputNodes(int amount) {
      outputNodes = std::vector<GenoNode>(amount);
      mergeNodes();
    }

    public:
      std::vector<GenoConnection> connections;
      double fitness = 0;
      std::vector<GenoNode> hiddenNodes;
      std::vector<GenoNode> inputNodes;
      std::vector<GenoNode> outputNodes;
    GenoType(int *innovNo) : innovNo(innovNo) {
      nodes = std::vector<GenoNode>();
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
    std::vector<GenoNode> getNodes() {
      return nodes;
    }
    std::vector<GenoNode> getInputNodes() {
      return inputNodes;
    }
    std::vector<GenoNode> getOutputNodes() {
      return outputNodes;
    }

    void mergeNodes() {
      nodes = std::vector<GenoNode>(inputNodes);
      nodes.insert(nodes.end(), outputNodes.begin(), outputNodes.end());
      nodes.insert(nodes.end(), hiddenNodes.begin(), hiddenNodes.end());
    }

    void setInputAndOutputNodes(int input, int output) {
      setInputNodes(input);
      setOutputNodes(output);
      for(int i = 0; i < output; i++) {
        addConnection(-1, inputNodes.size() + i, randomWeight(), true);
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

    void addNodeAtConnection(int connectionId) {
      if(connections[connectionId].inId != -1) {
        createHiddenNode();
        int newNodeId = nodes.size() - 1;
        connections[connectionId].enabled = false;
        int inId = connections[connectionId].inId;
        int outId = connections[connectionId].outId;
        addConnection(inId, newNodeId, 1, true);
        addConnection(newNodeId, outId, connections[connectionId].weight, true);
      }
    }

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
      if(inId != outId) {

        if(outId >= inputNodes.size() && !conExists) {
          addConnection(inId, outId, randomWeight(), true);
        } else if(conExists) {
          toggleConnection(inId, outId);
        }
      }
    }

    double randomWeight() {
      return ((double)rand() / RAND_MAX - 0.5) * 10;
    }

    void mutateAddNode() {
      if(connections.size() == 0) return;
      int conId = rand() % connections.size();
      if(connections[conId].enabled) {
        addNodeAtConnection(conId);
      }
    }

    void mutateWeight() {
      if(connections.size() == 0) return;
      int conId = rand() % (connections.size());

      if(connections[conId].enabled) {
        connections[conId].weight = randomWeight();
      }
    }

    void mutate() {
      double p = (double)rand() / RAND_MAX;
      if(p > 0.95) {
        mutateAddNode();
      } else if (p > 0.45) {
        mutateConnection();
      } else {
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
