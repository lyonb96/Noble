#include "Camera.h"

namespace Noble
{

	Camera::Camera()
		: m_Position(0.0F), 
		m_Direction(0.0F, 0.0F, 1.0F), 
		m_FieldOfView(70.0F),
		m_ViewMatrix(1.0F),
		m_ProjMatrix(1.0F)
	{}

	Matrix4x4f Camera::GetViewMatrix() const
	{
		return m_ViewMatrix;
	}

	Matrix4x4f Camera::GetProjectionMatrix() const
	{
		return m_ProjMatrix;
	}

	Matrix4x4f Camera::GetViewProjectionMatrix() const
	{
		return m_ViewMatrix * m_ProjMatrix;
	}

	Matrix4x4f Camera::GetInverseViewMatrix() const
	{
		return glm::inverse(GetViewMatrix());
	}

	Matrix4x4f Camera::GetInverseProjectionMatrix() const
	{
		return glm::inverse(GetProjectionMatrix());
	}

	Matrix4x4f Camera::GetInverseViewProjectionMatrix() const
	{
		return glm::inverse(GetViewProjectionMatrix());
	}

	void Camera::UpdateMatrices()
	{
		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Direction, Vector3f(0.0F, 1.0F, 0.0F));
		m_ProjMatrix = glm::perspectiveFov(
			m_FieldOfView, 
			m_RenderParams.Width, 
			m_RenderParams.Height, 
			m_RenderParams.Near, 
			m_RenderParams.Far);
	}

	void Camera::UpdateRenderParams(F32 width, F32 height, F32 near, F32 far)
	{
		m_RenderParams.Width = width;
		m_RenderParams.Height = height;
		m_RenderParams.Near = near;
		m_RenderParams.Far = far;

		const Size s = sizeof(Camera);
	}
}