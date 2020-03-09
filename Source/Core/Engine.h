#pragma once

#define NOBLE_VERSION "Early Dev"

#include "AssetManager.h"
#include "Time.h"
#include "Renderer.h"
#include "World.h"
#include "GameInstance.h"
#include "CameraManager.h"
#include "PhysicsEngine.h"

namespace Noble
{
	/**
	 * Called by main to begin running the game
	 */
	int LaunchEngine(GameInstance* inst);

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
		bool Start(GameInstance* inst);

		/**
		 * Runs the main game loop
		 * This is where the magic happens
		 */
		void RunLoop();

		/**
		 * Shuts down modules and kills the engine
		 */
		bool Stop();

		/**
		 * Returns a pointer to the game's Camera Manager
		 */
		CameraManager* GetCameraManager() { return &m_CamManager; };

	protected:

		/**
		 * Runs 60 times per second to handle fixed rate logic
		 */
		void FixedUpdate();

		/**
		 * Runs every render frame
		 */
		void Update();

	private:

		// Game Instance
		GameInstance* m_GameInstance;
		// Handles drawing the scene
		Renderer m_Renderer;
		// Handles the gameplay side of the engine
		World m_World;
		// Handles asset loading and unloading
		AssetManager m_AssetManager;
		// Camera manager instance
		CameraManager m_CamManager;
		// Physics handler
		PhysicsEngine m_Physics;

	};
}

#define DEFINE_GAME_INSTANCE(GameClass, GameName) \
int CALLBACK WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInst, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) \
{ \
	GameClass gameInst(GameName); \
	return Noble::LaunchEngine(&gameInst); \
}
