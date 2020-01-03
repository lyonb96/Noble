#include "World.h"

namespace Noble
{
	World::World()
	{
		ClearTempArray();
	}

	void World::ClearTempArray()
	{
		for (U8 i = 0; i < 32; ++i)
		{
			m_TempArray[i] = nullptr;
		}
		m_TempCount = 0;
	}
}