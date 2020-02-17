#include "GameObject.h"

#include "Component.h"
#include "StaticMeshComponent.h"
#include "World.h"

namespace Noble
{
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

	void GameObject::SetRotation(const Rotator& newRot) const
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

	Rotator GameObject::GetRotation() const
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

	const Array<Component*>& GameObject::GetComponents() const
	{
		return m_Components;
	}

	void GameObject::AddComponent(Component* comp)
	{
		m_Components.Add(comp);
	}

	void GameObject::SetController(Controller* cont)
	{
		m_Controller = cont;
	}
}