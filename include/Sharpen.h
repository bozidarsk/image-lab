#pragma once

#include "ImageKernel.h"

class Sharpen : public ImageKernel
{
private:
	static constexpr float matrix[9] =
	{
		 0.0f, -1.0f,  0.0f,
		-1.0f,  5.0f, -1.0f,
		 0.0f, -1.0f,  0.0f,
	};

public:
	virtual const std::string& GetName() const override;

	Sharpen();
};
