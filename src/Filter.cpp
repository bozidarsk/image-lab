#include <cstring>

#include "Filter.h"

#include "Threshold.h"
#include "Blur.h"
#include "BottomSobel.h"
#include "ContrastStretch.h"
#include "Emboss.h"
#include "Grayscale.h"
#include "Inverse.h"
#include "LeftSobel.h"
#include "Outline.h"
#include "RightSobel.h"
#include "Sharpen.h"
#include "TopSobel.h"

/* static */ std::expected<std::unique_ptr<Filter>, std::string> Filter::Parse(const std::string name, const std::vector<std::any>& arguments)
{
	if (!strcmp(name.c_str(), "Threshold"))
	{
		if (arguments.size() == 0 || arguments[0].type() != typeid(Color))
			return std::unexpected("Invalid arguments.");

		return std::make_unique<Threshold>(std::any_cast<Color>(arguments[0]));
	}

	if (!strcmp(name.c_str(), "Blur")) return std::make_unique<Blur>();
	if (!strcmp(name.c_str(), "BottomSobel")) return std::make_unique<BottomSobel>();
	if (!strcmp(name.c_str(), "ContrastStretch")) return std::make_unique<ContrastStretch>();
	if (!strcmp(name.c_str(), "Emboss")) return std::make_unique<Emboss>();
	if (!strcmp(name.c_str(), "Grayscale")) return std::make_unique<Grayscale>();
	if (!strcmp(name.c_str(), "Inverse")) return std::make_unique<Inverse>();
	if (!strcmp(name.c_str(), "LeftSobel")) return std::make_unique<LeftSobel>();
	if (!strcmp(name.c_str(), "Outline")) return std::make_unique<Outline>();
	if (!strcmp(name.c_str(), "RightSobel")) return std::make_unique<RightSobel>();
	if (!strcmp(name.c_str(), "Sharpen")) return std::make_unique<Sharpen>();
	if (!strcmp(name.c_str(), "TopSobel")) return std::make_unique<TopSobel>();

	return std::unexpected("Invalid name.");
}
