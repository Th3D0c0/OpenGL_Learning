#pragma once

#include <vector>
#include "Scene/Mesh/Mesh.h"
#include "Scene/Transform.h"
#include "ShaderClass/Shader.h"
#include "glm/glm.hpp"

class Object
{
public:
	Object();
	~Object();

	void Draw(DrawProperties& properties);
	void DrawObjectDepthPrepass(DrawProperties& properties);

	void SetLocation(const glm::vec3& location);
	void SetRotation(const glm::vec3& rotation);
	void SetScale(const glm::vec3& scale);

	std::vector<Mesh> GetMeshes();

	DrawProperties properties;

private:
	std::vector<Mesh> m_Meshes;
	Transform m_Transform;
};