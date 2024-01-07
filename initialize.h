#ifndef __INITIALIZE_H__
#define __INITIALIZE_H__

#include <set>

#include <3rd/Eigen/Dense>

#include "entity.h"
#include "util.h"
#include "eigen_types.h"

Eigen::MatrixXi generate_initial(size_t n, std::vector<size_t> given = std::vector<size_t>());

MatrixList clustering_nodes(
	std::map<std::string, Node*>& nodes,
	std::set<size_t> excludeNodes,
	std::vector<SingleBill>& orders,
	const size_t& vehicleNum,
	const size_t& pop_size
);

#endif