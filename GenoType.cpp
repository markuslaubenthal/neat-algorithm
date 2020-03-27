#include "GenoType.hpp"

void Evolution::GenoType::addNodeAtConnection(int connectionId) {
  if(connections[connectionId].inId != -1) {
    createHiddenNode();
    int newNodeId = nodes.size() - 1;
    connections[connectionId].enabled = false;
    int inId = connections[connectionId].inId;
    int outId = connections[connectionId].outId;
    HistoricalMarking hm(inId, outId, newNodeId, this);
    markingHistory->push_back(hm);

    addConnection(inId, newNodeId, 1, true);
    addConnection(newNodeId, outId, connections[connectionId].weight, true);
  }
}

void Evolution::GenoType::addNodeAtConnection(int connectionId, HistoricalMarking *hm) {
  std::vector<GenoConnection> hmConnections = hm->getConnections();
  connections.insert(connections.begin(), hmConnections.begin(), hmConnections.end());
  connections[connectionId].enabled = false;
  sortConnections();
}

bool Evolution::GenoType::historicalNodeMarkingExists(int conId) {
  for(int i = 0; i < markingHistory->size(); i++) {
    if(markingHistory[i].isEquivalentNodeCreation(
      connections[conId].inId, connections[conId].outId
    )) return true;
  }
}

Evolution::HistoricalMarking *Evolution::GenoType::getHistoricalNodeMarking(int conId) {
  for(int i = 0; i < markingHistory->size(); i++) {
    if(markingHistory[i].isEquivalentNodeCreation(
      connections[conId].inId, connections[conId].outId
    )) return &markingHistory[i];
  }
  return &markingHistory[0];
}

Evolution::HistoricalMarking *Evolution::GenoType::getHistoricalMarking(int conId) {
  GenoConnection con = connections[conId];
  for(int i = 0; i < markingHistory->size(); i++) {
    if(markingHistory[i].isEquivalentNodeCreation(con.inID, con.outId))
      return &markingHistory[i];
  }
  return &markingHistory[0];
}
