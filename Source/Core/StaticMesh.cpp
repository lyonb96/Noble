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
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Tangent, 3, bgfx::AttribType::Float)
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
			m_Vertices[i].Position =	data.Read<Vector3f>();
			m_Vertices[i].TexCoord =	data.Read<Vector2f>();
			m_Vertices[i].Normal =		data.Read<Vector3f>();
			m_Vertices[i].Tangent =		data.Read<Vector3f>();
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
		m_VertexBuffer = bgfx::createVertexBuffer(
			bgfx::makeRef(m_Vertices, sizeof(StaticVertex) * m_VertexCount, [] (void* data, void*) -> void { delete[] data; }),
			StaticVertex::Layout);
		m_IndexBuffer = bgfx::createIndexBuffer(
			bgfx::makeRef(m_Indices, sizeof(Index) * m_IndexCount, [] (void* data, void*) -> void { delete[] data; }),
			BGFX_BUFFER_INDEX32);
	}

	void StaticMesh::Destroy()
	{
		bgfx::destroy(m_VertexBuffer);
		bgfx::destroy(m_IndexBuffer);
	}
}