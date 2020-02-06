#pragma once

#include "SceneComponent.h"
#include "StaticMesh.h"
#include "Shader.h"
#include "Types.h"

#include <bgfx/bgfx.h>

namespace Noble
{
	/**
	 * This Component is used to render static (i.e. not rigged or animated) meshes
	 * Allows the mesh and material to be changed, and it can also have a relative offset
	 * from its parent Component
	 */
	class StaticMeshComponent : public SceneComponent
	{
		OBJECT_DECL(StaticMeshComponent, SceneComponent);
	public:

		StaticMeshComponent();

		/**
		 * Changes the StaticMesh rendered by this Component
		 */
		void SetMesh(StaticMesh* mesh);

		/**
		 * TEMP - sets the shader to draw with
		 */
		void SetShader(Shader* shader) { m_Shader = shader; }

		static void TemporaryInit();
		static void TemporaryDestroy();

	public:

		virtual void Draw() override;

	private:

		// The mesh to draw
		StaticMesh* m_Mesh;

		// TEMP - shader to draw with
		Shader* m_Shader;

		// Material class
		static bgfx::ProgramHandle m_Prog;
		static bgfx::ShaderHandle m_VS;
		static bgfx::ShaderHandle m_PS;
	};
}