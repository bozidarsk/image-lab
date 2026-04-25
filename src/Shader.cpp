#include <algorithm>
#include <cassert>

#include "Image.h"
#include "Shader.h"
#include "Uniforms.h"

/*static*/ Color Shader::Inverse(int x, int y, const Image& input, const Uniforms& uniforms) 
{
	auto color = input[x, y];
	return Color(0xff - color.r, 0xff - color.g, 0xff - color.b, color.a);
}

/*static*/ Color Shader::Grayscale(int x, int y, const Image& input, const Uniforms& uniforms) 
{
	auto color = input[x, y];
	auto luminance = (uint8_t)(255.0 * (0.2125*(color.r / 255.0) + 0.7154*(color.g / 255.0) + 0.0721*(color.b / 255.0)));

	return Color(luminance, luminance, luminance, color.a);
}

/*static*/ Color Shader::ContrastStretch(int x, int y, const Image& input, const Uniforms& uniforms) 
{
	auto color = input[x, y];
	auto min = uniforms.Get<Color>("min");
	auto max = uniforms.Get<Color>("max");

	auto r = (uint8_t)(255.0 * ((double)(color.r - min.r) / (double)(max.r - min.r)));
	auto g = (uint8_t)(255.0 * ((double)(color.g - min.g) / (double)(max.g - min.g)));
	auto b = (uint8_t)(255.0 * ((double)(color.b - min.b) / (double)(max.b - min.b)));

	return Color(r, g, b, color.a);
}

/*static*/ Color Shader::ImageKernel(int x, int y, const Image& input, const Uniforms& uniforms) 
{
	if (x == 0 || y == 0 || x == input.GetWidth() - 1 || y == input.GetHeight() - 1)
		return Color(0, 0, 0, input[x, y].a);

	auto top_left = input[x - 1, y - 1];
	auto top_center = input[x, y - 1];
	auto top_right = input[x + 1, y - 1];

	auto middle_left = input[x - 1, y];
	auto middle_center = input[x, y];
	auto middle_right = input[x + 1, y];

	auto bottom_left = input[x - 1, y + 1];
	auto bottom_center = input[x, y + 1];
	auto bottom_right = input[x + 1, y + 1];

	return Color(
		(uint8_t)std::clamp(
			top_left.r*uniforms.Get<double>("m00")
			+ top_center.r*uniforms.Get<double>("m01")
			+ top_right.r*uniforms.Get<double>("m02")
			+ middle_left.r*uniforms.Get<double>("m10")
			+ middle_center.r*uniforms.Get<double>("m11")
			+ middle_right.r*uniforms.Get<double>("m12")
			+ bottom_left.r*uniforms.Get<double>("m20")
			+ bottom_center.r*uniforms.Get<double>("m21")
			+ bottom_right.r*uniforms.Get<double>("m22"),
			0.0, 255.0
		),
		(uint8_t)std::clamp(
			top_left.g*uniforms.Get<double>("m00")
			+ top_center.g*uniforms.Get<double>("m01")
			+ top_right.g*uniforms.Get<double>("m02")
			+ middle_left.g*uniforms.Get<double>("m10")
			+ middle_center.g*uniforms.Get<double>("m11")
			+ middle_right.g*uniforms.Get<double>("m12")
			+ bottom_left.g*uniforms.Get<double>("m20")
			+ bottom_center.g*uniforms.Get<double>("m21")
			+ bottom_right.g*uniforms.Get<double>("m22"),
			0.0, 255.0
		),
		(uint8_t)std::clamp(
			top_left.b*uniforms.Get<double>("m00")
			+ top_center.b*uniforms.Get<double>("m01")
			+ top_right.b*uniforms.Get<double>("m02")
			+ middle_left.b*uniforms.Get<double>("m10")
			+ middle_center.b*uniforms.Get<double>("m11")
			+ middle_right.b*uniforms.Get<double>("m12")
			+ bottom_left.b*uniforms.Get<double>("m20")
			+ bottom_center.b*uniforms.Get<double>("m21")
			+ bottom_right.b*uniforms.Get<double>("m22"),
			0.0, 255.0
		),
		middle_center.a
	);
}

ShaderEntryPoint Shader::GetEntryPoint() const { return entryPoint; }

Shader::Shader(ShaderEntryPoint entryPoint) : entryPoint(entryPoint)
{
	assert(entryPoint);
}
