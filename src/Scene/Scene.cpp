#include "Scene/Scene.h"
#include <iostream>


Scene::Scene(unsigned int screenWidth, unsigned int screenHeight)
	:m_DepthPrepass(std::make_unique<Shader>("Shaders/ForwardPlus/DepthPrepass.vert", "Shaders/ForwardPlus/DepthPrepass.frag")),
	m_LightCullingCS(std::make_unique<Shader>("Shaders/ForwardPlus/LightCulling.comp")),
	m_ObjectShader(std::make_unique<Shader>("Shaders/ForwardPlus/Object.vert", "Shaders/ForwardPlus/Object.frag")),
	m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight)
{
	// --- Create the Light SSBO ---
	glGenBuffers(1, &m_LightSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LightSSBO);

	// --- Create the Visible Light Indices SSBO ---
	glGenBuffers(1, &m_VisibleIndicesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_VisibleIndicesSSBO);
	// Allocate memory for the output, but don't upload any data
	// Example size: max 1024 visible lights * number of tiles
	const int MAX_VISIBLE_LIGHTS = 1024;
	int numTiles = glm::ceil(m_ScreenWidth / 16.0f) * glm::ceil(m_ScreenHeight / 16.0f);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numTiles * MAX_VISIBLE_LIGHTS * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_LightGridSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LightGridSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), 0, GL_DYNAMIC_DRAW);

	// Unbind the buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//--------For Depth Framebuffer Creation----------
	// 1. Create the depth texture
	glGenTextures(1, &m_DepthTexture);
	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
	// Use the same dimensions as your screen
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_ScreenWidth, m_ScreenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// 2. Create the FBO
	glGenFramebuffers(1, &m_DepthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthFBO);

	// 3. Attach the texture to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);

	// 4. Tell OpenGL we're not going to draw to any color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// Check if the FBO is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Depth FBO is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_GlobalDrawProperties.shader = m_ObjectShader.get();
}

Scene::~Scene()
{
}

void Scene::RenderScene(const glm::mat4& view, const glm::mat4& projection)
{;
	m_GlobalDrawProperties.shader->use();
	m_GlobalDrawProperties.view = view;
	m_GlobalDrawProperties.projection = projection;

	m_GlobalDrawProperties.shader->setUniformValue("view", view);
	m_GlobalDrawProperties.shader->setUniformValue("projection", projection);

	for (const auto& objectPtr : m_Object)
	{
		objectPtr->DrawObject(m_GlobalDrawProperties);
	}

	for (const auto& planetPtr : m_Planet)
	{
		planetPtr->Draw(m_GlobalDrawProperties);
	}
}

void Scene::RenderPrepass(DrawProperties& properties)
{
	DepthPrepass(properties);
	LightCulling(properties);
}

void Scene::OnWindowResize(unsigned int newWidth, unsigned int newHeight)
{
	if (m_VisibleIndicesSSBO != 0)
	{
		glDeleteBuffers(1, &m_VisibleIndicesSSBO);
	}

	float TILE_SIZE = 16.0f;
	const int MAX_VISIBLE_LIGHTS = 1024;
	int numTiles = (static_cast<float>(newWidth) / TILE_SIZE) * (static_cast<float>(newHeight) / TILE_SIZE);
	GLsizeiptr newSize = numTiles * MAX_VISIBLE_LIGHTS * sizeof(unsigned int);

	glGenBuffers(1, &m_VisibleIndicesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_VisibleIndicesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	this->m_ScreenWidth = newWidth;
	this->m_ScreenHeight= newHeight;

	if (m_DepthTexture != 0)
	{
		glDeleteTextures(1, &m_DepthTexture);
	}

	glGenTextures(1, &m_DepthTexture);
	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, newWidth, newHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::AddLight(std::unique_ptr<Light> light)
{
	m_Lights.push_back(std::move(light));
}

void Scene::UpdateLightBuffer()
{
	if (m_Lights.empty())
	{
		// If there are no lights, you might still want to upload an empty buffer
		// to avoid using old data from a previous frame.
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LightSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		return;
	}

	std::vector<Light> lightData;
	lightData.reserve(m_Lights.size()); 

	for (const auto& lightPtr : m_Lights)
	{
		lightData.push_back(*lightPtr); 
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LightSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lightData.size() * sizeof(Light), lightData.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Scene::AddObject(std::unique_ptr<Planet> planet)
{
	m_Planet.push_back(std::move(planet));
}

void Scene::AddObject(std::unique_ptr<Object> object)
{
	m_Object.push_back(std::move(object));
}

void Scene::DepthPrepass(DrawProperties& properties)
{
	// Bind the Depth prepass Shader
	properties.shader = m_DepthPrepass.get();
	properties.shader->use();

	//Bind the Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set Uniform variables
	properties.shader->setUniformValue("view", properties.view);
	properties.shader->setUniformValue("perspective", properties.projection);

	// Diable Color (not needed for Framebuffer)
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// Loop over all the Objects in the scene and render to the depth buffer
	for (const auto& objectPtr : m_Object)
	{
		objectPtr->DrawObjectDepthPrepass(properties);
	}

	for (const auto& planetPtr : m_Planet)
	{
		planetPtr->DrawPlanetDepthPrepass(properties);
	}

	// Enable the Color and unbind the Depth Framebuffer
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::LightCulling(DrawProperties& properties)
{
	m_LightCullingCS->use();

	// Set Uniforms
	const glm::vec2 screenSize(m_ScreenWidth, m_ScreenHeight);
	m_LightCullingCS->setUniformValue("viewMatrix", properties.view);
	m_LightCullingCS->setUniformValue("projectionMatrix", properties.projection);
	m_LightCullingCS->setUniformValue("inverseProjectionMatrix", glm::inverse(properties.projection));
	m_LightCullingCS->setUniformValue("screenSize", screenSize);

	// Choose texture unit 0
	int depthTextureUnit = 0;
	// Set the sampler uniform in the shader to use texture unit 0
	m_LightCullingCS->setUniformValue("depthMap", depthTextureUnit);
	// Bind your depth texture to texture unit 0
	glBindTextureUnit(depthTextureUnit, m_DepthTexture);


	// TODO: Fix horrible fps because of atomic counter!!!
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_LightSSBO);
	GLuint zero = 0;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LightGridSSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &zero); // Reset counter to 0
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_LightGridSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_VisibleIndicesSSBO);

	glDispatchCompute(glm::ceil(properties.screenWidth / 16.0f), glm::ceil(properties.screenHeight/ 16.0f), 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}


