#pragma once

#include <type_traits>

#include "Array.h"
#include "GameObject.h"
#include "Logger.h"
#include "SceneComponent.h"

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
		// Renderer is friend to access list of SceneComponents
		friend class Renderer;

		World();

		/**
		 * Spawns a new GameObject of the specified type
		 * Optionally takes a location at which to spawn the GameObject
		 */
		template <class T>
		T* SpawnGameObject(Vector3f spawnPos = Vector3f(0.0F))
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

			object->OnSpawn();
			object->SetPosition(spawnPos);

			return object;
		}

		/**
		 * Creates a new Component that is part of the given GameObject
		 */
		template <class T>
		T* CreateComponent(GameObject* owner, const NIdentifier& name = "")
		{
			CHECK(owner);

			void* data = NE_BUFFER_ALLOC(m_GameMemory, T::GetStaticClass()->ObjectSize, T::GetStaticClass()->ObjectAlign);
			T* comp = (T*)Object::CreateInstance<T>(data);
			comp->SetOwningObject(owner);
			comp->SetComponentName(name);

			if (comp->IsA<SceneComponent>())
			{
				m_SceneComponents.Add(comp);
			}

			owner->AddComponent(comp);
			++m_CreatedComponentCount;

			return comp;
		}

		/**
		 * Called each frame - runs logic updates on all spawned GameObjects and Components
		 * The ordering is to call update on a GameObject, then on all of its Components immediately after
		 */
		void Update();

		/**
		 * Called at a fixed rate - runs logic updates on all spawned GameObjects and Components
		 * The ordering is to call update on a GameObject, then on all of its Components immediately after
		 */
		void FixedUpdate();

	private:

		// Number of components in the temp array
		U8 m_CreatedComponentCount;
		// Memory Arena for GameObjects + Components
		GameMemoryArena m_GameMemory;
		// Array of all currently spawned GameObjects
		Array<GameObject*> m_GameObjects;
		// Array of all renderable Components in the game
		Array<SceneComponent*> m_SceneComponents;

	};
}