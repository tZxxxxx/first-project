#ifndef __LOSS_FUNCTION_ROUTE_H__
#define __LOSS_FUNCTION_ROUTE_H__

#include <map>
#include <string>

#include <3rd/nlohmann/json.hpp>

#include "eigen_types.h"
#include "entity.h"

double objective_function_route(
	const AvailableVehicle& vehicle,
	const Eigen::MatrixXi& route,
	const std::map<int, std::string>& nodeIdx2consigneeId,
	const std::map<std::string, double>& distanceCostMap,
	const nlohmann::json& dispatchLimit
);

std::vector<double> fitness_function_route(
	Eigen::MatrixXi populations,
	const AvailableVehicle& vehicle,
	std::map<std::string, double>& costCache,
	const std::map<int, std::string>& nodeIdx2consigneeId,
	const std::map<std::string, double>& distanceCostMap,
	const nlohmann::json& dispatchLimit
);
#endif