#include <cassert>

#include "Image.h"
#include "Shader.h"
#include "Uniforms.h"

/*static*/ void Shader::MinMax(const Properties& input, Properties& output, const Uniforms& uniforms)
{
	auto color = input.Get<Color>("color");
	auto min = output.Has<Color>("min") ? output.Get<Color>("min") : Color::White;
	auto max = output.Has<Color>("max") ? output.Get<Color>("max") : Color::Black;

	if (color < min) output.Set<Color>("min", color);
	if (color > max) output.Set<Color>("max", color);
}

/*static*/ void Shader::Inverse(const Properties& input, Properties& output, const Uniforms& uniforms)
{
	auto x = input.Get<int>("x");
	auto y = input.Get<int>("y");

	auto& image = *input.Get<const Image*>("image");

	auto color = image[x, y];
	output.Set<Color>("color", Color(1.0f - color.r, 1.0f - color.g, 1.0f - color.b, color.a));
}

/*static*/ void Shader::Grayscale(const Properties& input, Properties& output, const Uniforms& uniforms)
{
	auto x = input.Get<int>("x");
	auto y = input.Get<int>("y");

	auto& image = *input.Get<const Image*>("image");

	auto color = image[x, y];
	auto luminance = color.Luminance();

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

	auto r = (color.r - min.r) / (max.r - min.r);
	auto g = (color.g - min.g) / (max.g - min.g);
	auto b = (color.b - min.b) / (max.b - min.b);

	output.Set<Color>("color", Color(r, g, b, color.a));
}

/*static*/ void Shader::ImageKernel(const Properties& input, Properties& output, const Uniforms& uniforms)
{
	auto x = input.Get<int>("x");
	auto y = input.Get<int>("y");

	auto& image = *input.Get<const Image*>("image");

	if (x == 0 || y == 0 || x == image.GetWidth() - 1 || y == image.GetHeight() - 1)
	{
		output.Set<Color>("color", Color(Color::Black, image[x, y].a));
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

	auto color =
		top_left*uniforms.Get<float>("m00")
		+ top_center*uniforms.Get<float>("m01")
		+ top_right*uniforms.Get<float>("m02")
		+ middle_left*uniforms.Get<float>("m10")
		+ middle_center*uniforms.Get<float>("m11")
		+ middle_right*uniforms.Get<float>("m12")
		+ bottom_left*uniforms.Get<float>("m20")
		+ bottom_center*uniforms.Get<float>("m21")
		+ bottom_right*uniforms.Get<float>("m22")
	;

	color.Clamp();
	color.a = middle_center.a;

	output.Set<Color>("color", color);
}

void Shader::operator () (const Properties& input, Properties& output, const Uniforms& uniforms) const
{
	entryPoint(input, output, uniforms);
}

Shader::Shader(ShaderEntryPoint entryPoint) : entryPoint(entryPoint)
{
	assert(entryPoint);
}
