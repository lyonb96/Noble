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

		ConstructorCallForward();
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

	void GameObject::AddComponent(Component* comp)
	{
		m_Components.Add(comp);
	}
}