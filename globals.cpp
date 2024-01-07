#include <3rd/nlohmann/json.hpp>
#include <fstream>

#include "globals.h"

#include <random>

std::random_device seeder;
std::mt19937_64 generator(seeder());

// 输入配置
const char* filename = "config.json";
Configuration config;

// 输出数据
nlohmann::json output_json = nlohmann::json::object();

/* 算法参数配置项 */
// mutate发生概率，经验值：0.001-0.1
extern const double RATE_M = load_configuration_from_file<double>(filename, "RATE_M", 0.1);
// crossover发生概率，经验值：0.4-0.99
extern const double RATE_C = load_configuration_from_file<double>(filename, "RATE_C", 0.8);
// 车辆-订单指派种群数目，越小速度越快，但代价是丧失准确性
extern const int ORDER_POP_SIZE = load_configuration_from_file<int>(filename, "ORDER_POP_SIZE", 10);
// 车辆-订单种群迭代次数，越小速度越快，但代价是丧失准确性（不建议小于50）
extern const int ORDER_N_GENERATEION = load_configuration_from_file<int>(filename, "ORDER_N_GENERATION", 400);
// 路线规划种群数目，越小速度越快，但代价是丧失准确性
extern const int ROUTE_POP_SIZE = load_configuration_from_file<int>(filename, "ROUTE_POP_SIZE", 6);
// 路线规划种群迭代次数，越小速度越快，但代价是丧失准确性
extern const int ROUTE_N_GENERATEION = load_configuration_from_file<int>(filename, "ROUTE_N_GENERATION", 20);

/* 业务逻辑配置项 */
// 体积扩张系数
extern const double VOLUME_MULTIPLIER = load_configuration_from_file<double>(filename, "VOLUME_MULTIPLIER", 1.2);
// 起步价
extern const double START_FEES = load_configuration_from_file<double>(filename, "START_FEES", 0.0);
// 每公里费用
extern const double KM_FEES = load_configuration_from_file<double>(filename, "KM_FEES", 1.0);
// 订单数
extern const int NBR_SHIPBILL = load_configuration_from_file<int>(filename, "NBR_SHIPBILL", 1000);
// 合并订单循环次数
extern const int MERGED_ORDERS_MAX_ITER = load_configuration_from_file<int>(filename, "MERGED_ORDERS_MAX_ITER", 1000);
// 车辆分配策略，为true时，先排大车；为false时，先排小车。为true时，车少，速度更快
extern const bool SORTED_VEHICLE_STRATEGY = load_configuration_from_file<bool>(filename, "SORTED_VEHICLE_STRATEGY", false);
// 装载率
extern const double load_ratio = load_configuration_from_file<double>(filename, "load_ratio", 1.0);
// 装载相关惩罚系数
extern const nlohmann::json c_load = load_configuration_from_file<nlohmann::json>(filename, "c_load", {
	{ "1", 10000 }, // volume
	{ "2", 10000 }, // weight
	{ "3", 0.0   }, // time
	{ "4", 10000 }, // mileage
	{ "5", 10000 }, // sites
	{ "6", 10000 }  // orders
});