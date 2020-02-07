#include "Shader.h"

namespace Noble
{
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

		U32 vsShaderSize = data.Read<U32>();
		const bgfx::Memory* vsMem = bgfx::alloc(U32(vsShaderSize + 1));
		data.ReadBytes(vsMem->data, vsShaderSize);
		vsMem->data[vsMem->size - 1] = '\0';

		U32 fsShaderSize = data.Read<U32>();
		const bgfx::Memory* fsMem = bgfx::alloc(U32(fsShaderSize + 1));
		data.ReadBytes(fsMem->data, fsShaderSize);
		fsMem->data[fsMem->size - 1] = '\0';

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
}