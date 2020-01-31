#pragma once

#include "Asset.h"

#include <bgfx\bgfx.h>

namespace Noble
{
	/**
	 * Describes a simple StaticMesh vertex format
	 * Vertex size is 32 bytes
	 */
	struct StaticVertex
	{
		StaticVertex()
			: Position(0), TexCoord(0), TangentSpace(F16(0))
		{}

		// Position + extra float for padding
		Vector4f Position; // (16 bytes)
		// Texture coordinates
		Vector2f TexCoord; // (8 bytes)
		// Half-precision quaternion describing tangent space of vertex
		Vector4h TangentSpace; // (8 bytes)

		// BGFX
		static bgfx::VertexLayout Layout;
		static void Init();
	};

	/**
	 * Static Meshes are non-rigged triangular meshes that can be used as input
	 * to the rendering pipeline.
	 *
	 * This class handles identifying and creating StaticMesh assets
	 */
	class StaticMesh : public Asset
	{
	public:

		// Indices are stored as unsigned 32-bit ints
		typedef U32 Index;

		/**
		 * Default constructor
		 */
		StaticMesh();

		/**
		 * Returns the number of vertices in the StaticMesh
		 */
		const U32 GetVertexCount() const { return m_VertexCount; }

		/**
		 * Returns an array of vertices for drawing this StaticMesh
		 */
		const StaticVertex* GetVertices() const { return m_Vertices; }

		/**
		 * Returns the handle to the vertex buffer object
		 */
		bgfx::VertexBufferHandle& GetVertexBuffer() { return m_VertexBuffer; }

		/**
		 * Returns the number of indices in the StaticMesh
		 */
		const U32 GetIndexCount() const { return m_IndexCount; }

		/**
		 * Returns an array of indices for drawing this StaticMesh
		 */
		const Index* GetIndices() const { return m_Indices; }

		/**
		 * Returns the handle to the index buffer object
		 */
		bgfx::IndexBufferHandle& GetIndexBuffer() { return m_IndexBuffer; }

	public:

		/**
		 * Fills out the StaticMesh instance from the given buffer
		 */
		virtual void CreateFromBuffer(BitStream& data) override;

	private:

		// Vertex array
		StaticVertex* m_Vertices;
		// Number of vertices
		U32 m_VertexCount;
		// Vertex buffer
		bgfx::VertexBufferHandle m_VertexBuffer;
		// Index array
		Index* m_Indices;
		// Number of indices
		U32 m_IndexCount;
		// Index buffer
		bgfx::IndexBufferHandle m_IndexBuffer;
	};
}