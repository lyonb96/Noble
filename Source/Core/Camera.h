#pragma once

#include "Types.h"

namespace Noble
{
	
	/**
	 * Camera class that holds data to create camera matrices
	 */
	class Camera
	{
		// Renderer is friend so it can set width/height/near/far
		friend class Renderer;
	public:

		Camera();

		/**
		 * Sets the world position
		 */
		void SetPosition(const Vector3f& pos) { m_Position = pos; }

		/**
		 * Sets the direction
		 */
		void SetDirection(const Vector3f& dir) { m_Direction = dir; }

		/**
		 * Sets the field of view
		 */
		void SetFieldOfView(F32 fov) { m_FieldOfView = fov; }

		/**
		 * Returns the current world position
		 */
		Vector3f GetPosition() const { return m_Position; }

		/**
		 * Returns the current direction
		 */
		Vector3f GetDirection() const { return m_Direction; }

		/**
		 * Returns the current field of view
		 */
		F32 GetFieldOfView() const { return m_FieldOfView; }

		/**
		 * Returns the view matrix
		 * (world space -> view space)
		 */
		Matrix4x4f GetViewMatrix() const;

		/**
		 * Returns the projection matrix
		 * (view space -> projection space)
		 */
		Matrix4x4f GetProjectionMatrix() const;

		/**
		 * Returns the view-projection matrix
		 * (world space -> projection space)
		 */
		Matrix4x4f GetViewProjectionMatrix() const;

		/**
		 * Returns the inverse of the view matrix
		 * (view space -> world space)
		 */
		Matrix4x4f GetInverseViewMatrix() const;

		/**
		 * Returns the inverse of the projection matrix
		 * (projection space -> view space)
		 */
		Matrix4x4f GetInverseProjectionMatrix() const;

		/**
		 * Returns the inverse of the view-projection matrix
		 * (projection space -> world space)
		 */
		Matrix4x4f GetInverseViewProjectionMatrix() const;
		
		/**
		 * Updates the matrices for this Camera according to its position and direction
		 */
		void UpdateMatrices();

	private:

		/**
		 * Updates the render parameters to use for constructing matrices
		 */
		void UpdateRenderParams(F32 width, F32 height, F32 near, F32 far);

	private:

		struct
		{
			// Width of framebuffer
			F32 Width;
			// Height of framebuffer
			F32 Height;
			// Near plane
			F32 Near;
			// Far plane
			F32 Far;
		} m_RenderParams;

		// World position of camera
		Vector3f m_Position;
		// Look direction of camera
		Vector3f m_Direction;
		// Field of view in degrees
		F32 m_FieldOfView;

		// View Matrix (world -> view)
		Matrix4x4f m_ViewMatrix;
		// Projection Matrix (view -> projection)
		Matrix4x4f m_ProjMatrix;

	};
}