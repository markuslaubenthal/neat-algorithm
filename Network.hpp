#include <eigen3/Eigen/Dense>
// #include <eigen3/Eigen/IOFormat>
#include <vector>
#include <fstream>
#include <string>
#include "NetworkState.hpp"
#include "ActivationFunction.hpp"
#include "Layer.hpp"



using Eigen::MatrixXd;
using Eigen::VectorXd;



namespace DNN {
  class Network {
  private:
    std::vector<std::vector<MatrixXd>> weights;
    std::vector<int> layer_dimensions;
    std::vector<std::vector<int>> layer_connections;
    int n_layers;
    NetworkState state;

    std::vector<Layer *> layers;


  public:
    Network(const std::vector<int> layer_dimensions, std::vector<std::vector<int>> layer_connections) :
      layer_dimensions(layer_dimensions),
      layer_connections(layer_connections) {
      n_layers = layer_dimensions.size();

      for(int layer_index = 0; layer_index < n_layers; layer_index++) {
        layers.push_back(new Layer(layer_dimensions[layer_index], layer_index));
      }
      for(int connection_index = 0; connection_index < layer_connections.size(); connection_index++) {
        int lhs_index = layer_connections[connection_index][0];
        int rhs_index = layer_connections[connection_index][1];
        layers[rhs_index]->connectLayer(layers[lhs_index]);
      }
    }

    VectorXd getLayerState(int layer_index) {
      return layers[layer_index]->getState();
    }

    void feedforwardStep(VectorXd input) {

      layers[0]->setState(input);
      for(int layer_index = 0; layer_index < n_layers; layer_index++) {
        layers[layer_index]->step();
      }
      for(int layer_index = 0; layer_index < n_layers; layer_index++) {
        layers[layer_index]->update();
      }
    }

    void printState() {
      for(int layer_index = 0; layer_index < n_layers; layer_index++) {
        std::cout << "Layer " << layer_index << ":" << std::endl <<
          *layers[layer_index] << std::endl;
      }
    }

    void printWeights() {
      for(int layer_index = 0; layer_index < n_layers; layer_index++) {
        std::cout << "Layer " << layer_index << ":" << std::endl;
        std::vector<MatrixXd> weights = layers[layer_index]->getWeights();
        for(int weight_index = 0; weight_index < weights.size(); weight_index++) {
          std::cout << weights[weight_index] << std::endl << std::endl;
        }
      }
    }

    VectorXd getState(int layer_index) {
      return layers[layer_index]->getState();
    }

    // void saveModel(std::string filename) {
    //
    //   std::stringstream ss;
    //
    //   std::string _output;
    //   std::vector<std::string> _layers;
    //   std::vector<std::string> _connections;
    //   std::vector<std::string> _weights;
    //
    //   Eigen::IOFormat JsonFormat(4, 0, ", ", ",", "[", "]");
    //
    //   for(int layer_index = 0; layer_index < n_layers; layer_index++) {
    //     Layer* layer = layers[layer_index];
    //     std::string _layerOutput = '"' + std::to_string(layer->getId()) + '"' + ": " + std::to_string(layer->getSize());
    //     _layers.push_back(_layerOutput);
    //
    //     std::string _connectionOutput;
    //     std::string _weightOutput;
    //     std::vector<Layer*> cLayerConnections = layer->getConnections();
    //     for(int con_index = 0; con_index < cLayerConnections.size(); con_index++) {
    //       _connectionOutput = '[' + std::to_string(cLayerConnections[con_index]->getId())
    //         + ", " + std::to_string(layer->getId()) + ']';
    //       _connections.push_back(_connectionOutput);
    //
    //       MatrixXd *wm = layer->getWeightMatrix(con_index);
    //       ss << wm->format(JsonFormat);
    //       _weightOutput = '[' + ss.str() + ']';
    //       _weights.push_back(_weightOutput);
    //     }
    //   }
    //
    //   _output = "{#}";
    //   std::string _layeroutput = "layers: {#}";
    //   std::string _layeroutput_replace = "";
    //   for(int i = 0; i < _layers.size(); i++) {
    //     if(i != 0) {
    //       _layeroutput_replace += ",";
    //     }
    //     _layeroutput_replace += _layers[i];
    //   }
    //   _layeroutput.replace(9,1, _layeroutput_replace);
    //   std::cout << _layeroutput << std::endl;
    //
    //   std::string _connectionsoutput = "connections: [#]";
    //   std::string _connectionsoutput_replace = "";
    //   for(int i = 0; i < _connections.size(); i++) {
    //     if(i != 0) {
    //       _connectionsoutput_replace += ",";
    //     }
    //     _connectionsoutput_replace += _connections[i];
    //   }
    //   _connectionsoutput.replace(14,1, _connectionsoutput_replace);
    //   std::cout << _connectionsoutput << std::endl;
    //
    //
    //   std::string _connectionsoutput = "connections: [#]";
    //   std::string _connectionsoutput_replace = "";
    //   for(int i = 0; i < _connections.size(); i++) {
    //     if(i != 0) {
    //       _connectionsoutput_replace += ",";
    //     }
    //     _connectionsoutput_replace += _connections[i];
    //   }
    //   _connectionsoutput.replace(14,1, _connectionsoutput_replace);
    //   std::cout << _connectionsoutput << std::endl;
    //
    //
    //   std::ofstream file;
    //   file.open(filename);
    //   file << "h";
    // }

  };

}
