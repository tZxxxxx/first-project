#include "mutate.h"
#include "util.h"
#include "initialize.h"
#include "loss_function_route.h"

extern const double RATE_M;

void mutate_order(MatrixList& current, std::map<std::string, std::vector<size_t>>& incomplete_vehicles) {
	for (int i = 0; i < current.size(); i++) {
		// 小于MUTATE阈值才变异
		if (uniform_real_random_in_range(0.0, 1.0) > RATE_M) {
			continue;
		}
		auto& pop = current[i];
		// 选择要派单给哪辆车
		auto key = serializeEigenMatrix(pop);
		auto vehicleNum = static_cast<int>(pop.rows());
		auto orderNum = static_cast<int>(pop.cols());
		Eigen::MatrixXi col;
		if (incomplete_vehicles.count(key) != 0) { // 倾向于派给未装满的车
			auto& given = incomplete_vehicles[key];
			col = generate_initial(vehicleNum, given);
		} else { // 否则随机选一辆
			col = generate_initial(vehicleNum);
		}
		// 选择要改派哪张订单
		const int& colIdx = uniform_integer_random_in_range(0, orderNum - 1);
		//std::cout << "colIdx: " << colIdx << std::endl;
		//std::cout << "before mutate: " << std::endl << pop << std::endl;
		pop.block(0, colIdx, vehicleNum, 1) = col;
		//std::cout << "end mutate: " << std::endl << pop << std::endl << std::endl;
	}
}

void mutate_route(
	Eigen::MatrixXi current,
	const AvailableVehicle& vehicle,
	std::map<std::string, double>& costCache,
	const std::map<int, std::string>& nodeIdx2consigneeId,
	const std::map<std::string, double>& distanceCostMap,
	const nlohmann::json& dispatchLimit
) {
	const int& popSize = static_cast<int>(current.rows());
	const int& nodeNum = static_cast<int>(current.cols());
	for (int i = 0; i < popSize; i++) {
		// 小于MUTATE阈值才变异
		if (uniform_real_random_in_range(0.0, 1.0) > RATE_M) {
			continue;
		}
		// 计算本线路初始cost
		const auto& route = current.row(i);
		double minCost = objective_function_route(
			vehicle,
			route,
			nodeIdx2consigneeId,
			distanceCostMap,
			dispatchLimit
		);
		// k-opt翻转[p,q)间的路径找代价最小的一条
		const int& k = 2;
		Eigen::MatrixXi bestRoute = route;
		for (int p = 1; p < nodeNum - 2; p++) { // 候选翻转站点需要排除起点
			for (int q = p + k; q < nodeNum; q++) {
				// 构造部分翻转后的路径
				Eigen::MatrixXi reversedRoute(1, nodeNum);
				for (int n = 0; n < nodeNum; n++) {
					if (n >= p && n < q) {
						reversedRoute(0, n) = route(0, q - (n - p + 1));
					} else {
						reversedRoute(0, n) = route(0, n);
					}
				}
				//std::cout << "[" <<p << ", " << q << ")" << std::endl;
				//std::cout << "原路径：" << route << std::endl;
				//std::cout << "现路径：" << reversedRoute << std::endl;
				// 评价部分翻转后的路径
				const std::string& key = serializeEigenMatrix(reversedRoute);
				if (costCache.count(key) == 0) {
					const double& cost = objective_function_route(
						vehicle,
						reversedRoute,
						nodeIdx2consigneeId,
						distanceCostMap,
						dispatchLimit
					);
					costCache[key] = cost;
				}
				const double& cost = costCache[key];
				// 找到最小代价的变异路径
				if (cost < minCost) {
					//std::cout << "找到代价更小变异路径，代价 " << minCost << " => " << cost << std::endl;
					//std::cout << "路线从 " << bestRoute << " 更新为 " << reversedRoute << std::endl;
					minCost = cost;
					bestRoute = reversedRoute;
				}
			}
		}
		// 替换成最小代价路径
		//std::cout << "替换成最小代价路径: " << bestRoute << std::endl << std::endl;;
		current.row(i) = bestRoute;
	}
}