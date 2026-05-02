#include <algorithm>
#include <cassert>

#include "Image.h"
#include "Shader.h"
#include "Uniforms.h"

/*static*/ void Shader::MinMax(const Properties& input, Properties& output, const Uniforms& uniforms)
{
	auto color = input.Get<Color>("color");
	auto min = output.Has<Color>("min") ? output.Get<Color>("min") : Color(0xff, 0xff, 0xff);
	auto max = output.Has<Color>("max") ? output.Get<Color>("max") : Color(0x00, 0x00, 0x00);

	if (color < min) output.Set<Color>("min", color);
	if (color > max) output.Set<Color>("max", color);
}

/*static*/ void Shader::Inverse(const Properties& input, Properties& output, const Uniforms& uniforms)
{
	auto x = input.Get<int>("x");
	auto y = input.Get<int>("y");

	auto& image = *input.Get<const Image*>("image");

	auto color = image[x, y];
	output.Set<Color>("color", Color(0xff - color.r, 0xff - color.g, 0xff - color.b, color.a));
}

/*static*/ void Shader::Grayscale(const Properties& input, Properties& output, const Uniforms& uniforms)
{
	auto x = input.Get<int>("x");
	auto y = input.Get<int>("y");

	auto& image = *input.Get<const Image*>("image");

	auto color = image[x, y];
	auto luminance = (uint8_t)(255.0 * (0.2125*(color.r / 255.0) + 0.7154*(color.g / 255.0) + 0.0721*(color.b / 255.0)));

	output.Set<Color>("color", Color(luminance, luminance, luminance, color.a));
}

/*static*/ void Shader::ContrastStretch(const Properties& input, Properties& output, const Uniforms& uniforms)
{
	auto x = input.Get<int>("x");
	auto y = input.Get<int>("y");

	auto& image = *input.Get<const Image*>("image");

	auto color = image[x, y];
	auto min = input.Get<Color>("min");
	auto max = input.Get<Color>("max");

	auto r = (uint8_t)(255.0 * ((double)(color.r - min.r) / (double)(max.r - min.r)));
	auto g = (uint8_t)(255.0 * ((double)(color.g - min.g) / (double)(max.g - min.g)));
	auto b = (uint8_t)(255.0 * ((double)(color.b - min.b) / (double)(max.b - min.b)));

	output.Set<Color>("color", Color(r, g, b, color.a));
}

/*static*/ void Shader::ImageKernel(const Properties& input, Properties& output, const Uniforms& uniforms)
{
	auto x = input.Get<int>("x");
	auto y = input.Get<int>("y");

	auto& image = *input.Get<const Image*>("image");

	if (x == 0 || y == 0 || x == image.GetWidth() - 1 || y == image.GetHeight() - 1)
	{
		output.Set<Color>("color", Color(0, 0, 0, image[x, y].a));
		return;
	}

	auto top_left = image[x - 1, y - 1];
	auto top_center = image[x, y - 1];
	auto top_right = image[x + 1, y - 1];

	auto middle_left = image[x - 1, y];
	auto middle_center = image[x, y];
	auto middle_right = image[x + 1, y];

	auto bottom_left = image[x - 1, y + 1];
	auto bottom_center = image[x, y + 1];
	auto bottom_right = image[x + 1, y + 1];

	output.Set<Color>("color",
		Color(
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
		)
	);
}

void Shader::operator () (const Properties& input, Properties& output, const Uniforms& uniforms) const
{
	entryPoint(input, output, uniforms);
}

Shader::Shader(ShaderEntryPoint entryPoint) : entryPoint(entryPoint)
{
	assert(entryPoint);
}
