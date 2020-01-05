#pragma once

#include "World.h"
#include "GameObject.h"
#include "StaticMeshComponent.h"

namespace Noble
{

	class TestGameObject : public GameObject
	{
		GAME_OBJECT_BODY(TestGameObject)

	private:

		StaticMeshComponent* m_SecondMesh;

	public:

		TestGameObject();

		StaticMeshComponent* GetSecondMesh() { return m_SecondMesh; }

		virtual void OnSpawn() override;
	};
}