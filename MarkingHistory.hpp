#ifndef __EVOLUTION_MARKING_HISTORY__
#define __EVOLUTION_MARKING_HISTORY__

#include <iostream>

#include "HistoricalMarking.hpp"
#include "GenoConnection.hpp"

namespace Evolution {
  class HistoricalMarking;
  class GenoType;

  class MarkingHistory {

    std::vector<HistoricalMarking> history;

  public:

    bool nodeMarkingExists(GenoConnection con, GenoType *b);

    bool connectionMarkingExists(int inId, int outId, GenoType *b);

    HistoricalMarking *getNodeMarking(GenoConnection con, GenoType *b);

    HistoricalMarking *getConnectionMarking(int inId, int outId, GenoType *b);

    HistoricalMarking *getMarking(int id);

    void addNodeCreation(int inId, int outId, int nodeId, std::vector<GenoConnection> cons, GenoType *genotype);

    void addConnectionCreation(int inId, int outId, GenoConnection con, GenoType *genotype);

    void clear();

    std::string toString();
  };
}

#endif
