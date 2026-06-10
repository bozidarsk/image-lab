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

	virtual const std::string& GetName() const = 0;
	virtual void Apply(Image& image) const = 0;

	virtual ~Filter() = default;
};
