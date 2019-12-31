#ifndef __DNN_ACTIVATIONFUNCTION__
#define __DNN_ACTIVATIONFUNCTION__


#include <cmath>
#include <ctime>

double logisticFunction(double x) {
  return 1.0 / ( 1.0 + std::exp(-x));
}
double thresholdFunction(double x) {
  return x > 0;
}
double activityFunction(double x) {
  return (x > 0) ? 0 : 1;
}
double identFunction(double x) {
  return x;
}


#endif
