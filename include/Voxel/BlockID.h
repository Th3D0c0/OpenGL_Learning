#pragma once
#include <cstdint>

enum class BlockID : uint16_t
{
	None = 0,
	Dirt = 1,
	Wood = 2,
	Air = 3,
	Water = 4,
	Stone = 5
};