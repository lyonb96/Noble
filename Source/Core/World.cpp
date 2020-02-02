#include "World.h"

#include "Time.h"

namespace Noble
{
	World::World()
	{
		GameObject::m_World = this;
		m_CreatedComponentCount = 0;
	}

	void World::Update()
	{
		for (GameObject* obj : m_GameObjects)
		{
			obj->Update(Time::GetDeltaTime());
			for (Component* comp : obj->m_Components)
			{
				comp->Update(Time::GetDeltaTime());
			}
		}
	}

	void World::FixedUpdate()
	{
		for (GameObject* obj : m_GameObjects)
		{
			obj->FixedUpdate();
			for (Component* comp : obj->m_Components)
			{
				comp->FixedUpdate();
			}
		}
	}
}