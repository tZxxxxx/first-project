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
	// ����ÿ����Ⱥ����Ӧ����Ϊ�������
	std::vector<double> fitnesses;
	for (int i = 0; i < populations.rows(); i++) {
		// ����Ⱥ����
		auto route = populations.row(i);
		const std::string& name = serializeEigenMatrix(route);
		// ����·�߶���Ӧ�ȼ���ֵ���л���
		if (costCache.count(name) == 0) {
			costCache[name] = objective_function_route(vehicle, route, nodeIdx2consigneeId, distanceCostMap, dispatchLimit);
		}
		fitnesses.emplace_back(costCache[name]);
	}
	return fitnesses;
}