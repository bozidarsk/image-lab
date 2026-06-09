#pragma once

#include "ImageKernel.h"

class Blur : public ImageKernel
{
private:
	static constexpr float matrix[9] =
	{
		0.0625f, 0.1250f, 0.0625f,
		0.1250f, 0.2500f, 0.1250f,
		0.0625f, 0.1250f, 0.0625f,
	};

public:
	virtual const std::string& GetName() const override;

	Blur();
};
