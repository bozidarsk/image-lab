#pragma once

#include "ImageKernel.h"

class Outline : public ImageKernel
{
private:
	static constexpr float matrix[9] =
	{
		-1.0f, -1.0f, -1.0f,
		-1.0f,  8.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
	};

public:
	virtual const std::string& GetName() const override;

	Outline();
};
