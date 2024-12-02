#ifndef __NEAT_ALGORITHM__
#define __NEAT_ALGORITHM__

#include <vector>
#include <cmath>
#include "../Genes/Species.hpp"
#include "../Genes/GenoType.hpp"
#include "../Genes/HistoricalMarking.hpp"
#include "../../DNN/Network.hpp"
#include "../../Evaluation/EvaluationStrategy.hpp"
#include "../../Visualization/Graph.hpp"
#include "../../DNN/SingleLayerNetwork.hpp"
#include "../../DNN/NetworkFactory.hpp"

namespace Evolution {
  namespace Algorithm {
    namespace Neat {
      class Neat {
        std::vector<Evolution::Genes::Species> species;
        std::vector<GenoType> population;
        int innovationNumber = 0;
        int *innovPtr;
        int populationSize = 150;
        int numberInputNodes;
        int numberOutputNodes;

        Evaluation::EvaluationStrategy *evaluationStrategy;

        MarkingHistory markingHistory;

      public:
        Neat(int inputNodes, int outputNodes) {
          this->numberInputNodes = inputNodes;
          this->numberOutputNodes = outputNodes;
          innovPtr = &innovationNumber;
        }

        void setEvaluationStrategy(Evaluation::EvaluationStrategy *strategy) {
          evaluationStrategy = strategy;
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

        std::vector<Evolution::Genes::Species> speciate(std::vector<GenoType> genotypes) {

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
              Evolution::Genes::Species sp;
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
          initialGenoType.initNodes(numberInputNodes, numberOutputNodes);
          // initialGenoType.addNodeAtConnection(1);
          //
          // initialGenoType.addConnection(1,6, 10, true);
          // //
          // initialGenoType.setConnection(-1, 2, true, -0.5 * 1.0);
          // initialGenoType.setConnection(0, 2, true, 1 * 1.0);
          // initialGenoType.setConnection(1, 2, true, 1 * 1.0);
          // // initialGenoType.setConnection(6, 2, true, -2 * 1.0);
          //
          // initialGenoType.setConnection(-1, 6, true, -1.5 * 1.0);
          // initialGenoType.setConnection(0, 6, true, 1 * 1.0);
          // initialGenoType.setConnection(1, 6, true, 1 * 1.0);

          population = std::vector<GenoType>(populationSize, initialGenoType);

          for(int i = 0; i < populationSize; i++) {
            population[i].randomizeWeights();
            population[i].mutate();
          }
          evaluate();
        }

        void init(GenoType initial) {
          population = std::vector<GenoType>(populationSize, initial);

          for(int i = 0; i < populationSize; i++) {
            population[i].randomizeWeights();
            population[i].mutate();
          }
          evaluate();
        }

        void speciate() {
          double species_threshold = 3;

          for(int sIndex = 0; sIndex < species.size(); sIndex++) {
            species[sIndex].clear();
          }

          for(int gIndex = 0; gIndex < population.size(); gIndex++) {
            GenoType *type = &population[gIndex];
            double fitness = type->fitness;

            //Search for correct species.
            bool speciesAdded = false;
            for(int sIndex = 0; sIndex < species.size() && !speciesAdded; sIndex++) {
              GenoType representative = species[sIndex].getRepresentativeGenotype();
              if(calculateDifference(*type, representative) < species_threshold) {
                species[sIndex].addGenoType(*type);
                speciesAdded = true;
              }
            }

            //Create new species, if none found
            if(!speciesAdded) {
              Evolution::Genes::Species newSpecies;
              newSpecies.addGenoType(*type);
              species.push_back(newSpecies);
            }
          }

          //Remove non improving or empty species
          for(int sIndex = 0; sIndex < species.size(); sIndex++) {
            if(!species[sIndex].size() > 0) {
              species.erase(species.begin() + sIndex);
              sIndex--;
            }
            else if(species[sIndex].timeSinceBestFitness > 15) {
              species.erase(species.begin() + sIndex);
              sIndex--;
            }
          }
        }

        void culling() {
          for(int sIndex = 0; sIndex < species.size(); sIndex++) {
            species[sIndex].setHeighestFitness();
            if(!species[sIndex].culling()) {
              species.erase(species.begin() + sIndex);
              sIndex--;
            }
          }
        }

        double shareFitness() {
          double totalFitness = 0;
          for(int sIndex = 0; sIndex < species.size(); sIndex++) {
            species[sIndex].shareFitness();
            totalFitness += species[sIndex].totalSharedFitness;
          }
          return totalFitness;
        }

        std::vector<GenoType> generateOffsprings(double totalFitness) {
          std::vector<GenoType> newPopulation;
          for(int sIndex = 0; sIndex < species.size(); sIndex++) {
            int numberOfOffsprings =
              populationSize * (species[sIndex].totalSharedFitness / totalFitness);
            int numberOfMutations = numberOfOffsprings * 0.25;

            for(int counter = 0; counter <= numberOfOffsprings; counter++) {
              if(counter < numberOfMutations) {
                newPopulation.push_back(species[sIndex].breed());
              } else {
                newPopulation.push_back(species[sIndex].mate(
                  species[sIndex].genotypes[rand() % species[sIndex].size()],
                  species[sIndex].genotypes[rand() % species[sIndex].size()]
                ));
              }

            }
          }
          return newPopulation;
        }

        void evaluate() {
          for(int gIndex = 0; gIndex < population.size(); gIndex++) {
            double fitness = evaluationStrategy->evaluate(&population[gIndex]);
            // std::cout << std::endl << "Start" << std::endl;
            // evaluationStrategy->evaluatePrint(&population[gIndex]);
            // std::cout << "End" << std::endl << std::endl;
            // std::cout << fitness << std::endl;
            population[gIndex].fitness = fitness;
          }
        }

        std::vector<GenoType> getWinners() {
          std::vector<GenoType> winners;
          for(int sIndex = 0; sIndex < species.size(); sIndex++) {
            if(species[sIndex].size() > 5) {
              GenoType winner;
              double bestFitness = -1000;
              for(int gIndex = 0; gIndex < species[sIndex].genotypes.size(); gIndex++) {
                GenoType current = species[sIndex].genotypes[gIndex];
                if(current.fitness > bestFitness) {
                  winner = current;
                  bestFitness = current.fitness;
                }
              }
              winners.push_back(winner);
            }
          }
          return winners;
        }

        void epoch() {
          markingHistory.clear();
          /**
           * Speciate every genome
           */
          speciate();
          /**
           * Calculate the shared fitness
           */
          if(species.size() == 0) {
            init(population[0]);
          } else {
            double totalFitness = shareFitness();
            /**
             * Remove half of genotypes and select new representative
             */
            std::vector<GenoType> winners = getWinners();
            culling();
            /**
             * Create new Population
             */
            population = generateOffsprings(totalFitness);
            population.insert( population.end(), winners.begin(), winners.end() );
            evaluate();

            double bestFitness = -1000;
            GenoType best;
            for(int i = 0; i < population.size(); i++) {

              if(population[i].fitness > bestFitness) {
                bestFitness = population[i].fitness;
                best = population[i];
              }
            }
            // avgNoOfLayers = avgNoOfLayers / (double)population.size();
            std::cout << "Maximum Fitness: " << bestFitness << std::endl;
            std::cout << "Number of Species: " << species.size() << std::endl;
            std::cout << "Population Size: " << population.size() << std::endl;
            // std::cout << "Average Layers: " << avgNoOfLayers << std::endl;
            std::cout << std::endl;
            Visualization::Graph::write(best, "best.graph");
          }

        }

        // void fit() {
        //   init(150);
        //
        //   VectorXd globalOutput(4);
        //
        //   for(int epoch = 0; epoch < 1000; epoch++) {
        //     markingHistory.clear();
        //
        //     for(int genoIndex = 0; genoIndex < population.size(); genoIndex++) {
        //       double fitness = 0;
        //       for(int problems = 0; problems < 4; problems++) {
        //         DNN::Network phenotype(population[genoIndex]);
        //         VectorXd input = problem.getInput(problems);
        //
        //         std::vector<VectorXd> outputs;
        //         std::vector<int> outputIdRange =
        //           population[genoIndex].getOutputIdRange();
        //         VectorXd output(population[genoIndex].getSizeOfOutputNodes());
        //
        //         for(int k = 0; k < 3; k++) {
        //           phenotype.feedforwardStep(input);
        //           // phenotype.printState();
        //           for(int outputNeuron = outputIdRange[0]; outputNeuron < outputIdRange[1]; outputNeuron++) {
        //             output(outputNeuron - outputIdRange[0]) = phenotype.getState(outputNeuron)(0);
        //           }
        //           if(k == 1) {
        //             globalOutput(problems) = phenotype.getState(outputIdRange[0])(0);
        //           }
        //
        //           outputs.push_back(output);
        //         }
        //
        //         fitness += problem.evaluate(outputs);
        //       }
        //
        //       population[genoIndex].fitness = fitness;
        //     }
        //
        //     // Selection
        //     speciate(population);
        //
        //     std::vector<GenoType> newPopulation;
        //
        //     double speciesTotalFitness = 0;
        //     std::cout << "Species: " << species.size() << std::endl;
        //     for(int i = 0; i < species.size(); i++) {
        //       GenoType *winner = species[i].selectHighestFitness();
        //       if(winner != NULL) {
        //         newPopulation.push_back(*winner);
        //         speciesTotalFitness += species[i].totalFitness;
        //       } else {
        //         species.erase(species.begin() + i);
        //         i--;
        //       }
        //     }
        //
        //     population = newPopulation;
        //
        //
        //
        //     std::sort(species.rbegin(), species.rend(),
        //       [](auto const &a, auto const &b) { return a.totalFitness < b.totalFitness; });
        //
        //     for(int i = population.size(); i < populationSize; i++) {
        //       // Random Breed
        //       int fitnessSelector = randomInteger(0, (int)speciesTotalFitness);
        //       int fitness = 0;
        //       int popFound = false;
        //       for(int speciesIndex = 0; speciesIndex < species.size(); speciesIndex++) {
        //         fitness += species[speciesIndex].totalFitness;
        //         if(fitnessSelector <= fitness) {
        //           population.push_back(species[speciesIndex].breed());
        //           popFound = true;
        //         }
        //       }
        //     }
        //     // Epoch End
        //   }
        // }

        // void fit2() {
        //   std::cout << "init" << std::endl;
        //   init(1);
        //
        //   markingHistory.clear();
        //
        //   speciate(population);
        //
        //   GenoType g1 = population[0];
        //   GenoType g2 = *(new GenoType(g1));
        //
        //   std::cout << "add Nodes" << std::endl;
        //   g1.addNode(1);
        //   g2.addNode(1);
        //
        //
        //   // if(g1.isEqual(g2)) {
        //   //   std::cout << "Equal!" << std::endl;
        //   // }
        //
        //   std::cout << "add connections" << std::endl;
        //   g1.addConnection(1, 2, 1.0, true, true);
        //   g2.addConnection(1, 2, 1.0, true, true);
        //
        //   // std::cout << g1.toString() << std::endl;
        //   // std::cout << g2.toString() << std::endl;
        //
        //   // if(g1.isEqual(g2)) {
        //   //   std::cout << "Equal!" << std::endl;
        //   // }
        //
        // }

        // void newFit() {
        //   init(150);
        //   for(int epoch = 0; epoch < 1000; epoch++) {
        //     evaluate();
        //     speciate();
        //     culling();
        //     generateOffsprings();
        //   }
        // }

        // void evaluate() {
        //   for(int genoIndex = 0; genoIndex < population.size(); genoIndex++) {
        //     double fitness = evaluate(genoIndex);
        //     population[genoIndex].setFitness(fitness);
        //   }
        // }

        // double evaluate(int genoIndex) {
        //   int fitness = 0;
        //   for(int problems = 0; problems < 4; problems++) {
        //     DNN::Network phenotype(population[genoIndex]);
        //     VectorXd input = problem.getInput(problems);
        //
        //     std::vector<VectorXd> outputs;
        //     std::vector<int> outputIdRange =
        //       population[genoIndex].getOutputIdRange();
        //     VectorXd output(population[genoIndex].getSizeOfOutputNodes());
        //
        //     for(int k = 0; k < 3; k++) {
        //       phenotype.feedforwardStep(input);
        //       for(int outputNeuron = outputIdRange[0]; outputNeuron < outputIdRange[1]; outputNeuron++) {
        //         output(outputNeuron - outputIdRange[0]) = phenotype.getState(outputNeuron)(0);
        //       }
        //       if(k == 1) {
        //         globalOutput(problems) = phenotype.getState(outputIdRange[0])(0);
        //       }
        //
        //       outputs.push_back(output);
        //     }
        //
        //     fitness += problem.evaluate(outputs);
        //   }
        //   return fitness;
        // }
      };
    }

  }
}



#endif
