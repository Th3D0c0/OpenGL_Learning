#pragma once
#include <memory>
#include "BlockID.h"

// Forward declarations
class Mesh;
class Shader;
class BlockRegistry;

class Chunk
{
public:
    static const int CHUNK_SIZE = 16;

    Chunk();

    // Sets a block and returns true if the mesh needs rebuilding
    bool setBlock(int x, int y, int z, BlockID id);
    BlockID getBlock(int x, int y, int z) const;

    void generateMesh(const BlockRegistry& registry);
    void draw(Shader& shader);

private:
    BlockID m_blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
    std::unique_ptr<Mesh> m_mesh;
};