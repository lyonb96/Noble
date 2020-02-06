#include "AssetManager.h"

#include "FileSystem.h"
#include "Logger.h"
#include "StaticMesh.h"

// 8MB blocks
#define ASSET_BLOCK_SIZE (1 << 23)

namespace Noble
{
	AssetManager::AssetManager()
		: m_AssetAlloc(/*ASSET_BLOCK_SIZE*/)
	{}

	void AssetManager::LoadAssetRegistry()
	{
		// to do
	}

	void AssetManager::RegisterAsset(const NIdentifier& id, const NString& path, AssetType type)
	{
		AssetRegistration reg;
		reg.AssetID = id;
		reg.Path = path;
		reg.Type = type;
		reg.LoadedAsset = nullptr;

		m_Registry.Add(std::move(reg));
	}

	void AssetManager::UnregisterAsset(const NIdentifier& id)
	{
		for (auto iter = m_Registry.Start(); iter != m_Registry.End(); ++iter)
		{
			if (iter->AssetID == id)
			{
				iter.RemoveCurrent();
				return;
			}
		}

		NE_LOG_WARNING("No asset registered with ID %s", id.GetString());
	}

	StaticMesh* AssetManager::GetStaticMesh(const NIdentifier& id)
	{
		if (m_LoadedAssets.ContainsKey(id))
		{
			Asset* sm = m_LoadedAssets[id];
			CHECK(sm->GetType() == AssetType::AT_STATIC_MESH);
			return static_cast<StaticMesh*>(sm);
		}
		else
		{
			Asset* newLoad = LoadAsset(id);
			if (newLoad)
			{
				CHECK(newLoad->GetType() == AssetType::AT_STATIC_MESH);
				return static_cast<StaticMesh*>(newLoad);
			}
		}

		return nullptr;
	}

	Asset* AssetManager::LoadAsset(const NIdentifier& id)
	{
		for (auto reg : m_Registry)
		{
			if (reg.AssetID == id)
			{
				return LoadFromRegistry(reg);
			}
		}

		NE_LOG_WARNING("No asset registered with ID %s", id.GetString());
		return nullptr;
	}

	Asset* AssetManager::LoadFromRegistry(AssetRegistration& reg)
	{
		if (reg.LoadedAsset)
		{
			return reg.LoadedAsset;
		}

		// for now, only support single files
		File assetFile(reg.Path.GetCharArray(), FileMode::FILE_READ, false);
		if (!assetFile.IsValid())
		{
			NE_LOG_ERROR("Failed to load %s: file not found or inaccessible", reg.Path);
			return nullptr;
		}

		// Read the file data
		BitStream data;
		assetFile.Read(data);
		Asset* result = nullptr;

		// Create the asset instance
		switch (reg.Type)
		{
		case AssetType::AT_STATIC_MESH:
			result = NE_NEW(m_AssetAlloc, StaticMesh);
			break;
		default:
			NE_LOG_WARNING("Unknown asset type of ID %u", (U8)reg.Type);
			break;
		}

		if (result)
		{
			// Load the asset from the buffer
			result->CreateFromBuffer(data);
			// Update the registration
			reg.LoadedAsset = result;
			// Store the loaded asset in the map
			m_LoadedAssets.Insert(reg.AssetID, result);

			return result;
		}

		return nullptr;
	}

	void AssetManager::UnloadAllAssets()
	{
		for (auto iter = m_LoadedAssets.Start(); iter != m_LoadedAssets.End(); ++iter)
		{
			if (iter->Value)
			{
				// Have the resource clean itself up
				iter->Value->Destroy();
				// Delete it from memory
				NE_DELETE(m_AssetAlloc, iter->Value);
				// Remove from the map
				iter.RemoveCurrent();
			}
		}
	}
}