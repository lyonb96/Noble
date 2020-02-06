#include "Shader.h"

namespace Noble
{
	Shader::Shader()
	{}

	void Shader::CreateFromBuffer(BitStream& data)
	{
		U32 vsShaderSize = data.Read<U32>();
		const bgfx::Memory* vsMem = bgfx::alloc(U32(vsShaderSize + 1));
		data.ReadBytes(vsMem->data, vsShaderSize);
		vsMem->data[vsMem->size - 1] = '\0';

		U32 fsShaderSize = data.Read<U32>();
		const bgfx::Memory* fsMem = bgfx::alloc(U32(fsShaderSize + 1));
		data.ReadBytes(fsMem->data, fsShaderSize);
		fsMem->data[fsMem->size - 1] = '\0';

		m_VertexShader = bgfx::createShader(vsMem);
		m_FragShader = bgfx::createShader(fsMem);
		m_Program = bgfx::createProgram(m_VertexShader, m_FragShader);
	}

	void Shader::Destroy()
	{
		bgfx::destroy(m_VertexShader);
		bgfx::destroy(m_FragShader);
		bgfx::destroy(m_Program);
	}
}