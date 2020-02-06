#pragma once

#include "Asset.h"

#include <bgfx/bgfx.h>

namespace Noble
{
	struct ShaderUniform
	{
		// Name of the uniform
		NIdentifier UniformName;
		// Type of the uniform
		bgfx::UniformType::Enum UniformType;
		// Handle to the uniform
		bgfx::UniformHandle UniformHandle;
	};

	/**
	 * A Shader describes a mode of rendering an object, including
	 * the textures and uniforms it can accept and its actual 
	 * hardware shaders.
	 */
	class Shader : public Asset
	{
	public:

		/**
		 * Default constructor
		 */
		Shader();

		/**
		 * Returns the handle to this Shader's vertex shader
		 */
		bgfx::ShaderHandle& GetVertexShader() { return m_VertexShader; }

		/**
		 * Returns the handle to this Shader's frag shader
		 */
		bgfx::ShaderHandle& GetFragShader() { return m_FragShader; }

		/**
		 * Returns the handle to this Shader's program
		 */
		bgfx::ProgramHandle& GetProgram() { return m_Program; }

		/**
		 * Returns the proper type for Shader assets
		 */
		virtual const AssetType GetType() const override { return AssetType::AT_SHADER; }

	protected:

		/**
		 * Creates the shader instance from the stream data
		 */
		virtual void CreateFromBuffer(BitStream& data) override;

		/**
		 * Releases shaders and uniforms
		 */
		virtual void Destroy() override;

	private:

		// Vertex shader handle
		bgfx::ShaderHandle m_VertexShader;
		// Pixel shader handle
		bgfx::ShaderHandle m_FragShader;
		// Program handle
		bgfx::ProgramHandle m_Program;
	};
}