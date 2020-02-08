#pragma once

#include "Engine.h"

namespace Noble
{
	// Global pointer to the engine instance
	extern Engine* g_Engine;
	// Global pointer to the world instance
	extern World* g_World;
	// Global pointer to the asset manager
	extern AssetManager* g_AssetManager;

	/**
	 * Returns a pointer to the world instance
	 */
	World* GetWorld();

	/**
	 * Returns a pointer to the asset manager
	 */
	AssetManager* GetAssetManager();
}