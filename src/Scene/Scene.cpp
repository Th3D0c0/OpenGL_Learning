#include <iostream>
#include "Scene/Scene.h"


Scene::Scene(unsigned int screenWidth, unsigned int screenHeight)
	:m_DepthPrepass(std::make_unique<Shader>("Shaders/ForwardPlus/DepthPrepass.vert", "Shaders/ForwardPlus/DepthPrepass.frag")),
	m_LightCullingCS(std::make_unique<Shader>("Shaders/ForwardPlus/LightCulling.comp")),
	m_FrustumShader(std::make_unique<Shader>("Shaders/ForwardPlus/Frustum.comp")),
	m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight),
	m_TileSize(16),
	m_MaxLightCount(1024),
	m_firstRun(true)
{
	// --- Create the Light SSBO ---
	glGenBuffers(1, &m_LightSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LightSSBO);

	// --- Create the Visible Light Indices SSBO ---
	glGenBuffers(1, &m_VisibleIndicesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_VisibleIndicesSSBO);
	// Allocate memory for the output, but don't upload any data
	// Example size: max 1024 visible lights * number of tiles
	int numTiles = glm::ceil((m_ScreenWidth + m_TileSize - 1) / m_TileSize) * glm::ceil((m_ScreenHeight + m_TileSize - 1) / m_TileSize);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numTiles * m_MaxLightCount * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_AtomicCounterSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_AtomicCounterSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

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

	CreateLightGridTexture();
}

Scene::~Scene()
{
	glDeleteBuffers(1, &m_LightSSBO);
	glDeleteBuffers(1, &m_VisibleIndicesSSBO);
	glDeleteBuffers(1, &m_AtomicCounterSSBO);

	glDeleteTextures(1, &m_DepthTexture);
	glDeleteTextures(1, &m_LightGridTexture);
	glDeleteFramebuffers(1, &m_DepthFBO);
}

struct Plane
{
	glm::vec3 normal;
	float distance; // This float will be placed right after the vec3, but due to alignas(16) on the struct below, the total size will be correct.
};

struct alignas(16) Frustum
{
	Plane planes[4]; // left, right, top, bottom
};

void Scene::RenderScene(const glm::mat4& view, const glm::mat4& projection)
{
	for (const auto& pair : m_renderQueue)
	{
		Shader* shader = m_ShaderChache.GetShader(pair.first);
		shader->use();
		m_GlobalDrawProperties.shader = shader;
		m_GlobalDrawProperties.view = view;
		m_GlobalDrawProperties.projection = projection;

		shader->setUniformValue("view", view);
		shader->setUniformValue("projection", projection);
		shader->setUniformValue("ambientColor", glm::vec3(0.0f, 0.0f, 0.0f));
		shader->setUniformValue("TILE_SIZE", m_TileSize);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_LightSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_VisibleIndicesSSBO);
		glBindImageTexture(4, m_LightGridTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32UI);
		// Loop through the list of drawables
		for (const Drawable& drawableVariant : pair.second)
		{
			// std::visit will call the lambda with the correct pointer type
			std::visit([&](auto* drawablePtr) 
				{
					drawablePtr->Draw(m_GlobalDrawProperties);
				}, drawableVariant);
		}
	}
	
}

void Scene::CreateShaders()
{
	for (const auto& object : m_Object)
	{
		for (Mesh& mesh : object->GetMeshes())
		{
			m_ShaderChache.GetShader(mesh.GetFeatureFlag());
			m_renderQueue[mesh.GetFeatureFlag()].push_back(&mesh);
		}
	}
	for (const auto& planet : m_Planet)
	{
		m_ShaderChache.GetShader(planet->GetFeatureFlag());
		m_renderQueue[planet->GetFeatureFlag()].push_back(planet.get());
	}
}

void Scene::RenderPrepass(DrawProperties& properties)
{
	// For Frustum Buffer Creation
	if (m_firstRun == true)
	{
		createAndPopulateFrustumBuffer(properties);
		m_firstRun = false;
	}
	DepthPrepass(properties);
	LightCulling(properties);
}

// In Scene.cpp
void Scene::OnWindowResize(unsigned int newWidth, unsigned int newHeight)
{
	// Update screen dimensions first
	this->m_ScreenWidth = newWidth;
	this->m_ScreenHeight = newHeight;

	// --- Safely delete and recreate SSBOs ---
	if (m_VisibleIndicesSSBO != 0)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0); // Unbind from indexed binding point
		glDeleteBuffers(1, &m_VisibleIndicesSSBO);
	}

	// Calculate new buffer sizes
	int numTiles = glm::ceil((m_ScreenWidth + m_TileSize - 1) / m_TileSize) * glm::ceil((m_ScreenHeight + m_TileSize - 1) / m_TileSize);
	GLsizeiptr visibleIndicesSize = numTiles * m_MaxLightCount * sizeof(unsigned int);
	GLsizeiptr lightGridSize = numTiles * sizeof(glm::uvec2);

	// Re-create VisibleIndices SSBO
	glGenBuffers(1, &m_VisibleIndicesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_VisibleIndicesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, visibleIndicesSize, nullptr, GL_DYNAMIC_DRAW);

	// --- Safely delete and recreate Depth FBO and Texture ---
	if (m_DepthTexture != 0)
	{
		glDeleteTextures(1, &m_DepthTexture);
	}
	if (m_DepthFBO != 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind FBO before deleting
		glDeleteFramebuffers(1, &m_DepthFBO);
	}

	// Re-create Texture
	glGenTextures(1, &m_DepthTexture);
	glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, newWidth, newHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Re-create FBO and attach texture
	glGenFramebuffers(1, &m_DepthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind FBO

	// Recreate Light Grid Texture
	if (m_LightGridTexture != 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &m_LightGridTexture);
	}
	CreateLightGridTexture();

	// Recreate Frustum Precalculations
	if (m_FrustumSSBO != 0)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		glDeleteBuffers(1, &m_FrustumSSBO);
	}
	createAndPopulateFrustumBuffer(m_GlobalDrawProperties);
}

void Scene::AddLight(std::unique_ptr<Light> light)
{
	m_Lights.push_back(std::move(light));
}

// In your Scene.h or Scene.cpp
// Change the function signature
void Scene::UpdateLightBuffer(const glm::mat4& view) // <-- Add the view matrix parameter
{
	if (m_Lights.empty())
	{
		// ... (no change here)
		return;
	}

	std::vector<Light> lightsInViewSpace;
	lightsInViewSpace.reserve(m_Lights.size());

	for (auto& worldSpaceLightPtr : m_Lights)
	{
		// Create a copy of the light to modify it
		Light viewSpaceLight = *worldSpaceLightPtr;

		viewSpaceLight.positionWS = view * worldSpaceLightPtr->positionWS;

		// For direction, we only want rotation, not translation, so w=0.
		viewSpaceLight.directionWS = view * glm::vec4(worldSpaceLightPtr->directionWS.x, worldSpaceLightPtr->directionWS.y, worldSpaceLightPtr->directionWS.z,  0.0f);

		lightsInViewSpace.push_back(viewSpaceLight);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_LightSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lightsInViewSpace.size() * sizeof(Light), lightsInViewSpace.data(), GL_DYNAMIC_DRAW);
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
	properties.shader->setUniformValue("projection", properties.projection);

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

	// Set Uniforms (This part was correct)
	const glm::vec2 screenSize(m_ScreenWidth, m_ScreenHeight);
	m_LightCullingCS->setUniformValue("inverseProjection", glm::inverse(properties.projection));
	m_LightCullingCS->setUniformValue("screenDimensions", screenSize);

	// Bind depth texture (This part was correct)
	int depthTextureUnit = 0;
	m_LightCullingCS->setUniformValue("depthMap", depthTextureUnit);
	glBindTextureUnit(depthTextureUnit, m_DepthTexture);

	// Bind all the SSBOs to their correct locations
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_LightSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_FrustumSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_VisibleIndicesSSBO);
	// Reset the global atomic counter to 0
	GLuint zero = 0;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_AtomicCounterSSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &zero);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_AtomicCounterSSBO);
	glBindImageTexture(4, m_LightGridTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32UI);

	glDispatchCompute(glm::ceil(properties.screenWidth / m_TileSize), glm::ceil(properties.screenHeight / m_TileSize), 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Scene::CreateLightGridTexture()
{
	const unsigned int numTilesX = (m_ScreenWidth + m_TileSize - 1) / m_TileSize;
	const unsigned int numTilesY = (m_ScreenHeight + m_TileSize - 1) / m_TileSize;

	glGenTextures(1, &m_LightGridTexture);
	glBindTexture(GL_TEXTURE_2D, m_LightGridTexture);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32UI, numTilesX, numTilesY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

}

void Scene::createAndPopulateFrustumBuffer(DrawProperties& properties)
{
	const unsigned int numTilesX = (m_ScreenWidth + m_TileSize - 1) / m_TileSize;
	const unsigned int numTilesY = (m_ScreenHeight + m_TileSize - 1) / m_TileSize;
	const unsigned int numTiles = numTilesX * numTilesY;

	glGenBuffers(1, &m_FrustumSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_FrustumSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numTiles * sizeof(Frustum), NULL, GL_STATIC_DRAW);

	m_FrustumShader->use();
	glm::mat4 invProjection = glm::inverse(properties.projection);
	glm::vec2 screenDimensioms = glm::vec2((float)m_ScreenWidth, (float)m_ScreenHeight);
	m_FrustumShader->setUniformValue("inverseProjection", invProjection);
	m_FrustumShader->setUniformValue("screenDimensions", screenDimensioms);
	m_FrustumShader->setUniformValue("TILE_SIZE", m_TileSize);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_FrustumSSBO);
	glDispatchCompute(numTilesX, numTilesY, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glUseProgram(0);
}


