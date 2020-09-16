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
  Evolution::GenoType g(&innovNo, hist);
  g.setInputAndOutputNodes(2,1);

  Visualization::Graph::write(g, "xor01.graph");
  g.mutateAddNode();
  hist->clear();
  Visualization::Graph::write(g, "xor02.graph");
  g.mutateAddNode();
  hist->clear();
  Visualization::Graph::write(g, "xor03.graph");
  g.mutateAddNode();
  hist->clear();
  Visualization::Graph::write(g, "xor04.graph");
  g.mutateAddNode();
  hist->clear();
  Visualization::Graph::write(g, "xor05.graph");

  g.mutateConnection();
  Visualization::Graph::write(g, "xor06.graph");
  g.mutateConnection();
  Visualization::Graph::write(g, "xor07.graph");
  g.mutateConnection();
  Visualization::Graph::write(g, "xor08.graph");
  g.mutateConnection();
  Visualization::Graph::write(g, "xor09.graph");


  // DNN::Network network(g);
  // std::vector<DNN::Layer *> layers = network.getLayers();

  return 0;
}
