#include "Emboss.h"

const std::string& Emboss::GetName() const
{
	static std::string name = "Emboss";
	return name;
}

Emboss::Emboss() : ImageKernel(matrix) {}
