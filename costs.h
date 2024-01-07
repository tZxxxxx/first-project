#ifndef __COSTS_H__
#define __COSTS_H__

#include <3rd/nlohmann/json.hpp>
#include <3rd/Eigen/Dense>

#include <string>
#include "entity.h"

// �����������Գɱ�
double constraint_penalty(
	std::vector<SingleBill> orders,
	std::string ordersAssignMap,
	const AvailableVehicle& vehicle,
	nlohmann::json dispatchLimit,
	nlohmann::json constraintCoefficentMap,
	double* loadRatio
);

// ·�̳ɱ�
double calculate_distance_cost(
	AvailableVehicle vehicle,
	Eigen::MatrixXi route,
	std::map<int, std::string> nodeIdx2consigneeId,
	std::map<std::string, double> distanceCostMap,
	nlohmann::json dispatchLimit
);

// ʱ��ɱ�
double calculate_time_cost();

#endif