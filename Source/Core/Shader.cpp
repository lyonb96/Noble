#include "Shader.h"

namespace Noble
{
	typedef bgfx::UniformType::Enum UniformEnum;

	/**
	 * Converts a stored type value into the BGFX UniformType
	 */
	bgfx::UniformType::Enum GetBGFXType(const U32 type)
	{
		switch (type)
		{
			case 0:
				return bgfx::UniformType::Sampler;
			case 1:
				return bgfx::UniformType::Vec4;
			case 2:
				return bgfx::UniformType::Mat3;
			case 3:
				return bgfx::UniformType::Mat4;
			default:
				return bgfx::UniformType::End;
		}
	}

	const U32 GetUniformTypeSize(const UniformEnum type)
	{
		switch (type)
		{
			case UniformEnum::Sampler:
				return sizeof(void*);
			case UniformEnum::Vec4:
				return sizeof(F32) * 4;
			case UniformEnum::Mat3:
				return sizeof(F32) * 9;
			case UniformEnum::Mat4:
				return sizeof(F32) * 16;
			default:
				return 0;
		}
	}

	Shader::Shader()
		: m_Program()
	{}

	void Shader::CreateFromBuffer(BitStream& data)
	{
		I32 attrCount = data.Read<I32>();
		m_Uniforms.Resize(attrCount);
		for (I32 i = 0; i < attrCount; ++i)
		{
			// Read name
			U8 strlen = data.Read<U8>();
			s_char* str = (s_char*)Memory::Malloc((strlen * sizeof(s_char)) + 1, 8);
			data.ReadBytes((UByte*)str, strlen);
			str[strlen] = '\0';

			// Read type
			U32 type = data.Read<U32>();

			// Read count
			U32 count = data.Read<U32>();

			ShaderUniform su;
			su.UniformName = NIdentifier(str, strlen, HASH(str));
			su.UniformType = GetBGFXType(type);
			su.UniformCount = count;
			su.UniformHandle = bgfx::createUniform(su.UniformName.GetString(), su.UniformType, su.UniformCount);

			m_Uniforms.Add(su);
		}

		// Load VS
		U32 vsShaderSize = data.Read<U32>();
		const bgfx::Memory* vsMem = bgfx::alloc(U32(vsShaderSize + 1));
		data.ReadBytes(vsMem->data, vsShaderSize);
		vsMem->data[vsMem->size - 1] = '\0';

		// Load FS
		U32 fsShaderSize = data.Read<U32>();
		const bgfx::Memory* fsMem = bgfx::alloc(U32(fsShaderSize + 1));
		data.ReadBytes(fsMem->data, fsShaderSize);
		fsMem->data[fsMem->size - 1] = '\0';

		// Build shaderse and create shader program
		bgfx::ShaderHandle vs = bgfx::createShader(vsMem);
		bgfx::ShaderHandle fs = bgfx::createShader(fsMem);
		m_Program = bgfx::createProgram(vs, fs, true);
	}

	void Shader::Destroy()
	{
		for (auto uniform : m_Uniforms)
		{
			bgfx::destroy(uniform.UniformHandle);
			// Free memory from the string alloc'd above
			Memory::Free(const_cast<char*>(uniform.UniformName.GetString()));
		}
		bgfx::destroy(m_Program);
	}

	const U32 Shader::GetUniformBufferSize() const
	{
		U32 out = 0;
		for (auto uniform : m_Uniforms)
		{
			out += GetUniformTypeSize(uniform.UniformType);
		}

		return out;
	}

	const U32 Shader::GetUniformSize(const U32 index) const
	{
		return GetUniformTypeSize(m_Uniforms[index].UniformType) * m_Uniforms[index].UniformCount;
	}

	const U32 Shader::GetUniformOffset(const U32 index) const
	{
		U32 offset = 0;
		for (U32 i = 0; i < index; ++i)
		{
			offset += GetUniformSize(i);
		}

		return offset;
	}

	const U32 Shader::GetUniformOffset(const NIdentifier& id) const
	{
		for (U32 i = 0; i < m_Uniforms.GetCount(); ++i)
		{
			if (m_Uniforms[i].UniformName == id)
			{
				return GetUniformOffset(i);
			}
		}
		
		return std::numeric_limits<U32>::max();
	}

	const UniformEnum Shader::GetUniformType(const U32 index) const
	{
		return m_Uniforms[index].UniformType;
	}
}