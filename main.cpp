#include <iostream>
#include <vector>
#include <algorithm>

#include <3rd/dkm/dkm.hpp>
#include <3rd/nlohmann/json.hpp>
#include <3rd/Eigen/Dense>
#include <3rd/Eigen/StdVector>

#include "read_data.h"
#include "entity.h"
#include "initialize.h"
#include "genetic_flow_order.h"
#include "util.h"

#include <random>
#include <fstream>

extern const nlohmann::json c_load;
extern const int MERGED_ORDERS_MAX_ITER;
extern const int ORDER_POP_SIZE;
extern const int ORDER_N_GENERATEION;
extern const int NBR_SHIPBILL;
extern const double VOLUME_MULTIPLIER;

extern nlohmann::json output_json;

int main() {
	std::map<std::string, Node*> nodes; // �����µ�վ��
	std::vector<AvailableVehicle> transporting_vehicles; // ��Ҫ���ȵĳ���
	double min_weight = std::numeric_limits<double>::infinity(); // ���е��ȳ�������������С����Ŀ
	double min_volume = std::numeric_limits<double>::infinity(); // ���е��ȳ������ݻ�����С����Ŀ
	std::map<std::string, double> distance_cost_map; // վ������ɱ�
	std::map<std::string, double> time_cost_map; // վ���ʱ��ɱ�
	nlohmann::json dispatch_litmit; // ������������

	// ��ʼʱ��
	std::time_t startTime;
	std::time(&startTime);

	read_data(
		"100վ������.json",
		//"02.json",
		//"20231017.json",
		&nodes, &transporting_vehicles, &min_weight, &min_volume, &distance_cost_map, &time_cost_map, &dispatch_litmit,
		NBR_SHIPBILL, false, 1e9, VOLUME_MULTIPLIER
	);

	std::map<std::string, int> consigneeId2nodeIdx;
	std::map<int, std::string> nodeIdx2consigneeId;
	int nodeIdx{ 0 };
	for (auto& kv : nodes) {
		const std::string& consigneeId = kv.first;
		consigneeId2nodeIdx[consigneeId] = nodeIdx;
		nodeIdx2consigneeId[nodeIdx] = consigneeId;
		nodeIdx++;
	}
	//std::cout << "min_weight: " << min_weight << std::endl;
	//std::cout << "min_volume: " << min_volume << std::endl;

	//for (auto& k : time_cost_map) {
	//	std::cout << k.first << ": " << k.second << std::endl;
	//}

	//std::cout << dispatch_litmit << std::endl;

	std::time_t now;
	std::time(&now);
	executionStatus("������������Ԥ����׶�", now - startTime, 0, 100);
	// ͬվ��С���������ϲ�
	std::map<std::string, std::array<std::string, 2>> origin_orders; // �ϲ������Ŷ�Ӧ��ԭ�������б�
	auto merged_orders = getSmallOrdersMergedResult(nodes, &origin_orders, min_weight * 0.7, min_volume * 0.7, MERGED_ORDERS_MAX_ITER);
	//for (auto& orders : origin_orders) {
	//	for (auto& kv : orders) {
	//		std::cout << kv.first << ": " << kv.second[0] << " " << kv.second[1] << std::endl;
	//	}
	//}
	//for (auto& order : merged_orders) {
	//	std::cout << order.get_id() << std::endl;
	//}

	size_t rows = transporting_vehicles.size();
	size_t cols = merged_orders.size();

	MatrixList populations;
	bool initialize_success = false;
	if (transporting_vehicles.size() <= nodes.size() - 1) {
		try {
			std::set<size_t> exclude_nodes;
			exclude_nodes.insert(0);
			MatrixList populations_kmeans = clustering_nodes(nodes, exclude_nodes, merged_orders, transporting_vehicles.size(), static_cast<size_t>(ORDER_POP_SIZE));
			//std::cout << "kmeans: " << std::endl;
			//for (auto& p : populations_kmeans) {
			//	std::cout << p << std::endl << std::endl;
			//}
			populations = populations_kmeans;
			initialize_success = true;
			std::cout << "initial by clustering" << std::endl;
		} catch (...) {
			std::cout << "kmeans��ʼ��ʧ�ܣ��л���random��ʼ��" << std::endl;
		}
	}
	if (!initialize_success) {
		MatrixList populations_random;
		for (int i = 0; i < ORDER_POP_SIZE; i++) {
			Eigen::MatrixXi matrix = Eigen::MatrixXi::Zero(rows, cols);
			for (int i = 0; i < cols; i++) {
				Eigen::MatrixXi col = generate_initial(rows);
				matrix.block(0, i, rows, 1) = col;
			}
			populations_random.emplace_back(matrix);
		}
		//std::cout << "random: " << std::endl;
		//for (auto& p : populations_random) {
		//	std::cout << p << std::endl << std::endl;
		//}
		populations = populations_random;
		initialize_success = true;
		std::cout << "initial by random" << std::endl;
	}

	/******** �㷨��ʼ ********/
	std::map<std::pair<std::string, std::string>, FitnessCache> fitness_cache;
	double final_best_fitness;
	Eigen::MatrixXi best_dispatch_orders = best_order_generic(
		populations,
		nodes,
		merged_orders,
		transporting_vehicles,
		dispatch_litmit,
		consigneeId2nodeIdx,
		nodeIdx2consigneeId,
		distance_cost_map,
		time_cost_map,
		&fitness_cache,
		&final_best_fitness
	);
	//std::cout << best_dispatch_orders << std::endl;
	
	/******** �㷨���� ********/
	nlohmann::json requiredVehicles;
	for (int i = 0; i < best_dispatch_orders.rows(); i++) {
		auto dispatch_orders = best_dispatch_orders.row(i);
		if (dispatch_orders.sum() == 0) {
			continue;
		}
		// ��������������Ҫ����
		auto vehicle = transporting_vehicles[i];
		auto vehicleType = vehicle.type_id;
		auto vehiclePlateNumber = vehicle.plate_number;
		if (!requiredVehicles.contains(vehicleType)) {
			nlohmann::json initial;
			initial["plateNumber"] = nlohmann::json::array();
			initial["quantity"] = 0;
			requiredVehicles[vehicleType] = initial;
		}
		auto& vehicleTypeItem = requiredVehicles[vehicleType];
		vehicleTypeItem["plateNumber"].push_back(vehiclePlateNumber);
		vehicleTypeItem["quantity"] = vehicleTypeItem["quantity"].get<int>() + 1;
		// �����������·��ȡ��Ӧԭʼ���������
		nlohmann::json vehicleRouteInfo;
		vehicleRouteInfo["vehicleType"] = vehicleType;
		vehicleRouteInfo["vehiclePlate"] = vehiclePlateNumber;
		std::pair<std::string, std::string> key{ transporting_vehicles[i].type_id, serializeEigenMatrix(dispatch_orders) };
		auto route = fitness_cache[key].best_route;
		std::vector<std::string> originBillIds;
		for (int j = 0; j < dispatch_orders.cols(); j++) {
			if (dispatch_orders(0, j) == 0) {
				continue;
			}
			auto orderId = merged_orders[j].get_id();
			if (origin_orders.count(orderId) != 0) {
				for (auto& billId : origin_orders[orderId]) {
					originBillIds.emplace_back(billId);
				}
			}
			else {
				originBillIds.emplace_back(orderId);
			}
		}
		// ����·��վ��
		nlohmann::json routeInfo;
		for (int j = 0; j < route.cols() - 1; j++) { // ���ֻд����ǰ��һ��
			nlohmann::json singleRouteInfo;
			const size_t& nodeIdx = route(0, j);
			auto consigneeId = nodeIdx2consigneeId[nodeIdx];
			auto node = nodes[consigneeId];
			// ����վ���ж���Ҫ����Ķ���
			const std::vector<std::string>& relatedBillIds = node->get_related_order_ids(originBillIds);
			nlohmann::json geo;
			geo.push_back(std::to_string(node->get_longitude()));
			geo.push_back(std::to_string(node->get_latitude()));
			singleRouteInfo["consigneeId"] = node->get_id();
			singleRouteInfo["deliveryOrder"] = j;
			singleRouteInfo["longitudeLatitude"] = geo;
			singleRouteInfo["shipBillIds"] = relatedBillIds;
			routeInfo.push_back(singleRouteInfo);
		}
		vehicleRouteInfo["consigneeRoute"] = routeInfo;
		output_json["routesInfo"].push_back(vehicleRouteInfo);
	}
	output_json["requiredVehicles"] = requiredVehicles;

	std::time(&now);
	std::ofstream ofs("output.json", std::ios::out);
	if (!ofs.is_open()) {
		std::cout << "��output.jsonʧ��" << std::endl;
		return -1;
	}
	const std::string& content = output_json.dump(4);
	ofs.write(content.c_str(), content.size());
	ofs.close();
	return 0;
}
