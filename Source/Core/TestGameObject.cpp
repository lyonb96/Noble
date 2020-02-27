#include "TestGameObject.h"

#include "Time.h"

namespace Noble
{

	OBJECT_DEF(TestGameObject);

	TestGameObject::TestGameObject()
		: Super()
	{
		// Sample

		StaticMeshComponent* meshComp = CreateChildComponent<StaticMeshComponent>(ID("Main Mesh"));
		m_RootComponent = meshComp;

		m_SecondMesh = CreateChildComponent<StaticMeshComponent>(ID("Submesh"));
		meshComp->AttachChildComponent(m_SecondMesh);

		m_SecondMesh->SetLocalPosition(Vector3f(0, 2, 0));
		m_SecondMesh->SetLocalScale(Vector3f(0.5F));

		StaticMeshComponent* test = CreateChildComponent<StaticMeshComponent>();
	}

	void TestGameObject::OnSpawn()
	{
	}

	void TestGameObject::Update()
	{
		static float test = 0.0F;
		test += Time::GetDeltaTime() * 0.2F;

		GetRootComponent()->SetLocalRotation({ test, 0.0F, 0.0F });
	}
}