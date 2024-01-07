#include <vector>
#include "costs.h"
#include "util.h"

#include "loss_function_route.h"

double objective_function_route(
	const AvailableVehicle& vehicle,
	const Eigen::MatrixXi& route,
	const std::map<int, std::string>& nodeIdx2consigneeId,
	const std::map<std::string, double>& distanceCostMap,
	const nlohmann::json& dispatchLimit
) {
	double distance_cost = calculate_distance_cost(vehicle, route, nodeIdx2consigneeId, distanceCostMap, dispatchLimit);
	return distance_cost;
	//double time_cost = calculate_time_cost();
	//return route_cost + time_cost;
}

std::vector<double> fitness_function_route(
	Eigen::MatrixXi populations,
	const AvailableVehicle& vehicle,
	std::map<std::string, double>& costCache,
	const std::map<int, std::string>& nodeIdx2consigneeId,
	const std::map<std::string, double>& distanceCostMap,
	const nlohmann::json& dispatchLimit
) {
	// 计算每个种群的适应度作为结果返回
	std::vector<double> fitnesses;
	for (int i = 0; i < populations.rows(); i++) {
		// 本种群名称
		auto route = populations.row(i);
		const std::string& name = serializeEigenMatrix(route);
		// 根据路线对适应度计算值进行缓存
		if (costCache.count(name) == 0) {
			costCache[name] = objective_function_route(vehicle, route, nodeIdx2consigneeId, distanceCostMap, dispatchLimit);
		}
		fitnesses.emplace_back(costCache[name]);
	}
	return fitnesses;
}