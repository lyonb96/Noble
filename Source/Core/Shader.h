#pragma once

#include "Asset.h"

#include <bgfx/bgfx.h>

namespace Noble
{
	/**
	 * Shader Uniforms describe a uniform and help set new values
	 * for shaders to read
	 */
	struct ShaderUniform
	{
		// Name of the uniform
		NIdentifier UniformName;
		// Type of the uniform
		bgfx::UniformType::Enum UniformType;
		// Number of elements in the uniform array
		U32 UniformCount;
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

		// Attribute list
		Array<ShaderUniform> m_Uniforms;
		// Program handle
		bgfx::ProgramHandle m_Program;
	};
}