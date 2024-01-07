#ifndef __LOSS_FUNCTION_ORDER_H__
#define __LOSS_FUNCTION_ORDER_H__

#include <map>
#include <vector>
#include <string>

#include <3rd/nlohmann/json.hpp>

#include "eigen_types.h"
#include "entity.h"

struct FitnessCache {
	FitnessCache()
		:total_cost(0), constraint_cost(0), distance_cost(0), time_cost(0),
		    best_route(Eigen::MatrixXi()) {}
	FitnessCache(double total_cost, double constraint_cost, double distance_cost, double time_cost, Eigen::MatrixXi best_route)
		: total_cost(total_cost), constraint_cost(constraint_cost), distance_cost(distance_cost), time_cost(time_cost),
		    best_route(best_route){}
	double total_cost;
	double constraint_cost;
	double distance_cost;
	double time_cost;
	Eigen::MatrixXi best_route;
};

std::vector<double> fitness_function_order(
	const MatrixList& populations,
	std::map<std::pair<std::string, std::string>, FitnessCache>& fitnessCache,
	const std::map<std::string, Node*> nodes,
	const std::vector<SingleBill>& orders,
	const std::vector<AvailableVehicle>& vehicles,
	const nlohmann::json& dispatchLimit,
	const std::map<std::string, int>& consigneeId2nodeIdx,
	const std::map<int, std::string>& nodeIdx2consigneeId,
	const std::map<std::string, double>& distanceCostMap,
	const std::map<std::string, double>& timeCostMap,
	std::map<std::string, std::vector<size_t>>* complete_vehicles,
	std::map<std::string, std::vector<size_t>>* incomplete_vehicles
);

#endif