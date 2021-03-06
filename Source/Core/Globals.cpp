#include "Globals.h"

#include "Engine.h"
#include "World.h"
#include "AssetManager.h"

namespace Noble
{
	Engine* g_Engine = nullptr;
	World* g_World = nullptr;
	AssetManager* g_AssetManager = nullptr;

	World* GetWorld()
	{
		return g_World;
	}

	AssetManager* GetAssetManager()
	{
		return g_AssetManager;
	}
}