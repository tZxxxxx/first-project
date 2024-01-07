#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <iostream>
#include <string>
#include <vector>
#include <set>

class SingleBill {
public:
	SingleBill(std::string shipBillId, std::string consigneeId, int packages, double weights, double volumes)
		: ship_bill_id(shipBillId), consignee_id(consigneeId), packages(packages), weights(weights), volumes(volumes) {}
public:
	int get_packages() const { return packages; }
	double get_weights() const { return weights; }
	double get_volumes() const { return volumes; }
	std::string get_id() const { return ship_bill_id; }
	std::string get_consignee_id() const { return consignee_id; }
private:
	std::string ship_bill_id;
	std::string consignee_id;
	int packages;
	double weights;
	double volumes;
};

class Node {
public:
	Node(const std::string& consignee_id, const double& latitude, const double& longitude)
		: consignee_id(consignee_id), latitude(latitude), longitude(longitude),
		  orders({}), orderIdsHere({}),
	      _order_num(0), _demand_packages(0), _demand_weights(0.0), _demand_volumes(0.0) {}
public:
	std::string get_id() const { return consignee_id; }
	double get_latitude() const { return latitude; }
	double get_longitude() const { return longitude; }
	int get_order_num() const { return _order_num; }
	int get_total_packages() const { return _demand_packages; }
	double get_total_weights() const { return _demand_weights; }
	double get_total_volumes() const { return _demand_volumes; }
	void addOrder(const SingleBill& order) {
		orders.emplace_back(order);
		orderIdsHere.insert(order.get_id());
		_order_num += 1;
		_demand_packages += order.get_packages();
		_demand_weights += order.get_weights();
		_demand_volumes += order.get_volumes();
	}
	std::vector<std::string> get_related_order_ids(std::vector<std::string> orderIds) {
		std::vector<std::string> relatedOrderIds;
		for (auto& id : orderIds) {
			if (orderIdsHere.count(id) != 0) {
				relatedOrderIds.emplace_back(id);
			}
		}
		return relatedOrderIds;
	}
	const SingleBill& operator[](int i) { return orders[i]; }
private:
	std::string consignee_id;
	double latitude;
	double longitude;
	std::vector<SingleBill> orders;
	std::set<std::string> orderIdsHere;
	int _order_num;
	int _demand_packages;
	double _demand_weights;
	double _demand_volumes;
};

struct AvailableVehicle {
	AvailableVehicle(std::string type_id, std::string plate_number, int max_p, double max_w, double max_v, double max_load_r, double min_load_r, double max_mileage,
		size_t max_sites, size_t max_orders)
		: type_id(type_id), plate_number(plate_number), max_packages(max_p), max_weights(max_w), max_volumes(max_v),
			max_load_ratio(max_load_r), min_load_ratio(min_load_r), max_mileage(max_mileage),
	          max_sites(max_sites), max_orders(max_orders) {}
	std::string type_id;
	std::string plate_number;
	int max_packages;
	double max_weights;
	double max_volumes;
	double max_load_ratio;
	double min_load_ratio;
	double max_mileage;
	size_t max_sites;
	size_t max_orders;
};

#endif