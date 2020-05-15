
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
    GenoType *genotype = NULL;

    std::vector<GenoConnection> connections;

    int nodeId = 0;
    int inId = 0;
    int outId = 0;
    int type = 0;

  public:

    HistoricalMarking(int inId, int outId, int nodeId, GenoType *genotype);

    HistoricalMarking(int inId, int outId, GenoType *genotype);

    HistoricalMarking(const Evolution::HistoricalMarking& hm);

    // HistoricalMarking& operator=(const Evolution::HistoricalMarking& hm);

    ~HistoricalMarking();

    bool match(Evolution::GenoType *b);
    bool isEquivalentConnectionCreation(int inId, int outId, GenoType *geno);
    bool isEquivalentNodeCreation(int _inId, int _outId, GenoType *geno);

    void addConnection(GenoConnection con);

    int getNodeId();
    int getInId();
    int getOutId();
    std::vector<GenoConnection> getConnections();

    std::string toString();

  };
}

#endif
