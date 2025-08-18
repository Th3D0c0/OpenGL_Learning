#pragma once 

#include "Mesh.h"

class Cube : public Mesh
{
public: 
	Cube(float length, float height, float width);
	~Cube();
};