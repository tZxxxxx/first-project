#ifndef __COSTS_H__
#define __COSTS_H__

#include <3rd/nlohmann/json.hpp>
#include <3rd/Eigen/Dense>

#include <string>
#include "entity.h"

// 调度限制属性成本
double constraint_penalty(
	std::vector<SingleBill> orders,
	std::string ordersAssignMap,
	const AvailableVehicle& vehicle,
	nlohmann::json dispatchLimit,
	nlohmann::json constraintCoefficentMap,
	double* loadRatio
);

// 路程成本
double calculate_distance_cost(
	AvailableVehicle vehicle,
	Eigen::MatrixXi route,
	std::map<int, std::string> nodeIdx2consigneeId,
	std::map<std::string, double> distanceCostMap,
	nlohmann::json dispatchLimit
);

// 时间成本
double calculate_time_cost();

#endif