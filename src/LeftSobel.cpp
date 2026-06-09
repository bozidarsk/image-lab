#include "LeftSobel.h"

const std::string& LeftSobel::GetName() const
{
	static std::string name = "LeftSobel";
	return name;
}

LeftSobel::LeftSobel() : ImageKernel(matrix) {}
