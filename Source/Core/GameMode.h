#pragma once

namespace Noble
{

	/**
	 * Game Modes describe the rules of a specific game or mode of a game
	 * They govern spawning players and other agents, and can also command
	 * the game to change to another Game Mode / Map
	 */
	class GameMode
	{
	public:

		/**
		 * Called when this Game Mode is set as the active mode
		 */
		virtual void OnGameModeStart() {}

		/**
		 * Called each frame
		 */
		virtual void Update() {}

		/**
		 * Called on each fixed update tick
		 */
		virtual void FixedUpdate() {}
	};
}