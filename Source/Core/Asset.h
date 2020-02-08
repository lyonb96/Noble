#pragma once

#include "BitStream.h"
#include "String.h"
#include "Types.h"

namespace Noble
{
	/**
	 * Asset Types
	 */
	enum class AssetType : U8
	{
		// Static mesh - non-rigged triangular mesh asset
		AT_STATIC_MESH = 0,
		// Shader
		AT_SHADER = 1,
		// Material
		AT_MATERIAL = 2,
		// 2D Texture
		AT_TEXTURE2D = 3
	};

	/**
	 * Abstract base class for Asset subtypes
	 * Provides facilities for identification and construction of subtypes
	 */
	class Asset
	{
		// Allow the AssetManager to access protected data (create func, etc)
		friend class AssetManager;
	public:

		/**
		 * Returns the ID of this Asset
		 */
		const NIdentifier& GetAssetID() const { return m_AssetID; }

		/**
		 * Returns the derived type of this asset
		 */
		virtual const AssetType GetType() const = 0;

	protected:

		/**
		 * Overridden in each Asset type to build an instance from a buffer
		 */
		virtual void CreateFromBuffer(BitStream& data) = 0;

		/**
		 * Overridden in each Asset type to free memory and release any resources
		 */
		virtual void Destroy() = 0;

	protected:

		// Unique identifier for this Asset
		NIdentifier m_AssetID;

	};
}