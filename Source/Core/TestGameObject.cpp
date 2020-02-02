#include "TestGameObject.h"

namespace Noble
{

	GAME_OBJECT_DEF(TestGameObject);

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