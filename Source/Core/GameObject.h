#pragma once

#include "Component.h"
#include "Types.h"
#include "Array.h"
#include "String.h"

/**
 * This macro automatically defines special functions that are required in child GameObjects
 */
#define GAME_OBJECT_BODY(CLASS_NAME) \
public: \
\
	static ::Noble::I16 ComponentCount; \
\
private:
// --------------------------------------------------------------------------------------------

/**
 * This macro takes care of defining necessary functions and members for child GameObjects
 */
#define GAME_OBJECT_DEF(CLASS_NAME) ::Noble::I16 CLASS_NAME::ComponentCount = -1

namespace Noble
{
	// Class prototype for the Game World
	class World;

	// Class prototype for Component
	class Component;

	/**
	 * GameObjects encompass any game world spawnable entity.
	 * They are the building block of gameplay in the engine
	 *
	 * All subclasses of GameObject have to provide an empty constructor
	 * that calls the parent empty constructor!
	 */
	class GameObject
	{
	public:

		// Allow the World class to access private members
		friend class World;

		/**
		 * Default constructor that handles back-end setup
		 */
		GameObject();

		GameObject(const GameObject&) = delete; // Cannot be copied
		GameObject(const GameObject&&) = delete; // Cannot be moved

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
		void SetRotation(const Quaternion& newRot) const;

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
		Quaternion GetRotation() const;

		/**
		 * Returns the scale of the Root Component
		 */
		Vector3f GetScale() const;

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

	public:

		/**
		 * Returns a pointer to the first component of the specified type
		 * whose name matches the given parameter
		 */
		template <class T>
		T* GetComponent(const U32 name)
		{
			// TODO: figure out if using typeid() and RTTI is a good idea

			const std::type_info& query = typeid(T);
			for (Size i = 0; i < m_Components.GetSize(); i++)
			{
				Component* comp = m_Components[i];
				if (m_Components[i] && query == typeid(*comp))
				{
					if (comp->GetName() == name)
					{
						return static_cast<T*>(m_Components[i]);
					}
				}
			}

			return nullptr;
		}

		/**
		 * Returns a pointer to the first component of the specified type
		 */
		template <class T>
		T* GetComponent()
		{
			// TODO: figure out if using typeid() and RTTI is a good idea

			const std::type_info& query = typeid(T);
			for (Size i = 0; i < m_Components.GetSize(); i++)
			{
				Component* comp = m_Components[i];
				if (m_Components[i] && query == typeid(*comp))
				{
					return static_cast<T*>(m_Components[i]);
				}
			}

			return nullptr;
		}

	protected:

		/**
		 * Returns a pointer to the World instance
		 */
		World* GetWorld() { return m_World; }

		/**
		 * Creates a child component and adds it to the list of members
		 * This function can only be called from the constructor!
		 */
		template <class T>
		T* CreateChildComponent(const U32 name = 0)
		{
			return GetWorld()->CreateComponent<T>(this, name);
		}

	private:

		/**
		 * Called automatically after the object is created to fill out the Components array
		 */
		void PostInit(Component** components, Size componentCount);

		bool IsInitialized() const;

	protected:

		// A pointer to the game's World instance
		static World* m_World;

		// If true, the object's Update function will be called each frame
		bool m_TickEachFrame;
		// Root Component
		Component* m_RootComponent;

	private:

		// Array of all components
		SafeArray<Component*> m_Components;

	};
}