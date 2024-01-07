#include <map>
#include <string>
#include <3rd/dkm/dkm.hpp>

#include "util.h"
#include "entity.h"
#include "initialize.h"

// ����Ϊÿ���������ѡȡһ�������е���
Eigen::MatrixXi generate_initial(size_t n, std::vector<size_t> given) {
	Eigen::MatrixXi col = Eigen::MatrixXi::Zero(n, 1); // ����n��һ�е�������
	if (given.empty()) { // ���ѡһ�������ռ��
		col(uniform_integer_random_in_range(0, n - 1), 0) = 1;
	} else { // �ڸ����ĳ�����ѡһ�����ռ��
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

// ���ڸ���վ�㾭γ�Ⱦ���󣬰�ĳ��վ������ж���ָ�ɸ����ѡȡ��һ����
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
		// ����γ�Ⱦ���ʱ�޳�ĳЩվ��
		if (excludeNodes.count(nodeNum) == 0) {
			locationIdx2nodeIdx[geo_locations.size()] = nodeNum;
			geo_locations.emplace_back(std::array<double, 2>{ node->get_longitude(), node->get_latitude() });
		}
		nodeNum++;
	}
	// ÿ��������Ӧ��վ������
	std::vector<size_t> correspondingNodeIdxForOrders;
	// ÿ��վ���Ӧ�Ķ��������б�
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
	// ��������ʼ��
	std::vector<Eigen::MatrixXi, Eigen::aligned_allocator<Eigen::MatrixXi>> populations;
	for (int i = 0; i < pop_size; i++) {
		Eigen::MatrixXi solution = Eigen::MatrixXi::Zero(vehicleNum, orderNum);
		populations.emplace_back(solution);
	}
	// ���ݾ�γ�ȶ�վ����о���
	const size_t K = std::min(geo_locations.size(), vehicleNum);
	auto clustered = dkm::kmeans_lloyd(geo_locations, static_cast<uint32_t>(K));
	//std::cout << "Labels: ";
	//for (auto label : std::get<1>(clustered)) {
	//	std::cout << label << "  ";
	//}
	//std::cout << std::endl;
	// ����pop_size����Ⱥ����Ӧ��ͬ�Ķ���ָ�ɷ���
	for (int i = 0; i < pop_size; i++) {
		auto& pop = populations[i];
		// �����г�����ѡK��
		std::vector<int> choiceIdx;
		for (int i = 0; i < vehicleNum; i++) {
			choiceIdx.emplace_back(i);
		}
		random_unique(choiceIdx.begin(), choiceIdx.end(), K);
		// ���ݾ�������ͬ��վ�����ж���ָ�ɸ���Ӧѡȡ����
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