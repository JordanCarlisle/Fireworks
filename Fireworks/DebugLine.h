#pragma once
#include <vector>
#include <Windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <vector>
#include <DirectXMath.h>
#include "v3.h"
#include "AlignmentClass.h"

using namespace DirectX;

extern D3D11_INPUT_ELEMENT_DESC LineInputLayout[2];

class DebugLine : public AlignmentClass16
{
public:
	struct vertex
	{
		v3 pos;
		v3 colour;
	};
	struct constant_buffer
	{
		XMMATRIX matWorldViewProj;
	};
private:
	std::vector<DWORD> indices;
	std::vector<vertex> vertices;

	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;

public:


	DebugLine() = default;
	DebugLine(std::vector<DWORD>& indexList, std::vector<vertex>& vertexList);
	~DebugLine();
	void AddIndex(DWORD index);
	void AddVertex(v3 pos, v3 colour);
	void AddVertexWithIndex(v3 pos, v3 colour);

	void AddIndices(std::vector<DWORD>& indexList);
	void AddVertices(std::vector<vertex>& vertexList);
	void End();

	int Vertices();
	int Indices();

	void SetupBuffers();

	ID3D11Buffer* GetIndexBuffer();
	ID3D11Buffer* GetVertexBuffer();
	std::vector<vertex>& GetVertexData() { return vertices; }
	std::vector<DWORD>& GetIndexData() { return indices; }
};