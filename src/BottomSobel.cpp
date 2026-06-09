#include "BottomSobel.h"

const std::string& BottomSobel::GetName() const
{
	static std::string name = "BottomSobel";
	return name;
}

BottomSobel::BottomSobel() : ImageKernel(matrix) {}
