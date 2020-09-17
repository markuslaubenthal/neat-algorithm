#ifndef __DNN_NETWORKFACTORY__
#define __DNN_NETWORKFACTORY__

#include <eigen3/Eigen/Dense>
#include <vector>
#include <map>
#include "SingleLayerNetwork.hpp"
#include "Network.hpp"
#include "NetworkState.hpp"
#include "ActivationFunction.hpp"
#include "Layer.hpp"
#include "../Evolution/Genes/GenoType.hpp"
#include "../Evolution/Genes/GenoConnection.hpp"


using Eigen::MatrixXd;
using Eigen::VectorXd;

namespace DNN {
  class NetworkFactory {
  public:
    static SingleLayerNetwork createSingleLayerQuadratic(Evolution::GenoType genotype) {
      std::vector<Evolution::GenoConnection> connections = genotype.getConnections();
      std::map<int, int> nodeIds;
      int inputNodes = genotype.getSizeOfInputNodes();
      int outputNodes = genotype.getSizeOfOutputNodes();
      int ioNodes = inputNodes + outputNodes;
      int hiddenNodes = genotype.getSizeOfHiddenNodes();

      for(int i = 0; i < ioNodes; i++) {
        nodeIds[i] = i;
      }

      MatrixXd biasWeights = MatrixXd::Zero(ioNodes + hiddenNodes, 1);
      MatrixXd weightMatrix = MatrixXd::Zero(
        ioNodes + hiddenNodes,
        ioNodes + hiddenNodes);

      for(int i = 0; i < connections.size(); i++) {
        if(connections[i].enabled) {
          int inId = connections[i].inId;
          int outId = connections[i].outId;
          if(inId != -1 && !nodeIds.contains(inId)) {
            nodeIds[inId] = nodeIds.size();
          }
          if(!nodeIds.contains(outId)) {
            nodeIds[outId] = nodeIds.size();
          }
          if(inId == -1) {
            biasWeights.block(nodeIds.at(outId), 0, 1, 1) << connections[i].weight;
          } else {
            weightMatrix.block(
              nodeIds.at(outId),
              nodeIds.at(inId),
              1, 1) << connections[i].weight;
          }
        }
      }

      SingleLayerNetwork network(weightMatrix, biasWeights, inputNodes, outputNodes);
      // network.printWeights();
      return network;
    }
  };
}

#endif
