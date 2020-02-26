#ifndef __NEAT_ALGORITHM__
#define __NEAT_ALGORITHM__

#include <vector>
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

        void addGenoType(Evolution::GenoType genotype) {
          genotypes.push_back(genotype);
        }

        void setScore(int index, double score) {
          genotypes[index].fitness = score;
        }

        std::vector<Species> mutate() {
          std::vector<Species> new_species;
          std::vector<GenoType> selection = selectHighestFitness();
          Species sp;



          for(int i = 0; i < selection.size(); i++) {
            std::vector<GenoType> mutations(numberOfMutations + 1, selection[i]);

            //Leave a copy of the original genotype in the species
            for(int k = 1; k < mutations.size(); k++) {
              mutations[k].mutate();
              sp.addGenoType(mutations[k]);
            }
          }

          new_species.push_back(sp);

          return new_species;

        }

        std::vector<GenoType> selectHighestFitness() {
          std::sort(genotypes.rbegin(), genotypes.rend(),
            [](auto const &a, auto const &b) { return a.fitness < b.fitness; });
          if(genotypes.size() > numberOfWinners)
            genotypes.resize(numberOfWinners);
          std::cout << "Highest Fitness: " << genotypes[0].fitness << std::endl;
          return genotypes;
        }


      };

      class Neat {
        std::vector<Species> species;
        int innovationNumber = 0;
        int *innovPtr;
        ProblemSets::XOR problem;

      public:
        Neat(int inputNodes, int outputNodes) {
          innovPtr = &innovationNumber;
          Species initialSpecies = Species();
          Evolution::GenoType initialGenoType(innovPtr);
          initialGenoType.setInputAndOutputNodes(inputNodes, outputNodes);
          initialSpecies.addGenoType(initialGenoType);
          species.push_back(initialSpecies);
        }

        void setProblem(ProblemSets::XOR problem) {
          this->problem = problem;
        }

        void fit() {

          std::cout << "Fitting Problem" << std::endl;
          for(int epoch = 0; epoch < 1000; epoch++) {

            std::vector<std::vector<double>> scores;

            for(int speciesIndex = 0; speciesIndex < species.size(); speciesIndex++) {
              for(int genoIndex = 0; genoIndex < species[speciesIndex].genotypes.size(); genoIndex++) {
                double fitness = 0;
                for(int problems = 0; problems < 4; problems++) {
                  DNN::Network phenotype(species[speciesIndex].genotypes[genoIndex]);
                  VectorXd input = problem.getInput(problems);

                  std::vector<VectorXd> outputs;
                  std::vector<int> outputIdRange =
                    species[speciesIndex].genotypes[genoIndex].getOutputIdRange();
                  VectorXd output(species[speciesIndex].genotypes[genoIndex].outputNodes.size());

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
                if(fitness == 0) {
                  std::cout << "START" << std::endl;
                  std::cout << species[speciesIndex].genotypes[genoIndex].toString();
                  std::cout << "END" << std::endl;
                }
                species[speciesIndex].setScore(genoIndex, fitness);
              }
            }

            // Mutations
            std::vector<Species> new_species;
            for(int speciesIndex = 0; speciesIndex < species.size(); speciesIndex++) {
              Species sp = species[speciesIndex];
              std::vector<Species> mutations = sp.mutate();
              new_species.insert(new_species.end(), mutations.begin(), mutations.end());
            }
            species = new_species;

            // Epoch End
          }
        }
      };
    }

  }
}



#endif
