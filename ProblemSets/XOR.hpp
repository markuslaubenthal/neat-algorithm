#ifndef __PROBLEMSETS_XOR__
#define __PROBLEMSETS_XOR__

#include <eigen3/Eigen/Dense>
#include <vector>

#include "Problem.hpp"
using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::ArrayXd;

namespace ProblemSets {
  class XOR {
  public:
    int inputNodes = 2;
    int outputNodes = 2;
    std::vector<VectorXd> inputs;
    std::vector<VectorXd> outputs;

    int state;

    XOR() {
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


    VectorXd getInput(int t) {
      state = t % 4;
      return inputs[t % 4];
    }

    double evaluate(std::vector<VectorXd> output) {
      double reward = 0;
      VectorXd expected_output = outputs[state];
      for(int i = 0; i < output.size(); i++) {
        if(i == 1) {
          reward += 1 - (expected_output - output[i]).norm();
        }
      }

      return reward;
    }
  };
}


#endif
