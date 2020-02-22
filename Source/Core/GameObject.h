#pragma once

#include "Object.h"
#include "Component.h"
#include "Types.h"
#include "Array.h"
#include "String.h"
#include "Logger.h"
#include "Globals.h"

namespace Noble
{
	// Class prototype for the Game World
	class World;

	// Class prototype for Controller
	class Controller;

	/**
	 * GameObjects encompass any game world spawnable entity.
	 * They are the building block of gameplay in the engine
	 *
	 * All subclasses of GameObject have to provide an empty constructor
	 * that calls the parent empty constructor!
	 */
	class GameObject : public Object
	{
		OBJECT_DECL(GameObject, Object)
	public:

		// Allow the World class to access private members
		friend class World;
		// Allow Controllers to access the fun stuff
		friend class Controller;

		/**
		 * Default constructor that handles back-end setup
		 */
		GameObject();

		GameObject(const GameObject&) = delete; // Cannot be copied
		GameObject(GameObject&&) = delete; // Cannot be moved
		GameObject& operator=(const GameObject&) = delete;
		GameObject& operator=(GameObject&&) = delete;

	public:

		/**
		 * Returns a pointer to this Object's root component.
		 * Note that it is possible for this to be nullptr
		 */
		Component* GetRootComponent() const { return m_RootComponent; }

		/**
		 * Returns true if this GameObject should tick each frame
		 */
		const bool IsTickable() const { return m_TickEachFrame; }

		/**
		 * Sets the position of the Root Component
		 */
		void SetPosition(const Vector3f& newPos) const;

		/**
		 * Sets the rotation of the Root Component
		 */
		void SetRotation(const Rotator& newRot) const;

		/**
		 * Sets the scale of the Root Component
		 */
		void SetScale(const Vector3f& newScale) const;

		/**
		 * Returns the position of the Root Component
		 */
		Vector3f GetPosition() const;

		/**
		 * Returns the rotation of the Root Component
		 */
		Rotator GetRotation() const;

		/**
		 * Returns the scale of the Root Component
		 */
		Vector3f GetScale() const;

		/**
		 * Returns the current Controller of this GameObject, if any
		 */
		Controller* GetController() const;

		/**
		 * Returns the array of components for this GameObject
		 */
		const Array<Component*>& GetComponents() const;

	public:

		/**
		 * When this GameObject is possessed by a player, this function gets called
		 * each time an action binding's state changes
		 */
		virtual void OnActionInput(const U32 bindingId, bool state) {}

		/**
		 * When this GameObject is possessed by a player, this function gets called
		 * each frame with the updated state of each axis binding
		 */
		virtual void OnAnalogInput(const U32 bindingId, float state) {}

		/**
		 * Called once each frame for every GameObject to complete frame-by-frame logic
		 */
		virtual void Update(float tpf) {}

		/**
		 * Called at a fixed rate, regardless of framerate
		 */
		virtual void FixedUpdate() {}

		/**
		 * Called once when this GameObject is spawned into a level
		 */
		virtual void OnSpawn() {}

		/**
		 * Called once when this GameObject is despawned from a level
		 */
		virtual void OnDespawn() {}

		/**
		 * Called when this GameObject is possessed by a new Controller
		 */
		virtual void OnPossess() {}

		/**
		 * Called when this GameObject is no longer possessed by a Controller
		 */
		virtual void OnUnpossess() {}

	public:

		/**
		 * Returns a pointer to the first component of the specified type
		 * whose name matches the given parameter
		 */
		template <typename T>
		T* GetComponent(const NIdentifier& name)
		{
			for (Component* comp : m_Components)
			{
				T* reqComp = comp->IsA<T>();
				if (reqComp)
				{
					if (comp->GetName() == name)
					{
						return reqComp;
					}
				}
			}

			return nullptr;
		}

		/**
		 * Returns a pointer to the first component of the specified type
		 */
		template <typename T>
		T* GetComponent()
		{
			for (Component* comp : m_Components)
			{
				if (T* reqComp = comp->IsA<T>())
				{
					return reqComp;
				}
			}

			return nullptr;
		}

	protected:

		/**
		 * Creates a child component and adds it to the list of members
		 * This function should only be called from the constructor!
		 */
		template <typename T>
		T* CreateChildComponent(const NIdentifier& name = ID(""))
		{
			return GetWorld()->CreateComponent<T>(this, name);
		}

	private:

		/**
		 * Called by the World class after it creates a Component instance to add it
		 * to this GameObject's array of components
		 */
		void AddComponent(Component* comp);

		/**
		 * Changes the Controller of this GameObject
		 */
		void SetController(Controller* cont);

	protected:

		// If true, the object's Update function will be called each frame
		bool m_TickEachFrame;
		// Root Component
		Component* m_RootComponent;
		// Current controller, if any
		Controller* m_Controller;
		// Array of all components
		Array<Component*> m_Components;

	private:
	};
}