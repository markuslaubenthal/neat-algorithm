#ifndef __EVALUATION_XOR__
#define __EVALUATION_XOR__

#include <vector>
#include <eigen3/Eigen/Dense>
#include "../Evolution/Genes/GenoType.hpp"
#include "EvaluationStrategy.hpp"
#include "../DNN/Network.hpp"
#include "../DNN/SingleLayerNetwork.hpp"
#include "../DNN/NetworkFactory.hpp"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::ArrayXd;


namespace Evaluation {
  class XOREvaluation : public Evaluation::EvaluationStrategy {

    std::vector<VectorXd> inputs;
    std::vector<VectorXd> outputs;

    public:
      XOREvaluation() {
        VectorXd input = VectorXd(2);
        input << 0,0;
        inputs.push_back(input);
        input << 1,1;
        inputs.push_back(input);
        input << 0,1;
        inputs.push_back(input);
        input << 1,0;
        inputs.push_back(input);

        VectorXd output = VectorXd(1);
        output << 0;
        outputs.push_back(output);
        outputs.push_back(output);
        output << 1;
        outputs.push_back(output);
        outputs.push_back(output);
      }

      VectorXd getInput(int t) const {
        return inputs[t%4];
      }

      VectorXd getOutput(int t) const {
        return outputs[t%4];
      }

      double evaluate(Evolution::GenoType *genotype) const override {
        double error = 4;
        for(int t = 0; t < 4; t++) {
          // std::cout << "INPUT: " << std::endl;
          // std::cout << getInput(t).transpose() << std::endl << std::endl;
          // DNN::Network network(*genotype);
          DNN::SingleLayerNetwork network =
            DNN::NetworkFactory::createSingleLayerQuadratic(*genotype);

          VectorXd input = getInput(t);
          VectorXd expectedOutput = getOutput(t);
          std::vector<int> outputIdRange =
            genotype->getOutputIdRange();

          for(int i = 0; i < 8; i++) {
            network.feedforwardStep(input);

            // network.printState();
            if(i > 6) {
              int outputNeuron = outputIdRange[0];
              // std::cout << "Output: " << network.getLayerStateById(0).coeff(outputNeuron, 0) << std::endl;
              double difference = std::abs(
                network.getLayerStateById(0).coeff(outputNeuron, 0) - expectedOutput[0]);
              // double difference = std::abs(network.getLayerStateById(outputNeuron)(0) - expectedOutput[0]);
              error -= difference;
            }
          }
          // std::cout << "-----" << std::endl;
        }
        double fitness = error * error;
        return fitness;
      }

      void evaluatePrint(Evolution::GenoType *genotype) const override {
        for(int t = 0; t < 4; t++) {
          DNN::Network network(*genotype);
          VectorXd input = getInput(t);
          VectorXd expectedOutput = getOutput(t);
          std::vector<int> outputIdRange =
            genotype->getOutputIdRange();

          for(int i = 0; i < 2; i++) {
            network.feedforwardStep(input);

            network.printState();
            std::cout << "---" << std::endl;

            // if(i > 0) {
            //   int outputNeuron = outputIdRange[0];
            //   std::cout
            //     << input.transpose()
            //     << " : " << network.getLayerStateById(outputNeuron)(0)
            //     << std::endl;
            //   double difference = std::abs(network.getLayerStateById(outputNeuron)(0) - expectedOutput[0]);
            // }
          }
          std::cout << "--------------------------------" << std::endl;
        }
      }
  };
}

#endif
