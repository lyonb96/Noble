#pragma once

#include "Asset.h"
#include "Shader.h"

namespace Noble
{

	/**
	 * Materials are a defined Shader instance, and allow setting Uniforms
	 * on a per-instance basis rather than globally for each Shader
	 */
	class Material : public Asset
	{
	public:

		/**
		 * Default constructor
		 */
		Material();

		/**
		 * Allow copy creation to clone mats
		 */
		Material(const Material& other);

		/**
		 * Allow copy creation to clone mats
		 */
		Material& operator=(const Material& other);

		/**
		 * Returns the shader set for this Material
		 */
		Shader* GetShader() const { return m_Shader; }

		/**
		 * Sets the shader for this Material
		 * Also clears any set uniforms
		 */
		void SetShader(Shader* shader);

		/**
		 * Sets the uniform to a new value
		 */
		template <class T>
		void SetUniform(const NIdentifier& id, const T& newVal)
		{
			CHECK(m_Shader);

			U32 offset = m_Shader->GetUniformOffset(id);

			if (offset != std::numeric_limits<U32>::max())
			{
				SetUniformData(offset, newVal);
			}
		}

		/**
		 * Pushes this Material's uniforms onto the render pipeline
		 */
		void EnableUniforms() const;

		/**
		 * Returns the corresponding AssetType for Materials
		 */
		virtual const AssetType GetType() const override { return AssetType::AT_MATERIAL; }

	protected:

		/**
		 * Creates the Material from the given buffer
		 */
		virtual void CreateFromBuffer(BitStream& data) override;

		/**
		 * Frees memory and releases rendering resources
		 */
		virtual void Destroy() override;

	private:

		/**
		 * Sets the data at the offset to a new value
		 */
		template <class T>
		void SetUniformData(U32 offset, const T& newValue)
		{
			*((T*)(m_UniformBuffer + offset)) = newValue;
		}

	private:

		// Shader instance assigned to this Material
		Shader* m_Shader;
		// Uniform buffer
		UByte* m_UniformBuffer;

	};
}