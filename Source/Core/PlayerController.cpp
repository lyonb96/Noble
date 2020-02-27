#include "PlayerController.h"

#include "GameInput.h"
#include "GameObject.h"
#include "Globals.h"
#include "Engine.h"

namespace Noble
{
	OBJECT_DEF(PlayerController);

	PlayerController::PlayerController()
	{}

	void PlayerController::Possess(GameObject* obj)
	{
		Super::Possess(obj);

		CameraComponent* cam = obj->GetComponent<CameraComponent>();
		if (cam)
		{
			g_Engine->GetCameraManager()->SetActiveCamera(cam);
		}
	}

	void PlayerController::Update()
	{
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