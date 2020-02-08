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
		friend class Material;
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

		/**
		 * Returns the required size of buffers for the given uniforms
		 */
		const U32 GetUniformBufferSize() const;

		/**
		 * Returns the size of the uniform at the given index
		 */
		const U32 GetUniformSize(const U32 index) const;

		/**
		 * Returns the offset of the uniform at the given index
		 */
		const U32 GetUniformOffset(const U32 index) const;

		/**
		 * Returns the offset of the uniform at the given index
		 */
		const U32 GetUniformOffset(const NIdentifier& id) const;

		/**
		 * Returns the number of uniforms for this Shader
		 */
		const Size GetUniformCount() const { return m_Uniforms.GetCount(); }

		/**
		 * Returns the UniformType for the given index
		 */
		const bgfx::UniformType::Enum GetUniformType(const U32 index) const;

	protected:

		/**
		 * Creates the shader instance from the stream data
		 */
		virtual void CreateFromBuffer(BitStream& data) override;

		/**
		 * Releases shaders and uniforms
		 */
		virtual void Destroy() override;

		Array<ShaderUniform>& GetUniforms() { return m_Uniforms; }

	private:

		// Attribute list
		Array<ShaderUniform> m_Uniforms;
		// Program handle
		bgfx::ProgramHandle m_Program;
	};
}