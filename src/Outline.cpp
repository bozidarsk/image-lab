#include "Outline.h"

const std::string& Outline::GetName() const
{
	static std::string name = "Outline";
	return name;
}

Outline::Outline() : ImageKernel(matrix) {}
