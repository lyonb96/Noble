#include "World.h"

namespace Noble
{
	World::World()
	{
		GameObject::m_World = this;
	}
}