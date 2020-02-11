#include "World.h"

#include "Time.h"

namespace Noble
{
	World::World()
	{
		GameObject::m_World = this;
	}

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

	BitStream World::SerializeWorld()
	{
		BitStream stream;

		// Write every GameObject to the stream
		// GameObjects serialize their own Components
		for (GameObject* obj : m_GameObjects)
		{
			obj->Serialize(stream);
		}

		return stream;
	}

	void World::DeserializeWorld(BitStream& stream)
	{
		while (stream.IsReadable())
		{
			// Grab ID of the GameObject
			U32 id = stream.Read<U32>();
			// Find the NClass for the given ID
			NClass* objectClass = Object::GetClassByID(id);
			// Verify that the class is valid in debug builds
			CHECK(objectClass);
			// Spawn the object
			GameObject* obj = SpawnGameObject(objectClass);
			// Let the object handle its own deserialization
			obj->Deserialize(stream);
		}
	}

	GameObject* World::BuildGameObject(NClass* type)
	{
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
		void* data = NE_BUFFER_ALLOC(m_GameMemory, type->ObjectSize, type->ObjectAlign);
		Component* comp = (Component*)Object::CreateInstance(type, data);

		if (SceneComponent* sc = comp->IsA<SceneComponent>())
		{
			m_SceneComponents.Add(sc);
		}

		return comp;
	}
}