#include "GameObject.h"

#include "Component.h"
#include "StaticMeshComponent.h"
#include "World.h"

namespace Noble
{
	World* GameObject::m_World = nullptr;

	OBJECT_DEF(GameObject);

	GameObject::GameObject()
	{
		m_TickEachFrame = false;
		m_RootComponent = nullptr;
		m_Controller = nullptr;
	}

	void GameObject::SetPosition(const Vector3f& newPos) const
	{
		CHECK(m_RootComponent != nullptr);

		m_RootComponent->SetPosition(newPos);
	}

	void GameObject::SetRotation(const Quaternion& newRot) const
	{
		CHECK(m_RootComponent != nullptr);

		m_RootComponent->SetRotation(newRot);
	}

	void GameObject::SetScale(const Vector3f& newScale) const
	{
		CHECK(m_RootComponent != nullptr);

		m_RootComponent->SetScale(newScale);
	}

	Vector3f GameObject::GetPosition() const
	{
		CHECK(m_RootComponent != nullptr);

		return m_RootComponent->GetPosition();
	}

	Quaternion GameObject::GetRotation() const
	{
		CHECK(m_RootComponent != nullptr);

		return m_RootComponent->GetRotation();
	}

	Vector3f GameObject::GetScale() const
	{
		CHECK(m_RootComponent != nullptr);

		return m_RootComponent->GetScale();
	}

	Controller* GameObject::GetController() const
	{
		return m_Controller;
	}

	void GameObject::AddComponent(Component* comp)
	{
		m_Components.Add(comp);
	}

	void GameObject::Serialize(BitStream& stream)
	{
		// Write the most-derived type's ID
		stream.Write<U32>(GetClass()->ObjectID.GetHash());

		// Every Component can be recreated from just calling the constructor of the obj
		// Therefore we can skip writing the Component class IDs and simply serialize them directly

		for (Component* comp : m_Components)
		{
			comp->Serialize(stream);
		}
	}

	void GameObject::Deserialize(BitStream& stream)
	{
		// The type ID was read to create this instance so we don't need to read it
		// Allow each Component to deserialize itself
		for (Component* comp : m_Components)
		{
			comp->Deserialize(stream);
		}
	}
}