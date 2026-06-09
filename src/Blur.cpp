#include "Blur.h"

const std::string& Blur::GetName() const
{
	static std::string name = "Blur";
	return name;
}

Blur::Blur() : ImageKernel(matrix) {}
