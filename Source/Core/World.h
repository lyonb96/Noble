#pragma once

#include <type_traits>

#include "Array.h"
#include "GameObject.h"
#include "Logger.h"

namespace Noble
{
#ifdef NOBLE_DEBUG
	typedef MemoryArena<BlockAllocator, SimpleTrackingPolicy> GameMemoryArena;
#else
	typedef MemoryArena<BlockAllocator, NoTrackingPolicy> GameMemoryArena;
#endif

	/**
	 * The World class encompasses a "map" or area of play.
	 * Worlds hold all of the currently spawned GameObjects.
	 */
	class World
	{
	public:

		friend class GameObject;

		World();

		/**
		 * Creates and spawns a new GameObject of the specified type
		 * Optionally takes a location at which to spawn the GameObject
		 */
		template <class T>
		T* CreateAndSpawnGameObject(Vector3f spawnPos = Vector3f(0))
		{
			T* object = CreateGameObject<T>();

			object->SetPosition(spawnPos);
			object->OnSpawn();

			return object;
		}

		template <class T>
		T* CreateGameObject()
		{
			NClass* ncls = T::GetStaticClass();
			void* data = NE_BUFFER_ALLOC(m_GameMemory, ncls->ObjectSize, ncls->ObjectAlign);
			T* object = (T*)Object::CreateInstance<T>(data);
			// If this is the first instance of this Object type created, fill out the fields
			if (T::ComponentCount == -1)
			{
				// Set the number of components used to minimize calls to Realloc in the array
				T::ComponentCount = m_CreatedComponentCount;
				m_CreatedComponentCount = 0;

				// Resize, if the GameObject actually has components
				if (T::ComponentCount > 0)
				{
					object->m_Components.Shrink();
				}
			}

			m_GameObjects.Add(object);

			return object;
		}

		/**
		 * Creates a new Component that is part of the given GameObject
		 */
		template <class T>
		T* CreateComponent(GameObject* owner, const NImmutableIdentifier& name = "")
		{
			CHECK(owner);

			void* data = NE_BUFFER_ALLOC(m_GameMemory, T::GetStaticClass()->ObjectSize, T::GetStaticClass()->ObjectAlign);
			T* comp = (T*)Object::CreateInstance<T>(data);
			comp->SetOwningObject(owner);
			comp->SetComponentName(name);
			m_Components.Add(comp);

			owner->AddComponent(comp);
			++m_CreatedComponentCount;

			return comp;
		}

	private:

		// Number of components in the temp array
		U8 m_CreatedComponentCount;
		// Memory Arena for GameObjects + Components
		GameMemoryArena m_GameMemory;
		// Array of all currently spawned GameObjects
		Array<GameObject*> m_GameObjects;
		// Array of all Components in the game
		Array<Component*> m_Components;

	};
}