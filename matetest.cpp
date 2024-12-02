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
  Evolution::MarkingHistory *hist = new Evolution::MarkingHistory();
  Evolution::GenoType a(&innovNo, hist);
  a.initNodes(2,1);
  std::vector<Evolution::GenoConnection> connections;
  connections.push_back(Evolution::GenoConnection(, 2, 1, true, 1));
  connections.push_back(Evolution::GenoConnection(-1, 2, 1, true, 1));
  connections.push_back(Evolution::GenoConnection(-1, 2, 1, true, 1));

  Evolution::GenoType b(&a);
  a.connections =
  Species s;
  )

  // DNN::Network network(g);
  // std::vector<DNN::Layer *> layers = network.getLayers();

  return 0;
}
