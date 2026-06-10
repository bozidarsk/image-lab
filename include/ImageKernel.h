#pragma once

#include "Filter.h"

class ImageKernel : public Filter
{
private:
	float matrix[9];

public:
	virtual const std::string& GetName() const override;
	virtual void Apply(Image& image) const override;

protected:
	ImageKernel& operator = (const ImageKernel&) = delete;
	ImageKernel(const ImageKernel&) = delete;

public:
	ImageKernel(const float matrix[9]);

	virtual ~ImageKernel() = default;
};
