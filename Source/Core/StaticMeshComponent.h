#pragma once

#include "Component.h"
#include "StaticMesh.h"
#include "Types.h"

#include <bgfx/bgfx.h>

namespace Noble
{
	struct Vertex
	{
		float X;
		float Y;
		float Z;
		U32 vColor;
	};

	/**
	 * This Component is used to render static (i.e. not rigged or animated) meshes
	 * Allows the mesh and material to be changed, and it can also have a relative offset
	 * from its parent Component
	 */
	class StaticMeshComponent : public Component
	{
		OBJECT_DECL(StaticMeshComponent, Component);
	public:

		StaticMeshComponent();

		/**
		 * Changes the StaticMesh rendered by this Component
		 */
		void SetMesh(StaticMesh* mesh);

		static void TemporaryInit();
		static void TemporaryDestroy();

	public:

		void TestDraw();

	private:

		// The mesh to draw
		StaticMesh* m_Mesh;

		// Mesh class
		static bgfx::VertexLayout m_Layout;
		static bgfx::VertexBufferHandle m_VBuff;
		static bgfx::IndexBufferHandle m_IBuff;

		// Material class
		static bgfx::ProgramHandle m_Prog;
		static bgfx::ShaderHandle m_VS;
		static bgfx::ShaderHandle m_PS;
	};
}