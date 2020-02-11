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

	void Component::Serialize(BitStream& stream)
	{
		Super::Serialize(stream);

		stream.Write<Vector3f>(m_Transform.Position);
		stream.Write<Quaternion>(m_Transform.Rotation);
		stream.Write<Vector3f>(m_Transform.Scale);
	}

	void Component::Deserialize(BitStream& stream)
	{
		Super::Deserialize(stream);

		m_Transform.Position = stream.Read<Vector3f>();
		m_Transform.Rotation = stream.Read<Quaternion>();
		m_Transform.Scale = stream.Read<Vector3f>();
	}
}