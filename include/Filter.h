#pragma once

#include <any>
#include <vector>
#include <string>
#include <memory>
#include <expected>

#include "Image.h"

class Filter
{
public:
	static std::expected<std::unique_ptr<Filter>, std::string> Parse(const std::string& name, const std::vector<std::any>& arguments);
	static bool TryParse(const std::string& name, const std::vector<std::any>& arguments, std::unique_ptr<Filter>* result);

	virtual const std::string& GetName() const = 0;
	virtual void Apply(Image& image) const = 0;

protected:
	Filter& operator = (const Filter&) = default;
	Filter(const Filter&) = default;
	Filter& operator = (Filter&&) = default;
	Filter(Filter&&) = default;

public:
	Filter() = default;

	virtual ~Filter() = default;
};
