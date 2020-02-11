#pragma once

#include "String.h"

namespace Noble
{

	class GameInstance
	{
	public:

		/**
		 * Creates a new Game Instance
		 */
		GameInstance(const char* gameName);

		/**
		 * No move or copy
		 */
		GameInstance(const GameInstance&) = delete;
		GameInstance(GameInstance&&) = delete;
		GameInstance& operator=(const GameInstance&) = delete;
		GameInstance& operator=(GameInstance&&) = delete;

		/**
		 * Returns the name of the Game Instance
		 */
		const NString& GetGameName() const { return m_GameName; }

	public:

		/**
		 * Called after all subsystems have been initialized, but
		 * just before the main loop starts
		 */
		virtual void OnGameStart() = 0;

		/**
		 * Called after the main loop has terminated but before
		 * subsystem shutdown
		 */
		virtual void OnGameEnd() = 0;

		/**
		 * Called on each rendered frame for logic updates
		 */
		virtual void Update() {}

		/**
		 * Called at each fixed-step for logic that requires a fixed step
		 */
		virtual void FixedUpdate() {}

	private:

		// Game instance name
		NString m_GameName;
	};
}