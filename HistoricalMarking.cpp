#include "HistoricalMarking.hpp"


bool Evolution::HistoricalMarking::match(Evolution::GenoType *b) {
  if(b->connections.size() == genotype->connections.size()) {
    for(int i = 0; i < genotype->connections.size(); i++) {
      if(genotype->connections[i].innovNo != b->connections[i].innovNo)
        return false;
    }
    return true;
  }
  return false;
}

bool Evolution::HistoricalMarking::isEquivalentConnectionCreation(int inId, int outId, Evolution::GenoType *geno) {
  return (this->inId == inId && this->outId == outId && type == CONNECTION && match(geno));
}

bool Evolution::HistoricalMarking::isEquivalentNodeCreation(int inId, int outId, Evolution::GenoType *geno) {
  return (this->inId == inId && this->outId == outId && type == NODE && match(geno));
}

void Evolution::HistoricalMarking::addConnection(Evolution::GenoConnection con) {
  connections.push_back(con);
}

std::vector<Evolution::GenoConnection> Evolution::HistoricalMarking::getConnections() {
  return connections;
}

int Evolution::HistoricalMarking::getNodeId() {return nodeId;}
int Evolution::HistoricalMarking::getInId() {return inId;}
int Evolution::HistoricalMarking::getOutId() {return outId;}
