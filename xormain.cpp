#include <iostream>
#include <eigen3/Eigen/Dense>
#include <cstdlib>
#include <ctime>

#include <chrono>
#include <thread>

// #include "NeatAlgorithm.hpp"
#include "ProblemSets/XOR.hpp"
#include "GenoType.hpp"
#include "Network.hpp"

using Eigen::MatrixXd;
using Eigen::VectorXd;

int main(int argc, char* argv[]) {
  std::srand(1);

  int innovNo = 0;
  Evolution::GenoType g(&innovNo);
  g.setInputAndOutputNodes(2,1);
  g.addConnection(0,2,0,true);
  g.addNodeAtConnection(1);
  g.addConnection(0,3, 1, true);
  g.addConnection(0,2, 1, true);
  g.addConnection(1,2, 1, true);

  g.addConnection(-1, 3, -1.5, true);
  g.addConnection(-1, 2, -0.5, true);
  g.addConnection(1, 3, 1, true);
  g.addConnection(3, 2, -2, true);

  g.setConnection(0, false, 0);
  g.setConnection(2, false, 0);
  g.setConnection(3, false, 0);
  g.setConnection(4, false, 0);

  std::cout << "init" << std::endl;
  std::cout << g.toString(false) << std::endl;
  DNN::Network network(g);

  VectorXd input(2);
  input << 1,1;
  std::cout << "start" << std::endl;
  network.feedforwardStep(input);
  std::cout << "step2" << std::endl;
  network.feedforwardStep(input);

  std::cout << "print" << std::endl;
  network.printState();

  return 0;
}
