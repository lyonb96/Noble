#include "GameObject.h"

#include "Component.h"
#include "StaticMeshComponent.h"
#include "World.h"

namespace Noble
{
	World* GameObject::m_World = nullptr;

	GameObject::GameObject()
	{
		m_TickEachFrame = false;
		m_World = nullptr;
		m_RootComponent = nullptr;
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

	void GameObject::PostInit(Component** components, Size componentCount)
	{
		m_Components.Initialize(components, componentCount);
	}

	bool GameObject::IsInitialized() const
	{
		return m_Components.GetSize() > 0;
	}

	// ...

	GAME_OBJECT_DEF(TestGameObject);

	TestGameObject::TestGameObject()
		: GameObject()
	{
		// Sample

		StaticMeshComponent* meshComp = CreateChildComponent<StaticMeshComponent>();
		m_RootComponent = meshComp;
		/*
		MeshHandle mesh = ResourceHandler::GetMeshHandle("barrel"); // or whatever this will look like

		CameraComponent* cam = CreateChildComponent<CameraComponent>();
		m_RootComponent->AttachChild(cam);
		cam->SetPosition(0.0F, 5.0F, -5.0F);
		
		*/
	}

	void TestGameObject::OnSpawn()
	{
		
	}
}