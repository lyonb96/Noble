#pragma once

#include "World.h"
#include "StaticMeshComponent.h"
#include "GameObject.h"

namespace Noble
{

	class TestGameObject : public GameObject
	{
		GAME_OBJECT_BODY(TestGameObject, GameObject)

	private:

		StaticMeshComponent* m_SecondMesh;

	public:

		TestGameObject();

		StaticMeshComponent* GetSecondMesh() { return m_SecondMesh; }

		virtual void OnSpawn() override;
	};
}