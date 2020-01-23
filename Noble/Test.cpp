#include <bgfx\bgfx.h>

#include "..\Source\Core\Renderer.h"
#include "..\Source\Core\Logger.h"
#include "..\Source\Core\Input.h"

namespace Noble
{
	namespace Input
	{
		/**
		 * Initializes raw input code and prepares buffers
		 * Definition in Input.cpp
		 */
		extern bool Initialize(HWND hwnd);
	}
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

int main_disabled()
{
	Noble::Window window;
	window.Create(true);

	Noble::Input::Initialize(window.GetWindowHandle());
	
	bgfx::Init initData;
	initData.type = bgfx::RendererType::Count;
	initData.resolution.width = window.GetClientAreaWidth();
	initData.resolution.height = window.GetClientAreaHeight();
	initData.resolution.reset = BGFX_RESET_VSYNC;
	initData.platformData.nwh = window.GetWindowHandle();

	bgfx::init(initData);

	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF);
	bgfx::setViewRect(0, 0, 0, initData.resolution.width, initData.resolution.height);

	MSG msg;
	while (true)
	{
		bool exit = HandleWindowsMessages(msg);

		Noble::Input::PrepFrame();

		if (Noble::Input::IsJustPressed(Noble::Input::KEY_SPACE))
		{
			// just pressed space = toggle style
			NE_LOG_DEBUG("Toggle---------------------------------");
			window.ResizeWindow(1600, 900);
			bgfx::reset(1600, 900);
			bgfx::setViewRect(0, 0, 0, 1600, 900);
		}

		if (!exit)
		{
			bgfx::touch(0);
			bgfx::frame();
		}
		else
		{
			break;
		}
	}

	bgfx::shutdown();

	window.Destroy();

	return 0;
}