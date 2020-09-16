#ifndef __GENOTYPE__
#define __GENOTYPE__

#include <eigen3/Eigen/Dense>
#include <sstream>
#include <vector>
#include <map>
#include <iterator>

#include "MarkingHistory.hpp"
#include "HistoricalMarking.hpp"
#include "GenoConnection.hpp"
#include "../../helper.hpp"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::ArrayXd;

namespace Evolution {
  class MarkingHistory;
  class HistoricalMarking;
  class GenoType {
    MarkingHistory *markingHistory;
    int *innovNo;

    int inputNodes = 0;
    int outputNodes = 0;
    int hiddenNodes = 0;

    std::map<int, int> nodeIdMap;
    std::vector<int> nodeIds;

    public:
    std::vector<GenoConnection> connections;
    double fitness = 0;
    double sharedFitness = 0;

    GenoType() {}

    GenoType(int *innovNo, MarkingHistory *markingHistory)
      : innovNo(innovNo), markingHistory(markingHistory) {
      connections = std::vector<GenoConnection>();
    }

    GenoType(std::vector<GenoConnection> connections) : connections(connections) {}

    ~GenoType() {}

    std::string toString(bool disabled = true);

    int getSizeOfInputNodes();
    int getSizeOfOutputNodes();

    int getRandomNonInputId();

    void addConnection(int inId, int outId, double weight, bool enabled, bool createHistoricalMarking = true);

    void setConnection(int index, bool enabled, double weight);

    void setConnection(int inId, int outId, bool enabled, double weight);

    void setInputAndOutputNodes(int input, int output);

    bool connectionExists(int inId, int outId);

    void sortConnections();

    void addNodeAtConnection(int connectionId);

    void addNodeAtConnection(int connectionId, HistoricalMarking *hm);

    void toggleConnection(int inId, int outId);

    void mutateConnection();

    void addNode(int conId);

    void mutateAddNode();

    void randomizeWeights();
    void mutateWeights();

    void mutate();

    HistoricalMarking *getHistoricalNodeMarking(int conId);

    HistoricalMarking *getHistoricalMarking(int conId);

    bool historicalNodeMarkingExists(int conId);

    double randomWeight();

    std::vector<int> getOutputIdRange();

    std::vector<GenoConnection> getConnections();

    //Copies a GenoType into the heap, needs cleanup
    GenoType *clone();

    bool isEqual(GenoType b);
  };
}

#endif
