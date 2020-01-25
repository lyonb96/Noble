#pragma once

#include "Component.h"
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

	class StaticMeshComponent : public Component
	{
		OBJECT_DECL(StaticMeshComponent, Component);
	public:

		StaticMeshComponent();

		static void TemporaryInit();
		static void TemporaryDestroy();

	public:

		void TestDraw();

	private:

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