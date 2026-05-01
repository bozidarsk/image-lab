#include "Material.h"

/*static*/ const Material Material::Inverse = Material("Inverse", Shader(&Shader::Inverse));
/*static*/ const Material Material::Grayscale = Material("Grayscale", Shader(&Shader::Grayscale));
/*static*/ const Material Material::ContrastStretch = Material("ContrastStretch", Shader(&Shader::ContrastStretch), { { "min", Color(0x00, 0x00, 0x00, 0xff) }, { "max", Color(0xff, 0xff, 0xff, 0xff) } });

/*static*/ const Material Material::Blur = Material("Blur", Shader(&Shader::ImageKernel),
	{
		{ "m00", 0.0625 }, { "m01", 0.125 }, { "m02", 0.0625 },
		{ "m10", 0.125  }, { "m11", 0.25  }, { "m12", 0.125  },
		{ "m20", 0.0625 }, { "m21", 0.125 }, { "m22", 0.0625 },
	}
);

/*static*/ const Material Material::Sharpen = Material("Sharpen", Shader(&Shader::ImageKernel),
	{
		{ "m00",  0.0 }, { "m01", -1.0 }, { "m02",  0.0 },
		{ "m10", -1.0 }, { "m11",  5.0 }, { "m12", -1.0 },
		{ "m20",  0.0 }, { "m21", -1.0 }, { "m22",  0.0 },
	}
);

/*static*/ const Material Material::Emboss = Material("Emboss", Shader(&Shader::ImageKernel),
	{
		{ "m00", -2.0 }, { "m01", -1.0 }, { "m02",  0.0 },
		{ "m10", -1.0 }, { "m11",  1.0 }, { "m12",  1.0 },
		{ "m20",  0.0 }, { "m21",  1.0 }, { "m22",  2.0 },
	}
);

/*static*/ const Material Material::Outline = Material("Outline", Shader(&Shader::ImageKernel),
	{
		{ "m00", -1.0 }, { "m01", -1.0 }, { "m02", -1.0 },
		{ "m10", -1.0 }, { "m11",  8.0 }, { "m12", -1.0 },
		{ "m20", -1.0 }, { "m21", -1.0 }, { "m22", -1.0 },
	}
);

/*static*/ const Material Material::TopSobel = Material("TopSobel", Shader(&Shader::ImageKernel),
	{
		{ "m00",  1.0 }, { "m01",  2.0 }, { "m02",  1.0 },
		{ "m10",  0.0 }, { "m11",  0.0 }, { "m12",  0.0 },
		{ "m20", -1.0 }, { "m21", -2.0 }, { "m22", -1.0 },
	}
);

/*static*/ const Material Material::BottomSobel = Material("BottomSobel", Shader(&Shader::ImageKernel),
	{
		{ "m00", -1.0 }, { "m01", -2.0 }, { "m02", -1.0 },
		{ "m10",  0.0 }, { "m11",  0.0 }, { "m12",  0.0 },
		{ "m20",  1.0 }, { "m21",  2.0 }, { "m22",  1.0 },
	}
);

/*static*/ const Material Material::LeftSobel = Material("LeftSobel", Shader(&Shader::ImageKernel),
	{
		{ "m00",  1.0 }, { "m01",  0.0 }, { "m02", -1.0 },
		{ "m10",  2.0 }, { "m11",  0.0 }, { "m12", -2.0 },
		{ "m20",  1.0 }, { "m21",  0.0 }, { "m22", -1.0 },
	}
);

/*static*/ const Material Material::RightSobel = Material("RightSobel", Shader(&Shader::ImageKernel),
	{
		{ "m00", -1.0 }, { "m01",  0.0 }, { "m02",  1.0 },
		{ "m10", -2.0 }, { "m11",  0.0 }, { "m12",  2.0 },
		{ "m20", -1.0 }, { "m21",  0.0 }, { "m22",  1.0 },
	}
);

const std::string& Material::GetName() const { return name; }
const Shader& Material::GetShader() const { return shader; }
const Uniforms& Material::GetUniforms() const { return uniforms; }

std::any& Material::operator [] (const char* name) { return uniforms[name]; }
std::any& Material::operator [] (const std::string& name) { return uniforms[name]; }
const std::any Material::operator [] (const char* name) const { return uniforms[name]; }
const std::any Material::operator [] (const std::string& name) const { return uniforms[name]; }

bool Material::operator == (const Material& other) const { return name == other.name; }
bool Material::operator != (const Material& other) const { return name != other.name; }

Material::Material(const std::string& name, const Shader& shader) : name(name), shader(shader) {}
Material::Material(const std::string& name, const Shader& shader, const Uniforms& uniforms) : name(name), shader(shader), uniforms(uniforms) {}
Material::Material(const std::string& name, const Shader& shader, Uniforms&& uniforms) : name(name), shader(shader), uniforms(std::move(uniforms)) {}
Material::Material(const std::string& name, const Shader& shader, std::initializer_list<std::pair<const std::string, std::any>> uniforms) : name(name), shader(shader), uniforms(uniforms) {}
