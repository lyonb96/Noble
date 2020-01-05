#include "Component.h"

namespace Noble
{
	Component::Component()
	{
		m_Owner = nullptr;
		m_ParentComponent = nullptr;
	}

	Transform Component::GetWorldTransform() const
	{
		if (!m_ParentComponent)
		{
			return m_Transform;
		}

		Transform parent = m_ParentComponent->GetWorldTransform();

		return parent + m_Transform;
	}

	void Component::AttachChildComponent(Component* child)
	{
		CHECK(child);

		child->m_ParentComponent = this;
	}
}