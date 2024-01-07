#ifndef __READ_DATA_H__
#define __READ_DATA_H__

#include <3rd/nlohmann/json.hpp>
#include "entity.h"

int read_data(
	const char* filename,
	std::map<std::string, Node*>* const nodes,
	std::vector<AvailableVehicle>* const transporting_vehicles,
	double* const min_weight,
	double* const min_volume,
	std::map<std::string, double>* const distance_cost_map,
	std::map<std::string, double>* const time_cost_map,
	nlohmann::json* const dispatch_limit,
	const int& shipBillNum = 50,
	const bool& carrierAppointment = false,
	const int& upperBound = 1e9,
	const double& volumeMultiplier = 1.2
);

std::vector<SingleBill> getSmallOrdersMergedResult(
	const std::map<std::string, Node*>& nodes,
	std::map<std::string, std::array<std::string, 2>>* const originOrders,
	const double& weightLimit,
	const double& volumeLimit,
	const size_t& maxIter = 500
);

struct DispatchDesc {
	std::string code;
	std::string id;
	std::string name;
	std::string remark;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(DispatchDesc, code, id, name, remark)
};

struct DispatchModes {
	std::string dispatchMode;
	std::string remark;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(DispatchModes, dispatchMode, remark)
};

struct BasicRegistrationInfo {
	std::string address;
	std::string city;
	std::string cityName;
	std::string code;
	std::string contact;
	std::string district;
	std::string districtName;
	std::string email;
	std::string id;
	std::string landlineNumber;
	std::string name;
	std::string phoneNumber;
	std::string province;
	std::string provinceName;
};

struct Consignee : BasicRegistrationInfo {
	std::string customerCode;
	std::string customerId;
	std::string customerName;
	std::string loadingTime;
	std::string latitude;
	std::string longitude;
	std::string relationPhoneNumber;
	std::string startTime;
	std::string endTime;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Consignee, address, city, cityName, code, contact, district, districtName, email, id, landlineNumber,
		name, phoneNumber, province, provinceName, customerCode, customerId, customerName, loadingTime, latitude, longitude, relationPhoneNumber,
		startTime, endTime)
};

struct Customer : BasicRegistrationInfo {
	std::string postCode;
	std::vector<Consignee> consignees;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Customer, address, city, cityName, code, contact, district, districtName, email, id, landlineNumber,
		name, phoneNumber, province, provinceName, postCode, consignees)
};

struct ShipBillDetail {
	std::string cargoThermosphere;
	std::string id;
	std::string itemCategoryCode;
	std::string itemCategoryId;
	std::string	itemCategoryName;
	std::string itemCode;
	std::string itemId;
	std::string itemName;
	std::string packType;
	std::string pcs;
	std::string seqno;
	std::string shipbillId;
	std::string shipbillNo;
	std::string unitVolume;
	std::string unitWeight;
	std::string volume;
	std::string weight;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ShipBillDetail, cargoThermosphere, id, itemCategoryCode, itemCategoryId, itemCategoryName, itemCode, itemId, itemName,
		packType, pcs, seqno, shipbillId, shipbillNo, unitVolume, unitWeight, volume, weight)
};

struct ShipBill {
	std::string branchCode;
	std::string branchId;
	std::string branchName;
	std::string cargoOwnerCode;
	std::string cargoOwnerId;
	std::string cargoOwnerName;
	std::string carrierCode;
	std::string carrierId;
	std::string carrierName;
	std::string carrierType;
	std::string consigneeAddress;
	std::string consigneeCity;
	std::string consigneeCityName;
	std::string consigneeCode;
	std::string consigneeDistrict;
	std::string consigneeDistrictName;
	std::string consigneeId;
	std::string consigneeLatitude;
	std::string consigneeLongitude;
	std::string consigneeName;
	std::string consigneeProvince;
	std::string consigneePrivinceName;
	std::string consigneeType;
	std::string consigneeUnitCode;
	std::string consigneeUnitId;
	std::string consigneeUnitName;
	std::string deliveryMethodType;
	std::string estimatedDeliveryEndTime;
	std::string estimatedDeliveryStartTime;
	std::string estimatedPickupEndTime;
	std::string estimatedPickupStartTime;
	std::string lineCode;
	std::string lineId;
	std::string lineName;
	std::string orderBusinessType;
	std::string orderDate;
	std::string orderType;
	std::string packageTotalVolume;
	std::string packageTotalWeight;
	std::string packgeWholePiece;
	std::string shipbillId;
	std::string shipbillNo;
	std::string shipperAddress;
	std::string shipperCity;
	std::string shipperCityName;
	std::string shipperCode;
	std::string shipperDistrict;
	std::string shipperDistrictName;
	std::string shipperId;
	std::string shipperLatitude;
	std::string shipperLongitude;
	std::string shipperName;
	std::string shipperProvince;
	std::string shipperProvinceName;
	std::string shipperType;
	std::string shipperUnitCode;
	std::string shipperUnitId;
	std::string shipperUnitName;
	std::string temperatureRequirement;
	std::string totalPcs;
	std::string totalVolume;
	std::string totalWeight;
	std::string transportAging;
	std::vector<ShipBillDetail> details;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		ShipBill,
		details,
		transportAging,
		deliveryMethodType,
		temperatureRequirement,
		lineCode, lineId, lineName,
		branchCode, branchId, branchName,
		totalPcs, totalVolume,totalWeight,
		orderBusinessType, orderDate, orderType,
		cargoOwnerCode, cargoOwnerId, cargoOwnerName,
		carrierCode, carrierId, carrierName, carrierType,
		packageTotalVolume, packageTotalWeight, packgeWholePiece,
		estimatedDeliveryEndTime, estimatedDeliveryStartTime, estimatedPickupEndTime, estimatedPickupStartTime,
		consigneeAddress, consigneeCity, consigneeCityName, consigneeCode, consigneeDistrict, consigneeDistrictName, consigneeId, consigneeLatitude,
		consigneeLongitude, consigneeName, consigneeProvince, consigneePrivinceName, consigneeType, consigneeUnitCode, consigneeUnitId, consigneeUnitName,
		shipbillId, shipbillNo, shipperAddress, shipperCity, shipperCityName, shipperCode, shipperDistrict, shipperDistrictName, shipperId,
		shipperLatitude, shipperLongitude, shipperName, shipperProvince, shipperProvinceName, shipperType, shipperUnitCode, shipperUnitId, shipperUnitName
	)
};

struct Vehicle {
	std::string affiliationCode;
	std::string affiliationId;
	std::string affiliationName;
	std::string code;
	std::string id;
	std::string memo;
	std::string plateNumber;
	std::string type;
	std::string vehicleTypeCode;
	std::string vehicleTypeId;
	std::string vehicleTypeName;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Vehicle, affiliationCode, affiliationId, affiliationName, code, id, memo, plateNumber, type,
		vehicleTypeCode, vehicleTypeId, vehicleTypeName)
};

struct VehicleType {
	std::string code;
	std::string height;
	std::string id;
	std::string length;
	std::string maxLoadingPcs;
	std::string maxLoadingRatio;
	std::string maxLoadingVolume;
	std::string maxLoadingWeight;
	std::string maxMileage;
	std::string maxOrders;
	std::string minLoadingRatio;
	std::string name;
	std::string sites;
	std::string type;
	std::string usableCount;
	std::string volume;
	std::string weight;
	std::string width;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(VehicleType, code, height, id, length, maxLoadingPcs, maxLoadingRatio, maxLoadingVolume, maxLoadingWeight,
		maxMileage, maxOrders, minLoadingRatio, name, sites, type, usableCount, volume, weight, width)
};

#endif