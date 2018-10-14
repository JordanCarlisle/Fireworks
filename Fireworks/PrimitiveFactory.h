#pragma once

#include "Mesh.h"

void prim_cube(std::vector<DWORD>& i, std::vector<Mesh::vertex>& v, float width, float height, float length);
void prim_cylinder(std::vector<DWORD>& ib, std::vector<Mesh::vertex>& vb, unsigned int slices, float radius, float height);
void prim_sphere(std::vector<DWORD>& ib, std::vector<Mesh::vertex>& vb, unsigned int slices, float radius);
