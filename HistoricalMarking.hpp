
#ifndef __EVOLUTION_HISTORICAL_MARKING__
#define __EVOLUTION_HISTORICAL_MARKING__

#include "GenoType.hpp"
#include "GenoConnection.hpp"
#include <vector>

namespace Evolution {
  class GenoType;

  class HistoricalMarking {
    const int NODE = 1;
    const int CONNECTION = 2;
    GenoType *genotype;

    std::vector<GenoConnection> connections;

    int nodeId;
    int inId;
    int outId;
    int type;

  public:

    HistoricalMarking(int inId, int outId, int nodeId, GenoType *genotype)
      : inId(inId), outId(outId), nodeId(nodeId), genotype(genotype) {
      this->type = NODE;
    }

    HistoricalMarking(int inId, int outId, GenoType *genotype)
      : inId(inId), outId(outId), genotype(genotype) {
      this->type = CONNECTION;
    }

    bool match(Evolution::GenoType *b);
    bool isEquivalentConnectionCreation(int inId, int outId, GenoType *geno);
    bool isEquivalentNodeCreation(int inId, int outId, GenoType *geno);

    void addConnection(GenoConnection con);

    int getNodeId();
    int getInId();
    int getOutId();
    std::vector<GenoConnection> getConnections();

  };
}

#endif
