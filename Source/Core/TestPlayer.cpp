#include "TestPlayer.h"

#include "CameraComponent.h"
#include "PlayerController.h"
#include "Time.h"
#include "StaticMeshComponent.h"
#include "World.h"

#include "GameInput.h"

namespace Noble
{
	OBJECT_DEF(TestPlayer)

	TestPlayer::TestPlayer()
		: Super(), m_MoveDir(0)
	{
		StaticMeshComponent* smc = CreateChildComponent<StaticMeshComponent>(ID("PlayerMesh"));
		m_RootComponent = smc;

		// Build a camera component
		m_Camera = CreateChildComponent<CameraComponent>(ID("MainCam"));
		m_RootComponent->AttachChildComponent(m_Camera);

		m_Camera->SetLocalPosition(Vector3f(0, 4, -8));
	}

	void TestPlayer::Update()
	{
		Vector3f posAdd = m_MoveDir * Time::GetDeltaTime() * 16.0F;
		posAdd = (GetRootComponent()->GetLocalRotation().GetMatrix3x3() * posAdd);
		GetRootComponent()->SetLocalPosition(GetRootComponent()->GetLocalPosition() + posAdd);
	}

	void TestPlayer::OnActionInput(const U32 bindingId, bool state)
	{
		switch (bindingId)
		{
			case HASH("Jump"):
				m_MoveDir.y = state ? 1.0F : 0.0F;
				break;
		}
	}

	void TestPlayer::OnAnalogInput(const U32 bindingId, F32 state)
	{
		switch (bindingId)
		{
			case HASH("MoveForward"):
				m_MoveDir.z = state;
				break;
			case HASH("MoveRight"):
				m_MoveDir.x = -state;
				break;
			case HASH("LookUp"):
			{
				m_RootComponent->GetLocalRotation().Yaw += state * 0.002F;
				break;
			}
			case HASH("LookRight"):
				m_RootComponent->GetLocalRotation().Roll += state * -0.002F;
				break;
		}
	}
}