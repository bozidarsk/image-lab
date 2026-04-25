#pragma once

#include "Color.h"
#include "Uniforms.h"

class Image;
typedef Color(*ShaderEntryPoint)(int x, int y, const Image& input, const Uniforms& uniforms);

class Shader 
{
private:
	ShaderEntryPoint entryPoint;

public:
	ShaderEntryPoint GetEntryPoint() const;

	Shader(ShaderEntryPoint entryPoint);
};
