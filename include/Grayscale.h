#pragma once

#include "Filter.h"

class Grayscale : public Filter
{
public:
	virtual const std::string& GetName() const override;
	virtual void Apply(Image& image) const override;
};
