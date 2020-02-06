#include "StaticMesh.h"

#include <bgfx/bgfx.h>

namespace Noble
{
	bgfx::VertexLayout StaticVertex::Layout;

	StaticMesh::StaticMesh()
		: m_VertexBuffer(), m_IndexBuffer()
	{
		m_VertexCount = 0;
		m_Vertices = nullptr;
		m_IndexCount = 0;
		m_Indices = nullptr;
	}

	void StaticVertex::Init()
	{
		Layout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Weight, 1, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Normal, 4, bgfx::AttribType::Half)
			.end();
	}

	void StaticMesh::CreateFromBuffer(BitStream& data)
	{
		// Read vertex count
		m_VertexCount = data.Read<U32>();
		CHECK(m_VertexCount > 0);

		// Allocate vertex array
		m_Vertices = new StaticVertex[m_VertexCount];

		// Read in vertices
		for (U32 i = 0; i < m_VertexCount; ++i)
		{
			m_Vertices[i].Position =	 data.Read<Vector4f>();
			m_Vertices[i].TexCoord =	 data.Read<Vector2f>();
			m_Vertices[i].TangentSpace = data.Read<Vector4h>();
		}

		// Read in number of indices
		m_IndexCount = data.Read<U32>();
		CHECK(m_IndexCount > 0);

		// Allocate index array
		m_Indices = new Index[m_IndexCount];

		// Read in indices
		for (U32 i = 0; i < m_IndexCount; ++i)
		{
			m_Indices[i] = data.Read<Index>();
		}

		// Create buffers
		m_VertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(m_Vertices, sizeof(StaticVertex) * m_VertexCount), StaticVertex::Layout);
		m_IndexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(m_Indices, sizeof(Index) * m_IndexCount), BGFX_BUFFER_INDEX32);
	}

	void StaticMesh::Destroy()
	{
		if (m_Vertices)
		{
			delete[] m_Vertices;
		}
		if (m_Indices)
		{
			delete[] m_Indices;
		}
		bgfx::destroy(m_VertexBuffer);
		bgfx::destroy(m_IndexBuffer);
	}
}