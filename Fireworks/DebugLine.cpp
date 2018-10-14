#include "DebugLine.h"
#include "App.h"

extern ID3D11Device* d3d11Device;
extern ID3D11DeviceContext* d3d11DevCon;

D3D11_INPUT_ELEMENT_DESC LineInputLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOUR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

DebugLine::DebugLine(std::vector<DWORD>& indexList, std::vector<vertex>& vertexList)
{
	AddIndices(indexList);
	AddVertices(vertexList);
	SetupBuffers();
}

DebugLine::~DebugLine()
{
	if (indexBuffer)
		indexBuffer->Release();
	if (vertexBuffer)
		vertexBuffer->Release();
}

void DebugLine::AddIndex(DWORD index)
{
	indices.emplace_back(index);
}

void DebugLine::AddVertex(v3 pos, v3 colour)
{
	vertex v = { pos, colour };
	vertices.emplace_back(v);
}

void DebugLine::AddVertexWithIndex(v3 pos, v3 colour)
{
	AddVertex(pos, colour);
	AddIndex(Indices());
}

void DebugLine::AddIndices(std::vector<DWORD>& indexList)
{
	for (auto index : indexList)
	{
		AddIndex(index);
	}
}

void DebugLine::AddVertices(std::vector<vertex>& vertexList)
{
	for (auto& v : vertexList)
	{
		AddVertex(v.pos, v.colour);
	}
}

int DebugLine::Vertices()
{
	return vertices.size();
}

int DebugLine::Indices()
{
	return indices.size();
}

void DebugLine::SetupBuffers()
{
	indices.reserve(1024);
	vertices.reserve(1024);

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 1024;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = indices.data();
	auto r = d3d11Device->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(vertex) * 1024;  //  VERTEX SIZE * NUMBER OF VERTICES
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = vertices.data();
	r = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer);
}

ID3D11Buffer* DebugLine::GetIndexBuffer()
{
	return indexBuffer;
}

ID3D11Buffer* DebugLine::GetVertexBuffer()
{
	return vertexBuffer;
}

void DebugLine::End()
{
	vertices.clear();
	indices.clear();
}