#include <iostream>
#include <vector>
#include <eigen3/Eigen/Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;

namespace DNN {
  class NetworkState {
  private:
    std::vector<VectorXd> layers;
  public:
    NetworkState() {}

    NetworkState(int n_layers, std::vector<int> layer_dimensions) {
      for(int i = 0; i < n_layers; i++) {
        VectorXd vec(layer_dimensions[i]);
        layers.push_back(vec);
      }
    }

    VectorXd getLayerState(int layer) {
      return layers[layer];
    }
    
    void setLayerState(int layer, VectorXd vec) {
      layers[layer] = vec;
    }

    void print() {
      for(int i = 0; i < layers.size(); i++) {
        std::cout << "Layer " << i << ": " << layers[i].transpose() << std::endl;
      }
    }
  };
}
