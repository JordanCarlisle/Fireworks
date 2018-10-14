#include "Shader.h"
#include "App.h"

extern ID3D11Device* d3d11Device;
extern ID3D11DeviceContext* d3d11DevCon;

VertexShader::~VertexShader()
{
	program->Release();
}

void VertexShader::Compile(const char* filename, const char* version, const char* entryPoint)
{
	D3DX11CompileFromFileA(filename, 0, 0, entryPoint, version, 0, 0, 0, &byteCodeBuffer, 0, 0);
	d3d11Device->CreateVertexShader(byteCodeBuffer->GetBufferPointer(), byteCodeBuffer->GetBufferSize(), NULL, &program);
}


PixelShader::~PixelShader()
{
	program->Release();
}

void PixelShader::Compile(const char* filename, const char* version, const char* entryPoint)
{
	D3DX11CompileFromFileA(filename, 0, 0, entryPoint, version, 0, 0, 0, &byteCodeBuffer, 0, 0);
	d3d11Device->CreatePixelShader(byteCodeBuffer->GetBufferPointer(), byteCodeBuffer->GetBufferSize(), NULL, &program);
}

GeometryShader::~GeometryShader()
{
	program->Release();
}

void GeometryShader::Compile(const char* filename, const char* version, const char* entryPoint)
{
	D3DX11CompileFromFileA(filename, 0, 0, entryPoint, version, 0, 0, 0, &byteCodeBuffer, 0, 0);
	d3d11Device->CreateGeometryShader(byteCodeBuffer->GetBufferPointer(), byteCodeBuffer->GetBufferSize(), NULL, &program);
}