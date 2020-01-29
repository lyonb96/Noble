#include "StaticMesh.h"

namespace Noble
{
	void StaticMesh::CreateFromBuffer(BitStream& data)
	{
		// Read vertex count
		m_VertexCount = data.Read<U32>();
		CHECK(m_VertexCount > 0);

		// Allocate vertex buffer
		//m_Vertices = NE_BUFFER_ALLOC(SomeArenaSomewhere, sizeof(StaticVertex) * m_VertexCount);

		// Read in vertices
		for (U32 i = 0; i < m_VertexCount; ++i)
		{
			m_Vertices[i].Position =	 data.Read<Vector4f>();
			m_Vertices[i].TexCoord =	 data.Read<Vector2f>();
			m_Vertices[i].TangentSpace = data.Read<HalfQuaternion>();
		}

		// Read in number of indices
		m_IndexCount = data.Read<U32>();
		CHECK(m_IndexCount > 0);

		// Allocate index buffer
		//m_Indices = NE_BUFFER_ALLOC(SomeArenaSomewhere, sizeof(Index) * m_IndexCount);

		for (U32 i = 0; i < m_IndexCount; ++i)
		{
			m_Indices[i] = data.Read<Index>();
		}
	}
}