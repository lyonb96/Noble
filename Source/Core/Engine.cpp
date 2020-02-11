#include "Engine.h"

#include <json/json.hpp>

#include <fstream>

#include "Config.h"
#include "GameInput.h"
#include "Globals.h"
#include "Logger.h"
#include "Renderer.h"
#include "WindowsMinimal.h"

// for testing
#include "Array.h"
#include "FileSystem.h"
#include "BitStream.h"

#ifndef MAX_FIXED_STEPS_PER_FRAME
#define MAX_FIXED_STEPS_PER_FRAME 5 // limit fixed steps per frame to avoid spiraling
#endif

#ifndef FIXED_STEP_RATE
#define FIXED_STEP_RATE (1.0F / 60.0F)
#endif

namespace Noble
{
	using json = nlohmann::json;

	namespace Input
	{
		/**
		 * Initializes raw input code and prepares buffers
		 * Definition in Input.cpp
		 */
		extern bool Initialize(HWND hwnd);
	}

	int LaunchEngine(GameInstance* inst)
	{
		bool result = false;
		Engine eng;

		// Turn the key...
		result = eng.Start(inst);
		if (!result)
		{
			// If it fails, shut down any modules that did start
			eng.Stop();
			return -1;
		}

		// This function is where entire game sessions happen...
		eng.RunLoop();

		// Once the loop terminates, shut down the engine
		result = eng.Stop();
		if (!result)
		{
			return -1;
		}

		return 0;
	}

	Engine::Engine()
	{
		Time::Initialize();
		m_GameInstance = nullptr;
	}

	bool Engine::Start(GameInstance* inst)
	{
		bool result = false;

		if (g_Engine)
		{
			// Engine already initialized
			return false;
		}

		// Store the game instance pointer
		m_GameInstance = inst;

		// Set global engine pointer
		g_Engine = this;
		g_World = &m_World;
		g_AssetManager = &m_AssetManager;

		// Initialize the logger
		NE_LOG_INFO("Noble Engine %s - %s", NOBLE_VERSION, __DATE__);

		// Load the config file for other subsystems to use
		Config::LoadConfig();

		// Create the game window and start the renderer
		result = m_Renderer.Initialize();
		if (!result)
		{
			NE_LOG_FATAL("Renderer failed to initialize");
			m_Renderer.Shutdown();
			return false;
		}

		// Initialize the input subsystem and load the saved bindings from the config
		result = Input::Initialize(m_Renderer.GetWindowHandle());
		if (!result)
		{
			NE_LOG_FATAL("Failed to initialize input system");
			return false;
		}
		Input::LoadBindings();

		// Init statics
		StaticVertex::Init();

		// Run game instance startup code
		m_GameInstance->OnGameStart();

		return true;
	}

	bool HandleWindowsMessages(MSG& msg)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				return true;
			}
		}

		return false;
	}

	void Engine::RunLoop()
	{
		bool finish = false;
		MSG msg;
		float accumulator = 0.0F;
		Timestamp last = Time::GetNowTimestamp();

		Sleep(1);

		while (!finish)
		{

			Timestamp now = Time::GetNowTimestamp();
			Time::SetLoopTime(now - last);
			last = now;

			accumulator += Time::GetDeltaTime();

			finish = HandleWindowsMessages(msg);
			if (!finish)
			{
				Input::PrepFrame();

				// Count fixed steps to circumvent spiraling from hard freezes
				U8 fixedSteps = 0;

				while (accumulator > FIXED_STEP_RATE && 
					++fixedSteps < MAX_FIXED_STEPS_PER_FRAME)
				{
					FixedUpdate();
					accumulator -= FIXED_STEP_RATE;
				}

				Update();

				// finish = Game::ShouldGameQuit();

				m_Renderer.Frame();
			}
		}
	}

	bool Engine::Stop()
	{
		// Notify game instance
		m_GameInstance->OnGameEnd();

		// Log shutdown
		NE_LOG_INFO("Engine shutting down");

		// Unload assets
		m_AssetManager.UnloadAllAssets();

		// Save config changes
		Config::SaveConfig();

		// Close the window down
		m_Renderer.Shutdown();

		// Print the current log
		Logger::PrintLog("LogFile.txt");

		return true;
	}

	void Engine::FixedUpdate()
	{
		// Call the game instance fixed update function
		m_GameInstance->FixedUpdate();
		// Then perform world fixed updates
		m_World.FixedUpdate();
	}

	void Engine::Update()
	{
		// Call the Game Instance update function
		m_GameInstance->Update();
		// Then perform world updates
		m_World.Update();
	}
}