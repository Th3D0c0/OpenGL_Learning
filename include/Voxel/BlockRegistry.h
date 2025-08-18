#pragma once

#include "Voxel/Block.h"
#include "Voxel/BlockID.h"
#include <vector>
#include <iostream>

class BlockRegistry
{
public:
	void RegisterBlock(BlockID id, Block& block);
	const Block& GetBlockProperties(BlockID id)const;

private:
	std::vector<Block> m_BlockRegister;
};