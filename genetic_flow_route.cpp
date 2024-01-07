#include <map>
#include <vector>

#include "util.h"
#include "genetic_flow_route.h"
#include "loss_function_route.h"
#include "selection.h"
#include "crossover.h"
#include "mutate.h"

extern const int ROUTE_POP_SIZE;
extern const int ROUTE_N_GENERATEION;

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
) {
	int routePopSize{ ROUTE_POP_SIZE };
	int routeNGeneration{ ROUTE_N_GENERATEION };
	const size_t& nodeNum = indexOfNodes.size();
	if (nodeNum > 5) {
		routePopSize *= 2;
		routeNGeneration *= 2;
	}
	// 随机初始化线路作为初代种群
	std::vector<int> route{};
	route.assign(indexOfNodes.begin(), indexOfNodes.end());
	Eigen::MatrixXi populations(routePopSize, nodeNum + 2); // 需要从起点出发且回到起点
	for (int i = 0; i < routePopSize; i++) {
		random_unique(route.begin(), route.end(), nodeNum);
		for (int j = 0; j < nodeNum; j++) {
			populations(i, j + 1) = route[j];
		}
		populations(i, 0) = 0;
		populations(i, nodeNum + 1) = 0;
	}
	//std::cout << populations << std::endl << std::endl;
	
	// 路线规划种群迭代
	std::vector<double> current_best_fitness;
	std::vector<double> min_fitness_results;
	MatrixList best_route_results;
	std::map<std::string, double> costCache;
	auto fitnesses = fitness_function_route(
		populations,
		vehicle,
		costCache,
		nodeIdx2consigneeId,
		distanceCostMap,
		dispatchLimit
	);

	//std::cout << std::endl << "Initial Route Fitness: " << std::endl;
	//for (auto& fitness : fitnesses) {
	//	std::cout << fitness << " ";
	//}
	//std::cout << std::endl;

	for (int i = 0; i < routeNGeneration; i++) {
		select_pop_route(populations, fitnesses);
		// crossover_route(population); // 不存在子路径时等同于原输入，跳过
		mutate_route(populations, vehicle, costCache, nodeIdx2consigneeId, distanceCostMap, dispatchLimit);
		fitnesses = fitness_function_route(
			populations,
			vehicle,
			costCache,
			nodeIdx2consigneeId,
			distanceCostMap,
			dispatchLimit
		);
		// 记录loss下降轨迹
		size_t minIdx = std::min_element(fitnesses.begin(), fitnesses.end()) - fitnesses.begin();
		min_fitness_results.emplace_back(fitnesses[minIdx]);
		best_route_results.emplace_back(populations.row(minIdx));
		minIdx = static_cast<int>(std::min_element(min_fitness_results.begin(), min_fitness_results.end()) - min_fitness_results.begin());
		current_best_fitness.emplace_back(min_fitness_results[minIdx]);
		// 至少迭代一半且近100次fitness形成的轨迹无变化，认为收敛
		if (current_best_fitness.size() > ROUTE_N_GENERATEION / 2 && i > 99) {
			auto start = current_best_fitness.begin() + (i - 99);
			auto end = current_best_fitness.end();
			const double& sum = std::accumulate(start, end, 0.0);
			const double& mean = sum / current_best_fitness.size();
			double accum{ 0.0 };
			std::for_each(start, end, [&](const double d) {
				accum += (d - mean) * (d - mean);
				});
			const double& std = std::sqrt(accum / 99);
			if (static_cast<int>(std) == 0) {
				std::cout << "Break at iter " << i << std::endl;
				break;
			}
		}
	}

	//std::cout << std::endl << "Final Route Fitness: " << std::endl;
	//for (auto& fitness : fitnesses) {
	//	std::cout << fitness << " ";
	//}
	//std::cout << std::endl;

	// 取最佳路线返回
	size_t minIdx = std::min_element(current_best_fitness.begin(), current_best_fitness.end()) - current_best_fitness.begin();
	auto& best_route = best_route_results[minIdx];
	*final_distance_cost = costCache[serializeEigenMatrix(best_route)]; // 总成本目前仅路程成本
	*final_time_cost = 0; // 时间成本暂不计算
	return best_route;
}