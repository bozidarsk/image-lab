#pragma once

#include <any>
#include <string>
#include <utility>
#include <unordered_map>
#include <initializer_list>
#include <cassert>

class Uniforms 
{
private:
	std::unordered_map<std::string, std::any> data;

public:
	template<typename T>
	void Set(const std::string& name, const T& value) { data[name] = value; }

	template<typename T>
	T Get(const std::string& name) const 
	{
		assert(data.contains(name));

		const std::any& value = data.at(name);
		assert(value.type() == typeid(T));

		return std::any_cast<T>(value);
	}

	std::any& operator [] (const char* name);
	std::any& operator [] (const std::string& name);
	const std::any operator [] (const char* name) const;
	const std::any operator [] (const std::string& name) const;

	Uniforms();
	Uniforms(std::initializer_list<std::pair<const std::string, std::any>> data);
};
