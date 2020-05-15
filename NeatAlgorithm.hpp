#ifndef __NEAT_ALGORITHM__
#define __NEAT_ALGORITHM__

#include <vector>
#include <cmath>
#include "GenoType.hpp"
#include "HistoricalMarking.hpp"
#include "Network.hpp"
#include "ProblemSets/XOR.hpp"


namespace Evolution {
  namespace Algorithm {
    namespace Neat {
      struct Species {
        // int numberOfWinners = 3;
        // int numberOfMutations = 10;
        std::vector<GenoType> genotypes;
        std::vector<DNN::Network> phenotypes;
        bool representativeSet = false;
        Evolution::GenoType representative;

        double totalFitness = 0;
        double bestFitness = 0;
        double timeSinceBestFitness = 0;


        void addGenoType(Evolution::GenoType genotype) {
          genotypes.push_back(genotype);
          if(!representativeSet) {
            representative = genotype;
            bestFitness = genotype.fitness;
            std::cout << "Best: " << bestFitness << std::endl;
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
          if(totalFitness > 0) {
            double random = (double)rand() / RAND_MAX * totalFitness;


            double fitness = 0;
            for(int i = 0; i < genotypes.size(); i++) {
              fitness += genotypes[i].fitness;
              if(fitness * 2 >= random) {
                GenoType selection = genotypes[i];
                selection.mutate();
                return selection;
              }
            }
          }
          GenoType selection = genotypes[rand() % genotypes.size()];
          selection.mutate();
          return selection;
        }

        GenoType *selectHighestFitness() {
          std::sort(genotypes.rbegin(), genotypes.rend(),
            [](auto const &a, auto const &b) { return a.fitness < b.fitness; });

          if(genotypes[0].fitness > bestFitness) {
            std::cout << "Fitness: " << genotypes[0].fitness << std::endl;
            bestFitness = genotypes[0].fitness;
            timeSinceBestFitness = 0;
          } else {
            timeSinceBestFitness++;
          }

          genotypes.resize((int)(genotypes.size() / 2));

          if(genotypes.size() > 0) {
            representative = genotypes[rand() % genotypes.size()];
          } else return NULL;

          totalFitness = 0;
          for(int i = 0; i < genotypes.size(); i++) {
            totalFitness += genotypes[i].fitness;
          }

          // std::cout << "Highest Fitness: " << genotypes[0].fitness << std::endl;
          GenoType *ret;
          if(genotypes.size() > 0) {
            ret = &genotypes[0];
          } else {
            ret = NULL;
          }
          return ret;
        }


      };

      class Neat {
        std::vector<Species> species;
        std::vector<GenoType> population;
        int innovationNumber = 0;
        int *innovPtr;
        int populationSize = 1000;
        ProblemSets::XOR problem;
        int numberInputNodes;
        int numberOutputNodes;

        MarkingHistory markingHistory;

      public:
        Neat(int inputNodes, int outputNodes) {
          this->numberInputNodes = inputNodes;
          this->numberOutputNodes = outputNodes;
          innovPtr = &innovationNumber;
        }

        void setProblem(ProblemSets::XOR problem) {
          this->problem = problem;
        }

        double calculateDifference(GenoType a, GenoType b) {
          int excess_genes = 0;
          int disjoint_genes = 0;
          int common_genes = 0;
          double w = 0;
          int i = 0;
          int k = 0;

          while(i < a.connections.size() && k < b.connections.size()) {
            int a_innov = a.connections[i].innovNo;
            int b_innov = b.connections[k].innovNo;

            if(a_innov == b_innov) {
              double weight_diff = std::abs(
                a.connections[i].weight - b.connections[k].weight
              );
              w += weight_diff;

              common_genes++;
              i++;
              k++;
            } else {
              if(i == 0 || k == 0) {
                excess_genes++;
              } else {
                disjoint_genes++;
              }
              if(a_innov < b_innov) i++;
              else k++;
            }
          }

          if(i != a.connections.size()) {
            excess_genes += a.connections.size() - i;
          }
          if(k != b.connections.size()) {
            excess_genes += b.connections.size() - k;
          }

          double c1, c2, c3;
          c1 = 1;
          c2 = 1;
          c3 = 0.4;
          double N = 1;
          double diff = c1 * (double)excess_genes / N + c2 * (double)disjoint_genes / N + c3 * w / (double)common_genes;

          return diff;
        }

        std::vector<Species> speciate(std::vector<GenoType> genotypes) {

          for(int i = 0; i < species.size(); i++) {
            species[i].clear();
          }

          if(genotypes.size() == 0) {
            return species;
          }

          double species_threshold = 3;

          for(int genoIndex = 0; genoIndex < genotypes.size(); genoIndex++) {

            bool genotypeAddedToSpecies = false;

            for(int speciesIndex = 0; speciesIndex < species.size(); speciesIndex++) {
              double difference = calculateDifference(
                genotypes[genoIndex],
                species[speciesIndex].getRepresentativeGenotype()
              );

              if(difference < species_threshold && !genotypeAddedToSpecies) {
                genotypeAddedToSpecies = true;
                species[speciesIndex].addGenoType(genotypes[genoIndex]);
              }
            }

            if(!genotypeAddedToSpecies) {
              Species sp;
              sp.addGenoType(genotypes[genoIndex]);
              species.push_back(sp);
            }

          }

          //Remove Species

          for(int i = 0; i < species.size(); i++) {
            if(species[i].size() == 0 || species[i].timeSinceBestFitness > 15) {
              species.erase(species.begin() + i);
              i--;
            }
          }


          return species;
        }

        void init(int populationSize) {
          this->populationSize = populationSize;

          Evolution::GenoType initialGenoType(innovPtr, &markingHistory);
          initialGenoType.setInputAndOutputNodes(numberInputNodes, numberOutputNodes);
          population = std::vector<GenoType>(populationSize, initialGenoType);

          for(int i = 0; i < populationSize; i++) {
            population[i].mutate();
          }
        }

        void fit() {
          std::cout << "Fitting Problem" << std::endl;

          init(150);

          VectorXd globalOutput(4);

          for(int epoch = 0; epoch < 1000; epoch++) {
            markingHistory.clear();

            for(int genoIndex = 0; genoIndex < population.size(); genoIndex++) {
              double fitness = 0;
              for(int problems = 0; problems < 4; problems++) {
                DNN::Network phenotype(population[genoIndex]);
                VectorXd input = problem.getInput(problems);

                std::vector<VectorXd> outputs;
                std::vector<int> outputIdRange =
                  population[genoIndex].getOutputIdRange();
                VectorXd output(population[genoIndex].getSizeOfOutputNodes());

                for(int k = 0; k < 3; k++) {
                  phenotype.feedforwardStep(input);
                  // phenotype.printState();
                  for(int outputNeuron = outputIdRange[0]; outputNeuron < outputIdRange[1]; outputNeuron++) {
                    output(outputNeuron - outputIdRange[0]) = phenotype.getState(outputNeuron)(0);
                  }
                  if(k == 1) {
                    globalOutput(problems) = phenotype.getState(outputIdRange[0])(0);
                  }

                  outputs.push_back(output);
                }

                fitness += problem.evaluate(outputs);
              }

              population[genoIndex].fitness = fitness;
            }

            // Selection
            speciate(population);

            std::vector<GenoType> newPopulation;

            double speciesTotalFitness = 0;
            std::cout << "Species: " << species.size() << std::endl;
            for(int i = 0; i < species.size(); i++) {
              GenoType *winner = species[i].selectHighestFitness();
              if(winner != NULL) {
                newPopulation.push_back(*winner);
                speciesTotalFitness += species[i].totalFitness;
              } else {
                species.erase(species.begin() + i);
                i--;
              }
            }

            population = newPopulation;



            std::sort(species.rbegin(), species.rend(),
              [](auto const &a, auto const &b) { return a.totalFitness < b.totalFitness; });

            for(int i = population.size(); i < populationSize; i++) {
              // Random Breed
              int fitnessSelector = randomInteger(0, (int)speciesTotalFitness);
              int fitness = 0;
              int popFound = false;
              for(int speciesIndex = 0; speciesIndex < species.size(); speciesIndex++) {
                fitness += species[speciesIndex].totalFitness;
                if(fitnessSelector <= fitness) {
                  population.push_back(species[speciesIndex].breed());
                  popFound = true;
                }
              }
            }
            // Epoch End
          }
        }

        void fit2() {
          std::cout << "init" << std::endl;
          init(1);

          markingHistory.clear();

          speciate(population);

          GenoType g1 = population[0];
          GenoType g2 = *(new GenoType(g1));

          std::cout << "add Nodes" << std::endl;
          g1.addNode(1);
          g2.addNode(1);


          // if(g1.isEqual(g2)) {
          //   std::cout << "Equal!" << std::endl;
          // }

          std::cout << "add connections" << std::endl;
          g1.addConnection(1, 2, 1.0, true, true);
          g2.addConnection(1, 2, 1.0, true, true);

          // std::cout << g1.toString() << std::endl;
          // std::cout << g2.toString() << std::endl;

          // if(g1.isEqual(g2)) {
          //   std::cout << "Equal!" << std::endl;
          // }

        }
      };
    }

  }
}



#endif
