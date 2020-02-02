#pragma once

#include "Engine.h"

namespace Noble
{
	// Global pointer to the engine instance
	extern Engine* g_Engine;
	// Global pointer to the world instance
	extern World* g_World;

	/**
	 * Returns a pointer to the world instance
	 */
	World* GetWorld();
}