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
  std::cout << "Initializing NEAT" << std::endl;
  std::cout << "1" << std::endl;
  Evolution::Algorithm::Neat::Neat neat(2,1);
  std::cout << "2" << std::endl;
  ProblemSets::XOR problem;
  std::cout << "3" << std::endl;
  neat.setProblem(problem);

  neat.fit();

  return 0;
}
