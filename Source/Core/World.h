#pragma once

#include <type_traits>

#include "GameObject.h"

namespace Noble
{
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
		T* CreateGameObject()
		{
			T* object = new (Allocate<T>()) T;
			// By now, all of the required components have been stored in the temp array
			Size tempSize = sizeof(Component*) * m_TempCount;
			Component** compList = m_GameObjectAllocator.Allocate(tempSize, 8);
			Memory::Memcpy(compList, m_TempArray, tempSize);

			object->PostInit(compList, m_TempCount);
			ClearTempArray();
			//m_GameObjects.Add(object);
			return object;
		}

		/**
		 * Creates a new Component that is part of the given GameObject
		 */
		template <class T>
		T* CreateComponent(GameObject* owner)
		{
			CHECK(owner);

			if (owner->IsInitialized())
			{
				NE_LOG_WARNING("Cannot add Components outside the constructor!");
				return nullptr;
			}

			T* comp = new (Allocate<T>()) T;
			comp->SetOwningObject(owner);
			//m_Components->Add(comp);

			m_TempArray[m_TempCount++] = comp;

			return comp;
		}

	private:

		/**
		 * Clears the temporary component list
		 */
		void ClearTempArray();

		/**
		 * Allocates memory for the requested GameObject/Component
		 */
		template <class T>
		void* Allocate()
		{
			Size size = sizeof(T);
			Size align = alignof(T);

			void* data = m_GameObjectAllocator.Allocate(size, align);
			CHECK(data);

			return data;
		}

	private:

		// Temporary array of components to be copied into new GameObjects
		Component* m_TempArray[32];
		// Number of components in the temp array
		U8 m_TempCount;

		// Handles allocating objects and components
		BlockAllocator m_GameObjectAllocator;
		//Array<GameObject*> m_GameObjects;
		//Array<Component*> m_Components;

	};
}