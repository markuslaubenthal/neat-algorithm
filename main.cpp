#include <iostream>
#include <eigen3/Eigen/Dense>
#include <cstdlib>
#include <ctime>

#include <chrono>
#include <thread>

#include "Network.hpp"

using Eigen::MatrixXd;
using Eigen::VectorXd;

int main(int argc, char* argv[]) {
  std::srand(time(0));
  std::cout << "Initializing neural net..." << std::endl;


  DNN::Network network(
    {2,25,25,25,25,25,25,25,25,25,25,25,25,25,25,1},
    {
      {0,1},
      {1,2},
      {2,3},
      {3,1},
      {3,4},
      {4,5},
      {5,6},
      {6,7},
      {7,8},
      {8,9},
      {9,10},
      {10,11},
      {11,12},
      {12,13},
      {13,14},
      {14,15},
      {14,3},
      {14,5},
    }
  );


  VectorXd input(2);
  input << 1,1;


  auto time_start = std::chrono::steady_clock::now();


  for(int iteration = 0; iteration < 1000; iteration++) {
    if(iteration == 1) {
      input << 0,0;
    }
    network.feedforwardStep(input);
    // network.printState();
    // std::cout << network.getLayerState(15) <<std::endl;
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  auto time_end = std::chrono::steady_clock::now();

  std::cout << "Calculation finished in: " << std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count() << " ms" << std::endl;
  return 0;
}
