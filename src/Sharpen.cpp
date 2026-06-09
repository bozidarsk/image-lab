#include "Sharpen.h"

const std::string& Sharpen::GetName() const
{
	static std::string name = "Sharpen";
	return name;
}

Sharpen::Sharpen() : ImageKernel(matrix) {}
