#pragma once

#include "GameInstance.h"
#include "Material.h"
#include "Shader.h"
#include "StaticMesh.h"
#include "TestGameObject.h"

namespace Noble
{
	class TestGame : public GameInstance
	{
	public:

		TestGame(const char* name)
			: GameInstance(name)
		{}

		virtual void OnGameStart() override;

		virtual void OnGameEnd() override;

		virtual void Update() override;

	private:

		TestGameObject* m_TestObject1;
		TestGameObject* m_TestObject2;
		TestGameObject* m_TestObject3;

		StaticMesh* m_TestMesh;
		Shader* m_TestShader;
		Material* m_TestMat;
	};
}