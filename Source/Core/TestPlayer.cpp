#include "TestPlayer.h"

#include "Time.h"
#include "StaticMeshComponent.h"
#include "World.h"

namespace Noble
{
	OBJECT_DEF(TestPlayer)

	TestPlayer::TestPlayer()
		: Super(), m_MoveDir(0)
	{
		StaticMeshComponent* smc = CreateChildComponent<StaticMeshComponent>(ID("PlayerMesh"));
		m_RootComponent = smc;

		// Build a camera component
	}

	void TestPlayer::Update(float tpf)
	{
		GetRootComponent()->SetPosition(GetRootComponent()->GetPosition() + (m_MoveDir * Time::GetDeltaTime() * 4.0F));
	}

	void TestPlayer::OnActionInput(const U32 bindingId, bool state)
	{
	}

	void TestPlayer::OnAnalogInput(const U32 bindingId, float state)
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
				break;
			case HASH("LookRight"):
				break;
		}
	}
}