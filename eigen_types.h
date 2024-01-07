#ifndef __EIGEN_TYPES_H__
#define __EIGEN_TYPES_H__

#include <vector>
#include <3rd/Eigen/Dense>

using MatrixList = std::vector<
	Eigen::MatrixXi,
	Eigen::aligned_allocator<Eigen::MatrixXi>
>;

#endif