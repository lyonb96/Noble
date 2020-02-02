#include "Globals.h"

namespace Noble
{
	Engine* g_Engine = nullptr;
	World* g_World = nullptr;

	World* GetWorld()
	{
		return g_World;
	}
}