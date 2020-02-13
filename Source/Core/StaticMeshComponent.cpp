#include "StaticMeshComponent.h"

#include "AssetManager.h"
#include "FileSystem.h"
#include "Globals.h"

#include <string.h>

namespace Noble
{
	OBJECT_DEF(StaticMeshComponent);

	StaticMeshComponent::StaticMeshComponent()
		: SceneComponent()
	{
		m_Mesh = nullptr;
		m_Material = nullptr;
	}

	void StaticMeshComponent::SetMesh(StaticMesh* mesh)
	{
		m_Mesh = mesh;
	}

	void StaticMeshComponent::SetMaterial(Material* material)
	{
		m_Material = material;
	}

	void StaticMeshComponent::Draw()
	{
		if (m_Mesh && m_Material)
		{
			uint64_t state = BGFX_STATE_DEFAULT;
			state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CCW | BGFX_STATE_MSAA;

			Matrix4x4f transform = GetWorldTransform();

			bgfx::setTransform(&transform);

			m_Material->EnableUniforms();
		
			bgfx::setVertexBuffer(0, m_Mesh->GetVertexBuffer());
			bgfx::setIndexBuffer(m_Mesh->GetIndexBuffer());
			bgfx::setState(state);
			bgfx::submit(0, m_Material->GetShader()->GetProgram());
		}
	}

	void StaticMeshComponent::Serialize(BitStream& stream)
	{
		Super::Serialize(stream);
		// Write mesh ID
		stream.Write<U32>(m_Mesh ? m_Mesh->GetAssetID().GetHash() : 0);
		// Write material ID
		stream.Write<U32>(m_Material ? m_Material->GetAssetID().GetHash() : 0);
	}

	void StaticMeshComponent::Deserialize(BitStream& stream)
	{
		Super::Deserialize(stream);
		// Read mesh ID
		U32 meshId = stream.Read<U32>();
		if (meshId != 0)
		{
			m_Mesh = GetAssetManager()->GetStaticMesh(meshId);
		}
		// Read material ID
		U32 matId = stream.Read<U32>();
		if (matId != 0)
		{
			m_Material = GetAssetManager()->GetMaterial(matId);
		}
	}
}