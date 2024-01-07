#ifndef __MUTATE_H__
#define __MUTATE_H__

#include <map>
#include <string>
#include <vector>

#include "3rd/nlohmann/json.hpp"

#include "eigen_types.h"
#include "entity.h"

void mutate_order(MatrixList& current, std::map<std::string, std::vector<size_t>>& incomplete_vehicles);

void mutate_route(
	Eigen::MatrixXi current,
	const AvailableVehicle& vehicle,
	std::map<std::string, double>& costCache,
	const std::map<int, std::string>& nodeIdx2consigneeId,
	const std::map<std::string, double>& distanceCostMap,
	const nlohmann::json& dispatchLimit
);

#endif