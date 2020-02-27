#pragma once

#include "Types.h"

namespace Noble
{
	class CameraComponent;

	/**
	 * Camera Manager handles game camera positioning, FOV, and camera effects like shake
	 */
	class CameraManager
	{
		friend class Renderer;

	public:

		/**
		 * Default constructor
		 */
		CameraManager();

		// No move or copy, the engine only needs 1 instance
		CameraManager(const CameraManager&) = delete;
		CameraManager(CameraManager&&) = delete;
		CameraManager& operator=(const CameraManager&) = delete;
		CameraManager& operator=(CameraManager&&) = delete;

		/**
		 * Changes the current camera view to the given Camera
		 */
		void SetActiveCamera(CameraComponent* cam);

		/**
		 * Called every frame to perform updates on the camera's position and rotation
		 */
		void Update();

		/**
		 * Returns the view matrix for the camera's current transform (world -> view)
		 */
		Matrix4x4f GetViewMatrix() const;

		/**
		 * Returns the projection matrix for the camera's current settings (view -> projection)
		 */
		Matrix4x4f GetProjectionMatrix() const;

	private:

		/**
		 * Called by Renderer to update underlying camera data
		 */
		void UpdateRenderData(F32 width, F32 height, F32 near, F32 far);

	private:

		/**
		 * Struct to wrap lower level render data
		 */
		struct RenderData
		{
			F32 Width;
			F32 Height;
			F32 Near;
			F32 Far;
		} m_RenderData;

		// Current camera anchor
		CameraComponent* m_SelectedCam;
		// View matrix
		Matrix4x4f m_ViewMatrix;
		// Projection matrix
		Matrix4x4f m_ProjMatrix;
	};
}