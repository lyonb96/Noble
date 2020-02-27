#include "CameraManager.h"

#include "CameraComponent.h"
#include "Logger.h"

namespace Noble
{
	CameraManager::CameraManager()
		: m_SelectedCam(nullptr)
	{}

	void CameraManager::SetActiveCamera(CameraComponent* cam)
	{
		m_SelectedCam = cam;
	}

	void CameraManager::Update()
	{
		if (m_SelectedCam)
		{
			Matrix4x4f tform = m_SelectedCam->GetWorldTransform();
			Vector3f dir = Vector3f(tform[2]);
			Vector3f up = Vector3f(tform[1]);
			Vector3f pos = Vector3f(tform[3]);

			m_ViewMatrix = glm::lookAt(pos, pos + dir, up);
			tform = glm::inverse(tform);

			// possible optimization - only recalculate if necessary
			m_ProjMatrix = glm::perspectiveFov(
				m_SelectedCam->GetFieldOfView(), 
				m_RenderData.Width, 
				m_RenderData.Height, 
				m_RenderData.Near, 
				m_RenderData.Far);
		}
		else
		{
			m_ViewMatrix = Matrix4x4f(1.0F);
			m_ProjMatrix = Matrix4x4f(1.0F);
		}
	}

	Matrix4x4f CameraManager::GetViewMatrix() const
	{
		return m_ViewMatrix;
	}

	Matrix4x4f CameraManager::GetProjectionMatrix() const
	{
		return m_ProjMatrix;
	}

	void CameraManager::UpdateRenderData(F32 width, F32 height, F32 near, F32 far)
	{
		m_RenderData.Width = width;
		m_RenderData.Height = height;
		m_RenderData.Near = near;
		m_RenderData.Far = far;
	}
}