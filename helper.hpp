#pragma once

#ifndef __HELPER_FUNCTIONS__
#define __HELPER_FUNCTIONS__

#include <cstdlib>
#include <string>


// Integer from [min, max)
int randomInteger(int min, int max);

double randomDouble(double min, double max);

bool replaceInString(std::string& str, const std::string& from, const std::string& to);

#endif
