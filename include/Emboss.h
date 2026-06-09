#pragma once

#include "ImageKernel.h"

class Emboss : public ImageKernel
{
private:
	static constexpr float matrix[9] =
	{
		-2.0f, -1.0f,  0.0f,
		-1.0f,  1.0f,  1.0f,
		 0.0f,  1.0f,  2.0f,
	};

public:
	virtual const std::string& GetName() const override;

	Emboss();
};
