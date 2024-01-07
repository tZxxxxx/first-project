#ifndef __CROSSOVER_H__
#define __CROSSOVER_H__

#include <map>
#include <string>

#include "eigen_types.h"

void crossover_order(MatrixList& current, std::map<std::string, std::vector<size_t>>& complete_vehicles);

#endif