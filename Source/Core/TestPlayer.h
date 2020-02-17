#pragma once

#include "GameObject.h"

namespace Noble
{

	class TestPlayer : public GameObject
	{
		OBJECT_DECL(TestPlayer, GameObject)
	public:

		TestPlayer();

		virtual void OnActionInput(const U32 bindingId, bool state) override;

		virtual void OnAnalogInput(const U32 bindingId, float state) override;

	private:

		Vector3f m_MoveDir;
	};
}