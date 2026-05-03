#pragma once

#include <any>
#include <string>
#include <vector>
#include <initializer_list>

#include "Shader.h"
#include "Uniforms.h"

class Material
{
private:
	std::string name;
	std::vector<Shader> shaders;
	Uniforms uniforms;

public:
	static const Material Threshold;
	static const Material Inverse;
	static const Material Grayscale;
	static const Material ContrastStretch;
	static const Material Blur;
	static const Material Sharpen;
	static const Material Emboss;
	static const Material Outline;
	static const Material TopSobel;
	static const Material BottomSobel;
	static const Material LeftSobel;
	static const Material RightSobel;

	const std::string& GetName() const;

	void SetShaders(const std::vector<Shader>& shaders);
	void SetShaders(std::vector<Shader>&& shaders);
	std::vector<Shader>& GetShaders();
	const std::vector<Shader>& GetShaders() const;

	void SetUniforms(const Uniforms& uniforms);
	void SetUniforms(Uniforms&& uniforms);
	Uniforms& GetUniforms();
	const Uniforms& GetUniforms() const;

	bool operator == (const Material& other) const;
	bool operator != (const Material& other) const;

	Material(const std::string& name, std::initializer_list<Shader> shaders);
	Material(const std::string& name, std::initializer_list<Shader> shaders, const Uniforms& uniforms);
	Material(const std::string& name, std::initializer_list<Shader> shaders, Uniforms&& uniforms);
	Material(const std::string& name, std::initializer_list<Shader> shaders, std::initializer_list<std::pair<const std::string, std::any>> uniforms);
};
