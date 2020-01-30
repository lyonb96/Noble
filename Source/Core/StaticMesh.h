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

		/**
		 * Describes a simple StaticMesh vertex format
		 * Vertex size is 32 bytes
		 */
		struct Vertex
		{
			// Position + extra float for padding
			Vector4f Position; // (16 bytes)
			// Texture coordinates
			Vector2f TexCoord; // (8 bytes)
			// Half-precision quaternion describing tangent space of vertex
			HalfQuaternion TangentSpace; // (8 bytes)
		};

		// Indices are stored as unsigned 32-bit ints
		typedef U32 Index;

		/**
		 * Returns the number of vertices in the StaticMesh
		 */
		const U32 GetVertexCount() const { return m_VertexCount; }

		/**
		 * Returns an array of vertices for drawing this StaticMesh
		 */
		const Vertex* GetVertices() const { return m_Vertices; }

		/**
		 * Returns the number of indices in the StaticMesh
		 */
		const U32 GetIndexCount() const { return m_IndexCount; }

		/**
		 * Returns an array of indices for drawing this StaticMesh
		 */
		const Index* GetIndices() const { return m_Indices; }

	protected:

		/**
		 * Fills out the StaticMesh instance from the given buffer
		 */
		virtual void CreateFromBuffer(BitStream& data) override;

	private:

		// Vertex array
		Vertex* m_Vertices;
		// Number of vertices
		U32 m_VertexCount;
		// Index array
		Index* m_Indices;
		// Number of indices
		U32 m_IndexCount;
	};
}