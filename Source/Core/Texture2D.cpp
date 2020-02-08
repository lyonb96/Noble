#include "Texture2D.h"

#include "Logger.h"

namespace Noble
{

	Texture2D::Texture2D()
		: m_TexHandle()
	{}

	void Texture2D::CreateFromBuffer(BitStream& data)
	{
		// TODO: There's certainly a way to optimize this to not require a memcpy
		const Size bufSize = data.GetStoredBytes();
		void* buf = Memory::Malloc(bufSize, 16);
		Memory::Memcpy(buf, data.GetData(), bufSize);

		const bgfx::Memory* mem = bgfx::makeRef((const void*)buf, U32(bufSize), 
			[] (void* data, void*) -> void { Memory::Free(data); }); // lambda is for freeing the buffer allocated above

		m_TexHandle = bgfx::createTexture(mem);
		if (!bgfx::isValid(m_TexHandle))
		{
			NE_LOG_ERROR("Failed to create texture");
		}
	}

	void Texture2D::Destroy()
	{
		bgfx::destroy(m_TexHandle);
	}
}