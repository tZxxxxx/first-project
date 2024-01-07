#include <set>

#include "loss_function_order.h"
#include "util.h"
#include "costs.h"
#include "genetic_flow_route.h"

extern const nlohmann::json c_load;

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
) {
	// ����ÿ����Ⱥ����Ӧ����Ϊ�������
	std::vector<double> fitnesses;
	for (auto& pop : populations) {
		// ����Ⱥ����
		const std::string& name = serializeEigenMatrix(pop);
		// ������������Ҫ�ܵ�վ�㣬����ÿ������loss�������Ϊ����Ⱥ����Ӧ��
		double fitnessesPerPopulation = 0;
		for (size_t i = 0; i < pop.rows(); i++) {
			// ���ݳ���+��Ҫ�ܵ�վ�����Ӧ�ȼ���ֵ���л���
			FitnessCache* pCacheItem;
			const std::string ordersAssignMap = serializeEigenMatrix(pop.row(i));
			std::pair<std::string, std::string> key{ vehicles[i].type_id, ordersAssignMap };
			if (fitnessCache.count(key) == 0) {
				// ��·���滮��ط���
				double load_ratio = 0;
				double constraint_cost = constraint_penalty(orders, ordersAssignMap, vehicles[i], dispatchLimit, c_load, &load_ratio);
				if (std::abs(load_ratio - 1.0) < 1e-9) {
					if (complete_vehicles->count(name) == 0) {
						complete_vehicles->operator[](name) = std::vector<size_t>();
					}
					complete_vehicles->operator[](name).emplace_back(i);
				} else if (load_ratio < 1.0) {
					if (incomplete_vehicles->count(name) == 0) {
						incomplete_vehicles->operator[](name) = std::vector<size_t>();
					}
					incomplete_vehicles->operator[](name).emplace_back(i);
				}
				// ·���滮��ط���
				double distance_cost{ 0 };
				double time_cost{ 0 };
				Eigen::MatrixXi best_route;
				if (constraint_cost < 5e9) {
					int orderIndex = 0;
					std::set<int> indexOfNodes;
					for (auto& order : orders) {
						if (ordersAssignMap[orderIndex] == '0') {
							orderIndex += 2;
							continue;
						}
						std::string consigneeId = order.get_consignee_id();
						indexOfNodes.insert(consigneeId2nodeIdx.at(consigneeId));
						orderIndex += 2;
					}
					best_route = best_route_generic(
						vehicles[i],
						indexOfNodes,
						nodeIdx2consigneeId,
						distanceCostMap,
						timeCostMap,
						dispatchLimit,
						&distance_cost,
						&time_cost
					);
				}
				fitnessCache[key] = FitnessCache{ constraint_cost + distance_cost + time_cost, constraint_cost, distance_cost, time_cost, best_route };
			}
			pCacheItem = &fitnessCache[key];
			fitnessesPerPopulation += pCacheItem->total_cost;
		}
		fitnesses.emplace_back(fitnessesPerPopulation);;
	}
	return fitnesses;
}