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
		if (!m_RootComponent)
		{
			NE_LOG_WARNING("Cannot set transform of GameObject with no Root Component");
			return;
		}

		m_RootComponent->SetLocalPosition(newPos);
	}

	void GameObject::SetRotation(const Rotator& newRot) const
	{
		if (!m_RootComponent)
		{
			NE_LOG_WARNING("Cannot set transform of GameObject with no Root Component");
			return;
		}

		m_RootComponent->SetLocalRotation(newRot);
	}

	void GameObject::SetScale(const Vector3f& newScale) const
	{
		if (!m_RootComponent)
		{
			NE_LOG_WARNING("Cannot set transform of GameObject with no Root Component");
			return;
		}

		m_RootComponent->SetLocalScale(newScale);
	}

	Vector3f GameObject::GetPosition() const
	{
		if (!m_RootComponent)
		{
			NE_LOG_WARNING("Cannot retrieve transform of GameObject with no Root Component");
			return Vector3f(0.0F);
		}

		return m_RootComponent->GetLocalPosition();
	}

	Rotator GameObject::GetRotation() const
	{
		if (!m_RootComponent)
		{
			NE_LOG_WARNING("Cannot retrieve transform of GameObject with no Root Component");
			return Rotator(0.0F, 0.0F, 0.0F);
		}

		return m_RootComponent->GetLocalRotation();
	}

	Vector3f GameObject::GetScale() const
	{
		if (!m_RootComponent)
		{
			NE_LOG_WARNING("Cannot retrieve transform of GameObject with no Root Component");
			return Vector3f(1.0F);
		}

		return m_RootComponent->GetLocalScale();
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