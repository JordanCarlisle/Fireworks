#pragma once
#include <vector>
#include <Windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <vector>
#include "v3.h"
#include <DirectXMath.h>
#include "AlignmentClass.h"

using namespace DirectX;

extern D3D11_INPUT_ELEMENT_DESC MeshInputLayout[2];

class Mesh : public AlignmentClass16
{
public:
	struct vertex
	{
		v3 pos;
		v3 norm;
	};
	struct constant_buffer
	{
		XMMATRIX matWorldViewProj;
		XMMATRIX matWorld;
		XMMATRIX matModel;
	};
private:
	std::vector<DWORD> indices;
	std::vector<vertex> vertices;
	
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;

	v3 bounds[2];
public:


	Mesh() = default;
	Mesh(std::vector<DWORD>& indexList, std::vector<vertex>& vertexList);
	virtual ~Mesh();
	void AddIndex(DWORD index);
	void AddVertex(float x, float y, float z, float nx, float ny, float nz);

	void AddIndices(std::vector<DWORD>& indexList);
	void AddVertices(std::vector<vertex>& vertexList);

	int Vertices();
	int Indices();

	void SetupBuffers();

	ID3D11Buffer* GetIndexBuffer();
	ID3D11Buffer* GetVertexBuffer();
	std::vector<vertex>& GetVertexData() { return vertices; }
	void GetBounds(v3 bound[2]);
};