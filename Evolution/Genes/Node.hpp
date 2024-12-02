#ifndef __EVOLUTION_GENES_NODE__
#define __EVOLUTION_GENES_NODE__

namespace Evolution {
  namespace Genes {
    enum NodeType { input, hidden, output };
    struct Node {
      int nodeId;
      NodeType nodeType;
      Node() {}
      Node(int nodeId, NodeType nodeType) : nodeId(nodeId), nodeType(nodeType) {

      }
      bool operator==(const Node& rhs) const {
        return nodeId == rhs.nodeId;
      }
      bool operator<(const Node& rhs) const {
        return nodeId < rhs.nodeId;
      }
      bool operator>(const Node& rhs) const {
        return nodeId > rhs.nodeId;
      }
      bool operator<=(const Node& rhs) const {
        return nodeId <= rhs.nodeId;
      }
      bool operator>=(const Node& rhs) const {
        return nodeId >= rhs.nodeId;
      }
    };

    inline bool operator<(const Node& lhs, const int& rhs)
    {
      return lhs.nodeId < rhs;
    }
    inline bool operator>(const Node& lhs, const int& rhs)
    {
      return lhs.nodeId < rhs;
    }
    inline bool operator==(const Node& lhs, const int& rhs)
    {
      return lhs.nodeId == rhs;
    }
  }
}

#endif
