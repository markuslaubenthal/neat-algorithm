#ifndef __NEAT_ALGORITHM__
#define __NEAT_ALGORITHM__

#include <vector>
#include <cmath>
#include "GenoType.hpp"
#include "Network.hpp"
#include "ProblemSets/XOR.hpp"



namespace Evolution {
  namespace Algorithm {
    namespace Neat {
      struct Species {
        int numberOfWinners = 5;
        int numberOfMutations = 10;
        std::vector<GenoType> genotypes;
        std::vector<DNN::Network> phenotypes;
        bool representativeSet = false;
        GenoType representative;

        void addGenoType(Evolution::GenoType genotype) {
          genotypes.push_back(genotype);
          if(!representativeSet) representative = genotype;
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
          GenoType selection = genotypes[rand() % genotypes.size()];
          selection.mutate();
          return selection;
        }

        std::vector<GenoType> selectHighestFitness() {
          std::sort(genotypes.rbegin(), genotypes.rend(),
            [](auto const &a, auto const &b) { return a.fitness < b.fitness; });

          genotypes.resize(genotypes.size() / 2);
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
          double w;
          int i = 0;
          int k = 0;
          while(i < a.connections.size() && k < b.connections.size()) {
            int a_innov = a.connections[i].innovNo;
            int b_innov = b.connections[k].innovNo;
            if(a_innov == b_innov) {
              // weight difference TODO

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
          double diff = c1 * excess_genes / N + c2 * disjoint_genes / N + c3 * w / common_genes;

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

          //Remove Empty Species

          for(int i = 0; i < species.size(); i++) {
            if(species[i].size() == 0) {
              species.erase(species.begin() + i);
              i--;
            }
          }


          return species;
        }

        void init(int populationSize) {
          this->populationSize = populationSize;

          Evolution::GenoType initialGenoType(innovPtr);
          initialGenoType.setInputAndOutputNodes(numberInputNodes, numberOutputNodes);
          population = std::vector<GenoType>(populationSize, initialGenoType);

          for(int i = 0; i < populationSize; i++) {
            population[i].mutate();
          }
        }

        void fit() {
          std::cout << "Fitting Problem" << std::endl;

          init(300);

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

                for(int k = 0; k < 2; k++) {
                  phenotype.feedforwardStep(input);
                  // phenotype.printState();
                  for(int outputNeuron = outputIdRange[0]; outputNeuron < outputIdRange[1]; outputNeuron++) {
                    output(outputNeuron - outputIdRange[0]) = phenotype.getState(outputNeuron)(0);
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

            for(int i = 0; i < species.size(); i++) {
              std::vector<GenoType> selection = species[i].selectHighestFitness();
              if(species[i].size() > 0) {
                newPopulation.insert(newPopulation.begin(), selection.begin(), selection.end());
              } else {
                species.erase(species.begin() + i);
                i--;
              }
            }

            population = newPopulation;
            std::cout << population.size() << std::endl;
            for(int i = population.size(); i < populationSize; i++) {
              // Random Breed
              int randomSpeciesIndex = rand() % species.size();
              population.push_back(species[randomSpeciesIndex].breed());
            }
            // Epoch End
          }
        }
      };
    }

  }
}



#endif
