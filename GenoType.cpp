#include "GenoType.hpp"
#include <iostream>
int Evolution::GenoType::getSizeOfInputNodes() {
  return inputNodes;
}
int Evolution::GenoType::getSizeOfOutputNodes() {
  return outputNodes;
}

void Evolution::GenoType::addConnection(int inId, int outId, double weight, bool enabled, bool createHistoricalMarking) {

  if(markingHistory->connectionMarkingExists(inId, outId, this)) {
    HistoricalMarking *hm = markingHistory->getConnectionMarking(inId, outId, this);
    std::vector<GenoConnection> cons = hm->getConnections();
    connections.insert(connections.begin(), cons.begin(), cons.end());
    sortConnections();
  } else {

    GenoType *backup = new GenoType(*this);

    GenoConnection con(inId, outId, weight, enabled, *innovNo);
    (*innovNo)++;
    connections.push_back(con);
    if(createHistoricalMarking) {
      markingHistory->addConnectionCreation(inId, outId, con, backup);
    }
  }
}

void Evolution::GenoType::addNodeAtConnection(int conId) {
  if(connections[conId].inId != -1) {

    hiddenNodes++;
    int newNodeId = *innovNo;
    (*innovNo)++;

    connections[conId].enabled = false;
    int inId = connections[conId].inId;
    int outId = connections[conId].outId;
    // HistoricalMarking hm(inId, outId, newNodeId, this);

    GenoType *backup = new GenoType(*this);

    addConnection(inId, newNodeId, 1, true, false);
    addConnection(-1, newNodeId, randomWeight(), true, false);
    addConnection(newNodeId, outId, connections[conId].weight, true, false);

    std::vector<GenoConnection> added_connections(connections.end() - 3, connections.end());
    markingHistory->addNodeCreation(inId, outId, newNodeId, added_connections, backup);
  }
}

void Evolution::GenoType::addNodeAtConnection(int connectionId, HistoricalMarking *hm) {
  hiddenNodes++;
  std::vector<GenoConnection> hmConnections = hm->getConnections();

  connections.insert(connections.begin(), hmConnections.begin(), hmConnections.end());
  connections[connectionId].enabled = false;
  sortConnections();
}

std::string Evolution::GenoType::toString(bool disabled) {
  int totalNodes = inputNodes + outputNodes + hiddenNodes;
  std::string line1 = "Number of Nodes: " + std::to_string(totalNodes);
  std::string s_connections = "";
  for(int i = 0; i < connections.size(); i++) {
    GenoConnection con = connections[i];
    if(con.enabled || disabled) {
      s_connections += "Index: " + std::to_string(i) + "\n" + con.toString() + "\n--------\n";
    }
  }
  return line1 + "\n" + s_connections;
}

void Evolution::GenoType::setConnection(int index, bool enabled, double weight) {
  connections[index].enabled = enabled;
  connections[index].weight = weight;
}

std::vector<Evolution::GenoConnection> Evolution::GenoType::getConnections() {
  return connections;
}

void Evolution::GenoType::setInputAndOutputNodes(int input, int output) {
  inputNodes = input;
  outputNodes = output;
  hiddenNodes = 0;

  nodeIdMap.clear();
  for(int i = 0; i < input + output; i++) {
    nodeIdMap[*innovNo] = i;
    (*innovNo)++;
  }

  connections.clear();

  for(int onode = 0; onode < outputNodes; onode++) {
    addConnection(-1, onode + inputNodes, randomWeight(), true);
    for(int inode = 0; inode < inputNodes; inode++) {
      addConnection(inode, onode + inputNodes, randomWeight(), true);
    }
  }

}

bool Evolution::GenoType::connectionExists(int inId, int outId) {
  for(int i = 0; i < connections.size(); i++) {
    if(connections[i].inId == inId && connections[i].outId == outId) return true;
  }
  return false;
}

void Evolution::GenoType::sortConnections() {
  std::sort(connections.rbegin(), connections.rend(),
    [](auto const &a, auto const &b) { return a.innovNo > b.innovNo; });
}

void Evolution::GenoType::toggleConnection(int inId, int outId) {
  for(int i = 0; i < connections.size(); i++) {
    if(connections[i].inId == inId && connections[i].outId == outId) {
      connections[i].enabled = !connections[i].enabled;
      return;
    }
  }
}

int Evolution::GenoType::getRandomNonInputId() {
  std::map<int,int>::iterator it = nodeIdMap.begin();
  std::advance(it, randomInteger(inputNodes, nodeIdMap.size()));
  return it->second;
}

void Evolution::GenoType::mutateConnection() {
  // Add Random connection
  int numberNonMutatableNodes = inputNodes + outputNodes;

  int inId = getRandomNonInputId();
  int outId = getRandomNonInputId();

  bool conExists = connectionExists(inId, outId);

  if(!conExists) {
    addConnection(inId, outId, randomWeight(), true);
  } else {
    toggleConnection(inId, outId);
  }
}

double Evolution::GenoType::randomWeight() {
  return ((double)rand() / RAND_MAX - 0.5) * 5;
}

void Evolution::GenoType::addNode(int conId) {
  GenoConnection con = connections[conId];
  if(markingHistory->nodeMarkingExists(con, this)) {
    addNodeAtConnection(conId, markingHistory->getNodeMarking(con, this));
  } else {
    addNodeAtConnection(conId);
  }
}

void Evolution::GenoType::mutateAddNode() {
  if(connections.size() == 0) return;
  int conId = rand() % connections.size();
  if(connections[conId].enabled) {
    addNode(conId);
  }
}

void Evolution::GenoType::mutateWeight() {

  double p = (double)rand() / RAND_MAX;

  if(connections.size() == 0) return;
  int conId = rand() % (connections.size());

  if(connections[conId].enabled) {
    if(p < 0.1) {
      connections[conId].weight = randomWeight();
    } else {
      connections[conId].weight += randomDouble(-0.5, 0.5);
    }
  }
}

void Evolution::GenoType::mutate() {
  double p;
  p = (double)rand() / RAND_MAX;
  if(p < 0.03) {
    mutateAddNode();
  }
  p = (double)rand() / RAND_MAX;
  if(p < 0.05) {
    mutateConnection();
  }
  p = (double)rand() / RAND_MAX;
  if(p < 0.8) {
    mutateWeight();
  }
}

std::vector<int> Evolution::GenoType::getOutputIdRange() {
  std::vector<int> idRange;
  idRange.push_back(inputNodes);
  idRange.push_back(inputNodes + outputNodes);
  return idRange;
}



Evolution::GenoType *Evolution::GenoType::clone() {
  GenoType *c = new GenoType(connections);
  return c;
}

bool Evolution::GenoType::isEqual(Evolution::GenoType b) {
  if(b.connections.size() == connections.size()) {
    for(int i = 0; i < connections.size(); i++) {
      if(connections[i].innovNo != b.connections[i].innovNo) return false;
    }
    return true;
  }
  return false;
}
