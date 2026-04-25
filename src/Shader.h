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
	static Color Inverse(int x, int y, const Image& input, const Uniforms& uniforms);
	static Color Grayscale(int x, int y, const Image& input, const Uniforms& uniforms);
	static Color ContrastStretch(int x, int y, const Image& input, const Uniforms& uniforms);
	static Color ImageKernel(int x, int y, const Image& input, const Uniforms& uniforms);

	ShaderEntryPoint GetEntryPoint() const;

	Shader(ShaderEntryPoint entryPoint);
};
