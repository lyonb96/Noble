#pragma once

#include "Object.h"
#include "Component.h"
#include "Types.h"
#include "Array.h"
#include "String.h"
#include "Logger.h"

/**
 * This macro automatically declares special functions that are required in child GameObjects
 */
#define GAME_OBJECT_BODY(CLASS_NAME, PARENT_NAME) \
OBJECT_DECL(CLASS_NAME, PARENT_NAME)\
public: \
\
	static ::Noble::I16 ComponentCount; \
\
protected:\
\
	virtual void ConstructorCallForward() override;\
\
private:
// --------------------------------------------------------------------------------------------

/**
 * This macro takes care of defining necessary functions and members for child GameObjects
 */
#define GAME_OBJECT_DEF(CLASS_NAME) \
OBJECT_DEF(CLASS_NAME); \
::Noble::I16 CLASS_NAME::ComponentCount = -1; \
\
void CLASS_NAME::ConstructorCallForward() { if (ComponentCount > 0) { m_Components.Resize(ComponentCount); } } \

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
	class GameObject : public Object
	{
		OBJECT_DECL(GameObject, Object)
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
		T* GetComponent(const NImmutableIdentifier& name)
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
		template <class T>
		T* GetComponent()
		{
			for (Component* comp : m_Components)
			{
				T* reqComp = comp->IsA<T>();
				if (reqComp)
				{
					return reqComp;
				}
			}

			return nullptr;
		}

	protected:

		/**
		 * Called by the constructor and forwarded to the overridden class
		 */
		virtual void ConstructorCallForward() {};

		/**
		 * Returns a pointer to the World instance
		 */
		World* GetWorld() { return m_World; }

		/**
		 * Creates a child component and adds it to the list of members
		 * This function can only be called from the constructor!
		 */
		template <class T>
		T* CreateChildComponent(const NImmutableIdentifier& name = "")
		{
			return GetWorld()->CreateComponent<T>(this, name);
		}

	private:

		/**
		 * Called by the World class after it creates a Component instance to add it
		 * to this GameObject's array of components
		 */
		void AddComponent(Component* comp);

	protected:

		// A pointer to the game's World instance
		static World* m_World;

		// If true, the object's Update function will be called each frame
		bool m_TickEachFrame;
		// Root Component
		Component* m_RootComponent;
		// Array of all components
		Array<Component*> m_Components;

	private:
	};
}