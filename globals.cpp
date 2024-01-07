#include <3rd/nlohmann/json.hpp>
#include <fstream>

#include "globals.h"

#include <random>

std::random_device seeder;
std::mt19937_64 generator(seeder());

// ��������
const char* filename = "config.json";
Configuration config;

// �������
nlohmann::json output_json = nlohmann::json::object();

/* �㷨���������� */
// mutate�������ʣ�����ֵ��0.001-0.1
extern const double RATE_M = load_configuration_from_file<double>(filename, "RATE_M", 0.1);
// crossover�������ʣ�����ֵ��0.4-0.99
extern const double RATE_C = load_configuration_from_file<double>(filename, "RATE_C", 0.8);
// ����-����ָ����Ⱥ��Ŀ��ԽС�ٶ�Խ�죬��������ɥʧ׼ȷ��
extern const int ORDER_POP_SIZE = load_configuration_from_file<int>(filename, "ORDER_POP_SIZE", 10);
// ����-������Ⱥ����������ԽС�ٶ�Խ�죬��������ɥʧ׼ȷ�ԣ�������С��50��
extern const int ORDER_N_GENERATEION = load_configuration_from_file<int>(filename, "ORDER_N_GENERATION", 400);
// ·�߹滮��Ⱥ��Ŀ��ԽС�ٶ�Խ�죬��������ɥʧ׼ȷ��
extern const int ROUTE_POP_SIZE = load_configuration_from_file<int>(filename, "ROUTE_POP_SIZE", 6);
// ·�߹滮��Ⱥ����������ԽС�ٶ�Խ�죬��������ɥʧ׼ȷ��
extern const int ROUTE_N_GENERATEION = load_configuration_from_file<int>(filename, "ROUTE_N_GENERATION", 20);

/* ҵ���߼������� */
// �������ϵ��
extern const double VOLUME_MULTIPLIER = load_configuration_from_file<double>(filename, "VOLUME_MULTIPLIER", 1.2);
// �𲽼�
extern const double START_FEES = load_configuration_from_file<double>(filename, "START_FEES", 0.0);
// ÿ�������
extern const double KM_FEES = load_configuration_from_file<double>(filename, "KM_FEES", 1.0);
// ������
extern const int NBR_SHIPBILL = load_configuration_from_file<int>(filename, "NBR_SHIPBILL", 1000);
// �ϲ�����ѭ������
extern const int MERGED_ORDERS_MAX_ITER = load_configuration_from_file<int>(filename, "MERGED_ORDERS_MAX_ITER", 1000);
// ����������ԣ�Ϊtrueʱ�����Ŵ󳵣�Ϊfalseʱ������С����Ϊtrueʱ�����٣��ٶȸ���
extern const bool SORTED_VEHICLE_STRATEGY = load_configuration_from_file<bool>(filename, "SORTED_VEHICLE_STRATEGY", false);
// װ����
extern const double load_ratio = load_configuration_from_file<double>(filename, "load_ratio", 1.0);
// װ����سͷ�ϵ��
extern const nlohmann::json c_load = load_configuration_from_file<nlohmann::json>(filename, "c_load", {
	{ "1", 10000 }, // volume
	{ "2", 10000 }, // weight
	{ "3", 0.0   }, // time
	{ "4", 10000 }, // mileage
	{ "5", 10000 }, // sites
	{ "6", 10000 }  // orders
});