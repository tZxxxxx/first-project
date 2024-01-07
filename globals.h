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
	// ���Զ�ȡ�����ļ�
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
	// �����������ļ��Ҵ�����Ӧ������������ȷ��ȡ�����ø�ֵ����������ָ��Ĭ��ֵ
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