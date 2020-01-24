#include "StaticMeshComponent.h"

#include "FileSystem.h"

#include <string.h>

namespace Noble
{

	bgfx::ShaderHandle CreateShader(const char* name)
	{
		char filePath[512];

		const char* shaderPath = "???";

		switch (bgfx::getRendererType())
		{
			case bgfx::RendererType::Noop:
			case bgfx::RendererType::Direct3D9:  shaderPath = "Content/shaders/dx9/";   break;
			case bgfx::RendererType::Direct3D11:
			case bgfx::RendererType::Direct3D12: shaderPath = "Content/shaders/dx11/";  break;
			case bgfx::RendererType::Gnm:        shaderPath = "Content/shaders/pssl/";  break;
			case bgfx::RendererType::Metal:      shaderPath = "Content/shaders/metal/"; break;
			case bgfx::RendererType::Nvn:        shaderPath = "Content/shaders/nvn/";   break;
			case bgfx::RendererType::OpenGL:     shaderPath = "Content/shaders/glsl/";  break;
			case bgfx::RendererType::OpenGLES:   shaderPath = "Content/shaders/essl/";  break;
			case bgfx::RendererType::Vulkan:     shaderPath = "Content/shaders/spirv/"; break;
		}

		strcpy_s(filePath, shaderPath);
		strcat_s(filePath, name);
		strcat_s(filePath, ".bin");

		bool exists = CheckExists(filePath);

		if (exists)
		{
			File shaderFile;
			shaderFile.OpenFile(filePath);

			Size fSize = shaderFile.GetFileSize();
			const bgfx::Memory* mem = bgfx::alloc(fSize + 1);
			
			shaderFile.Read(mem->data, fSize);
			mem->data[mem->size - 1] = '\0';

			bgfx::ShaderHandle handle = bgfx::createShader(mem);
			bgfx::setName(handle, name);

			return handle;
		}

		return BGFX_INVALID_HANDLE;
	}

	static Vertex s_cubeVertices[] =
	{
		{-1.0f,  1.0f,  1.0f, 0xff000000 },
		{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
		{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
		{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
		{-1.0f,  1.0f, -1.0f, 0xffff0000 },
		{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
		{-1.0f, -1.0f, -1.0f, 0xffffff00 },
		{ 1.0f, -1.0f, -1.0f, 0xffffffff },
	};

	static const U16 s_cubeTriList[] =
	{
		0, 1, 2, // 0
		1, 3, 2,
		4, 6, 5, // 2
		5, 6, 7,
		0, 2, 4, // 4
		4, 2, 6,
		1, 5, 3, // 6
		5, 7, 3,
		0, 4, 1, // 8
		4, 5, 1,
		2, 3, 6, // 10
		6, 3, 7,
	};

	bgfx::VertexLayout StaticMeshComponent::m_Layout;
	bgfx::VertexBufferHandle StaticMeshComponent::m_VBuff;
	bgfx::IndexBufferHandle StaticMeshComponent::m_IBuff;

	bgfx::ProgramHandle StaticMeshComponent::m_Prog;
	bgfx::ShaderHandle StaticMeshComponent::m_VS;
	bgfx::ShaderHandle StaticMeshComponent::m_PS;

	StaticMeshComponent::StaticMeshComponent(const NIdentifier& name)
		: Component(name)
	{}

	void StaticMeshComponent::TemporaryInit()
	{
		m_Layout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.end();

		m_VBuff = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), m_Layout);
		m_IBuff = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList)));

		m_VS = CreateShader("vs_cubes");
		m_PS = CreateShader("fs_cubes");
		m_Prog = bgfx::createProgram(m_VS, m_PS, true);
	}

	void StaticMeshComponent::TemporaryDestroy()
	{
		bgfx::destroy(m_VBuff);
		bgfx::destroy(m_IBuff);
		bgfx::destroy(m_VS);
		bgfx::destroy(m_PS);
		bgfx::destroy(m_Prog);
	}

	void StaticMeshComponent::TestDraw()
	{
		uint64_t state = BGFX_STATE_DEFAULT;
		state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CCW | BGFX_STATE_MSAA;

		Matrix4x4f transform = GetWorldTransform();

		bgfx::setTransform(&transform);

		bgfx::setVertexBuffer(0, m_VBuff);
		bgfx::setIndexBuffer(m_IBuff);
		bgfx::setState(state);
		bgfx::submit(0, m_Prog);
	}
}