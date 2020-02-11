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
			T* object = (T*)BuildGameObject(T::GetStaticClass());

			object->OnSpawn();
			object->SetPosition(spawnPos);

			return object;
		}

		/**
		 * Spawns a new GameObject of the specified type
		 * Optionally takes a location at which to spawn the GameObject
		 */
		GameObject* SpawnGameObject(NClass* type, Vector3f spawnPos = Vector3f(0.0F));

		/**
		 * Creates a new Component that is part of the given GameObject
		 */
		template <class T>
		T* CreateComponent(GameObject* owner, const NIdentifier& name)
		{
			CHECK(owner);

			T* comp = (T*)BuildComponent(T::GetStaticClass());
			comp->SetOwningObject(owner);
			comp->SetComponentName(name);

			owner->AddComponent(comp);

			return comp;
		}

		/**
		 * Creates a new Component that is part of the given GameObject
		 */
		Component* CreateComponent(NClass* type, GameObject* owner, const NIdentifier& name);

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

		/**
		 * Stores the current world state in a stream and returns the stream
		 */
		BitStream SerializeWorld();

		/**
		 * Creates the world from the given stream
		 */
		void DeserializeWorld(BitStream& stream);

	private:

		/**
		 * Internal code to create a game object from an NClass
		 */
		GameObject* BuildGameObject(NClass* type);

		/**
		 * Internal code to create a component from an NClass
		 */
		Component* BuildComponent(NClass* type);

	private:

		// Memory Arena for GameObjects + Components
		GameMemoryArena m_GameMemory;
		// Array of all currently spawned GameObjects
		Array<GameObject*> m_GameObjects;
		// Array of all renderable Components in the game
		Array<SceneComponent*> m_SceneComponents;

	};
}