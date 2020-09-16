#include <iostream>
#include <eigen3/Eigen/Dense>
#include <cstdlib>
#include <ctime>

#include <chrono>
#include <thread>

#include "Evolution/Genes/GenoType.hpp"
#include "Visualization/Graph.hpp"
#include "Evolution/Genes/MarkingHistory.hpp"
#include "Evaluation/XOREvaluation.hpp"
#include "DNN/Network.hpp"
#include "DNN/Layer.hpp"

using Eigen::MatrixXd;
using Eigen::VectorXd;

int main(int argc, char* argv[]) {
  // std::srand(1);
  std::srand(time(0));

  int innovNo = 0;

  Evolution::GenoType g(&innovNo, new Evolution::MarkingHistory());

  g.setInputAndOutputNodes(2,1);
  g.addNodeAtConnection(1);
  g.addConnection(1,6, 10, true);

  g.setConnection(-1, 2, true, -0.5 * 100.0);
  g.setConnection(0, 2, true, 1 * 100.0);
  g.setConnection(1, 2, true, 1 * 100.0);
  g.setConnection(6, 2, true, -2 * 100.0);

  g.setConnection(-1, 6, true, -1.5 * 100.0);
  g.setConnection(0, 6, true, 1 * 100.0);
  g.setConnection(1, 6, true, 1 * 100.0);
  Visualization::Graph::write(g, "xor5.graph");


  Evaluation::XOREvaluation *_xor = new Evaluation::XOREvaluation();

  DNN::Network network(g);
  // network.printWeights();
  // return 0;
  std::vector<DNN::Layer *> layers = network.getLayers();
  // std::cout << "Printing Layer IDs: " << std::endl;
  // for(int i = 0; i < layers.size(); i++) {
  //   std::cout << layers[i]->getId() << std::endl;
  // }

  for(int i = 0; i < 1; i++) {
    // g.randomizeWeights();
    double fitness = _xor->evaluate(&g);

    // std::cout << "---" << std::endl;
    std::cout << fitness << std::endl;

  }




  //
  // g.addConnection(-1, 3, -1.5, true);
  // g.addConnection(-1, 2, -0.5, true);
  // g.addConnection(1, 3, 1, true);
  // g.addConnection(3, 2, -2, true);
  //
  // g.setConnection(0, false, 0);
  // g.setConnection(2, false, 0);
  // g.setConnection(3, false, 0);
  // g.setConnection(4, false, 0);



  // std::cout << "init" << std::endl;
  // std::cout << g.toString(false) << std::endl;




  // DNN::Network network(g);

  // VectorXd input(2);
  // input << 1,1;
  // std::cout << "start" << std::endl;
  // network.feedforwardStep(input);
  // std::cout << "step2" << std::endl;
  // network.feedforwardStep(input);
  //
  // std::cout << "print" << std::endl;
  // network.printState();

  return 0;
}
