#pragma once

#include "SceneComponent.h"
#include "StaticMesh.h"
#include "Material.h"
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
		OBJECT_DECL(StaticMeshComponent, SceneComponent)
	public:

		StaticMeshComponent();

		/**
		 * Changes the StaticMesh rendered by this Component
		 */
		void SetMesh(StaticMesh* mesh);

		/**
		 * Changes the Material rendered by this Component
		 */
		void SetMaterial(Material* material);

	public:

		/**
		 * Handles submitting this StaticMesh for drawing
		 */
		virtual void Draw() override;

		/**
		 * Writes the data for this StaticMeshComponent to the stream
		 */
		virtual void Serialize(BitStream& stream) override;

		/**
		 * Reads data from the stream to reconstruct this StaticMeshComponent
		 */
		virtual void Deserialize(BitStream& stream) override;

	private:

		// The mesh to draw
		StaticMesh* m_Mesh;
		// The material to draw with
		Material* m_Material;
	};
}