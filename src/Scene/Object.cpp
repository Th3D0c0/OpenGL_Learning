#include "Scene/Object.h"


Object::Object()
{
}

Object::~Object()
{
}

void Object::DrawObject(DrawProperties& properties)
{
	for (Mesh& mesh: m_Meshes)
	{
		mesh.Draw(properties);
	}
}

void Object::DrawObjectDepthPrepass(DrawProperties& properties)
{
	for (Mesh& mesh : m_Meshes)
	{
		glm::mat4 meshLocalMatrix = mesh.GetModelMatrix();

		// Calculate the final world matrix for this specific mesh
		glm::mat4 finalMatrix = this->m_Transform.GetModelMatrix() * meshLocalMatrix;

		// Set the final model matrix uniform for this mesh
		properties.shader->setUniformValue("model", finalMatrix);
		mesh.DrawMeshDepthPrepass(properties);
	}
}

void Object::SetLocation(const glm::vec3& location)
{
	m_Transform.SetLocation(location);
}

void Object::SetRotation(const glm::vec3& rotation)
{
	m_Transform.SetRotation(rotation);
}

void Object::SetScale(const glm::vec3& scale)
{
	m_Transform.SetScale(scale);
}
