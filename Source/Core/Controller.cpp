#include "Controller.h"

#include "GameObject.h"

namespace Noble
{
	OBJECT_DEF(Controller);

	Controller::Controller()
		: m_Object(nullptr)
	{}

	void Controller::Possess(GameObject* obj)
	{
		if (!obj)
		{
			NE_LOG_WARNING("Cannot possess null object");
			return;
		}

		if (obj->GetController() != nullptr)
		{
			NE_LOG_WARNING("Cannot possess a GameObject that is already possessed");
			return;
		}

		m_Object = obj;
		m_Object->SetController(this);
		m_Object->OnPossess();
	}

	void Controller::Unpossess()
	{
		if (m_Object)
		{
			m_Object->SetController(nullptr);
			m_Object->OnUnpossess();
			m_Object = nullptr;
		}
	}
}