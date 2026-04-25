#pragma once

#include <any>
#include <string>
#include <utility>
#include <initializer_list>

#include "Shader.h"
#include "Uniforms.h"

class Material 
{
private:
	Shader shader;
	Uniforms uniforms;

public:
	const Shader& GetShader() const;
	const Uniforms& GetUniforms() const;

	std::any& operator [] (const char* name);
	std::any& operator [] (const std::string& name);
	const std::any operator [] (const char* name) const;
	const std::any operator [] (const std::string& name) const;

	Material(const Shader& shader);
	Material(const Shader& shader, const Uniforms& uniforms);
	Material(const Shader& shader, Uniforms&& uniforms);
	Material(const Shader& shader, std::initializer_list<std::pair<const std::string, std::any>> uniforms);
};
