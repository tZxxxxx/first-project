#include <random>
#include <iostream>
#include <locale>
#include <codecvt>

#include "util.h"

extern std::mt19937_64 generator;

int uniform_integer_random_in_range(size_t start, size_t end) {
    std::uniform_int_distribution<> distribution(static_cast<int>(start), static_cast<int>(end));
    return distribution(generator);
}

double uniform_real_random_in_range(double start, double end) {
    std::uniform_real_distribution<double> distribution(start, end);
    return distribution(generator);
}

int discrete_random_in_range(std::vector<double>::const_iterator first, std::vector<double>::const_iterator last) {
    std::discrete_distribution<> distribution(first, last);
    return distribution(generator);
}

std::string serializeEigenMatrix(Eigen::MatrixXi m) {
    std::string s;
    for (int i = 0; i < m.rows(); i++) {
        for (int j = 0; j < m.cols(); j++) {
            s += std::to_string(m(i, j));
            s += " ";
        }
    }
    return s;
}

std::string logTime(const std::time_t& t) {
    std::time_t time = 57600 + t;
    char tmp[64];
    struct tm timeinfo;
    localtime_s(&timeinfo, &time);
    std::strftime(tmp, sizeof(tmp), "%Hh::%Mm::%Ss", &timeinfo);
    return std::string(tmp);
}

void executionStatus(const std::string& phase, const std::time_t& timeCns, const std::time_t& timeRmn, const double& execution) {
    std::cout << phase << ": 执行度=" << execution << "%, 已用时长=" << logTime(timeCns) << ", 预估剩余时长: " << logTime(timeRmn) << std::endl;
}

std::string utf8_to_gbk(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring tmp_wstr = conv.from_bytes(str);
    //GBK locale name in windows
    const char* GBK_LOCALE_NAME = ".936";
    std::wstring_convert<std::codecvt_byname<wchar_t, char, mbstate_t>> convert(new std::codecvt_byname<wchar_t, char, mbstate_t>(GBK_LOCALE_NAME));
    return convert.to_bytes(tmp_wstr);
}