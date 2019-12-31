#pragma once

#define NOBLE_VERSION "Early Dev"

#include "Time.h"
#include "Renderer.h"

namespace Noble
{
	/**
	 * Called by main to begin running the game
	 */
	int LaunchEngine();

	/**
	 * Handles startup and shutdown of engine subsystems, and hosts the main loop
	 */
	class Engine
	{
	public:

		/**
		 * Default constructor, just inits some variables
		 */
		Engine();

		/**
		 * Starts up modules and prepares the engine for running
		 */
		bool Start();

		/**
		 * Runs the main game loop
		 * This is where the magic happens
		 */
		void RunLoop();

		/**
		 * Shuts down modules and kills the engine
		 */
		bool Stop();

	protected:

		/**
		 * Runs 60 times per second to handle fixed rate logic
		 */
		void FixedUpdate();

		/**
		 * Runs every render frame
		 */
		void Update(float tpf);

	private:

		// Clock for timing the main loop
		Clock m_LoopClock;
		Renderer m_Renderer;
	};
}