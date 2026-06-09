#include "TopSobel.h"

const std::string& TopSobel::GetName() const
{
	static std::string name = "TopSobel";
	return name;
}

TopSobel::TopSobel() : ImageKernel(matrix) {}
