#include <map>
#include <string>
#include <3rd/dkm/dkm.hpp>

#include "util.h"
#include "entity.h"
#include "initialize.h"

// 用于为每个订单随机选取一辆车进行调度
Eigen::MatrixXi generate_initial(size_t n, std::vector<size_t> given) {
	Eigen::MatrixXi col = Eigen::MatrixXi::Zero(n, 1); // 创建n行一列的列向量
	if (given.empty()) { // 随机选一辆车标记占用
		col(uniform_integer_random_in_range(0, n - 1), 0) = 1;
	} else { // 在给定的车辆中选一辆标记占用
		std::vector<int> choiceIdx;
		for (int i = 0; i < given.size(); i++) {
			choiceIdx.emplace_back(i);
		}
		random_unique(choiceIdx.begin(), choiceIdx.end(), 1);
		auto pos = given[choiceIdx[0]];
		col(pos, 0) = 1;
	}
	return col;
}

// 用于根据站点经纬度聚类后，把某类站点的所有订单指派给随机选取的一辆车
std::vector<
	Eigen::MatrixXi,
	Eigen::aligned_allocator<Eigen::MatrixXi>
> clustering_nodes(
	std::map<std::string, Node*>& nodes,
	std::set<size_t> excludeNodes,
	std::vector<SingleBill>& orders,
	const size_t& vehicleNum,
	const size_t& pop_size
) {

	std::vector<std::array<double, 2>> geo_locations;
	std::map<std::string, size_t> consigneeId2nodeIdx;
	std::map<size_t, size_t> locationIdx2nodeIdx;
	size_t nodeNum = 0;
	for (auto& kv : nodes) {
		Node* node = kv.second;
		consigneeId2nodeIdx[node->get_id()] = nodeNum;
		// 按经纬度聚类时剔除某些站点
		if (excludeNodes.count(nodeNum) == 0) {
			locationIdx2nodeIdx[geo_locations.size()] = nodeNum;
			geo_locations.emplace_back(std::array<double, 2>{ node->get_longitude(), node->get_latitude() });
		}
		nodeNum++;
	}
	// 每个订单对应的站点索引
	std::vector<size_t> correspondingNodeIdxForOrders;
	// 每个站点对应的订单索引列表
	std::map<size_t, std::vector<size_t>> nodeIdx2relatedOrderIdx;
	size_t orderNum = 0;
	for (auto& order : orders) {
		size_t correspondingNodeIdx = consigneeId2nodeIdx[order.get_consignee_id()];
		correspondingNodeIdxForOrders.emplace_back(correspondingNodeIdx);
		if (nodeIdx2relatedOrderIdx.count(correspondingNodeIdx) == 0) {
			nodeIdx2relatedOrderIdx[correspondingNodeIdx] = std::vector<size_t>();
		}
		nodeIdx2relatedOrderIdx[correspondingNodeIdx].emplace_back(orderNum);
		orderNum++;
	}
	// 结果矩阵初始化
	std::vector<Eigen::MatrixXi, Eigen::aligned_allocator<Eigen::MatrixXi>> populations;
	for (int i = 0; i < pop_size; i++) {
		Eigen::MatrixXi solution = Eigen::MatrixXi::Zero(vehicleNum, orderNum);
		populations.emplace_back(solution);
	}
	// 根据经纬度对站点进行聚类
	const size_t K = std::min(geo_locations.size(), vehicleNum);
	auto clustered = dkm::kmeans_lloyd(geo_locations, static_cast<uint32_t>(K));
	//std::cout << "Labels: ";
	//for (auto label : std::get<1>(clustered)) {
	//	std::cout << label << "  ";
	//}
	//std::cout << std::endl;
	// 生成pop_size个种群，对应不同的订单指派方案
	for (int i = 0; i < pop_size; i++) {
		auto& pop = populations[i];
		// 从所有车辆中选K辆
		std::vector<int> choiceIdx;
		for (int i = 0; i < vehicleNum; i++) {
			choiceIdx.emplace_back(i);
		}
		random_unique(choiceIdx.begin(), choiceIdx.end(), K);
		// 根据聚类结果把同类站点所有订单指派给对应选取车辆
		int iterNum = 0;
		for (auto& label : std::get<1>(clustered)) {
			const size_t relatedNodeIdx = locationIdx2nodeIdx[iterNum];
			for (auto& orderIdx : nodeIdx2relatedOrderIdx[relatedNodeIdx]) {
				int vehicleIdx = choiceIdx[label];
				pop(vehicleIdx, orderIdx) = 1;
			}
			iterNum++;
		}
	}
	return populations;
}