#pragma once

#include <string>

enum class BlockMeshType
{
	Cube, 
	Cross 
};

struct Block
{
    std::string name;
    BlockMeshType meshType = BlockMeshType::Cube;
    bool isSolid = true;
    bool isTransparent = false;

    // Texture Atlas indices for each face
    int texture_top = 0;
    int texture_bottom = 0;
    int texture_side = 0;
};