#ifndef __GENETIC_FLOW_ROUTE_H__
#define __GENETIC_FLOW_ROUTE_H__

#include <set>
#include <3rd/Eigen/Dense>
#include <3rd/nlohmann/json.hpp>

#include "entity.h"

/* 给定某辆车要跑的站点，以及站点间的路程和时间成本，给出最优路线 */
Eigen::MatrixXi best_route_generic(
	AvailableVehicle vehicle,
	const std::set<int>& indexOfNodes,
	const std::map<int, std::string>& nodeIdx2consigneeId,
	const std::map<std::string, double>& distanceCostMap,
	const std::map<std::string, double>& timeCostMap,
	const nlohmann::json& dispatchLimit,
	double* const final_distance_cost,
	double* const final_time_cost
);

#endif