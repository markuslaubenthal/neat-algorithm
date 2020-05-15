#include "helper.hpp"

int randomInteger(int min, int max) {
  return rand() % (max - min) + min;
}

double randomDouble(double min, double max) {
  return (double)rand() / RAND_MAX * (max - min) + min;
}

bool replaceInString(std::string& str, const std::string& from, const std::string& to) {
  if(from.empty())
      return false;
  size_t start_pos = 0;
  while((start_pos = str.find(from, start_pos)) != std::string::npos) {
      str.replace(start_pos, from.length(), to);
      start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
  }
  return true;
}
