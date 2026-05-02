#pragma once

#include <any>
#include <string>
#include <utility>
#include <unordered_map>
#include <initializer_list>
#include <cassert>

class Properties
{
private:
	std::unordered_map<std::string, std::any> data;

public:
	template<typename T>
	bool Has(const char* name) const { return data.contains(name) && data.at(name).type() == typeid(T); }

	template<typename T>
	bool Has(const std::string& name) const { return data.contains(name) && data.at(name).type() == typeid(T); }

	template<typename T>
	void Set(const char* name, const T& value) { data[name] = value; }

	template<typename T>
	void Set(const std::string& name, const T& value) { data[name] = value; }

	template<typename T>
	void Set(const char* name, T&& value) { data[name] = std::move(value); }

	template<typename T>
	void Set(const std::string& name, T&& value) { data[name] = std::move(value); }

	template<typename T>
	T Get(const char* name) const
	{
		assert(data.contains(name));

		const std::any& value = data.at(name);
		assert(value.type() == typeid(T));

		return std::any_cast<T>(value);
	}

	Properties();
	Properties(std::initializer_list<std::pair<const std::string, std::any>> data);
};
