#include <iostream>
#include <eigen3/Eigen/Dense>
#include <cstdlib>
#include <ctime>

#include <chrono>
#include <thread>

#include "Evolution/Algorithm/NeatAlgorithm.hpp"
#include "Evolution/Genes/MarkingHistory.hpp"
#include "Evolution/Genes/GenoType.hpp"
#include "Evaluation/XOREvaluation.hpp"

#include "DNN/Network.hpp"

using Eigen::MatrixXd;
using Eigen::VectorXd;

int main(int argc, char* argv[]) {
  std::srand(time(0));

  for(int k = 0; k < 1; k++) {
    Evolution::Algorithm::Neat::Neat neat(2,1);
    neat.setEvaluationStrategy(new Evaluation::XOREvaluation());
    neat.init(150);
    for(int i = 0; i < 500; i++) {
      // std::cout << "Epoch: " << i << std::endl;
      neat.epoch();
    }
  }

  return 0;
}
