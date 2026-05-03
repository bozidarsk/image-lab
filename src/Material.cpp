#include "Material.h"
#include "Color.h"

/*static*/ const Material Material::Threshold = Material(
	"Threshold",
	{ &Shader::Threshold },
	{
		{ "threshold", Color::Gray }
	}
);

/*static*/ const Material Material::Inverse = Material(
	"Inverse",
	{ &Shader::Inverse }
);

/*static*/ const Material Material::Grayscale = Material(
	"Grayscale",
	{ &Shader::Grayscale }
);

/*static*/ const Material Material::ContrastStretch = Material(
	"ContrastStretch",
	{ &Shader::MinMax, &Shader::ContrastStretch }
);

/*static*/ const Material Material::Blur = Material(
	"Blur",
	{ &Shader::ImageKernel },
	{
		{ "m00", 0.0625f }, { "m01", 0.125f }, { "m02", 0.0625f },
		{ "m10", 0.125f  }, { "m11", 0.25f  }, { "m12", 0.125f  },
		{ "m20", 0.0625f }, { "m21", 0.125f }, { "m22", 0.0625f },
	}
);

/*static*/ const Material Material::Sharpen = Material(
	"Sharpen",
	{ &Shader::ImageKernel },
	{
		{ "m00",  0.0f }, { "m01", -1.0f }, { "m02",  0.0f },
		{ "m10", -1.0f }, { "m11",  5.0f }, { "m12", -1.0f },
		{ "m20",  0.0f }, { "m21", -1.0f }, { "m22",  0.0f },
	}
);

/*static*/ const Material Material::Emboss = Material(
	"Emboss",
	{ &Shader::ImageKernel },
	{
		{ "m00", -2.0f }, { "m01", -1.0f }, { "m02",  0.0f },
		{ "m10", -1.0f }, { "m11",  1.0f }, { "m12",  1.0f },
		{ "m20",  0.0f }, { "m21",  1.0f }, { "m22",  2.0f },
	}
);

/*static*/ const Material Material::Outline = Material(
	"Outline",
	{ &Shader::ImageKernel },
	{
		{ "m00", -1.0f }, { "m01", -1.0f }, { "m02", -1.0f },
		{ "m10", -1.0f }, { "m11",  8.0f }, { "m12", -1.0f },
		{ "m20", -1.0f }, { "m21", -1.0f }, { "m22", -1.0f },
	}
);

/*static*/ const Material Material::TopSobel = Material(
	"TopSobel",
	{ &Shader::ImageKernel },
	{
		{ "m00",  1.0f }, { "m01",  2.0f }, { "m02",  1.0f },
		{ "m10",  0.0f }, { "m11",  0.0f }, { "m12",  0.0f },
		{ "m20", -1.0f }, { "m21", -2.0f }, { "m22", -1.0f },
	}
);

/*static*/ const Material Material::BottomSobel = Material(
	"BottomSobel",
	{ &Shader::ImageKernel },
	{
		{ "m00", -1.0f }, { "m01", -2.0f }, { "m02", -1.0f },
		{ "m10",  0.0f }, { "m11",  0.0f }, { "m12",  0.0f },
		{ "m20",  1.0f }, { "m21",  2.0f }, { "m22",  1.0f },
	}
);

/*static*/ const Material Material::LeftSobel = Material(
	"LeftSobel",
	{ &Shader::ImageKernel },
	{
		{ "m00",  1.0f }, { "m01",  0.0f }, { "m02", -1.0f },
		{ "m10",  2.0f }, { "m11",  0.0f }, { "m12", -2.0f },
		{ "m20",  1.0f }, { "m21",  0.0f }, { "m22", -1.0f },
	}
);

/*static*/ const Material Material::RightSobel = Material(
	"RightSobel",
	{ &Shader::ImageKernel },
	{
		{ "m00", -1.0f }, { "m01",  0.0f }, { "m02",  1.0f },
		{ "m10", -2.0f }, { "m11",  0.0f }, { "m12",  2.0f },
		{ "m20", -1.0f }, { "m21",  0.0f }, { "m22",  1.0f },
	}
);

const std::string& Material::GetName() const { return name; }

void Material::SetShaders(const std::vector<Shader>& shaders) { this->shaders = shaders; }
void Material::SetShaders(std::vector<Shader>&& shaders) { this->shaders = std::move(shaders); }
std::vector<Shader>& Material::GetShaders() { return shaders; }
const std::vector<Shader>& Material::GetShaders() const { return shaders; }

void Material::SetUniforms(const Uniforms& uniforms) { this->uniforms = uniforms; }
void Material::SetUniforms(Uniforms&& uniforms) { this->uniforms = std::move(uniforms); }
Uniforms& Material::GetUniforms() { return uniforms; }
const Uniforms& Material::GetUniforms() const { return uniforms; }

bool Material::operator == (const Material& other) const { return name == other.name; }
bool Material::operator != (const Material& other) const { return name != other.name; }

Material::Material(const std::string& name, std::initializer_list<Shader> shaders) : name(name), shaders(shaders) {}
Material::Material(const std::string& name, std::initializer_list<Shader> shaders, const Uniforms& uniforms) : name(name), shaders(shaders), uniforms(uniforms) {}
Material::Material(const std::string& name, std::initializer_list<Shader> shaders, Uniforms&& uniforms) : name(name), shaders(shaders), uniforms(std::move(uniforms)) {}
Material::Material(const std::string& name, std::initializer_list<Shader> shaders, std::initializer_list<std::pair<const std::string, std::any>> uniforms) : name(name), shaders(shaders), uniforms(uniforms) {}
