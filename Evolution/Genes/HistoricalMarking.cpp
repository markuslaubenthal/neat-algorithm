#include "HistoricalMarking.hpp"
#include <iostream>

Evolution::HistoricalMarking::HistoricalMarking(int inId, int outId, int nodeId, GenoType *genotype)
  : inId(inId), outId(outId), nodeId(nodeId) {
  this->type = NODE;
  this->genotype = new GenoType(*genotype);
}

Evolution::HistoricalMarking::HistoricalMarking(int inId, int outId, GenoType *genotype)
  : inId(inId), outId(outId) {
  this->type = CONNECTION;
  this->genotype = new GenoType(*genotype);
}

Evolution::HistoricalMarking::HistoricalMarking(const Evolution::HistoricalMarking& hm) {
  genotype = new GenoType(*(hm.genotype));
  connections = hm.connections;
  nodeId = hm.nodeId;
  inId = hm.inId;
  outId = hm.outId;
  type = hm.type;
}

Evolution::HistoricalMarking::~HistoricalMarking() {
  delete genotype;
  genotype = NULL;
}

bool Evolution::HistoricalMarking::match(Evolution::GenoType *b) {
  return b->isEqual(*genotype);
}

bool Evolution::HistoricalMarking::isEquivalentConnectionCreation(int inId, int outId, Evolution::GenoType *geno) {
  return (
    this->inId == inId &&
    this->outId == outId &&
    type == CONNECTION &&
    match(geno));
}

bool Evolution::HistoricalMarking::isEquivalentNodeCreation(int _inId, int _outId, Evolution::GenoType *geno) {
  return
    (this->inId == _inId &&
     this->outId == _outId &&
     type == NODE &&
     match(geno)
    );
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

std::string Evolution::HistoricalMarking::toString() {
  std::string str;
  std::string _node = (type == NODE) ? "\nNodeId: " + std::to_string(nodeId) : "";
  str = std::string("HistoricalMarking: \n") +
        "inId  : " + std::to_string(inId) +
        "\noutId : " + std::to_string(outId) +
        _node;
  str += "\nConnections:\n";

  for(int i = 0; i < connections.size(); i++) {
    std::string tmp = connections[i].toString();
    replaceInString(tmp, "\n", "\n ");
    str += tmp;
    str += "\n\n";
  }
  return str;
}
