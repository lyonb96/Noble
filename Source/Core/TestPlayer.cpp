#include "TestPlayer.h"

namespace Noble
{
	OBJECT_DEF(TestPlayer)

	TestPlayer::TestPlayer()
		: m_MoveDir(0)
	{}

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
				m_MoveDir.x = state;
				break;
			case HASH("LookUp"):
				break;
			case HASH("LookRight"):
				break;
		}
	}
}