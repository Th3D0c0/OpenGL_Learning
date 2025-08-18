#include "Voxel/BlockRegistry.h"

void BlockRegistry::RegisterBlock(BlockID id, Block& block)
{
	size_t index = static_cast<size_t>(id);
	if (index >= m_BlockRegister.size())
	{
		m_BlockRegister.resize(index + 1);
	}
	m_BlockRegister[index] = block;
}

const Block& BlockRegistry::GetBlockProperties(BlockID id) const
{
	size_t index = static_cast<size_t>(id);
	if (index >= m_BlockRegister.size())
	{
		std::cerr << "Block ID not registered";
	}
	return m_BlockRegister[index];
}
