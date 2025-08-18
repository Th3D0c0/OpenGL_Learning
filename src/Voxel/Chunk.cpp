#include "Voxel/Chunk.h"
#include "Mesh.h"
#include "Voxel/BlockRegistry.h"
#include <vector>

Chunk::Chunk()
{
    // Initialize all blocks to Air
    for (int x = 0; x < CHUNK_SIZE; ++x)
    {
        for (int y = 0; y < CHUNK_SIZE; ++y)
        {
            for (int z = 0; z < CHUNK_SIZE; ++z)
            {
                m_blocks[x][y][z] = BlockID::Air;
            }
        }
    }
}

BlockID Chunk::getBlock(int x, int y, int z) const
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
    {
        return BlockID::Air; // Treat out-of-bounds as Air
    }
    return m_blocks[x][y][z];
}

bool Chunk::setBlock(int x, int y, int z, BlockID id)
{
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
    {
        return false;
    }
    if (m_blocks[x][y][z] != id)
    {
        m_blocks[x][y][z] = id;
        return true; // Mesh needs to be rebuilt
    }
    return false;
}

void Chunk::draw(Shader& shader)
{
    if (m_mesh)
    {
        m_mesh->Draw(shader);
    }
}

void Chunk::generateMesh(const BlockRegistry& registry)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int index_counter = 0;

    for (int x = 0; x < CHUNK_SIZE; ++x)
    {
        for (int y = 0; y < CHUNK_SIZE; ++y)
        {
            for (int z = 0; z < CHUNK_SIZE; ++z)
            {
                BlockID current_block_id = m_blocks[x][y][z];
                if (current_block_id == BlockID::Air)
                {
                    continue; // Skip air blocks
                }

                const Block& block_props = registry.GetBlockProperties(current_block_id);

                // Check each of the 6 faces
                // Face directions: {normal}, {up}, {right}
                glm::vec3 faceData[6][3] = {
                    { { 0, 0, 1 },  { 0, 1, 0 }, { -1, 0, 0 } }, // Front
                    { { 0, 0, -1 }, { 0, 1, 0 }, { 1, 0, 0 } },  // Back
                    { { 1, 0, 0 },  { 0, 1, 0 }, { 0, 0, -1 } }, // Right
                    { { -1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } },  // Left
                    { { 0, 1, 0 },  { 0, 0, 1 }, { 1, 0, 0 } },  // Top
                    { { 0, -1, 0 }, { 0, 0, -1 },{ 1, 0, 0 } }   // Bottom
                };

                for (int i = 0; i < 6; ++i)
                {
                    glm::vec3 dir = faceData[i][0];
                    BlockID neighbor_id = getBlock(x + dir.x, y + dir.y, z + dir.z);
                    const Block& neighbor_props = registry.GetBlockProperties(neighbor_id);

                    if (!neighbor_props.isSolid || neighbor_props.isTransparent)
                    {
                        // This face is visible, so we add its vertices
                        glm::vec3 normal = faceData[i][0];
                        glm::vec3 up = faceData[i][1];
                        glm::vec3 right = faceData[i][2];

                        glm::vec3 pos = {(float)x, (float)y, (float)z};

                        // Define the 4 vertices for this face
                        glm::vec3 v1 = pos + (normal - up - right) * 0.5f;
                        glm::vec3 v2 = pos + (normal - up + right) * 0.5f;
                        glm::vec3 v3 = pos + (normal + up + right) * 0.5f;
                        glm::vec3 v4 = pos + (normal + up - right) * 0.5f;

                        // Define UVs
                        glm::vec2 uv1 = {0.0f, 0.0f};
                        glm::vec2 uv2 = {1.0f, 0.0f};
                        glm::vec2 uv3 = {1.0f, 1.0f};
                        glm::vec2 uv4 = {0.0f, 1.0f};

                        vertices.push_back({v1, normal, uv1});
                        vertices.push_back({v2, normal, uv2});
                        vertices.push_back({v3, normal, uv3});
                        vertices.push_back({v4, normal, uv4});

                        // Add indices for the two triangles
                        indices.push_back(index_counter + 0);
                        indices.push_back(index_counter + 1);
                        indices.push_back(index_counter + 2);
                        indices.push_back(index_counter + 0);
                        indices.push_back(index_counter + 2);
                        indices.push_back(index_counter + 3);
                        index_counter += 4;
                    }
                }
            }
        }
    }

    if (vertices.empty())
    {
        m_mesh.reset(); // No visible faces, no mesh
    }
    else
    {
        // We don't have a real Texture class yet, so this is empty
        std::vector<Texture> textures;
        Texture texture("res/brick.png", "texture_diffuse");
        if (texture.LoadTexture())
        {
            textures.push_back(texture);
        }

        m_mesh = std::make_unique<Mesh>(vertices, indices, textures);
    }
}