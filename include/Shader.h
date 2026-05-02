#pragma once

#include "Uniforms.h"
#include "Properties.h"

typedef void(*ShaderEntryPoint)(const Properties&, Properties&, const Uniforms& uniforms);

class Shader
{
private:
	ShaderEntryPoint entryPoint;

public:
	static void MinMax(const Properties& input, Properties& output, const Uniforms& uniforms);
	static void Inverse(const Properties& input, Properties& output, const Uniforms& uniforms);
	static void Grayscale(const Properties& input, Properties& output, const Uniforms& uniforms);
	static void ContrastStretch(const Properties& input, Properties& output, const Uniforms& uniforms);
	static void ImageKernel(const Properties& input, Properties& output, const Uniforms& uniforms);

	void operator () (const Properties& input, Properties& output, const Uniforms& uniforms) const;

	Shader(ShaderEntryPoint entryPoint);
};
