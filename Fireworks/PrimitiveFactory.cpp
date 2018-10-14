#include "PrimitiveFactory.h"

static const float PI = 3.14159265359;

//Generate cube vertex and index buffer
void prim_cube(std::vector<DWORD>& i, std::vector<Mesh::vertex>& v, float width, float length, float height)
{
	i = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	width *= 0.5;
	height *= 0.5;
	length *= 0.5;

	v = {
		{ { -width, -length, -height },{ 0.0, 0.0, 0.0 } },
		{ { -width, +length, -height },{ 0.0, 0.0, 0.0 } },
		{ { +width, +length, -height },{ 0.0, 0.0, 0.0 } },
		{ { +width, -length, -height },{ 0.0, 0.0, 0.0 } },
		{ { -width, -length, +height },{ 0.0, 0.0, 0.0 } },
		{ { -width, +length, +height },{ 0.0, 0.0, 0.0 } },
		{ { +width, +length, +height },{ 0.0, 0.0, 0.0 } },
		{ { +width, -length, +height },{ 0.0, 0.0, 0.0 } }
	};
}

//Generate cylinder vertex and index buffer
void prim_cylinder(std::vector<DWORD>& ib, std::vector<Mesh::vertex>& vb, unsigned int slices, float radius, float height)
{
	//top vertex
	vb = { { { 0.0f, 0.0f,  height / 2 },{ 0.0, 0.0, 0.0 } } };

	//top cone
	for (int j = 0; j < slices; j++)
	{
		float angle = (float)j * (2 * PI / (float)slices);
		float x = -cosf(angle) * radius;
		float y = sinf(angle) * radius; 
		float z = height / 2;
		vb.emplace_back(Mesh::vertex{ { x, y, z },{ 0.0, 0.0, 0.0 } });
	}

	//bottom cone
	for (int j = 0; j < slices; j++)
	{
		float angle = (float)j * (2 * PI / (float)slices);
		float x = -cosf(angle) * radius;
		float y = sinf(angle) * radius; 
		float z = -height / 2;
		vb.emplace_back(Mesh::vertex{ { x, y, z },{ 0.0, 0.0, 0.0 } });
	}

	//top cone
	for (int i = 1; i <= slices; i++)
	{
		int wrapIndex = (i != slices) ? (i + 1) : 1;
		ib.emplace_back(0);
		ib.emplace_back(wrapIndex);
		ib.emplace_back(i);
	}

	for (int j = 1; j <= slices; j++)
	{
		ib.emplace_back(j);
		if (j != slices)
		{
			ib.emplace_back(j + 1);
			ib.emplace_back(slices + j);

			ib.emplace_back(slices + j);
			ib.emplace_back(j + 1);
			ib.emplace_back(slices + j + 1);
		}
		else
		{
			ib.emplace_back(1);
			ib.emplace_back(slices + j);

			ib.emplace_back(slices + j);
			ib.emplace_back(1);
			ib.emplace_back(slices + 1);
		}
	}
}

//Generate sphere vertex and index buffer
void prim_sphere(std::vector<DWORD>& ib, std::vector<Mesh::vertex>& vb, unsigned int slices, float radius)
{
	vb = { { { 0.0f, 0.0f, radius },{ 0.0, 0.0, 0.0 } } };
	for (int i = 1; i <= slices - 1; i++)
	{
		float tilt = (float)i * (PI / (float)slices);

		for (int j = 0; j <= slices; j++)
		{
			float pan = (float)j * (2 * PI / (float)slices);
			float x = -cosf(pan) * sinf(tilt) * radius;
			float y = sinf(pan) * sinf(tilt) * radius;
			float z = cosf(tilt) * radius;
			vb.emplace_back(Mesh::vertex{ { x, y, z },{ 0.0, 0.0, 0.0 } });
		}
	}
	vb.emplace_back(Mesh::vertex{ { 0.0f, 0.0f, -radius }, { 0.0, 0.0, 0.0 } });

	//top cone
	for (int i = 1; i <= slices; i++)
	{
		ib.emplace_back(0);
		ib.emplace_back(i + 1);
		ib.emplace_back(i);
	}

	int segVerts = 1 + slices;
	for (int i = 0; i < slices - 2; i++)
	{
		for (int j = 0; j < slices; j++)
		{
			ib.emplace_back(i * segVerts + j + 1);
			ib.emplace_back(i * segVerts + j + 2);
			ib.emplace_back((i + 1) * segVerts + j + 1);

			ib.emplace_back((i + 1) * segVerts + j + 1);
			ib.emplace_back(i * segVerts + j + 2);
			ib.emplace_back((i + 1) * segVerts + j + 2);
		}
	}

	//bottom cone
	int lastVertIndex = vb.size() - 1;
	for (int i = 0; i < slices; i++)
	{
		ib.emplace_back(lastVertIndex);
		ib.emplace_back(i + lastVertIndex - segVerts);
		ib.emplace_back(1 + i + lastVertIndex - segVerts);
	}
}