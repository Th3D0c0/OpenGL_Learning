#pragma once

#include "Mesh.h"

class Sphere: public Mesh
{
public:
	Sphere(float radius, int sectorCount, int stackCount);
};