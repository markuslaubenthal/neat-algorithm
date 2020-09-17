#ifndef __DNN_SINGLELAYERNETWORK__
#define __DNN_SINGLELAYERNETWORK__

#include <vector>
#include <fstream>
#include <string>
#include <map>
#include "Network.hpp"

using Eigen::MatrixXd;
using Eigen::VectorXd;

namespace DNN {
  class SingleLayerNetwork: public Network {
    int noOfInputNodes = 0;
    int noOfOutputNodes = 0;
  public:
    SingleLayerNetwork() {}
    SingleLayerNetwork(MatrixXd weightMatrix,
        MatrixXd bias,
        int inputNodes,
        int outputNodes) : noOfInputNodes(inputNodes), noOfOutputNodes(outputNodes) {

      if(weightMatrix.rows() == weightMatrix.cols() &&
        bias.rows() == weightMatrix.rows()) {
        Layer* layer = new Layer(weightMatrix.rows(), 0);
        layer->setBias(bias);
        layer->connectLayer(layer, weightMatrix);
        layers.push_back(layer);
      } else {
        throw "Matrix not quadratic!";
      }
    }

    void setInputNodes(VectorXd input) override {
      Layer* layer = layers[0];
      VectorXd state = layer->getState();
      state.head(noOfInputNodes) = input;
      layer->setState(state);
    }
  };
}

#endif
