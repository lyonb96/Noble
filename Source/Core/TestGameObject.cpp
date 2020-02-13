#include "TestGameObject.h"

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

		m_SecondMesh->SetPosition(Vector3f(0, 2, 0));
		m_SecondMesh->SetScale(Vector3f(0.5F));

		StaticMeshComponent* test = CreateChildComponent<StaticMeshComponent>();
	}

	void TestGameObject::OnSpawn()
	{
	}

	void TestGameObject::Update(float tpf)
	{
		static float test = 0.0F;
		test += tpf * 0.2F;

		GetRootComponent()->SetRotation(glm::angleAxis(test, Vector3f(1.0F, 0.0F, 0.0F)));
	}
}