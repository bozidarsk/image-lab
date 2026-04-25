#include "Material.h"

/*static*/ const Material Material::Inverse = Material(Shader(&Shader::Inverse));
/*static*/ const Material Material::Grayscale = Material(Shader(&Shader::Grayscale));
/*static*/ const Material Material::ContrastStretch = Material(Shader(&Shader::ContrastStretch), { { "min", Color(0x00, 0x00, 0x00, 0xff) }, { "max", Color(0xff, 0xff, 0xff, 0xff) } });

/*static*/ const Material Material::Blur = Material(Shader(&Shader::ImageKernel),
	{
		{ "m00", 0.0625 }, { "m01", 0.125 }, { "m02", 0.0625 },
		{ "m10", 0.125  }, { "m11", 0.25  }, { "m12", 0.125  },
		{ "m20", 0.0625 }, { "m21", 0.125 }, { "m22", 0.0625 },
	}
);

/*static*/ const Material Material::Sharpen = Material(Shader(&Shader::ImageKernel),
	{
		{ "m00",  0.0 }, { "m01", -1.0 }, { "m02",  0.0 },
		{ "m10", -1.0 }, { "m11",  5.0 }, { "m12", -1.0 },
		{ "m20",  0.0 }, { "m21", -1.0 }, { "m22",  0.0 },
	}
);

/*static*/ const Material Material::Emboss = Material(Shader(&Shader::ImageKernel),
	{
		{ "m00", -2.0 }, { "m01", -1.0 }, { "m02",  0.0 },
		{ "m10", -1.0 }, { "m11",  1.0 }, { "m12",  1.0 },
		{ "m20",  0.0 }, { "m21",  1.0 }, { "m22",  2.0 },
	}
);

/*static*/ const Material Material::Outline = Material(Shader(&Shader::ImageKernel),
	{
		{ "m00", -1.0 }, { "m01", -1.0 }, { "m02", -1.0 },
		{ "m10", -1.0 }, { "m11",  8.0 }, { "m12", -1.0 },
		{ "m20", -1.0 }, { "m21", -1.0 }, { "m22", -1.0 },
	}
);

/*static*/ const Material Material::TopSobel = Material(Shader(&Shader::ImageKernel),
	{
		{ "m00",  1.0 }, { "m01",  2.0 }, { "m02",  1.0 },
		{ "m10",  0.0 }, { "m11",  0.0 }, { "m12",  0.0 },
		{ "m20", -1.0 }, { "m21", -2.0 }, { "m22", -1.0 },
	}
);

/*static*/ const Material Material::BottomSobel = Material(Shader(&Shader::ImageKernel),
	{
		{ "m00", -1.0 }, { "m01", -2.0 }, { "m02", -1.0 },
		{ "m10",  0.0 }, { "m11",  0.0 }, { "m12",  0.0 },
		{ "m20",  1.0 }, { "m21",  2.0 }, { "m22",  1.0 },
	}
);

/*static*/ const Material Material::LeftSobel = Material(Shader(&Shader::ImageKernel),
	{
		{ "m00",  1.0 }, { "m01",  0.0 }, { "m02", -1.0 },
		{ "m10",  2.0 }, { "m11",  0.0 }, { "m12", -2.0 },
		{ "m20",  1.0 }, { "m21",  0.0 }, { "m22", -1.0 },
	}
);

/*static*/ const Material Material::RightSobel = Material(Shader(&Shader::ImageKernel),
	{
		{ "m00", -1.0 }, { "m01",  0.0 }, { "m02",  1.0 },
		{ "m10", -2.0 }, { "m11",  0.0 }, { "m12",  2.0 },
		{ "m20", -1.0 }, { "m21",  0.0 }, { "m22",  1.0 },
	}
);

const Shader& Material::GetShader() const { return shader; }
const Uniforms& Material::GetUniforms() const { return uniforms; }

std::any& Material::operator [] (const char* name) { return uniforms[name]; }
std::any& Material::operator [] (const std::string& name) { return uniforms[name]; }
const std::any Material::operator [] (const char* name) const { return uniforms[name]; }
const std::any Material::operator [] (const std::string& name) const { return uniforms[name]; }

Material::Material(const Shader& shader) : shader(shader) {}
Material::Material(const Shader& shader, const Uniforms& uniforms) : shader(shader), uniforms(uniforms) {}
Material::Material(const Shader& shader, Uniforms&& uniforms) : shader(shader), uniforms(std::move(uniforms)) {}
Material::Material(const Shader& shader, std::initializer_list<std::pair<const std::string, std::any>> uniforms) : shader(shader), uniforms(uniforms) {}
