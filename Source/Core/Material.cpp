#include "Material.h"

#include "Globals.h"

namespace Noble
{
	typedef bgfx::UniformType::Enum UniformEnum;

	Material::Material()
	{
		m_Shader = nullptr;
		m_UniformBuffer = nullptr;
	}

	void Material::SetShader(Shader* shader)
	{
		m_Shader = shader;

		if (m_UniformBuffer)
		{
			Memory::Free(m_UniformBuffer);
		}

		if (shader)
		{
			m_UniformBuffer = (UByte*) Memory::Malloc(m_Shader->GetUniformBufferSize(), 16);
			Memory::Memset(m_UniformBuffer, 0, m_Shader->GetUniformBufferSize());
		}
	}

	void Material::CreateFromBuffer(BitStream& data)
	{
		U32 shaderId = data.Read<U32>();
		SetShader(GetAssetManager()->GetShader(shaderId));

		if (!m_Shader)
		{
			NE_LOG_WARNING("Failed to load shader with ID %u", shaderId);
			return;
		}

		U32 attrCount = m_Shader->GetUniformCount();
		for (U32 i = 0; i < attrCount; ++i)
		{
			U32 attrIndex = data.Read<U32>();
			U32 attrOffset = m_Shader->GetUniformOffset(attrIndex);
			UniformEnum type = m_Shader->GetUniformType(attrIndex);

			switch (type)
			{
				case UniformEnum::Sampler:
					// Read a 32-bit uint for the texture ID
					// TODO: setup texture loading
					//U32 texId = data.Read<U32>();
					//Texture* tex = GetAssetManager()->GetTexture(texId);
					//if (tex)
					//{
					//	SetUniformData(attrOffset, tex);
					//}
					break;
				case UniformEnum::Vec4:
					Vector4f v = data.Read<Vector4f>();
					SetUniformData(attrOffset, v);
					break;
				case UniformEnum::Mat3:
					Matrix3x3f m3 = data.Read<Matrix3x3f>();
					SetUniformData(attrOffset, m3);
					break;
				case UniformEnum::Mat4:
					Matrix4x4f m4 = data.Read<Matrix4x4f>();
					SetUniformData(attrOffset, m4);
					break;
				default:
					NE_LOG_WARNING("Invalid type for Uniform!");
					return;
			}
		}
	}

	void Material::EnableUniforms() const
	{
		U32 index = 0;
		for (auto uniform : m_Shader->GetUniforms())
		{
			const U32 offset = m_Shader->GetUniformOffset(index);
			const void* data = (m_UniformBuffer + offset);
			bgfx::setUniform(uniform.UniformHandle, data, uniform.UniformCount);
		}
	}

	void Material::Destroy()
	{
		if (m_UniformBuffer)
		{
			Memory::Free(m_UniformBuffer);
		}
	}
}