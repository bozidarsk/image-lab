#include "Uniforms.h"

std::any& Uniforms::operator [] (const char* name) { return data[name]; }
std::any& Uniforms::operator [] (const std::string& name) { return data[name]; }
const std::any Uniforms::operator [] (const char* name) const { return data.contains(name) ? data.at(name) : std::any(); }
const std::any Uniforms::operator [] (const std::string& name) const { return data.contains(name) ? data.at(name) : std::any(); }

Uniforms::Uniforms() {}
Uniforms::Uniforms(std::initializer_list<std::pair<const std::string, std::any>> data) : data(data) {}
