#pragma once

#include <type_traits>

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
		 * Creates a new GameObject, but does not immediately spawn it
		 */
		template <class T>
		T* SpawnGameObject()
		{
			//T* object = new (Allocate<T>()) T;
			T* object = NE_NEW(m_GameMemory, T);
			// By now, all of the required components have been stored in the temp array
			Size tempSize = sizeof(Component*) * m_TempCount;
			//Component** compList = m_GameObjectAllocator.Allocate(tempSize, 8);
			Component** compList = (Component**)NE_BUFFER_ALLOC(m_GameMemory, tempSize, 8);
			Memory::Memcpy(compList, m_TempArray, tempSize);

			object->PostInit(compList, m_TempCount);
			ClearTempArray();
			//m_GameObjects.Add(object);

			object->OnSpawn();

			return object;
		}

		/**
		 * Creates a new Component that is part of the given GameObject
		 */
		template <class T>
		T* CreateComponent(GameObject* owner, const U32 name = 0)
		{
			CHECK(owner);

			if (owner->IsInitialized())
			{
				NE_LOG_WARNING("Cannot add Components outside the constructor!");
				return nullptr;
			}

			T* comp = NE_NEW(m_GameMemory, T);
			comp->SetOwningObject(owner);
			comp->SetComponentName(name);
			//m_Components->Add(comp);

			m_TempArray[m_TempCount++] = comp;

			return comp;
		}

	private:

		/**
		 * Clears the temporary component list
		 */
		void ClearTempArray();

	private:

		// Temporary array of components to be copied into new GameObjects
		Component* m_TempArray[32];
		// Number of components in the temp array
		U8 m_TempCount;
		// Memory Arena for GameObjects + Components
		GameMemoryArena m_GameMemory;

		// Handles allocating objects and components
		//BlockAllocator m_GameObjectAllocator;

		//Array<GameObject*> m_GameObjects;
		//Array<Component*> m_Components;

	};
}