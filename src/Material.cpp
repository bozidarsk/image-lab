#include "Material.h"

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
