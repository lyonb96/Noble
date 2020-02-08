#pragma once

#include "Array.h"
#include "Asset.h"
#include "Map.h"
#include "Memory.h"

#include "Shader.h"
#include "StaticMesh.h"
#include "Material.h"
#include "Texture2D.h"

/**
 * The Asset Management subsystem of the engine handles registering/unregistering
 * assets, as well as loading them ffrom their registrations. Asset Registration
 * links a unique ID to a file, so game code doesn't need to be aware of the actual
 * path to a file, only the relevant asset's UID.
 */

namespace Noble
{

	/**
	 * AssetRegistration - describes an asset and some surrounding info
	 */
	struct AssetRegistration
	{
		// Unique identifier for this asset
		NIdentifier AssetID;
		// Path to the file where this asset is stored
		NString Path;
		// Asset's type
		AssetType Type;
		// Pointer to the asset, if it's been loaded
		Asset* LoadedAsset;
	};

	typedef Array<AssetRegistration> AssetRegistry;
	typedef MemoryArena<BasicAllocator, DefaultTracking> AssetAllocator;
	typedef Map<NIdentifier, Asset*> LoadedAssetMap;

	/**
	 * Handles loading, unloading, and providing access to assets
	 */
	class AssetManager
	{
	public:

		/**
		 * Default constructor
		 */
		AssetManager();

		/**
		 * Loads the asset registry file
		 */
		void LoadAssetRegistry();

		/**
		 * Manually register an asset
		 */
		void RegisterAsset(const NIdentifier& id, const NString& path, AssetType type);

		/**
		 * Unregisters an asset - this makes it so it can't be loaded
		 * but it does not unload or destroy existing instances of the asset
		 */
		void UnregisterAsset(const NIdentifier& id);

		/**
		 * Returns a pointer to the StaticMesh of the given ID
		 * Will load the asset if it's not yet loaded
		 */
		StaticMesh* GetStaticMesh(const NIdentifier& id);

		/**
		 * Returns a pointer to the StaticMesh of the given ID
		 * Will load the asset if it's not yet loaded
		 */
		StaticMesh* GetStaticMesh(const U32 id);

		/**
		 * Returns a pointer to the Shader of the given ID
		 * Will load the asset if it's not yet loaded
		 */
		Shader* GetShader(const NIdentifier& id);

		/**
		 * Returns a pointer to the Shader of the given ID
		 * Will load the asset if it's not yet loaded
		 */
		Shader* GetShader(const U32 id);

		/**
		 * Returns a pointer to the Material of the given ID
		 * Will load the asset if it's not yet loaded
		 */
		Material* GetMaterial(const NIdentifier& id);

		/**
		 * Returns a pointer to the Material of the given ID
		 * Will load the asset if it's not yet loaded
		 */
		Material* GetMaterial(const U32 id);

		/**
		 * Returns a pointer to the Texture2D of the given ID
		 * Will load the asset if it's not yet loaded
		 */
		Texture2D* GetTexture2D(const NIdentifier& id);

		/**
		 * Returns a pointer to the Texture2D of the given ID
		 * Will load the asset if it's not yet loaded
		 */
		Texture2D* GetTexture2D(const U32 id);

		/**
		 * Creates a new Material, optionally cloning an existing Material
		 * If the Material is not cloned from an existing Material, the 
		 * returned instance is uninitialized and needs to be setup by the 
		 * caller.
		 */
		Material* CreateMaterial(Material* copy = nullptr);

		/**
		 * Loads the requested asset, if it has not already been
		 * Returns a pointer to the requested asset
		 */
		Asset* LoadAsset(const NIdentifier& id);

		/**
		 * Unloads every loaded asset, regardless of flags
		 * Used on shutdown
		 */
		void UnloadAllAssets();

	private:

		/**
		 * Loads the requested asset from its registration data
		 */
		Asset* LoadFromRegistry(AssetRegistration& reg);

	private:

		// Array of all registered assets
		AssetRegistry m_Registry;
		// Allocates each asset
		AssetAllocator m_AssetAlloc;
		// Map of all loaded assets
		LoadedAssetMap m_LoadedAssets;

	};
}