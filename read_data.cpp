#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <algorithm>
#include <iomanip>

#include <3rd/nlohmann/json.hpp>

#include "read_data.h"
#include "entity.h"
#include "util.h"

extern nlohmann::json output_json;
extern const bool SORTED_VEHICLE_STRATEGY;
extern const double VOLUME_MULTIPLIER;

void addAvailableVehicle(std::string plateNumber, VehicleType* pVehicleType, std::vector<AvailableVehicle>& vs) {
	const std::string& plateNum = utf8_to_gbk(plateNumber);
	int max_p = 1000;
	if (pVehicleType->maxLoadingPcs != "") {
		max_p = std::stoi(pVehicleType->maxLoadingPcs);
	} else {
		std::cout << "该车辆 " << plateNum << " (车型 " << pVehicleType->id << " ) maxLoadingPcs为空，设为1000！" << std::endl;
	}
	double max_w = 1000.0;
	if (pVehicleType->maxLoadingWeight != "") {
		max_w = std::stod(pVehicleType->maxLoadingWeight);
	} else {
		std::cout << "该车辆 " << plateNum << " (车型 " << pVehicleType->id << " ) maxLoadingWeight为空，设为1000！" << std::endl;
	}
	double max_v = 1000.0;
	if (pVehicleType->maxLoadingVolume != "") {
		max_v = std::stod(pVehicleType->maxLoadingVolume);
	} else {
		std::cout << "该车辆 " << plateNum << " (车型 " << pVehicleType->id << " ) maxLoadingVolume为空，设为1000！" << std::endl;
	}
	double max_load_r = 0.95;
	if (pVehicleType->maxLoadingRatio != "") {
		max_load_r = std::stod(pVehicleType->maxLoadingRatio);
	} else {
		std::cout << "该车辆 " << plateNum << " (车型 " << pVehicleType->id << " ) maxLoadingRatio为空，设为0.95！" << std::endl;
	}
	double min_load_r = 0.6;
	if (pVehicleType->minLoadingRatio != "") {
		min_load_r= std::stod(pVehicleType->minLoadingRatio);
	} else {
		std::cout << "该车辆 " << plateNum << " (车型 " << pVehicleType->id << " ) minLoadingRatio为空，设为0.6！" << std::endl;
	}
	double max_mileage = 10000.0;
	if (pVehicleType->maxMileage != "") {
		max_mileage = std::stod(pVehicleType->maxMileage);
	} else {
		std::cout << "该车辆 " << plateNum << " (车型 " << pVehicleType->id << " ) maxMileage为空，设为1000！" << std::endl;
	}
	size_t max_sites = 1000;
	if (pVehicleType->sites != "") {
		max_sites = static_cast<size_t>(std::stoi(pVehicleType->sites));
	} else {
		std::cout << "该车辆 " << plateNum << " (车型 " << pVehicleType->id << " ) sites为空，设为1000！" << std::endl;
	}
	size_t max_orders = 10000;
	if (pVehicleType->maxOrders != "") {
		max_orders = static_cast<size_t>(std::stoi(pVehicleType->maxOrders));
	} else {
		std::cout << "该车辆 " << plateNum << " (车型 " << pVehicleType->id << " ) maxOrders为空，设为10000！" << std::endl;
	}
	vs.emplace_back(AvailableVehicle{ pVehicleType->id, plateNumber, max_p, max_w, max_v, max_load_r, min_load_r, max_mileage, max_sites, max_orders });
}

std::vector<SingleBill> getSmallOrdersMergedResult(
	const std::map<std::string, Node*>& nodes,
	std::map<std::string, std::array<std::string, 2>>* const origin_orders,
	const double& weightLimit,
	const double& volumeLimit,
	const size_t& maxIter
) {
	int totalOrders = 0;
	for (auto& kv : nodes) {
		totalOrders += kv.second->get_order_num();
	}
	const int stopSize = static_cast<int>(totalOrders * 0.4);
	std::vector<SingleBill> mergedOrders;
	for (auto& kv : nodes) {
		Node* node = kv.second;
		std::vector<int> choiceIndex;
		for (int i = 0; i < node->get_order_num(); i++) {
			choiceIndex.emplace_back(i);
		}
		size_t iterCount = 0;
		while (mergedOrders.size() < stopSize && choiceIndex.size() >= 2 && iterCount < maxIter) {
			random_unique(choiceIndex.begin(), choiceIndex.end(), 2);
			const SingleBill& order_i = node->operator[](choiceIndex[0]);
			const SingleBill& order_j = node->operator[](choiceIndex[1]);
			double w_i = order_i.get_weights(), v_i = order_i.get_volumes();
			double w_j = order_j.get_weights(), v_j = order_j.get_volumes();
			if (w_i + w_j < weightLimit && v_i + v_j < volumeLimit) {
				std::string shipBillId = "pseudo___" + order_i.get_id() + "___" + order_j.get_id();
				mergedOrders.emplace_back(SingleBill{
					shipBillId,
					node->get_id(),
					order_i.get_packages() + order_j.get_packages(),
					w_i + w_j,
					v_i + v_j
				});
				std::array<std::string, 2> originShipBillIds{ {order_i.get_id(), order_j.get_id()} };
				origin_orders->operator[](shipBillId) = originShipBillIds;
				// 剔除两份已合并订单的索引
				choiceIndex.erase(choiceIndex.begin());
				choiceIndex.erase(choiceIndex.begin());
			}
			iterCount += 1;
		}
		// 剩余订单原样加入
		for (auto& i : choiceIndex) {
			mergedOrders.emplace_back(node->operator[](i));
		}
	}
	return mergedOrders;
}

int read_data(
	const char* filename,
	std::map<std::string, Node*>* const nodes,
	std::vector<AvailableVehicle>* const transporting_vehicles,
	double* const min_weight,
	double* const min_volume,
	std::map<std::string, double>* const distance_cost_map,
	std::map<std::string, double>* const time_cost_map,
	nlohmann::json* const dispatch_limit,
	const int& shipBillNum,
	const bool& carrierAppointment,
	const int& upperBound,
	const double& volumeMultiplier
) {
	// 开始时间
	std::time_t startTime;
	std::time(&startTime);

	std::ifstream ifs{ filename, std::ios::in };
	nlohmann::json data;
	ifs >> data;

	if (!data.contains("dispatchLimit")) {
		std::cout << "需要勾选至少一项调度限制条件" << std::endl;
		return -1;
	} else if (!data.contains("dispatchModes")) {
		std::cout << "需要勾选一项调度模式" << std::endl;
		return -1;
	} else if (!data.contains("dispatchRules")) {
		std::cout << "需要勾选一项最优调度规则" << std::endl;
		return -1;
	}

	//std::cout << data["pathDistance"];
	*dispatch_limit = data["dispatchLimit"];

	// 保存输入条件
	output_json["dispatchLimit"] = data["dispatchLimit"];
	output_json["dispatchRules"] = data["dispatchRules"];
	if (data.contains("dispatchModes") && data["dispatchModes"].contains("dispatchMode")) {
		output_json["dispatchMode"] = data["dispatchModes"]["dispatchMode"].get<std::string>();
	} else {
		output_json["dispatchMode"] = "2";
	}


	/**************************************************** 订单、站点相关 ****************************************************/

	// 构建consigneeId到latitude/longitude/startTime/endTime的映射
	auto customers = data["customers"]["customerList"].get<std::vector<Customer>>();
	std::map<std::string, std::array<std::string, 4>> mConsignees;
	for (auto& customer : customers) {
		for (auto& c : customer.consignees) {
			mConsignees[c.id] = std::array<std::string, 4>{ c.latitude, c.longitude, c.startTime, c.endTime };
		}
	}
	//for (auto& k : mConsignees) {
	//	std::cout << k.second[0] << ", " << k.second[1] << ", " << k.second[2] << ", " << k.second[3] << std::endl;
	//}

	// 首个必须添加的站点信息
	{
		auto site = data["branchs"][0];
		const std::string consigneeId = site["id"].get<std::string>();
		Node* node = new Node{
			consigneeId,
			std::stod(site["latitude"].get<std::string>()),
			std::stod(site["longitude"].get<std::string>()),
		};
		nodes->operator[](consigneeId) = node;
	}

	// 读取订单，建立相应站点信息
	const size_t maxBillNum = std::min(static_cast<size_t>(shipBillNum), data["shipbills"].size());
	for (size_t i = 0; i < maxBillNum; i++) {
		auto shipBill = data["shipbills"][i];
		std::string consigneeId = shipBill["consigneeId"].get<std::string>();
		SingleBill order{
			shipBill["shipbillId"].get<std::string>(),
			consigneeId,
			std::stoi(shipBill["totalPcs"].get<std::string>()),
			std::stod(shipBill["totalWeight"].get<std::string>()),
			std::stod(shipBill["totalVolume"].get<std::string>())
		};
		if (nodes->count(consigneeId)) {
			Node* node = nodes->operator[](consigneeId);
			node->addOrder(order);
		} else {
			Node* node = new Node{
				consigneeId,
				std::stod(std::get<0>(mConsignees[consigneeId])),
				std::stod(std::get<1>(mConsignees[consigneeId]))
			};
			nodes->operator[](consigneeId) = node;
			node->addOrder(order);
		}
		//std::cout << "order " << order.get_id() << ": [packges]" << order.get_packages() << ", [weights]" << order.get_weights() << ", [volumes]" << order.get_volumes() << std::endl;
		//std::cout << "consignee " << consigneeId << ": [packages]" << nodes[consigneeId]->get_total_packages() << ", [weights]" << nodes[consigneeId]->get_total_weights() << ", [volumes]" << nodes[consigneeId]->get_total_volumes() << std::endl;
	}

	/**************************************************** 车辆调度相关 ****************************************************/

	// 统计所有订单收货方的件数、重量、体积等信息
	double totalWeights = 0.0;
	double totalVolumes = 0.0;
	size_t totalPackages = 0;
	size_t totalOrders = 0;
	size_t totalConsignees = 0;
	for (auto& kv : *nodes) {
		auto node = kv.second;
		totalWeights += node->get_total_weights();
		totalVolumes += node->get_total_volumes();
		totalPackages += node->get_total_packages();
		totalOrders += node->get_order_num();
		totalConsignees++;
	}
	std::cout << "订单统计={'total_packages': " << std::fixed << std::setprecision(1) << totalPackages  << ", ";
	std::cout << "'total_volumes': " << totalVolumes << ", ";
	std::cout << "'total_weights': " << totalWeights << ", ";
	std::cout << "'total_consignees': " << totalConsignees << ", ";
	std::cout << "'total_orders': " << totalOrders << "}" << std::endl;

	std::time_t now;
	std::time(&now);
	executionStatus("读取订单数据", now - startTime, 0, 100);

	// 读取车辆相关输入信息
	auto vehicles = data["vehicle"].get<std::vector<Vehicle>>();
	auto vehicleTypes = data["vehicleTypes"].get<std::vector<VehicleType>>();
	std::time(&now);
	executionStatus("导入车辆信息", now - startTime, 0, 100);

	// 提取站点间距离、时间成本
	for (auto& routes : data["pathDistances"]) {
		for (auto& kv : routes.get<std::map<std::string, std::string>>()) {
			std::regex pattern{ "([^-]+)-([^-]+)-([^-]+)-([^-]+)" };
			std::smatch res;
			bool match = std::regex_match(kv.first, res, pattern);
			if (!match || res.size() < 5) continue;
			std::string key{ res[2].str() + "-" + res[4].str() };
			double distance = std::stod(kv.second);
			distance_cost_map->operator[](key) = distance;
			time_cost_map->operator[](key) = distance / 50 * 3600;
		}
	}
	std::time(&now);
	executionStatus("导入距离时间信息", now - startTime, 0, 100);

	// 构建vehicleTypeId到VehicleType的映射
	std::map<std::string, VehicleType*> mVehicleTypes;
	for (auto& vType : vehicleTypes) {
		mVehicleTypes[vType.id] = &vType;
	}
	// 准备可调度车辆列表
	std::vector<AvailableVehicle> availableVehicles;
	// 假如有具体车辆，按具体车辆调度
	if (vehicles.size()) {
		for (auto vehicle : vehicles) {
			// 只处理不指定承运商的情况，要处理时根据具体车辆的vehicle.affiliationId做筛选
			if (mVehicleTypes.count(vehicle.vehicleTypeId) == 0) {
				std::cout << "车辆" << vehicle.plateNumber << "的车型id " << vehicle.vehicleTypeId << "不存在给定数据中，请检查输入数据完整性" << std::endl;
			} else {
				auto pVehicleType = mVehicleTypes[vehicle.vehicleTypeId];
				addAvailableVehicle(vehicle.plateNumber, pVehicleType, availableVehicles);
			}
		}
	} else { // 假如没有具体车辆，按车型和对应可用数量调度
		for (auto& vType : vehicleTypes) {
			int k = 0;
			std::string plateNumberPrefix{ vType.id };
			plateNumberPrefix.append("-pseudo-");

			int usableCount = 5;
			if (vType.usableCount != "") {
				usableCount = std::stoi(vType.usableCount);
			} else {
				std::cout << "车型 " << vType.id << " usableCount为空，设为5！" << std::endl;
			}

			for (int i = 0; i < usableCount; i++) {
				auto plateNumber = plateNumberPrefix + std::to_string(k);
				addAvailableVehicle(plateNumber, &vType, availableVehicles);
				k += 1;
			}
		}
	}
	// 把可调度车辆按容积大小排序，升降序策略由SORTED_VEHICLE_STRATEGY全局配置项指定，默认为false，从小到大升序排列
	std::sort(availableVehicles.begin(), availableVehicles.end(), [](auto a, auto b) {
		if (SORTED_VEHICLE_STRATEGY) {
			return a.max_volumes > b.max_volumes;
		} else {
			return a.max_volumes < b.max_volumes;
		}
	});
	// 根据订单总件数、重量、体积信息选取要调度的最少车辆调度数目，可能存在车辆不够，一辆车被调度多次的情况
	double weights_avail = 0, volumes_avail = 0;
	const size_t& totalAvailableVehicles = availableVehicles.size();
	const size_t& totalWeightsAfterMultiplier = static_cast<size_t>(VOLUME_MULTIPLIER * totalWeights);
	const size_t& totalVolumesAfterMultiplier = static_cast<size_t>(VOLUME_MULTIPLIER * totalVolumes);
	for (;;) {
		const size_t index = transporting_vehicles->size() % totalAvailableVehicles;
		AvailableVehicle& v = availableVehicles[index];
		transporting_vehicles->emplace_back(v);
		weights_avail += v.max_weights * v.max_load_ratio;
		volumes_avail += v.max_volumes * v.max_load_ratio;
		if (v.max_weights < *min_weight) {
			*min_weight = v.max_weights;
		}
		if (v.max_volumes < *min_volume) {
			*min_volume = v.max_volumes;
		}
		if (weights_avail >= totalWeightsAfterMultiplier && volumes_avail >= totalVolumesAfterMultiplier) {
			break;
		}
	}

	//// 查看调度情况
	//std::cout << "Scheduled: " << transportingVehicles.size() << std::endl;
	//for (auto& v : transportingVehicles) {
	//	std::cout << v.plate_number << std::endl;
	//}

	std::time(&now);
	executionStatus("初始化车辆数目", now - startTime, 0, 100);
	// 先初始化最终调度车辆结果
	output_json["requiredVehicles"] = nlohmann::json::object();

	return 0;
}