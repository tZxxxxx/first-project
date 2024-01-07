#include <set>

#include "crossover.h"
#include "util.h"

#include <iostream>

extern const double RATE_C;

void crossover_order(MatrixList& current, std::map<std::string, std::vector<size_t>>& complete_vehicles) {
	for (int i = 0; i < current.size(); i++) {
		// 小于CROSSOVER阈值才进化
		if (uniform_real_random_in_range(0.0, 1.0) > RATE_C) {
			continue;
		}
		auto& pop = current[i];
		// 已经装满的车不考虑
		auto key = serializeEigenMatrix(pop);
		auto v = complete_vehicles[key];
		std::set<size_t> vehicles{ v.begin(), v.end() };
		std::vector<size_t> candidate_vehicles;
		for (int j = 0; j < pop.rows(); j++) {
			if (vehicles.count(j) == 0) {
				candidate_vehicles.emplace_back(j);
			}
		}
		// 车辆太少无法CROSSOVER
		if (candidate_vehicles.size() < 2) {
			continue;
		}
		// 候选车辆中随机选两辆出来CROSSOVER
		std::vector<int> choiceIdx;
		for (int i = 0; i < candidate_vehicles.size(); i++) {
			choiceIdx.emplace_back(i);
		}
		random_unique(choiceIdx.begin(), choiceIdx.end(), 2);
		size_t x = candidate_vehicles[choiceIdx[0]];
		size_t y = candidate_vehicles[choiceIdx[1]];
		if (pop.row(x).sum() == 0 && pop.row(y).sum() == 0) {
			continue;
		}
		// 选择要交换的片段，片段长度至少1
		choiceIdx.clear();
		for (int i = 0; i < pop.cols() + 1; i++) {
			choiceIdx.emplace_back(i);
		}
		random_unique(choiceIdx.begin(), choiceIdx.end(), 2);
		size_t start = std::min(choiceIdx[0], choiceIdx[1]);
		size_t end = std::max(choiceIdx[0], choiceIdx[1]);
		Eigen::MatrixXi fragment_for_vehicle_x = pop.block(x, start, 1, end - start);
		Eigen::MatrixXi fragment_for_vehicle_y = pop.block(y, start, 1, end - start);
		if (fragment_for_vehicle_x.sum() == 0 && fragment_for_vehicle_y.sum() == 0) {
			continue;
		}
		//std::cout << x << ", " << y << " " << start << ", " << end << std::endl;
		//std::cout << "before crossover:" << std::endl << pop << std::endl;
		pop.block(x, start, 1, end - start) = fragment_for_vehicle_y;
		pop.block(y, start, 1, end - start) = fragment_for_vehicle_x;
		//std::cout << "after crossover:" << std::endl << pop << std::endl << std::endl;
	}
}