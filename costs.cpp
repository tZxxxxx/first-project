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
	// ��ö�Ӧ�����ĸ�����������
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
	// �����Ӧ�������Եĳͷ�
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
	// ���ص������������ܳͷ�
	return value;
}

double calculate_distance_cost(
	AvailableVehicle vehicle,
	Eigen::MatrixXi route,
	std::map<int, std::string> nodeIdx2consigneeId,
	std::map<std::string, double> distanceCostMap,
	nlohmann::json dispatchLimit
) {
	// ����Ƿ�����������
	double mileageCoefficent{ 0 };
	for (auto& limit : dispatchLimit) {
		const std::string& id = limit["id"].get<std::string>();
		if (id == "4") {
			mileageCoefficent = c_load["4"].get<int>();
			break;
		}
	}
	double totalCost{ 0 };
	// ����·���ۼӸ�վ����·��
	for(int i = 0; i < route.cols() - 1; i++) {
		const std::string& start = nodeIdx2consigneeId[route(0, i)];
		const std::string& end = nodeIdx2consigneeId[route(0, i+1)];
		std::string key{start};
		key.append("-");
		key.append(end);
		const double& distance = distanceCostMap[key];
		totalCost += distance;
	}
	// ���������������ʱ��ϵ���ͷ�
	totalCost += std::max(0.0, (totalCost - vehicle.max_mileage) * mileageCoefficent);
	return totalCost;
}

double calculate_time_cost() { /* ʱ��ɱ���ʱ����Ҫ���룬���� */
	return 0;
}