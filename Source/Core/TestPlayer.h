#pragma once

#include "GameObject.h"

namespace Noble
{

	class CameraComponent;

	class TestPlayer : public GameObject
	{
		OBJECT_DECL(TestPlayer, GameObject)
	public:

		TestPlayer();

		virtual void Update() override;

		virtual void OnActionInput(const U32 bindingId, bool state) override;

		virtual void OnAnalogInput(const U32 bindingId, float state) override;

		virtual void OnPossess() override;

	private:

		Vector3f m_MoveDir;
		CameraComponent* m_Camera;
	};
}