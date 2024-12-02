#ifndef __EVOLUTION_GENES_NODECONTAINER__
#define __EVOLUTION_GENES_NODECONTAINER__

#include <set>
#include <vector>
#include "Node.hpp"

namespace Evolution {
  namespace Genes {
    class NodeContainer {
      std::set<Node> inputNodes;
      std::set<Node> outputNodes;
      std::set<Node> hiddenNodes;
      std::vector<Node> nodes;

    public:
      NodeContainer() {

      }

      void insertInputNode(int nodeId) {
        Node node(nodeId, Evolution::Genes::NodeType::input);
        inputNodes.insert(node);
        nodes.push_back(node);
      }
      void insertOutputNode(int nodeId) {
        Node node(nodeId, Evolution::Genes::NodeType::output);
        outputNodes.insert(node);
        nodes.push_back(node);
      }
      void insertHiddenNode(int nodeId) {
        Node node(nodeId, Evolution::Genes::NodeType::hidden);
        hiddenNodes.insert(node);
        nodes.push_back(node);
      }

      int getRandomNodeId() {
        int r = rand() % nodes.size();
        return nodes[r].nodeId;
      }

      int getRandomNonInputNodeId() {
        int r = rand() % (hiddenNodes.size() + outputNodes.size());
        std::set<Node>* ref;
        if(r < hiddenNodes.size()) {
          ref = &hiddenNodes;
        } else {
          r -= hiddenNodes.size();
          ref = &outputNodes;
        }
        auto it = std::begin(*ref);
        std::advance(it,r);
        return (*it).nodeId;
      }

      std::set<Node> getInputNodes() {
        return inputNodes;
      }

      std::set<Node> getOutputNodes() {
        return outputNodes;
      }

      std::set<Node> getHiddenNodes() {
        return hiddenNodes;
      }

      int inputSize() {
        return inputNodes.size();
      }

      int outputSize() {
        return outputNodes.size();
      }

      int hiddenSize() {
        return hiddenNodes.size();
      }

      int size() {
        return nodes.size();
      }

      Node& operator[](int i) {
        return nodes[i];
      }

      void setHiddenNodes(std::set<Node> hiddenNodes) {
        this->hiddenNodes = hiddenNodes;
      }
      void setOutputNodes(std::set<Node> outputNodes) {
        this->outputNodes = outputNodes;
      }
      void setInputNodes(std::set<Node> inputNodes) {
        this->inputNodes = inputNodes;
      }


    };
  }
}

#endif
