#pragma once

#include <vector>
#include <memory>

#include "Scene/Object.h"
#include "PlanetGenerator/PlanetGen.h"
#include "Scene/Skybox/Skybox.h"
#include "Scene/Light.h"
#include "Shader.h"
#include "Scene/Mesh/MeshData.h"

#include "glm/glm.hpp"
#include "gl/glew.h"

class Scene
{
public:
	Scene(unsigned int m_ScreenWidth, unsigned int m_ScreenHeight);
	~Scene();

	void RenderScene(const glm::mat4& view, const glm::mat4& projection);
	void RenderPrepass(DrawProperties& properties);
	void OnWindowResize(unsigned int newWidth, unsigned int newHeight);

	void AddLight(std::unique_ptr<Light> light);
	void UpdateLightBuffer();

	void AddObject(std::unique_ptr<Planet> planet);
	void AddObject(std::unique_ptr<Object> object);

private:
	std::vector<std::unique_ptr<Object>> m_Object;
	std::vector<std::unique_ptr<Light>> m_Lights;
	std::vector<std::unique_ptr<Planet>> m_Planet;
	Skybox m_Skybox;
	std::unique_ptr<Shader>m_DepthPrepass;
	std::unique_ptr<Shader>m_LightCullingCS;
	std::unique_ptr<Shader>m_ObjectShader;
	GLuint m_LightSSBO, m_VisibleIndicesSSBO, m_LightGridSSBO;
	unsigned int m_ScreenWidth, m_ScreenHeight;

	unsigned int m_DepthFBO;
	unsigned int m_DepthTexture;

	DrawProperties m_GlobalDrawProperties;

	void DepthPrepass(DrawProperties& properties);
	void LightCulling(DrawProperties& properties);

};