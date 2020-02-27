#pragma once

#include "Component.h"
#include "Camera.h"

namespace Noble
{
	/**
	 * Camera Component acts as a location on a GameObject that the game camera
	 * can attach to on the object.
	 */
	class CameraComponent : public Component
	{
		OBJECT_DECL(CameraComponent, Component)

	public:

		CameraComponent();

		/**
		 * Changes the field of view of the camera when it is anchored to this Component
		 */
		void SetFieldOfView(const F32 fov) { m_FieldOfView = fov; }

		/**
		 * Returns the field of view set for this Camera anchor
		 */
		const F32 GetFieldOfView() const { return m_FieldOfView; }

	private:

		// Field of view for this Camera anchor
		F32 m_FieldOfView;
	};
}