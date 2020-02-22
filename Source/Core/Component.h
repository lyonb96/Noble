#pragma once

#include "Object.h"
#include "Types.h"
#include "String.h"

#include <typeinfo>

namespace Noble
{
	// Dummy GameObject class
	class GameObject;

	/**
	 * Components are modular pieces of functionality that attach to GameObjects
	 */
	class Component : public Object
	{
		OBJECT_DECL(Component, Object)
	public:

		friend class World;

		Component();

	public:

		/**
		 * Called each frame to update logic
		 */
		virtual void Update(float tpf) {}

		/**
		 * Called at a fixed interval (default 60/sec) for fixed-step logic
		 */
		virtual void FixedUpdate() {}

	public:

		/**
		 * Returns a pointer to this Component's parent Component
		 * Note that this can be null if this Component is the root
		 */
		Component* GetParent() const { return m_ParentComponent; }

		/**
		 * Returns a pointer to the object that owns this Component
		 */
		GameObject* GetOwningObject() const { return m_Owner; }

		/**
		 * Returns the component's local transform
		 * This transform is local to this Component's parent Component
		 * If the Component has no parent, it is relative to the world origin.
		 */
		Transform& GetLocalTransform() { return m_Transform; }

		/**
		 * Returns the component's world transform (transform is relative to world origin)
		 */
		Matrix4x4f GetWorldTransform() const;

		/**
		 * Returns a hashed representation of the Component's name
		 */
		const NIdentifier& GetName() const { return m_ComponentName; }

		/**
		 * Sets this component's local position
		 */
		FORCEINLINE void SetPosition(const Vector3f& newPos) { m_Transform.Position = newPos; }

		/**
		 * Sets this component's local rotation
		 */
		FORCEINLINE void SetRotation(const Rotator& newRot) { m_Transform.Rotation = newRot; }

		/**
		 * Sets this component's local scale
		 */
		FORCEINLINE void SetScale(const Vector3f& newScale) { m_Transform.Scale = newScale; }

		/**
		 * Enables or disables the Component
		 */
		FORCEINLINE void SetEnabled(const bool enabled) { m_Enabled = enabled; }

		/**
		 * Returns this component's position - note that it returns a reference
		 */
		Vector3f& GetPosition() { return m_Transform.Position; }

		/**
		 * Returns this component's rotation - note that it returns a reference
		 */
		Rotator& GetRotation() { return m_Transform.Rotation; }

		/**
		 * Returns this component's scale - note that it returns a reference
		 */
		Vector3f& GetScale() { return m_Transform.Scale; }

		/**
		 * Returns true if this Component is enabled, false if it is disabled
		 */
		bool IsEnabled() const { return m_Enabled; }

		/**
		 * Attaches the given component to this one as a child
		 */
		void AttachChildComponent(Component* child);

	private:

		/**
		 * Called automatically during creation
		 */
		void SetOwningObject(GameObject* owner) { m_Owner = owner; }

		/**
		 * Changes the Component's name
		 */
		void SetComponentName(const NIdentifier& name) { m_ComponentName = name; }

		/**
		 * Changes the Component's name
		 */
		void SetComponentName(NIdentifier&& name) { m_ComponentName = std::move(name); }

	protected:

		// Component Name
		NIdentifier m_ComponentName;
		// Component's local Transform
		Transform m_Transform;
		// Object that owns this component (will always be valid)
		GameObject* m_Owner;
		// This component's parent (may be nullptr)
		Component* m_ParentComponent;
		// Enable state - component does not tick or contribute to scene if disabled
		bool m_Enabled;
	};
}