#include "TestGame.h"

#include "Engine.h"
#include "Globals.h"
#include "Input.h"

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
		//g_TestMesh2 = m_AssetManager.GetStaticMesh(ID("TriangleMesh"));

		// Test shader loading
		m_TestShader = GetAssetManager()->GetShader(ID("TestShader"));

		// Test texture loading
		Texture2D* tex = GetAssetManager()->GetTexture2D(ID("TestTex"));

		// Test material loading
		m_TestMat = GetAssetManager()->CreateMaterial();
		m_TestMat->SetShader(m_TestShader);
		m_TestMat->SetUniform(ID("DiffuseTex"), tex);
		m_TestMat->SetUniform(ID("NormalTex"), nullptr);
		//Material* mat2 = GetAssetManager()->CreateMaterial(g_TestMat);
		//Material* mat3 = GetAssetManager()->CreateMaterial(g_TestMat);

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
	}

	void TestGame::OnGameEnd()
	{
	}

	void TestGame::Update()
	{
		Sleep(1); // short sleep to keep the loop from running super fast during testing

		static float test = 0.0F;
		test += Time::GetDeltaTime();

		float x = glm::sin(test);
		float y = glm::cos(test);
		//float x = 1.0F, y = 1.0F;
		float sinTest = glm::sin(test);

		{
			Matrix4x4f view = glm::lookAt(Vector3f(x * 15.0F, 15, y * 15.0F), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
			Matrix4x4f proj = glm::perspectiveFov(70.0F, 1280.0F, 720.0F, 0.1F, 100.0F);

			bgfx::setViewTransform(0, &view[0][0], &proj[0][0]);
		}

		m_TestObject1->SetPosition(Vector3f(5, 5, 5));
		m_TestObject2->SetScale(Vector3f(2, 2, 2));
		m_TestObject3->SetRotation(glm::angleAxis(x, Vector3f(1, 0, 0)));
		m_TestObject3->SetPosition(Vector3f(5, y * 5.0F, 0));

		if (Input::IsJustPressed(Input::KEY_P))
		{
			BENCHMARK(ObjectCreation);
			// spawn another object
			auto newObj = GetWorld()->SpawnGameObject<TestGameObject>(Vector3f(0, -5, 0));
			newObj->SetScale(Vector3f(2.0F));
			newObj->GetRootComponent()->IsA<StaticMeshComponent>()->SetMesh(m_TestMesh);
			newObj->GetRootComponent()->IsA<StaticMeshComponent>()->SetMaterial(m_TestMat);
			newObj->GetSecondMesh()->SetMesh(m_TestMesh);
			newObj->GetSecondMesh()->SetMaterial(m_TestMat);
		}
	}
}

DEFINE_GAME_INSTANCE(Noble::TestGame, "Test Game");