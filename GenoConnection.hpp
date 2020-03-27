#ifndef __EVOLUTION_GENOCONNECTION__
#define __EVOLUTION_GENOCONNECTION__

namespace Evolution {
  struct GenoConnection {
    int inId;
    int outId;
    double weight;
    bool enabled;
    int innovNo;


    GenoConnection(int inId, int outId, double weight, bool enabled, int innovNo) :
      inId(inId), outId(outId), weight(weight), enabled(enabled), innovNo(innovNo) {
    }

    std::string toString() {
      std::stringstream ss;
      return "In:  " + std::to_string(inId) + "\n" + "Out: " + std::to_string(outId) + "\n" + "w:   " + std::to_string(weight);
    }

  };
}

#endif
