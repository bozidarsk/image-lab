#pragma once

#include "ImageKernel.h"

class RightSobel : public ImageKernel
{
private:
	static constexpr float matrix[9] =
	{
		-1.0f,  0.0f,  1.0f,
		-2.0f,  0.0f,  2.0f,
		-1.0f,  0.0f,  1.0f,
	};

public:
	virtual const std::string& GetName() const override;

	RightSobel();
};
