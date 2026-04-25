#include <cassert>

#include "Shader.h"

ShaderEntryPoint Shader::GetEntryPoint() const { return entryPoint; }

Shader::Shader(ShaderEntryPoint entryPoint) : entryPoint(entryPoint)
{
	assert(entryPoint);
}
