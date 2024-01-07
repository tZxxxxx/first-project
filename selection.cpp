#include "selection.h"
#include "util.h"

#include <iostream>

std::vector<double> get_normalized(const std::vector<double>& fitnesses, double* const sum) {
	// 0-1标准化
	std::vector<double> normalized = min_max_normalization(fitnesses);
	// 生成概率分布
	double total{ 0.0 };
	for (int i = 0; i < normalized.size(); i++) {
		const double& v = std::exp(-10 * normalized[i]);
		normalized[i] = v;
		total += v;
	}
	if (total > 0) {
		for (auto& v : normalized) {
			v = v / total;
		}
	}
	*sum = total;
	return normalized;
}
void select_pop_order(MatrixList& current, const std::vector<double>& fitnesses) {
	double sum{ 0 };
	const std::vector<double> normalized = get_normalized(fitnesses, &sum);
	if (sum < 0) {
		return;
	}
	// 根据给定优劣概率的轮盘淘汰劣势个体
	MatrixList selected;
	for (int i = 0; i < current.size(); i++) {
		const int& choice = discrete_random_in_range(normalized.begin(), normalized.end());
		selected.emplace_back(current[choice]);
	}
	current = selected;
}

void select_pop_route(Eigen::MatrixXi& current, const std::vector<double>& fitnesses) {
	double sum{ 0 };
	const std::vector<double> normalized = get_normalized(fitnesses, &sum);
	if (sum < 0) {
		return;
	}
	// 根据给定优劣概率的轮盘淘汰劣势个体
	const int& popSize = static_cast<int>(current.rows());
	const int& orderNum = static_cast<int>(current.cols());
	Eigen::MatrixXi selected(popSize, orderNum);
	for (int i = 0; i < popSize; i++) {
		const int& choice = discrete_random_in_range(normalized.begin(), normalized.end());
		selected.row(i) = current.row(choice);
	}
	current = selected;
}