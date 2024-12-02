#ifndef __EVOLUTION_GENOCONNECTION__
#define __EVOLUTION_GENOCONNECTION__

#include "Node.hpp"
#include "NodeContainer.hpp"

namespace Evolution {
  struct GenoConnection {
    int inId;
    int outId;
    Evolution::Genes::Node inNode;
    Evolution::Genes::Node outNode;
    double weight;
    bool enabled;
    int innovNo;


    // GenoConnection(int inId, int outId, double weight, bool enabled, int innovNo) :
    //   inId(inId), outId(outId), weight(weight), enabled(enabled), innovNo(innovNo) {
    // }
    GenoConnection(int inId, int outId, double weight, bool enabled, int innovNo) :
      inNode(inNode), outNode(outNode), weight(weight), enabled(enabled), innovNo(innovNo) {
    }

    std::string toString() {
      std::stringstream ss;
      std::string _enabled = enabled ? "1" : "0";
      return   "State: " + _enabled +
        "\n" + "Innov: " + std::to_string(innovNo) +
        "\n" + "In:    " + std::to_string(inId) +
        "\n" + "Out:   " + std::to_string(outId) +
        "\n" + "w:     " + std::to_string(weight);
    }

  };
}

#endif
