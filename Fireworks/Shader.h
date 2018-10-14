#pragma once

#include <d3d11.h>
#include <d3dx11.h>

template<typename ShaderType>
class BaseShader
{
protected:
	ID3D10Blob* byteCodeBuffer;
	ShaderType* program;
public:
	BaseShader() = default;
	virtual ~BaseShader() = default;

	virtual void Compile(const char* filename, const char* version, const char* entryPoint) = 0;
	ShaderType* GetProgram() { return program; }
	ID3D10Blob* GetByteCodeBuffer() { return byteCodeBuffer; }
};

class VertexShader : public BaseShader<ID3D11VertexShader>
{
public:
	virtual ~VertexShader();
	void Compile(const char* filename, const char* version, const char* entryPoint) override;
};


class PixelShader : public BaseShader<ID3D11PixelShader>
{
public:
	virtual ~PixelShader();
	void Compile(const char* filename, const char* version, const char* entryPoint) override;
};


class GeometryShader : public BaseShader<ID3D11GeometryShader>
{
public:
	virtual ~GeometryShader();
	void Compile(const char* filename, const char* version, const char* entryPoint) override;
};
