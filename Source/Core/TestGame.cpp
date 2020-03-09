#include "TestGame.h"

#include "Engine.h"
#include "Globals.h"
#include "GameInput.h"
#include "TestPlayer.h"
#include "PlayerController.h"

namespace Noble
{
	void TestGame::OnGameStart()
	{
		// Test registration
		GetAssetManager()->RegisterAsset(ID("CubeMesh"), "Content/TestMesh.txt", AssetType::AT_STATIC_MESH);
		GetAssetManager()->RegisterAsset(ID("TriangleMesh"), "Content/TestMesh2.txt", AssetType::AT_STATIC_MESH);
		GetAssetManager()->RegisterAsset(ID("TestShader"), "Content/shaders/simple_light.shd", AssetType::AT_SHADER);
		GetAssetManager()->RegisterAsset(ID("TestMaterial"), "Content/TestMat.bin", AssetType::AT_MATERIAL);
		GetAssetManager()->RegisterAsset(ID("TestTex"), "Content/TestTex.dds", AssetType::AT_TEXTURE2D);
		GetAssetManager()->RegisterAsset(ID("TestMesh3"), "Content/TestMesh3.bin", AssetType::AT_STATIC_MESH);

		m_TestObject1 = GetWorld()->SpawnGameObject<TestGameObject>();
		m_TestObject2 = GetWorld()->SpawnGameObject<TestGameObject>();
		m_TestObject3 = GetWorld()->SpawnGameObject<TestGameObject>();

		m_TestMesh = GetAssetManager()->GetStaticMesh(ID("TestMesh3"));

		// Test shader loading
		m_TestShader = GetAssetManager()->GetShader(ID("TestShader"));

		// Test texture loading
		Texture2D* tex = GetAssetManager()->GetTexture2D(ID("TestTex"));

		// Test material loading
		m_TestMat = GetAssetManager()->CreateMaterial();
		m_TestMat->SetShader(m_TestShader);
		m_TestMat->SetUniform(ID("DiffuseTex"), tex);
		m_TestMat->SetUniform(ID("NormalTex"), nullptr);

		((StaticMeshComponent*)m_TestObject1->GetRootComponent())->SetMesh(m_TestMesh);
		((StaticMeshComponent*)m_TestObject1->GetRootComponent())->SetMaterial(m_TestMat);
		((StaticMeshComponent*)m_TestObject2->GetRootComponent())->SetMesh(m_TestMesh);
		((StaticMeshComponent*)m_TestObject2->GetRootComponent())->SetMaterial(m_TestMat);
		((StaticMeshComponent*)m_TestObject3->GetRootComponent())->SetMesh(m_TestMesh);
		((StaticMeshComponent*)m_TestObject3->GetRootComponent())->SetMaterial(m_TestMat);

		m_TestObject1->GetSecondMesh()->SetMesh(m_TestMesh);
		m_TestObject1->GetSecondMesh()->SetMaterial(m_TestMat);
		m_TestObject2->GetSecondMesh()->SetMesh(m_TestMesh);
		m_TestObject2->GetSecondMesh()->SetMaterial(m_TestMat);
		m_TestObject3->GetSecondMesh()->SetMesh(m_TestMesh);
		m_TestObject3->GetSecondMesh()->SetMaterial(m_TestMat);

		TestPlayer* tgo = GetWorld()->SpawnGameObject<TestPlayer>(Vector3f(0, -50.0F, 0));
		tgo->SetScale(Vector3f(100.0F, 0.5F, 100.0F));
		tgo->GetRootComponent()->IsA<StaticMeshComponent>()->SetMesh(m_TestMesh);
		tgo->GetRootComponent()->IsA<StaticMeshComponent>()->SetMaterial(m_TestMat);
	}

	void TestGame::OnGameEnd()
	{
	}

	void TestGame::FixedUpdate()
	{
	}

	void TestGame::Update()
	{
		Sleep(1); // short sleep to keep the loop from running super fast during testing

		static float test = 0.0F;
		test += Time::GetDeltaTime();

		float x = glm::sin(test);
		float y = glm::cos(test);
		float sinTest = glm::sin(test);

		m_TestObject1->SetPosition(Vector3f(5, 5, 5));
		m_TestObject2->SetScale(Vector3f(2, 2, 2));
		m_TestObject3->SetRotation({x, 0.0F, 0.0F});
		m_TestObject3->SetPosition(Vector3f(5, y * 5.0F, 0));
		
		static bool created = false;
		if (Input::GetActionBinding(HASH("Fire"))->IsJustPressed() && !created)
		{
			created = true;

			BENCHMARK(ObjectCreation);
			// spawn another object
			auto newObj = GetWorld()->SpawnGameObject<TestPlayer>(Vector3f(0, -5, 0));
			newObj->GetRootComponent()->IsA<StaticMeshComponent>()->SetMesh(m_TestMesh);
			newObj->GetRootComponent()->IsA<StaticMeshComponent>()->SetMaterial(m_TestMat);
			auto newPC = GetWorld()->CreateController<PlayerController>();
			newPC->Possess(newObj);
		}
	}
}

DEFINE_GAME_INSTANCE(Noble::TestGame, "Noble Engine Test");