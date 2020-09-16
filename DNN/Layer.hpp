#ifndef __DNN_LAYER__
#define __DNN_LAYER__

#include <eigen3/Eigen/Dense>
#include "ActivationFunction.hpp"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::ArrayXd;

namespace DNN {
  class Layer {
  private:
    int id;
    std::vector<Layer*> connecting_layers;
    std::vector<MatrixXd> weight_matrices;
    std::vector<MatrixXd> short_term_weight_usage_matrices;
    std::vector<MatrixXd> long_term_weight_usage_matrices;

    std::vector<MatrixXd> excitatory_receptors;
    std::vector<MatrixXd> inhibitory_receptors;

    std::vector<MatrixXd> excitatory_transmitters;
    std::vector<MatrixXd> inhibitory_transmitters;

    double voltage_inhibitory = -0.1;
    double voltage_excitatory = 0.1;
    double dormant_phase = 0;


    MatrixXd bias_weights;
    VectorXd state;
    VectorXd last_activity;
    int size;
    int t = 0;

    VectorXd temporary_state;


  public:
    // Layer() {}

    Layer(int size, int id) : size(size), id(id) {
      temporary_state = VectorXd::Zero(size);
      state = VectorXd::Zero(size);
      bias_weights = MatrixXd::Constant(size, 1, -0.1);
      last_activity = VectorXd::Constant(size, -10);
    }

    int getSize() {
      return size;
    }

    int getId() {
      return id;
    }

    std::vector<MatrixXd> getWeights() {
      return weight_matrices;
    }

    void setBias(double weight) {
      bias_weights = MatrixXd::Constant(size, 1, weight);
    }

    VectorXd getState() const {
      return state;
    }

    void setState(VectorXd new_state) {
      state = new_state;
    }

    void setActivityVector(VectorXd layer_output) {
      VectorXd new_activity = layer_output.unaryExpr(&thresholdFunction);
      last_activity = last_activity.array() + (new_activity.array() * ((double)t - last_activity.array()));
    }

    void connectLayer(Layer* layer) {

      // Setup Weight Matrix
      MatrixXd weight_matrix(size, layer->getSize());
      weight_matrix << MatrixXd::Random(size,layer->getSize());
      connectLayer(layer, weight_matrix);
    }

    void connectLayer(Layer* layer, MatrixXd weight_matrix) {
      connecting_layers.push_back(layer);
      weight_matrices.push_back(weight_matrix);
      setupUsageMatrices(layer);
      // setupNeuroTransmitterMatrices(layer);
    }

    void setupUsageMatrices(Layer* layer) {
      MatrixXd usage_matrix(size, layer->getSize());

      usage_matrix << MatrixXd::Zero(size,layer->getSize());
      short_term_weight_usage_matrices.push_back(usage_matrix);

      usage_matrix << MatrixXd::Zero(size,layer->getSize());
      long_term_weight_usage_matrices.push_back(usage_matrix);
    }

    void setupNeuroTransmitterMatrices(Layer* layer) {
      MatrixXd inhibitory = MatrixXd::Random(size,layer->getSize()) * 10;
      inhibitory_receptors.push_back(inhibitory);

      MatrixXd excitatory = MatrixXd::Random(size,layer->getSize()) * 10;
      excitatory_receptors.push_back(excitatory);

      inhibitory = MatrixXd::Random(size,layer->getSize()) * 10;
      inhibitory_transmitters.push_back(inhibitory);

      excitatory = MatrixXd::Random(size,layer->getSize()) * 10;
      excitatory_transmitters.push_back(excitatory);
    }

    VectorXd calculateActivityMatrix() {
      VectorXd activity(size);
      activity = (t - last_activity.array() - dormant_phase);
      activity = activity.unaryExpr(&activityFunction);
      return activity;
    }

    VectorXd calculateLayerOutput(VectorXd activityVector) {
      temporary_state = bias_weights.array() - 1000 * activityVector.array();
      // if(getId() == 6) {
      //   std::cout << "-- Temp Bias --" << std::endl;
      //   std::cout << temporary_state << std::endl;
      //   std::cout << "Layer Count: " << connecting_layers.size() << std::endl;
      //   std::cout << "-- Temp Bias END --" << std::endl;
      // }



      for(int layer_index = 0; layer_index < connecting_layers.size(); layer_index++) {
        MatrixXd *weight_matrix = getWeightMatrix(layer_index);

        Layer* layer = connecting_layers[layer_index];
        VectorXd input = layer->getState();

        MatrixXd delta = *weight_matrix * layer->getState();
        // if(getId() == 6) {
        //   std::cout << "-- Temp Delta --" << std::endl;
        //   std::cout << layer->getState() << std::endl;
        //   std::cout << "-- Temp Delta END --" << std::endl;
        // }
        temporary_state += delta;
      }
      // if(getId() == 6) {
      //   std::cout << "-- Temp Output --" << std::endl;
      //   std::cout << temporary_state << std::endl;
      //   std::cout << "-- Temp Output END --" << std::endl;
      // }
      // temporary_state = temporary_state.unaryExpr(&thresholdFunction);
      temporary_state = temporary_state.unaryExpr(&logisticFunction);
      return temporary_state;
    }

    // Calculate weights that contributed to activity of each neuron
    void calculateUsageMatrices() {
      for(int layer_index = 0; layer_index < connecting_layers.size(); layer_index++) {
        Layer* layer = connecting_layers[layer_index];

        // Short term Memory, has value in (0, 10], goes through shifted
        // logistic function from -2 to 8 probably
        // TODO: Drop rate to be determined
        // on successfull usage of a neuron, its matrix value will be set to 10
        // and drop from there one
        MatrixXd* usageMatrix = &short_term_weight_usage_matrices[layer_index];
        MatrixXd tmpUsage = temporary_state * layer->getState().transpose();
        *usageMatrix = 0.95 * usageMatrix->cwiseMax(10 * tmpUsage);

        // Long term Memory
        // High value means high frequency
        // Threshold to be identified
        usageMatrix = &long_term_weight_usage_matrices[layer_index];
        *usageMatrix = 0.99 * (*usageMatrix + tmpUsage);
      }
    }

    VectorXd step() {
      t++;
      VectorXd activityVector(size);
      activityVector = calculateActivityMatrix();

      if(connecting_layers.size() > 0) {
        calculateLayerOutput(activityVector);
        calculateUsageMatrices();
        setActivityVector(temporary_state);
        return temporary_state;
      } else {
        return state;
      }
    }


    MatrixXd *calculateWeightMatrix(int index) {
      weight_matrices[index] =
          excitatory_transmitters[index].cwiseProduct(excitatory_receptors[index]) * voltage_excitatory
          + inhibitory_transmitters[index].cwiseProduct(inhibitory_receptors[index]) * voltage_inhibitory;

      return &weight_matrices[index];
    }

    MatrixXd *getWeightMatrix(int index) {
      return &weight_matrices[index];
    }

    std::vector<Layer*> getConnections() {
      return connecting_layers;
    }

    // TODO: fix parameters. cwiseMax(20) is a random guess.
    void addExcitatoryTransmitters() {
      for(int layer_index = 0; layer_index < excitatory_transmitters.size(); layer_index++) {
        MatrixXd *transmitter = &excitatory_transmitters[layer_index];
        *transmitter += 0.1 * short_term_weight_usage_matrices[layer_index];
        *transmitter = transmitter->cwiseMax(20);
      }
    }

    void addInhibitoryTransmitters() {
      for(int layer_index = 0; layer_index < excitatory_transmitters.size(); layer_index++) {
        MatrixXd *transmitter = &inhibitory_transmitters[layer_index];
        *transmitter += 0.1 * short_term_weight_usage_matrices[layer_index];
        *transmitter = transmitter->cwiseMax(20);
      }
    }

    void update() {
      state = temporary_state;
    }
  };
}

std::ostream& operator<<(std::ostream &strm, const DNN::Layer &layer) {
  return strm << layer.getState().transpose();
}

#endif
