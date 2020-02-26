#ifndef __PROBLEMSETS_PROBLEM__
#define __PROBLEMSETS_PROBLEM__

#include <eigen3/Eigen/Dense>
#include <vector>

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::ArrayXd;

namespace ProblemSets {
  class Problem {
  public:
    int inputNodes = 0;
    int outputNodes = 0;

    virtual ~Problem() = 0;
    virtual VectorXd getInput(int t) = 0;
    virtual double evaluate(std::vector<VectorXd> output) = 0;
  };
}

#endif
