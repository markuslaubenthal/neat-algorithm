#include "GenoType.hpp"
#include <iostream>
#include <algorithm>

// Evolution::GenoType(std::vector<GenoConnection> connections) :
//   connections(connections) {
//
// }

int Evolution::GenoType::getSizeOfInputNodes() {
  return nodeContainer.inputSize();
}
int Evolution::GenoType::getSizeOfOutputNodes() {
  return nodeContainer.outputSize();
}
int Evolution::GenoType::getSizeOfHiddenNodes() {
  return nodeContainer.hiddenSize();
}

int* Evolution::GenoType::getInnovNo() {
  return innovNo;
}

// std::vector<int> Evolution::GenoType::getNodeIds() {
//   return nodeIds;
// }

Evolution::MarkingHistory* Evolution::GenoType::getMarkingHistory() {
  return markingHistory;
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

    int newNodeId = *innovNo;
    (*innovNo)++;
    nodeContainer.insertHiddenNode(newNodeId);

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
  std::vector<GenoConnection> hmConnections = hm->getConnections();

  int newNodeId = hmConnections[0].outId;

  // if (std::find(nodeIds.begin(), nodeIds.end(), newNodeId) != nodeIds.end())
  // {
  //   nodeIds.push_back(newNodeId);
  // }

  bool nodeExists = false;
  for(int i = 0; i < nodeContainer.size(); i++) {
    if(nodeContainer[i] == newNodeId){
      nodeExists = true;
      break;
    }
  }
  if(!nodeExists) nodeContainer.insertHiddenNode(newNodeId);

  connections[connectionId].enabled = false;
  connections.insert(connections.begin(), hmConnections.begin(), hmConnections.end());
  sortConnections();
}

std::string Evolution::GenoType::toString(bool disabled) {
  int totalNodes = nodeContainer.size();
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

void Evolution::GenoType::setConnection(int inId, int outId, bool enabled, double weight) {
  for(int i = 0; i < connections.size(); i++) {
    if(connections[i].inId == inId && connections[i].outId == outId) {
      setConnection(i, enabled, weight);
      break;
    }
  }
}

std::vector<Evolution::GenoConnection> Evolution::GenoType::getConnections() {
  return connections;
}

void Evolution::GenoType::initNodes(int input, int output) {
  iNodes.clear();
  oNodes.clear();
  hNodes.clear();

  nodeContainer = Evolution::Genes::NodeContainer();

  int noOfNodes = input + output;
  for(int i = 0; i < noOfNodes; i++) {
    if(i < input) nodeContainer.insertInputNode(innovno);
    else nodeContainer.insertOutputNode(innovno);
    (*innovNo)++;
  }

  connections.clear();

  for(auto outIterator : nodeContainer.getOutputNodes()) {
    addConnection(-1, onode + input, randomWeight(), true);
    for(auto inIterator : nodeContainer.getInputNodes()) {
      addConnection((*inTerator).nodeId, (*outIterator).nodeId);
    }
  }

  // for(int onode = 0; onode < output; onode++) {
  //   addConnection(-1, onode + input, randomWeight(), true);
  //   for(int inode = 0; inode < output; inode++) {
  //     addConnection(inode, onode + input, randomWeight(), true);
  //   }
  // }
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

// int Evolution::GenoType::getRandomNonInputId() {
//   return randomInteger(inputNodes, nodeIds.size());
// }

void Evolution::GenoType::mutateConnection() {
  // Add Random connection
  int numberOfNonInputNodes =
    nodeContainer.outputSize() +
    nodeContainer.outputSize();
  int numberOfNodes = nodeContainer.size();

  int inId = nodeContainer.getRandomNodeId();
  int outId = nodeContainer.getRandomNonInputNodeId();
  bool conExists = connectionExists(inId, outId);

  if(inId != outId) {
    if(!conExists) {
      addConnection(inId, outId, randomWeight(), true);
    } else if(inId != -1) {
      toggleConnection(inId, outId);
    }
  }
}

double Evolution::GenoType::randomWeight() {
  return ((double)rand() / RAND_MAX - 0.5) * 20;
}

void Evolution::GenoType::addNode(int conId) {
  GenoConnection con = connections[conId];
  if(connections[conId].inId != -1) {
    if(markingHistory->nodeMarkingExists(con, this)) {
      addNodeAtConnection(conId, markingHistory->getNodeMarking(con, this));
    } else {
      addNodeAtConnection(conId);
    }
  }
}

void Evolution::GenoType::mutateAddNode() {
  if(connections.size() == 0) return;
  int enabledConnections = 0;
  for(int i = 0; i < connections.size(); i++) {
    if(connections[i].enabled && connections[i].inId != -1) {
      enabledConnections++;
    }
  }
  if(enabledConnections == 0) return;

  int conId = randomInteger(0, enabledConnections + 1);
  for(int i = 0; i < connections.size(); i++) {
    if(connections[i].enabled && connections[i].inId != -1) {
      conId--;
    }
    if(conId < 0) {
      addNode(i);
      break;
    }
  }
}

void Evolution::GenoType::randomizeWeights() {
  for(int i = 0; i < connections.size(); i++) {
    connections[i].weight = randomWeight();
  }
}

void Evolution::GenoType::mutateWeights() {

  for(int i = 0; i < connections.size(); i++) {
    // if(connections[i].enabled) {
      double p = (double)rand() / RAND_MAX;
      if(p < 0.1) {
        connections[i].weight = randomWeight();
      } else {
        connections[i].weight += randomDouble(-0.5, 0.5);
      }
    // }
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
    mutateWeights();
  }
}

std::vector<int> Evolution::GenoType::getOutputIdRange() {
  std::vector<int> idRange;
  std::set<Evolution::Genes::Node> outputNodes = nodeContainer.getOutputNodes();
  for(auto it : outputNodes) {
    idRange.push_back((*it).nodeId);
  }
  return idRange;
}

std::set<Evolution::Genes::Node> getOutputNodes() {
  return nodeContainer.getOutputNodes();
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
