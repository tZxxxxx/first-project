#include "mutate.h"
#include "util.h"
#include "initialize.h"
#include "loss_function_route.h"

extern const double RATE_M;

void mutate_order(MatrixList& current, std::map<std::string, std::vector<size_t>>& incomplete_vehicles) {
	for (int i = 0; i < current.size(); i++) {
		// С��MUTATE��ֵ�ű���
		if (uniform_real_random_in_range(0.0, 1.0) > RATE_M) {
			continue;
		}
		auto& pop = current[i];
		// ѡ��Ҫ�ɵ���������
		auto key = serializeEigenMatrix(pop);
		auto vehicleNum = static_cast<int>(pop.rows());
		auto orderNum = static_cast<int>(pop.cols());
		Eigen::MatrixXi col;
		if (incomplete_vehicles.count(key) != 0) { // �������ɸ�δװ���ĳ�
			auto& given = incomplete_vehicles[key];
			col = generate_initial(vehicleNum, given);
		} else { // �������ѡһ��
			col = generate_initial(vehicleNum);
		}
		// ѡ��Ҫ�������Ŷ���
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
		// С��MUTATE��ֵ�ű���
		if (uniform_real_random_in_range(0.0, 1.0) > RATE_M) {
			continue;
		}
		// ���㱾��·��ʼcost
		const auto& route = current.row(i);
		double minCost = objective_function_route(
			vehicle,
			route,
			nodeIdx2consigneeId,
			distanceCostMap,
			dispatchLimit
		);
		// k-opt��ת[p,q)���·���Ҵ�����С��һ��
		const int& k = 2;
		Eigen::MatrixXi bestRoute = route;
		for (int p = 1; p < nodeNum - 2; p++) { // ��ѡ��תվ����Ҫ�ų����
			for (int q = p + k; q < nodeNum; q++) {
				// ���첿�ַ�ת���·��
				Eigen::MatrixXi reversedRoute(1, nodeNum);
				for (int n = 0; n < nodeNum; n++) {
					if (n >= p && n < q) {
						reversedRoute(0, n) = route(0, q - (n - p + 1));
					} else {
						reversedRoute(0, n) = route(0, n);
					}
				}
				//std::cout << "[" <<p << ", " << q << ")" << std::endl;
				//std::cout << "ԭ·����" << route << std::endl;
				//std::cout << "��·����" << reversedRoute << std::endl;
				// ���۲��ַ�ת���·��
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
				// �ҵ���С���۵ı���·��
				if (cost < minCost) {
					//std::cout << "�ҵ����۸�С����·�������� " << minCost << " => " << cost << std::endl;
					//std::cout << "·�ߴ� " << bestRoute << " ����Ϊ " << reversedRoute << std::endl;
					minCost = cost;
					bestRoute = reversedRoute;
				}
			}
		}
		// �滻����С����·��
		//std::cout << "�滻����С����·��: " << bestRoute << std::endl << std::endl;;
		current.row(i) = bestRoute;
	}
}