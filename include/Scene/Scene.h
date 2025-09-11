#pragma once

#include <vector>
#include <memory>
#include <variant>

#include "Scene/Object.h"
#include "PlanetGenerator/PlanetGen.h"
#include "Scene/Skybox/Skybox.h"
#include "Scene/Light.h"
#include "ShaderClass/Shader.h"
#include "Scene/Mesh/MeshData.h"
#include "ShaderClass/ShaderCache.h"

#include "glm/glm.hpp"

class Scene
{
public:
	Scene(unsigned int m_ScreenWidth, unsigned int m_ScreenHeight);
	~Scene();

	void RenderScene(const glm::mat4& view, const glm::mat4& projection);
	void CreateShaders();
	void RenderPrepass(DrawProperties& properties);
	void OnWindowResize(unsigned int newWidth, unsigned int newHeight);

	void AddLight(std::unique_ptr<Light> light);
	void UpdateLightBuffer(const glm::mat4& view);

	void AddObject(std::unique_ptr<Planet> planet);
	void AddObject(std::unique_ptr<Object> object);

	using Drawable = std::variant<Mesh*, Planet*>;

private:
	std::vector<std::unique_ptr<Object>> m_Object;
	std::vector<std::unique_ptr<Light>> m_Lights;
	std::vector<std::unique_ptr<Planet>> m_Planet;
	Skybox m_Skybox;
	std::unique_ptr<Shader>m_DepthPrepass;
	std::unique_ptr<Shader>m_LightCullingCS;
	std::unique_ptr<Shader>m_FrustumShader;
	GLuint m_LightSSBO, m_VisibleIndicesSSBO, m_AtomicCounterSSBO, m_FrustumSSBO;
	DrawProperties m_GlobalDrawProperties;
	ShaderCache m_ShaderChache;
	std::map <uint32_t, std::vector<Drawable>> m_renderQueue;
	unsigned int m_ScreenWidth, m_ScreenHeight;

	unsigned int m_DepthFBO;
	unsigned int m_DepthTexture;
	unsigned int m_LightGridTexture;

	unsigned int m_TileSize;
	unsigned int m_MaxLightCount;

	bool m_firstRun;

	void DepthPrepass(DrawProperties& properties);
	void LightCulling(DrawProperties& properties);
	void CreateLightGridTexture();
	void createAndPopulateFrustumBuffer(DrawProperties& properties);
};