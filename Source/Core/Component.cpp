#include "Component.h"

namespace Noble
{

	Component::Component()
	{
		m_ComponentName = 0;
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