#ifndef __SELECTION_H__
#define __SELECTION_H__

#include "eigen_types.h"

void select_pop_order(MatrixList& current, const std::vector<double>& fitnesses);

void select_pop_route(Eigen::MatrixXi& current, const std::vector<double>& fitnesses);

#endif