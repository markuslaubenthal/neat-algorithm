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
        GenoType selection = genotypes[rand() % genotypes.size()];
        selection.mutate();
        return selection;
      }

      Evolution::GenoType mate(Evolution::GenoType a, Evolution::GenoType b) {
        int i = 0, k = 0;
        std::vector<Evolution::GenoConnection> connections;
        std::vector<int> nodeIds = a.getNodeIds();
        std::vector<int> bNodeIds = b.getNodeIds();
        nodeIds.insert( nodeIds.end(), bNodeIds.begin(), bNodeIds.end() );
        removeDuplicates(nodeIds);

        while(i < a.connections.size() && k < b.connections.size()) {
          int a_innov = a.connections[i].innovNo;
          int b_innov = b.connections[k].innovNo;
          Evolution::GenoConnection* ab[] = {&a.connections[i], &b.connections[k]};

          if(a_innov == b_innov) {
            bool enabled = true;
            if(!ab[0]->enabled || !ab[1]->enabled) {
              int r = randomDouble(0,1);
              if(r > 0.25) {
                enabled = false;
              }
            }
            int r = rand() % 2;
            connections.push_back(*ab[r]);
            connections.back().enabled = enabled;
            i++;
            k++;
          } else {
            int selectionId = (a_innov < b_innov ? 0 : 1);
            bool select = false;
            if(a.fitness == b.fitness) {
              select = (bool) rand() % 2;
              // If Selected fitness > other fitness
            } else if(selectionId == 0 && a.fitness > b.fitness ||
                      selectionId == 1 && b.fitness > a.fitness) {
              select = true;
            }
            if(select) {
              connections.push_back(*ab[selectionId]);
            }
            // connections.push_back(*ab[selectionId]);
            if(a_innov < b_innov) i++;
            else k++;
          }
        }
        if(i < a.connections.size() && a.fitness >= b.fitness) {
          connections.insert(connections.end(), a.connections.begin() + i, a.connections.end());
        }
        if(k < b.connections.size() && b.fitness >= a.fitness) {
          connections.insert(connections.end(), b.connections.begin() + i, b.connections.end());
        }

        Evolution::GenoType genotype(
          a.getInnovNo(),
          a.getMarkingHistory(),
          connections,
          nodeIds,
          a.getSizeOfInputNodes(),
          a.getSizeOfOutputNodes()
        );
        genotype.mutate();
        return genotype;
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
