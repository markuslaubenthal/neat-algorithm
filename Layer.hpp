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


    MatrixXd bias_weights;
    VectorXd state;
    VectorXd last_activity;
    int size;
    int t = 0;

    VectorXd temporary_state;


  public:
    // Layer() {}

    Layer(int size, int id) : size(size), id(id) {
      temporary_state = VectorXd(size);
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

    VectorXd getState() const {
      return state;
    }

    void setState(VectorXd new_state) {
      state = new_state;
    }

    void setActivity(VectorXd layer_output) {
      VectorXd new_activity = layer_output.unaryExpr(&thresholdFunction);
      last_activity = last_activity.array() + (new_activity.array() * ((double)t - last_activity.array()));
    }

    void connectLayer(Layer* layer) {
      connecting_layers.push_back(layer);

      // Setup Weight Matrix
      MatrixXd weight_matrix(size, layer->getSize());
      weight_matrix << MatrixXd::Random(size,layer->getSize());
      weight_matrices.push_back(weight_matrix);

      // Setup Usage Matrices
      MatrixXd usage_matrix(size, layer->getSize());
      usage_matrix << MatrixXd::Zero(size,layer->getSize());
      short_term_weight_usage_matrices.push_back(usage_matrix);

      // MatrixXd usage_matrix(size, layer->getSize());
      usage_matrix << MatrixXd::Zero(size,layer->getSize());
      long_term_weight_usage_matrices.push_back(usage_matrix);

      MatrixXd inhibitory = MatrixXd::Constant(size,layer->getSize(), 10);
      inhibitory_receptors.push_back(inhibitory);
      MatrixXd excitatory = MatrixXd::Constant(size,layer->getSize(), 10);
      excitatory_receptors.push_back(inhibitory);

    }

    VectorXd step() {
      t++;
      VectorXd activity(size);

      double dormant_phase = 5;

      activity = (t - last_activity.array() - dormant_phase);
      activity = activity.unaryExpr(&activityFunction);


      if(connecting_layers.size() > 0) {
        temporary_state = bias_weights.array() - 1000 * activity.array();
        for(int layer_index = 0; layer_index < connecting_layers.size(); layer_index++) {
          Layer* layer = connecting_layers[layer_index];
          VectorXd input = layer->getState();
          MatrixXd delta = weight_matrices[layer_index] * layer->getState();
          temporary_state += delta;
        }
        temporary_state = temporary_state.unaryExpr(&thresholdFunction);

        // Calculate weights that contributed to activity of each neuron
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

        // std::cout << short_term_weight_usage_matrices[0] << std::endl;

        setActivity(temporary_state);

        return temporary_state;
      } else {
        return state;
      }
    }

    MatrixXd *getWeightMatrix(int index) {
      return &weight_matrices[index];
    }

    std::vector<Layer*> getConnections() {
      return connecting_layers;
    }

    void update() {
      state = temporary_state;
    }
  };
}

std::ostream& operator<<(std::ostream &strm, const DNN::Layer &layer) {
  return strm << layer.getState().transpose();
}
