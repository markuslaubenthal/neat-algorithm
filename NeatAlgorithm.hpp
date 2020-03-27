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
        int numberOfWinners = 3;
        int numberOfMutations = 10;
        std::vector<GenoType> genotypes;
        std::vector<DNN::Network> phenotypes;
        bool representativeSet = false;
        GenoType representative;

        double totalFitness = 0;
        double bestFitness;
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
          genotypes = std::vector<GenoType>();
        }

        int size() {
          return genotypes.size();
        }

        GenoType getRepresentativeGenotype() {
          return representative;
        }

        std::vector<GenoType> mutate() {
          std::vector<GenoType> selection = selectHighestFitness();
          Species sp;
          std::vector<GenoType> new_genotypes;

          for(int i = 0; i < selection.size(); i++) {
            std::vector<GenoType> mutations(numberOfMutations + 1, selection[i]);

            //Leave a copy of the original genotype in the species
            for(int k = 1; k < mutations.size(); k++) {
              mutations[k].mutate();
              new_genotypes.push_back(mutations[k]);
            }
          }
          return new_genotypes;
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

        std::vector<GenoType> selectHighestFitness() {
          std::sort(genotypes.rbegin(), genotypes.rend(),
            [](auto const &a, auto const &b) { return a.fitness < b.fitness; });

          if(genotypes[0].fitness > bestFitness) {
            bestFitness = genotypes[0].fitness;
            timeSinceBestFitness = 0;
          } else {
            timeSinceBestFitness++;
          }
          genotypes.resize(std::max(1, (int)(genotypes.size() / 2)));

          if(genotypes.size() > 0) {
            representative = genotypes[rand() % genotypes.size()];
          }

          totalFitness = 0;
          for(int i = 0; i < genotypes.size(); i++) {
            totalFitness += genotypes[i].fitness;
          }

          std::cout << "Highest Fitness: " << genotypes[0].fitness << std::endl;
          return genotypes;
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

        std::vector<HistoricalMarking> markingHistory;

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
          c1 = .001;
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

            for(int genoIndex = 0; genoIndex < population.size(); genoIndex++) {
              double fitness = 0;
              for(int problems = 0; problems < 4; problems++) {
                DNN::Network phenotype(population[genoIndex]);
                VectorXd input = problem.getInput(problems);

                std::vector<VectorXd> outputs;
                std::vector<int> outputIdRange =
                  population[genoIndex].getOutputIdRange();
                VectorXd output(population[genoIndex].outputNodes.size());

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


              // if(fitness > -1.0002) {
              //   std::cout << "OUTPUT: " << std::endl;
              //   std::cout << globalOutput.transpose() << std::endl;
              // }


            }

            // Selection
            speciate(population);

            std::vector<GenoType> newPopulation;

            double speciesTotalFitness = 0;
            for(int i = 0; i < species.size(); i++) {
              std::vector<GenoType> selection = species[i].selectHighestFitness();
              if(species[i].size() > 0) {
                newPopulation.insert(newPopulation.begin(), selection.begin(), selection.end());
                speciesTotalFitness += species[i].totalFitness;
              } else {
                species.erase(species.begin() + i);
                i--;
              }
            }

            population = newPopulation;
            std::cout << population.size() << std::endl;

            std::sort(species.rbegin(), species.rend(),
              [](auto const &a, auto const &b) { return a.totalFitness < b.totalFitness; });

            for(int i = population.size(); i < populationSize; i++) {
              // Random Breed
              double fitnessSelector = (double)rand() / RAND_MAX * speciesTotalFitness;
              double fitness = 0;
              for(int speciesIndex = 0; speciesIndex < species.size(); speciesIndex++) {
                fitness += species[speciesIndex].totalFitness;
                if(fitnessSelector * 2 >= fitness) {
                  population.push_back(species[speciesIndex].breed());
                }
              }
            }
            // Epoch End
          }
        }
      };
    }

  }
}



#endif
