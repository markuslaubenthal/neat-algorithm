#ifndef __DNN_NETWORK__
#define __DNN_NETWORK__

#include <eigen3/Eigen/Dense>
// #include <eigen3/Eigen/IOFormat>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include "NetworkState.hpp"
#include "ActivationFunction.hpp"
#include "Layer.hpp"
#include "../Evolution/Genes/GenoType.hpp"
#include "../Evolution/Genes/GenoConnection.hpp"


using Eigen::MatrixXd;
using Eigen::VectorXd;



namespace DNN {
  class Network {
  protected:
    // std::vector<int> layer_dimensions;
    std::vector<std::vector<int>> layer_connections;
    NetworkState state;

    std::vector<Layer *> layers;

    Network() {}
  public:
    Network(const std::vector<int> layer_dimensions, std::vector<std::vector<int>> layer_connections) :
      // layer_dimensions(layer_dimensions),
      layer_connections(layer_connections) {
        int n_layers = layer_dimensions.size();
        for(int layer_index = 0; layer_index < n_layers; layer_index++) {
          layers.push_back(new Layer(layer_dimensions[layer_index], layer_index));
        }
        for(int connection_index = 0; connection_index < layer_connections.size(); connection_index++) {
          int lhs_index = layer_connections[connection_index][0];
          int rhs_index = layer_connections[connection_index][1];
          layers[rhs_index]->connectLayer(layers[lhs_index]);
        }
    }

    Network(Evolution::GenoType genotype) {
      std::vector<Evolution::GenoConnection> connections = genotype.getConnections();
      // std::vector<Evolution::GenoNode *> nodes = genotype.getNodes();
      // std::vector<Evolution::GenoNode> inputNodes = genotype.getInputNodes();
      // std::vector<Evolution::GenoNode> outputNodes = genotype.getOutputNodes();

      // int sizeOfInputNodes = genotype.getSizeOfInputNodes();
      // int sizeOfOutputNodes = genotype.getSizeOfOutputNodes();
      // int sizeOfHiddenNodes = genotype.getSizeOfHiddenNodes();
      //
      // int totalNodes = sizeOfInputNodes + sizeOfOutputNodes + sizeOfHiddenNodes;

      // for(int i = 0; i < nodes.size(); i++) {
      //   layer_dimensions.push_back(1);
      //   layers.push_back(new Layer(1, i));
      // }

      std::map<int, int> nodeIds;

      for(int i = 0; i < connections.size(); i++) {

        createLayersIfNotExist(connections[i], &nodeIds, i);
        if(connections[i].enabled) {
          int outId = nodeIds[connections[i].outId];
          double weight = connections[i].weight;
          if(connections[i].inId == -1) {
            layers[outId]->setBias(weight);
          } else {
            int inId = nodeIds[connections[i].inId];
            std::vector<int> connection = {inId, outId};
            layer_connections.push_back(connection);
            MatrixXd weight_matrix = MatrixXd::Constant(1,1, weight);
            layers[outId]->connectLayer(layers[inId], weight_matrix);
          }
        }
      }

    }

    ~Network() {
      for (auto p : layers) {
        delete p;
      }
      layers.clear();
    }

    // void generateWeightMatrix() {
    //   int numberOfNeurons = 0;
    //   for(int i = 0; i < layers.size(); i++) {
    //     numberOfNeurons += layers[i]->getSize();
    //   }
    //   MatrixXd weightMatrix = MatrixXd::Zero(numberOfNeurons, numberOfNeurons);
    //   int offset = 0;
    //   for(int i = 0; i < layers.size(); i++) {
    //
    //     std::vector<MatrixXd> weight_matrices = layers[i].getWeights();
    //     int matrixSize = 0;
    //     for(int wIndex = 0; wIndex < weight_matrices.size(); wIndex++) {
    //       matrixSize += weight_matrices[wIndex].rows();
    //     }
    //     int layerSize = layers[i]->getSize();
    //     weightMatrix.block<layerSize,layerSize>(offset, offset) =
    //       layers[i]->
    //
    //   }
    // }

    int getNoOfLayers() {
      return layers.size();
    }

    void createLayersIfNotExist(Evolution::GenoConnection con,
      std::map<int, int> *nodeIds, int i) {

      // If input is not Bias Neuron and it does not exist -> Add new Layer
      if(con.inId != -1 && nodeIds->count(con.inId) == 0) {
        (*nodeIds)[con.inId] = nodeIds->size();
        // layer_dimensions.push_back(1);

        //maybe wrong, maybe i need inId/outId respectively as Layer ID
        layers.push_back(new Layer(1, con.inId));
      }

      // If output Neuron does not exist -> Add new Layer
      if(nodeIds->count(con.outId) == 0) {
        (*nodeIds)[con.outId] = nodeIds->size();
        // layer_dimensions.push_back(1);
        layers.push_back(new Layer(1, con.outId));
      }
    }

    std::vector<Layer *> getLayers() {
      return layers;
    }

    VectorXd getLayerState(int layer_index) {
      return layers[layer_index]->getState();
    }

    VectorXd getLayerStateById(int layer_id) {
      for(int layer_index = 0; layer_index < layers.size(); layer_index++) {
        if(layers[layer_index]->getId() == layer_id)
          return layers[layer_index]->getState();
      }
      return layers[100]->getState();
    }

    Layer* getLayerById(int id) {
      for(int i = 0; i < layers.size(); i++) {
        if(layers[i]->getId() == id) return layers[i];
      }
      return NULL;
    }

    virtual void setInputNodes(VectorXd input) {
      if(layers[0]->getSize() == input.size()) {
        layers[0]->setState(input);
      } else {
        for(int i = 0; i < input.size(); i++) {
          Layer* t_layer = getLayerById(i);
          t_layer->setState(input.segment(i,1));
        }
      }
    }

    void feedforwardStep(VectorXd input) {
      setInputNodes(input);
      for(int layer_index = 0; layer_index < layers.size(); layer_index++) {
        layers[layer_index]->step();
      }
      for(int layer_index = 0; layer_index < layers.size(); layer_index++) {
        layers[layer_index]->update();
      }
    }

    void printState() {
      for(int layer_index = 0; layer_index < layers.size(); layer_index++) {
        std::cout << "Layer " << layers[layer_index]->getId() << ":" << std::endl <<
          *layers[layer_index] << std::endl;
      }
    }

    void printWeights() {
      for(int layer_index = 0; layer_index < layers.size(); layer_index++) {
        std::cout << "Layer " << layers[layer_index]->getId() << ":" << std::endl;
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

#endif
