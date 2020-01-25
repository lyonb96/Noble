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
#include "StaticMeshComponent.h"
#include "World.h"
#include "TestGameObject.h"
#include "Array.h"
#include "Freelist.h"
#include "FileSystem.h"

#ifndef MAX_FIXED_STEPS_PER_FRAME
#define MAX_FIXED_STEPS_PER_FRAME 5 // limit fixed steps per frame to avoid spiraling
#endif

#ifndef FIXED_STEP_RATE
#define FIXED_STEP_RATE (1.0F / 60.0F)
#endif

struct TestStruct1
{
	char Data[128];
};

struct TestStruct2
{
	char Data[96];
};

struct TestStruct3
{
	char Data[144];
};

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
		World g_TestWorld;
		TestGameObject* g_TestObject1;
		TestGameObject* g_TestObject2;
		TestGameObject* g_TestObject3;
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
		Time::Initialize();
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

		// Testing stuff
		StaticMeshComponent::TemporaryInit();

		g_TestObject1 = g_TestWorld.CreateAndSpawnGameObject<TestGameObject>();
		g_TestObject2 = g_TestWorld.CreateAndSpawnGameObject<TestGameObject>();
		//g_TestObject3 = g_TestWorld.CreateAndSpawnGameObject<TestGameObject>();

		void* testData = Memory::Malloc(sizeof(TestGameObject), alignof(TestGameObject));
		g_TestObject3 = (TestGameObject*)Object::CreateInstance(SID("TestGameObject"), testData);
		
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

				Update(Time::GetDeltaTime());

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

		// Print the current log
		Logger::PrintLog("LogFile.txt");

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

		g_TestObject1->SetPosition(Vector3f(5, 5, 5));
		g_TestObject2->SetScale(Vector3f(2, 2, 2));
		g_TestObject3->SetRotation(glm::angleAxis(x, Vector3f(1, 0, 0)));
		g_TestObject3->SetPosition(Vector3f(5, y * 5.0F, 0));

		StaticMeshComponent* sm1 = (StaticMeshComponent*) g_TestObject1->GetRootComponent();
		StaticMeshComponent* sm2 = (StaticMeshComponent*) g_TestObject2->GetRootComponent();
		StaticMeshComponent* sm3 = (StaticMeshComponent*) g_TestObject3->GetRootComponent();

		sm1->TestDraw();
		sm2->TestDraw();
		sm3->TestDraw();

		g_TestObject1->GetSecondMesh()->TestDraw();
		g_TestObject2->GetSecondMesh()->TestDraw();
		//g_TestObject3->GetSecondMesh()->TestDraw();

		StaticMeshComponent* sm4 = g_TestObject3->GetComponent<StaticMeshComponent>("Submesh");
		if (sm4)
		{
			sm4->TestDraw();
		}
	}
}

int CALLBACK WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInst, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	return Noble::LaunchEngine();
}