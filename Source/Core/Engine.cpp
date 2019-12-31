#include "Engine.h"

#include <json/json.hpp>

#include "Config.h"
#include "GameInput.h"
#include "Globals.h"
#include "Logger.h"
#include "Renderer.h"
#include "WindowsMinimal.h"

// for testing
#include "StaticMeshComponent.h"

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

	namespace
	{
		StaticMeshComponent g_TestComponent;
	}

	int LaunchEngine()
	{
		bool result = false;
		Engine eng;

		// Turn the key...
		result = eng.Start();
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

	}

	bool Engine::Start()
	{
		bool result = false;

		if (g_Engine)
		{
			// Engine already initialized
			return false;
		}

		// Set global engine pointer
		g_Engine = this;

		// Initialize the logger
		Logger::InitLog();
		NE_LOG_INFO("Noble Engine " << NOBLE_VERSION << " - " << __DATE__);

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

		// Testing stuff
		StaticMeshComponent::TemporaryInit();

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

		Clock::UpdateNow();
		m_LoopClock.Mark();

		while (!finish)
		{
			Clock::UpdateNow();
			U64 time = m_LoopClock.GetMicrosecondsSinceMark();
			m_LoopClock.Mark();

			F32 seconds = (F32)time / 1000000.0F;
			F32 fps = 1.0F / seconds;

			accumulator += seconds;

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

				Update(seconds);

				// finish = Game::ShouldGameQuit();

				m_Renderer.Frame();
			}
		}
	}

	bool Engine::Stop()
	{
		NE_LOG_INFO("Engine shutting down");

		StaticMeshComponent::TemporaryDestroy();

		// Save config changes
		Config::SaveConfig();

		// Close the window down
		m_Renderer.Shutdown();

		// Print and stop the logger
		Logger::PrintLog();
		Logger::CloseLog();

		return true;
	}

	void Engine::FixedUpdate()
	{

	}

	void Engine::Update(float tpf)
	{
		Sleep(1); // short sleep to keep the loop from running super fast during testing

		static float test = 0.0F;
		test += tpf;

		float x = glm::sin(test);
		float y = glm::cos(test);
		//float x = 1.0F, y = 1.0F;
		float sinTest = glm::sin(test);

		{
			Matrix4x4f view = glm::lookAt(Vector3f(x * 15.0F, 15, y * 15.0F), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
			Matrix4x4f proj = glm::perspectiveFov(70.0F, 1280.0F, 720.0F, 0.1F, 100.0F);

			bgfx::setViewTransform(0, &view[0][0], &proj[0][0]);
		}

		g_TestComponent.SetPosition(Vector3f(5, 5, 5));
		g_TestComponent.TestDraw();
		g_TestComponent.GetLocalTransform().Position = Vector3f(5, 0, 5);
		g_TestComponent.TestDraw();
		g_TestComponent.GetLocalTransform().Position = Vector3f(0, 0, 5);
		g_TestComponent.TestDraw();
		g_TestComponent.GetLocalTransform().Position = Vector3f(5, 0, 0);
		g_TestComponent.TestDraw();
		g_TestComponent.GetLocalTransform().Position = Vector3f(0, 0, 0);
		g_TestComponent.GetLocalTransform().Scale = Vector3f(1.0F + sinTest);
		g_TestComponent.TestDraw();
		g_TestComponent.GetLocalTransform().Scale = Vector3f(1, 1, 1);
	}
}

int CALLBACK WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInst, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	return Noble::LaunchEngine();
}