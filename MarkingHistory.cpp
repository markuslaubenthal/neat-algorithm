#include "MarkingHistory.hpp"

bool Evolution::MarkingHistory::nodeMarkingExists(Evolution::GenoConnection con, GenoType *b) {
  for(int i = 0; i < history.size(); i++) {
    if(history[i].isEquivalentNodeCreation(
      con.inId, con.outId, b
    )) return true;
  }
  return false;
}
bool Evolution::MarkingHistory::connectionMarkingExists(int inId, int outId, GenoType *b) {
  for(int i = 0; i < history.size(); i++) {
    if(history[i].isEquivalentConnectionCreation(
      inId, outId, b
    )) return true;
  }
  return false;
}

Evolution::HistoricalMarking *Evolution::MarkingHistory::getNodeMarking(Evolution::GenoConnection con, GenoType *b) {
  for(int i = 0; i < history.size(); i++) {
    if(history[i].isEquivalentNodeCreation(
      con.inId, con.outId, b
    )) return &history[i];
  }
  return &history[0];
}

Evolution::HistoricalMarking *Evolution::MarkingHistory::getConnectionMarking(int inId, int outId, GenoType *b) {
  for(int i = 0; i < history.size(); i++) {
    if(history[i].isEquivalentConnectionCreation(inId, outId, b))
      return &history[i];
  }
  return &history[0];
}

Evolution::HistoricalMarking *Evolution::MarkingHistory::getMarking(int id) {
  if(id < history.size()) return &history[id];
  return NULL;
}

void Evolution::MarkingHistory::addNodeCreation(int inId, int outId, int nodeId, std::vector<GenoConnection> cons, GenoType *genotype) {
  HistoricalMarking hm(inId, outId, nodeId, genotype);
  for(int i = 0; i < cons.size(); i++) {
    hm.addConnection(cons[i]);
  }


  history.push_back(hm);

}

void Evolution::MarkingHistory::addConnectionCreation(int inId, int outId, GenoConnection con, GenoType *genotype) {
  HistoricalMarking hm(inId, outId, genotype);
  hm.addConnection(con);
  history.push_back(hm);
}

void Evolution::MarkingHistory::clear() {
  history.clear();
}

std::string Evolution::MarkingHistory::toString() {
  std::string res = "";
  for(int i = 0; i < history.size(); i++) {
    res += history[i].toString() + "\n";
  }
  return res;
}
