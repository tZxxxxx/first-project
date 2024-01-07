#ifndef __GLOBAL_H__
#define __GLOABL_H__

#include <3rd/nlohmann/json.hpp>

struct Configuration
{
	Configuration()
		: configuration_file_exist(true) { }

	nlohmann::json user_configuration;
	bool configuration_file_exist;
};

extern Configuration config;
template <typename T>
T load_configuration_from_file(const char* filename, const char* key, T default_value) {
	// 尝试读取配置文件
	if (config.user_configuration.empty() && config.configuration_file_exist) {
		std::ifstream ifs(filename, std::ios::in);
		if (!ifs.good()) {
			config.configuration_file_exist = false;
		} else {
			try {
				ifs >> config.user_configuration;
			} catch (...) {
				config.configuration_file_exist = false;
			}
		}
	}
	// 假如有配置文件且存在相应配置项且能正确读取则配置该值，否则配置指定默认值
	T value;
	const nlohmann::json& c = config.user_configuration;
	if (!c.empty() && c.contains(key)) {
		try {
			value = c[key].get<T>();
		} catch (...) {
			value = default_value;
		}
	} else {
		value = default_value;
	}
	return value;
}

#endif