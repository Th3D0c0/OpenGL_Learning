#pragma once

#include <vector>
#include "Scene/Mesh/Mesh.h"
#include "Scene/Transform.h"
#include "Shader.h"
#include "glm/glm.hpp"

class Object
{
public:
	Object();
	~Object();

	void DrawObject(DrawProperties& properties);
	void DrawObjectDepthPrepass(DrawProperties& properties);

	void SetLocation(const glm::vec3& location);
	void SetRotation(const glm::vec3& rotation);
	void SetScale(const glm::vec3& scale);

	DrawProperties properties;

private:
	std::vector<Mesh> m_Meshes;
	Transform m_Transform;
};