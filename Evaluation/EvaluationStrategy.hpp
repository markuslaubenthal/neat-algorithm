#ifndef __EVALUATION_STRATEGY__
#define __EVALUATION_STRATEGY__

#include "../Evolution/Genes/GenoType.hpp"

namespace Evaluation {
  class EvaluationStrategy
  {
    public:
        virtual ~EvaluationStrategy() {}
        virtual double evaluate(Evolution::GenoType *genotype) const = 0;
        virtual void evaluatePrint(Evolution::GenoType *genotype) const = 0;
  };
}

#endif
