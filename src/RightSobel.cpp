#include "RightSobel.h"

const std::string& RightSobel::GetName() const
{
	static std::string name = "RightSobel";
	return name;
}

RightSobel::RightSobel() : ImageKernel(matrix) {}
