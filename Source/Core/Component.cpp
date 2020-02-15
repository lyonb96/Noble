#include "Component.h"

namespace Noble
{
	OBJECT_DEF(Component);

	Component::Component()
	{
		m_Owner = nullptr;
		m_ParentComponent = nullptr;
	}

	Matrix4x4f Component::GetWorldTransform() const
	{
		if (!m_ParentComponent)
		{
			return m_Transform.GetMatrix4x4();
		}

		return m_ParentComponent->GetWorldTransform() * m_Transform.GetMatrix4x4();
	}

	void Component::AttachChildComponent(Component* child)
	{
		CHECK(child);

		child->m_ParentComponent = this;
	}
}