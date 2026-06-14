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
	ImageKernel& operator = (const ImageKernel&) = default;
	ImageKernel(const ImageKernel&) = default;
	ImageKernel& operator = (ImageKernel&&) = default;
	ImageKernel(ImageKernel&&) = default;

public:
	ImageKernel(const float matrix[9]);

	virtual ~ImageKernel() = default;
};
