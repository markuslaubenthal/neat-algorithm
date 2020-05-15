#include <iostream>
#include <eigen3/Eigen/Dense>
#include <cstdlib>
#include <ctime>

#include <chrono>
#include <thread>

#include "NeatAlgorithm.hpp"
#include "ProblemSets/XOR.hpp"

using Eigen::MatrixXd;
using Eigen::VectorXd;

int main(int argc, char* argv[]) {
  std::srand(time(0));
  Evolution::Algorithm::Neat::Neat neat(2,1);
  ProblemSets::XOR problem;
  neat.setProblem(problem);

  neat.fit();

  return 0;
}
