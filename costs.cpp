#include "costs.h"

extern const nlohmann::json c_load;

double constraint_penalty(
	std::vector<SingleBill> orders,
	std::string ordersAssignMap,
	const AvailableVehicle& vehicle,
	nlohmann::json dispatchLimit,
	nlohmann::json constraintCoefficentMap,
	double* loadRatio
) {
	double value{ 0.0 };
	// 获得对应订单的各属性需求量
	double volumeNeed = 0, weightNeed = 0;
	int orderIndex = 0;
	for (auto& order : orders) {
		if (ordersAssignMap[orderIndex] == '0') {
			orderIndex += 2;
			continue;
		}
		volumeNeed += order.get_volumes();
		weightNeed += order.get_weights();
		orderIndex += 2;
	}
	// 计算对应限制属性的惩罚
	for (auto& limit : dispatchLimit) {
		const std::string& constraintId = limit["id"].get<std::string>();
		if (constraintId == "1") {
			const int& coefficent = constraintCoefficentMap[constraintId].get<int>();
			value += std::max(0.0, volumeNeed - vehicle.max_volumes) * coefficent;
			const double& ratio = volumeNeed / vehicle.max_volumes;
			if (ratio > *loadRatio) {
				*loadRatio = ratio;
			}
		}
		else if (constraintId == "2") {
			const int& coefficent = constraintCoefficentMap[constraintId].get<int>();
			value += std::max(0.0, weightNeed - vehicle.max_weights) * coefficent;
			const double& ratio = weightNeed / vehicle.max_weights;
			if (ratio > *loadRatio) {
				*loadRatio = ratio;
			}
		}
	}
	// 返回调度限制属性总惩罚
	return value;
}

double calculate_distance_cost(
	AvailableVehicle vehicle,
	Eigen::MatrixXi route,
	std::map<int, std::string> nodeIdx2consigneeId,
	std::map<std::string, double> distanceCostMap,
	nlohmann::json dispatchLimit
) {
	// 检查是否包含里程限制
	double mileageCoefficent{ 0 };
	for (auto& limit : dispatchLimit) {
		const std::string& id = limit["id"].get<std::string>();
		if (id == "4") {
			mileageCoefficent = c_load["4"].get<int>();
			break;
		}
	}
	double totalCost{ 0 };
	// 根据路线累加各站点间的路程
	for(int i = 0; i < route.cols() - 1; i++) {
		const std::string& start = nodeIdx2consigneeId[route(0, i)];
		const std::string& end = nodeIdx2consigneeId[route(0, i+1)];
		std::string key{start};
		key.append("-");
		key.append(end);
		const double& distance = distanceCostMap[key];
		totalCost += distance;
	}
	// 超出车辆里程限制时按系数惩罚
	totalCost += std::max(0.0, (totalCost - vehicle.max_mileage) * mileageCoefficent);
	return totalCost;
}

double calculate_time_cost() { /* 时间成本暂时不需要纳入，留空 */
	return 0;
}