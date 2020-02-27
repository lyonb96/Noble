#include "World.h"

#include "Time.h"
#include "Controller.h"

namespace Noble
{
	World::World()
	{}

	GameObject* World::SpawnGameObject(NClass* type, Vector3f spawnPos)
	{
		CHECK(type);

		GameObject* obj = BuildGameObject(type);

		obj->SetPosition(spawnPos);
		obj->OnSpawn();

		return obj;
	}

	Component* World::CreateComponent(NClass* type, GameObject* owner, const NIdentifier& name)
	{
		CHECK(type);

		Component* comp = BuildComponent(type);

		comp->SetOwningObject(owner);
		comp->SetComponentName(name);

		owner->AddComponent(comp);

		return comp;
	}

	Controller* World::CreateController(NClass* type)
	{
		CHECK(type);

		return BuildController(type);
	}

	void World::Update()
	{
		for (Controller* control : m_Controllers)
		{
			control->Update();
		}

		for (GameObject* obj : m_GameObjects)
		{
			obj->Update();
			for (Component* comp : obj->m_Components)
			{
				if (comp->IsEnabled())
				{
					comp->Update();
				}
			}
		}
	}

	void World::FixedUpdate()
	{
		for (Controller* control : m_Controllers)
		{
			control->FixedUpdate();
		}

		for (GameObject* obj : m_GameObjects)
		{
			obj->FixedUpdate();
			for (Component* comp : obj->m_Components)
			{
				if (comp->IsEnabled())
				{
					comp->FixedUpdate();
				}
			}
		}
	}

	GameObject* World::BuildGameObject(NClass* type)
	{
		CHECK(type->IsA<GameObject>());

		// Allocate memory for the new object
		void* data = NE_BUFFER_ALLOC(m_GameMemory, type->ObjectSize, type->ObjectAlign);
		// Initialize the new object
		GameObject* obj = (GameObject*)Object::CreateInstance(type, data);
		// Store it to the current list of objects
		m_GameObjects.Add(obj);

		return obj;
	}

	Component* World::BuildComponent(NClass* type)
	{
		CHECK(type->IsA<Component>());

		void* data = NE_BUFFER_ALLOC(m_GameMemory, type->ObjectSize, type->ObjectAlign);
		Component* comp = (Component*)Object::CreateInstance(type, data);

		if (SceneComponent* sc = comp->IsA<SceneComponent>())
		{
			m_SceneComponents.Add(sc);
		}

		return comp;
	}

	Controller* World::BuildController(NClass* type)
	{
		CHECK(type->IsA<Controller>());

		void* data = NE_BUFFER_ALLOC(m_GameMemory, type->ObjectSize, type->ObjectAlign);
		Controller* cntrl = (Controller*)Object::CreateInstance(type, data);

		m_Controllers.Add(cntrl);

		return cntrl;
	}
}