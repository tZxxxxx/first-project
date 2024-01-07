#ifndef __UTIL_H__
#define __UTIL_H__

#include <3rd/Eigen/Dense>

int uniform_integer_random_in_range(size_t start, size_t end);
double uniform_real_random_in_range(double start, double end);
int discrete_random_in_range(std::vector<double>::const_iterator first, const std::vector<double>::const_iterator last);

template<class bidiiter>
bidiiter random_unique(bidiiter begin, bidiiter end, size_t num_random) {
    size_t left = std::distance(begin, end);
    while (num_random--) {
        int choice = uniform_integer_random_in_range(0, left-1);
        bidiiter r = begin;
        std::advance(r, choice);
        std::swap(*begin, *r);
        ++begin;
        --left;
    }
    return begin;
}

std::string serializeEigenMatrix(Eigen::MatrixXi m);

template<class T>
std::vector<T> min_max_normalization(std::vector<T> data) {
    T min{ 0 }, max{ 0 };
    for (auto& datum : data) {
        if (datum < min) {
            min = datum;
        } else if (datum > max) {
            max = datum;
        }
    }
    T range = max - min + 1e-5;
    std::vector<T> normalized;
    for (auto& datum : data) {
        normalized.emplace_back((datum - min) / range);
    }
    return normalized;
}

void executionStatus(const std::string& phase, const std::time_t& timeCns, const std::time_t& timeRmn, const double& execution);

std::string utf8_to_gbk(const std::string& str);

#endif