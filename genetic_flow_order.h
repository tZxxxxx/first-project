#ifndef __GENETIC_FLOW_ORDER_H__
#define __GENETIC_FLOW_ORDER_H__

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include <3rd/nlohmann/json.hpp>

#include "eigen_types.h"
#include "entity.h"
#include "loss_function_order.h"

Eigen::MatrixXi best_order_generic(
	MatrixList& populations,
	std::map<std::string, Node*> nodes,
	std::vector<SingleBill> orders,
	std::vector<AvailableVehicle> vehicles,
	nlohmann::json dispatchLimit,
	std::map<std::string, int> consigneeId2nodeIdx,
	std::map<int, std::string> nodeIdx2consigneeId,
	std::map<std::string, double> distanceCostMap,
	std::map<std::string, double> timeCostMap,
	std::map<std::pair<std::string, std::string>, FitnessCache>* const fitness_cache,
	double* final_best_fitness
);

#endif