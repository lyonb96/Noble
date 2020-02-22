#include "PlayerController.h"

#include "GameInput.h"
#include "GameObject.h"

namespace Noble
{
	OBJECT_DEF(PlayerController);

	PlayerController::PlayerController(bool local)
		: m_IsLocal(local)
	{}

	void PlayerController::Possess(GameObject* obj)
	{
		Super::Possess(obj);

		if (m_Object)
		{
			//CameraComponent* cam = obj->GetComponent<CameraComponent>();
			//if (cam)
			//{
			//	m_Camera = cam->GetCamera();
			//}
		}
	}

	void PlayerController::Update()
	{
		if (!m_IsLocal)
			//return;

		if (m_Object)
		{
			// Forward action inputs to possessed object
			for (auto binding : Input::GetActionBindings())
			{
				if (binding.IsJustPressed())
				{
					m_Object->OnActionInput(binding.GetIdentifierHash(), true);
				}
				else if (binding.IsJustReleased())
				{
					m_Object->OnActionInput(binding.GetIdentifierHash(), false);
				}
			}

			// Forward axis inputs to possessed object
			for (auto binding : Input::GetAxisBindings())
			{
				m_Object->OnAnalogInput(binding.GetIdentifierHash(), binding.GetState());
			}
		}
	}

	void PlayerController::FixedUpdate()
	{}
}