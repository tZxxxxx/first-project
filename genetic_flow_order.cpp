#include "genetic_flow_order.h"
#include "loss_function_order.h"
#include "selection.h"
#include "crossover.h"
#include "mutate.h"
#include "util.h"

#include <algorithm>

extern const int ORDER_N_GENERATEION;

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
) {
	// 开始时间
	std::time_t startTime;
	std::time(&startTime);
	// 订单指派种群迭代
	std::vector<double> current_best_fitness;
	std::vector<double> min_fitness_results;
	MatrixList best_dispatch_results;
	std::map<std::string, std::vector<size_t>> complete_vehicles, incomplete_vehicles;
	auto fitnesses = fitness_function_order(
		populations,
		*fitness_cache,
		nodes,
		orders,
		vehicles,
		dispatchLimit,
		consigneeId2nodeIdx,
		nodeIdx2consigneeId,
		distanceCostMap,
		timeCostMap,
		&complete_vehicles,
		&incomplete_vehicles
	);
	std::time_t now;
	std::time(&now);
	executionStatus("运单车辆匹配初始化阶段", now - startTime, 0, 100);
	//std::cout << std::endl << "Initial Fitness: " << std::endl;
	//for (auto& fitness : fitnesses) {
	//	std::cout << fitness << " ";
	//}
	//std::cout << std::endl;
	for (int i = 0; i < ORDER_N_GENERATEION; i++) {
		select_pop_order(populations, fitnesses);
		crossover_order(populations, complete_vehicles);
		mutate_order(populations, incomplete_vehicles);
		fitnesses = fitness_function_order(
			populations,
			*fitness_cache,
			nodes,
			orders,
			vehicles,
			dispatchLimit,
			consigneeId2nodeIdx,
			nodeIdx2consigneeId,
			distanceCostMap,
			timeCostMap,
			&complete_vehicles,
			&incomplete_vehicles
		);
		// 进度估计日志
		std::time(&now);
		const std::time_t used = now - startTime;
		executionStatus("车辆路线规划阶段", used, static_cast<double>(used) / (i + 1) * (ORDER_N_GENERATEION - i), (i + 1.0) / ORDER_N_GENERATEION * 100);
		// 记录loss下降轨迹
		int minIdx = static_cast<int>(std::min_element(fitnesses.begin(), fitnesses.end()) - fitnesses.begin());
		min_fitness_results.emplace_back(fitnesses[minIdx]);
		best_dispatch_results.emplace_back(populations[minIdx]);
		minIdx = static_cast<int>(std::min_element(min_fitness_results.begin(), min_fitness_results.end()) - min_fitness_results.begin());
		current_best_fitness.emplace_back(min_fitness_results[minIdx]);
		// 至少迭代一半且近100次fitness形成的轨迹无变化，认为收敛
		if (current_best_fitness.size() > ORDER_N_GENERATEION / 2 && i > 99) {
			const double& sum = std::accumulate(current_best_fitness.begin() + (i - 99), current_best_fitness.end(), 0.0);
			const double& mean = sum / 100;
			double accum{ 0.0 };
			std::for_each(current_best_fitness.begin() + (i - 99), current_best_fitness.end(), [&](const double d) {
				accum += (d - mean) * (d - mean);
				});
			const double& std = std::sqrt(accum / 99);
			if (static_cast<int>(std) == 0) {
				std::cout << "Break at iter " << i << std::endl;
				break;
			}
		}
		if (i % 10 == 0) {
			std::cout << "finish order generation i=" << i << ", and best fitness " << std::fixed << *std::min_element(current_best_fitness.begin(), current_best_fitness.end()) << std::endl;
		}
	}
	std::cout << "Final Fitnesses:" << std::endl;
	for (auto& fitness : fitnesses) {
		std::cout << fitness << " ";
	}
	// 取历次种群迭代中fitness最佳者
	int minIdx = static_cast<int>(std::min_element(current_best_fitness.begin(), current_best_fitness.end()) - current_best_fitness.begin());
	*final_best_fitness = current_best_fitness[minIdx];
	return best_dispatch_results[minIdx];
}