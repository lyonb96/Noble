#include "StaticMeshComponent.h"

#include "FileSystem.h"

#include <string.h>

namespace Noble
{
	OBJECT_DEF(StaticMeshComponent);

	//bgfx::ShaderHandle CreateShader(const char* name)
	//{
	//	char filePath[512];

	//	//const char* shaderPath = "???";

	//	//switch (bgfx::getRendererType())
	//	//{
	//	//	case bgfx::RendererType::Noop:
	//	//	case bgfx::RendererType::Direct3D9:  shaderPath = "Content/shaders/dx9/";   break;
	//	//	case bgfx::RendererType::Direct3D11:
	//	//	case bgfx::RendererType::Direct3D12: shaderPath = "Content/shaders/dx11/";  break;
	//	//	case bgfx::RendererType::Gnm:        shaderPath = "Content/shaders/pssl/";  break;
	//	//	case bgfx::RendererType::Metal:      shaderPath = "Content/shaders/metal/"; break;
	//	//	case bgfx::RendererType::Nvn:        shaderPath = "Content/shaders/nvn/";   break;
	//	//	case bgfx::RendererType::OpenGL:     shaderPath = "Content/shaders/glsl/";  break;
	//	//	case bgfx::RendererType::OpenGLES:   shaderPath = "Content/shaders/essl/";  break;
	//	//	case bgfx::RendererType::Vulkan:     shaderPath = "Content/shaders/spirv/"; break;
	//	//}

	//	strcpy_s(filePath, "Content/shaders/test/");
	//	strcat_s(filePath, name);
	//	strcat_s(filePath, ".bin");

	//	bool exists = CheckExists(filePath);

	//	if (exists)
	//	{
	//		File shaderFile(filePath, FileMode::FILE_READ);

	//		Size fSize = shaderFile.GetFileSize();
	//		const bgfx::Memory* mem = bgfx::alloc(U32(fSize + 1));
	//		
	//		shaderFile.Read(mem->data, fSize);
	//		mem->data[mem->size - 1] = '\0';

	//		bgfx::ShaderHandle handle = bgfx::createShader(mem);
	//		bgfx::setName(handle, name);

	//		return handle;
	//	}

	//	return BGFX_INVALID_HANDLE;
	//}

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
}