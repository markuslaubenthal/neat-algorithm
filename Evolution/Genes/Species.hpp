#ifndef __EVOLUTION_SPECIES__
#define __EVOLUTION_SPECIES__

#include <vector>
#include <cmath>
#include "GenoType.hpp"
#include "HistoricalMarking.hpp"
#include "../../helper.hpp"

namespace Evolution {
  namespace Genes {
    struct Species {
      std::vector<GenoType> genotypes;
      bool representativeSet = false;
      Evolution::GenoType representative;

      double totalFitness = 0;
      double totalSharedFitness = 0;
      double bestFitness = 0;
      double timeSinceBestFitness = 0;


      void addGenoType(Evolution::GenoType genotype) {
        genotypes.push_back(genotype);
        if(!representativeSet) {
          representative = genotype;
          bestFitness = genotype.fitness;
        }
      }

      void setScore(int index, double score) {
        genotypes[index].fitness = score;
      }

      void clear() {
        genotypes.clear();
      }

      int size() {
        return genotypes.size();
      }

      GenoType getRepresentativeGenotype() {
        return representative;
      }


      GenoType breed() {
        // if(totalFitness > 0) {
        //   double random = (double)rand() / RAND_MAX * totalFitness;
        //
        //
        //   double fitness = 0;
        //   for(int i = 0; i < genotypes.size(); i++) {
        //     fitness += genotypes[i].fitness;
        //     if(fitness * 2 >= random) {
        //       GenoType selection = genotypes[i];
        //       selection.mutate();
        //       return selection;
        //     }
        //   }
        // }

        GenoType selection = genotypes[rand() % genotypes.size()];
        selection.mutate();
        return selection;
      }

      void setRandomRepresentative() {
        if(genotypes.size() > 0) {
          int representativeIndex = randomInteger(0, genotypes.size());
          representativeSet = true;
          representative = genotypes[representativeIndex];
        }
      }

      void sortByFitness() {
        std::sort(genotypes.rbegin(), genotypes.rend(),
          [](auto const &a, auto const &b) { return a.fitness < b.fitness; });
      }

      void setHeighestFitness() {
        sortByFitness();
        if(genotypes[0].fitness > bestFitness) {
          bestFitness = genotypes[0].fitness;
          timeSinceBestFitness = 0;
        } else {
          timeSinceBestFitness++;
        }
      }

      bool culling() {
        genotypes.resize((int)(genotypes.size() / 2));
        setRandomRepresentative();
        return size() > 0;
      }

      void shareFitness() {
        for(int gIndex = 0; gIndex < size(); gIndex++) {
          genotypes[gIndex].sharedFitness =
            (double)genotypes[gIndex].fitness / (double)size();
          totalSharedFitness += genotypes[gIndex].sharedFitness;
        }
      }

    };
  }
}



#endif
