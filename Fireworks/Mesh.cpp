#include "Mesh.h"
#include "App.h"

extern ID3D11Device* d3d11Device;
extern ID3D11DeviceContext* d3d11DevCon;

D3D11_INPUT_ELEMENT_DESC MeshInputLayout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

Mesh::Mesh(std::vector<DWORD>& indexList, std::vector<vertex>& vertexList)
{
	AddIndices(indexList);
	AddVertices(vertexList);
	SetupBuffers();
}

Mesh::~Mesh()
{
	if(indexBuffer)
		indexBuffer->Release();
	if(vertexBuffer)
		vertexBuffer->Release();
}

void Mesh::AddIndex(DWORD index)
{
	indices.emplace_back(index);
}

void Mesh::AddVertex(float x, float y, float z, float nx, float ny, float nz)
{
	vertex v = {
		{ x, y, z },
		{ nx, ny, nz }
	};
	vertices.emplace_back(v);
}

void Mesh::AddIndices(std::vector<DWORD>& indexList)
{
	for (auto index : indexList)
	{
		AddIndex(index);
	}
}

void Mesh::AddVertices(std::vector<vertex>& vertexList)
{
	for (auto& v : vertexList)
	{
		AddVertex(v.pos.x , v.pos.y, v.pos.z, v.norm.x, v.norm.y, v.norm.z);
	}
}

int Mesh::Vertices()
{
	return vertices.size();
}

int Mesh::Indices()
{
	return indices.size();
}

void Mesh::SetupBuffers()
{
	//calculate normals
	for (uint32_t j = 0; j < indices.size(); j += 3)
	{
		Mesh::vertex& v1 = vertices[indices[j]];
		Mesh::vertex& v2 = vertices[indices[j + 1]];
		Mesh::vertex& v3 = vertices[indices[j + 2]];

	//	v1.norm = { 0, 0, 0 };
	//	v2.norm = { 0, 0, 0 };
	//	v3.norm = { 0, 0, 0 };

		XMVECTOR vertPos1 = XMVectorSet(v1.pos.x, v1.pos.y, v1.pos.z, 1.0f);
		XMVECTOR vertPos2 = XMVectorSet(v2.pos.x, v2.pos.y, v2.pos.z, 1.0f);
		XMVECTOR vertPos3 = XMVectorSet(v3.pos.x, v3.pos.y, v3.pos.z, 1.0f);

		XMVECTOR a = XMVectorSubtract(vertPos1, vertPos2);
		XMVECTOR b = XMVectorSubtract(vertPos1, vertPos3);
		a = XMVector3Cross(a, b);
		a = XMVector3Normalize(a);

		float norm[3]{
			XMVectorGetByIndex(a, 0),
			XMVectorGetByIndex(a, 1),
			XMVectorGetByIndex(a, 2)
		};

		v1.norm.x += norm[0];
		v1.norm.y += norm[1];
		v1.norm.z += norm[2];

		v2.norm.x += norm[0];
		v2.norm.y += norm[1];
		v2.norm.z += norm[2];

		v3.norm.x += norm[0];
		v3.norm.y += norm[1];
		v3.norm.z += norm[2];
	}

	//set bounds
	for (auto& vert : vertices)
	{
		if (vert.pos.x < bounds[0].x) bounds[0].x = vert.pos.x;
		if (vert.pos.y < bounds[0].y) bounds[0].y = vert.pos.y;
		if (vert.pos.z < bounds[0].z) bounds[0].z = vert.pos.z;

		if (vert.pos.x > bounds[1].x) bounds[1].x = vert.pos.x;
		if (vert.pos.y > bounds[1].y) bounds[1].y = vert.pos.y;
		if (vert.pos.z > bounds[1].z) bounds[1].z = vert.pos.z;

		XMVECTOR norm = XMVectorSet(vert.norm.x, vert.norm.y, vert.norm.z, 1.0f);
		norm = XMVector3Normalize(norm);
		vert.norm.x = XMVectorGetByIndex(norm, 0);
		vert.norm.y = XMVectorGetByIndex(norm, 1);
		vert.norm.z = XMVectorGetByIndex(norm, 2);
	}

	//index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = indices.data();
	d3d11Device->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer);

	//vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(vertex) * vertices.size();  //  VERTEX SIZE * NUMBER OF VERTICES
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = vertices.data();
	d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer);
	
	
}

ID3D11Buffer* Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

ID3D11Buffer* Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

void Mesh::GetBounds(v3 bound[2])
{
	bound[0] = bounds[0];
	bound[1] = bounds[1];
}