#include "Renderer.h"

#include "Config.h"
// Needed to pass input messages
#include "Input.h"
#include "Logger.h"

#include <bgfx/bgfx.h>
#include <json/json.hpp>

#define NE_STYLE_WINDOWED (WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define NE_STYLE_BORDERLESS (WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP)

namespace Noble
{
	using json = nlohmann::json;

	namespace Input
	{
		// functions defined in Input.cpp
		extern void OnCharInput(WPARAM wp);
		extern void OnRawInput(WPARAM wp, LPARAM lp);
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
	{
		switch (msg)
		{
			case WM_CLOSE:
				PostQuitMessage(0);
				return 0;

			case WM_CHAR:
				Input::OnCharInput(wp);
				return DefWindowProc(hWnd, msg, wp, lp);
			case WM_INPUT:
				Input::OnRawInput(wp, lp);
				return DefWindowProc(hWnd, msg, wp, lp);

			default:
				return DefWindowProc(hWnd, msg, wp, lp);
		}
	}

	ATOM RegisterClass_Noble(HINSTANCE hInst)
	{
		WNDCLASSEX wcex;
		ZeroMemory(&wcex, sizeof(wcex));
		wcex.cbSize = sizeof(wcex);
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wcex.lpfnWndProc = WndProc;
		wcex.hInstance = hInst;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = "NobleWindow";

		return RegisterClassEx(&wcex);
	}

	// -----------------------------------

	Renderer::Renderer()
	{
		m_WindowHandle = 0;
		m_WindowClass = 0;
		m_CurrentStyle = 0;

		m_RenderWidth = 1280;
		m_RenderHeight = 720;
		m_WindowWidth = 0;
		m_WindowHeight = 0;
		m_WindowX = 0;
		m_WindowY = 0;
	}

	void Renderer::AdjustAndCenterWindow()
	{
		// Adjust the WindowWidth and Height based on style
		RECT rect = { 0, 0, m_RenderWidth, m_RenderHeight };
		AdjustWindowRect(&rect, m_CurrentStyle, false);
		NE_LOG_DEBUG("After adjustment, size: %i x %i", (rect.right - rect.left), (rect.bottom - rect.top));

		m_WindowWidth = rect.right - rect.left;
		m_WindowHeight = rect.bottom - rect.top;

		// Then center the window's X and Y coordinates
		RECT displaySize;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &displaySize, 0);
		m_WindowX = (displaySize.right - m_WindowWidth) / 2;
		m_WindowY = (displaySize.bottom - m_WindowHeight) / 2;
	}

	bool Renderer::CreateGameWindow()
	{
		bool result = false;
		HINSTANCE hInst = GetModuleHandle(NULL);

		m_WindowClass = MAKEINTATOM(RegisterClass_Noble(hInst));

		if (m_WindowClass == 0)
		{
			NE_LOG_FATAL("Failed to register window class");
			return false;
		}

		m_CurrentStyle = NE_STYLE_WINDOWED;
		AdjustAndCenterWindow();

		m_WindowHandle = CreateWindow(
			m_WindowClass, "Noble Engine",
			m_CurrentStyle,
			m_WindowX, m_WindowY,
			m_WindowWidth, m_WindowHeight,
			NULL, NULL,
			hInst, NULL);

		ShowWindow(m_WindowHandle, SW_SHOW);

		return true;
	}

	bool Renderer::Initialize()
	{
		NE_LOG_DEBUG("Creating game window");

		bool result = true;

		// Retrieve resolution settings from config
		json& graphics = Config::GetSubConfig("Graphics");

		if (graphics.find("ResolutionWidth") == graphics.end())
		{
			NE_LOG_WARNING("No resolution data stored in config, using defaults");
			m_RenderWidth = 1280;
			m_RenderHeight = 720;
		}
		else
		{
			m_RenderWidth = graphics["ResolutionWidth"];
			m_RenderHeight = graphics["ResolutionHeight"];
		}

		// Create window
		result = CreateGameWindow();
		if (!result)
		{
			return false;
		}

		bgfx::Init initData;
		initData.type = bgfx::RendererType::Count;
		initData.resolution.width = m_RenderWidth;
		initData.resolution.height = m_RenderHeight;
		initData.resolution.reset = BGFX_RESET_VSYNC;
		initData.platformData.nwh = m_WindowHandle;

		result = bgfx::init(initData);
		if (!result)
		{
			return false;
		}

		bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x222222FF);
		bgfx::setViewRect(0, 0, 0, initData.resolution.width, initData.resolution.height);

		return true;
	}

	void Renderer::Frame()
	{
		bgfx::touch(0);
		bgfx::frame();
	}

	void Renderer::Shutdown()
	{
		bgfx::shutdown();

		if (m_WindowHandle)
		{
			DestroyWindow(m_WindowHandle);
		}
	}
}