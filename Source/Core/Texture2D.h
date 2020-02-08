#pragma once

#include "Asset.h"

#include <bgfx/bgfx.h>

namespace Noble
{

	/**
	 * 2D Texture assets
	 */
	class Texture2D : public Asset
	{
	public:

		/**
		 * Default constructor
		 */
		Texture2D();

		/**
		 * Returns the corresponding asset type for 2D textures
		 */
		virtual const AssetType GetType() const override { return AssetType::AT_TEXTURE2D; }

		/**
		 * Returns the handle to the loaded texture
		 */
		bgfx::TextureHandle GetTextureHandle() { return m_TexHandle; }

	protected:

		/**
		 * Creates the texture from the given data stream
		 */
		virtual void CreateFromBuffer(BitStream& data) override;

		/**
		 * Frees memory and releases resources for the texture
		 */
		virtual void Destroy() override;

	private:

		// Texture Handle
		bgfx::TextureHandle m_TexHandle;
	};
}