#pragma once

#include "Asset.h"

namespace Noble
{
	/**
	 * Static Meshes are non-rigged triangular meshes that can be used as input
	 * to the rendering pipeline.
	 *
	 * This class handles identifying and creating StaticMesh assets
	 */
	class StaticMesh : public Asset
	{
	public:

	protected:

		/**
		 * Describes a simple StaticMesh vertex format
		 * Vertex size is 32 bytes
		 */
		struct StaticVertex
		{
			// Position + extra float for padding
			Vector4f Position; // (16 bytes)
			// Texture coordinates
			Vector2f TexCoord; // (8 bytes)
			// Half-precision quaternion describing tangent space of vertex
			HalfQuaternion TangentSpace; // (8 bytes)
		};

		typedef U32 Index;

		/**
		 * Fills out the StaticMesh instance from the given buffer
		 */
		virtual void CreateFromBuffer(BitStream& data) override;

	private:

		// Vertex array
		StaticVertex* m_Vertices;
		// Number of vertices
		U32 m_VertexCount;
		// Index array
		Index* m_Indices;
		// Number of indices
		U32 m_IndexCount;
	};
}