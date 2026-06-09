#pragma once

#include "Filter.h"

class Threshold : public Filter
{
private:
	Color threshold;

public:
	virtual const std::string& GetName() const override;
	virtual void Apply(Image& image) const override;

	Threshold(Color threshold);
};
