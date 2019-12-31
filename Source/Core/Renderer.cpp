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


	Window::Window()
		: Window(1280, 720, false)
	{
		// Create default window, 1280x720 windowed
	}

	Window::Window(I32 width, I32 height, bool fullscreen)
	{
		m_WindowConfig.Width = width;
		m_WindowConfig.Height = height;
		m_WindowConfig.Fullscreen = fullscreen;

		m_WindowConfig.WindowWidth = width;
		m_WindowConfig.WindowHeight = height;
		m_WindowConfig.WindowX = CW_USEDEFAULT;
		m_WindowConfig.WindowY = 0;

		m_Window = 0;
#ifdef NE_OPENGL_WINDOW
		m_WindowDC = 0;
		m_GLContext = 0;
#endif // NE_OPENGL_WINDOW

		m_WindowClass = 0;
		m_WindowStyle = WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		m_BorderlessStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;

		m_CurrentStyle = fullscreen ? m_BorderlessStyle : m_WindowStyle;
	}

	bool Window::Create(bool fromConfig)
	{
		NE_LOG_INFO("Attempting to create game window");

		if (fromConfig)
		{
			NE_LOG_DEBUG("Loading window options from config");

			json& graphics = Config::GetSubConfig("Graphics");

			if (graphics.find("ResolutionWidth") == graphics.end())
			{
				NE_LOG_WARNING("No resolution data stored in config, using defaults");
			}
			else
			{
				m_WindowConfig.Width = graphics["ResolutionWidth"];
				m_WindowConfig.Height = graphics["ResolutionHeight"];
				m_WindowConfig.Fullscreen = graphics["Fullscreen"];

				AdjustSize();
				Center();
			}
		}

		bool result = false;
		HINSTANCE hInst = GetModuleHandle(NULL);

		m_WindowClass = MAKEINTATOM(RegisterClass_Noble(hInst));

		if (m_WindowClass == 0)
		{
			NE_LOG_FATAL("Failed to register window class");
			return false;
		}

		// Steps to create a full featured OpenGL context:
		// Create a fake window
		// Attach a "fake" OpenGL 1.2 context to it
		// Use said "fake" context to grab fn pointers to ARB context creators
		// Use ARB fn pointers to create real window and context

#ifdef NE_OPENGL_WINDOW

		// Step one: fake window
		HWND fakeWindow = CreateWindow(
			m_WindowClass, "ContextAcquisition", 
			m_WindowStyle, 
			0, 0, 
			1, 1, 
			NULL, NULL, 
			hInst, NULL);

		// Step two: fake OpenGL Context

		HDC fakeDC = GetDC(fakeWindow);

		PIXELFORMATDESCRIPTOR fakePixFormat;
		ZeroMemory(&fakePixFormat, sizeof(fakePixFormat));
		fakePixFormat.nSize = sizeof(fakePixFormat);
		fakePixFormat.nVersion = 1;
		fakePixFormat.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		fakePixFormat.iPixelType = PFD_TYPE_RGBA;
		fakePixFormat.cColorBits = 32;
		fakePixFormat.cAlphaBits = 8;
		fakePixFormat.cDepthBits = 24;

		const int fakePixFormat_ID = ChoosePixelFormat(fakeDC, &fakePixFormat);
		if (fakePixFormat_ID == 0)
		{
			NE_LOG_FATAL("Failed to acquire pixel format");
			return false;
		}

		result = SetPixelFormat(fakeDC, fakePixFormat_ID, &fakePixFormat);
		if (!result)
		{
			NE_LOG_FATAL("Failed to set pixel format");
			return false;
		}

		HGLRC fakeRC = wglCreateContext(fakeDC);
		if (fakeRC == 0)
		{
			NE_LOG_FATAL("Failed to create OpenGL Context");
			return false;
		}

		result = wglMakeCurrent(fakeDC, fakeRC);
		if (!result)
		{
			NE_LOG_FATAL("Failed to assign OpenGL Context to window");
			return false;
		}

		// Step 3: Grab ARB functions to make a real OpenGL Context

		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
		wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
		if (wglChoosePixelFormatARB == nullptr)
		{
			NE_LOG_FATAL("Failed to acquire OpenGL Extension: wglChoosePixelFormatARB");
			return false;
		}

		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
		wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
		if (wglCreateContextAttribsARB == nullptr)
		{
			NE_LOG_FATAL("Failed to acquire OpenGL Extension: wglCreateContextAttribsARB");
			return false;
		}

		// Step 4: Create the real window and context using acquired function pointers

#endif

		// Before creating the real window, adjust size and position
		AdjustSize();
		Center();

		m_Window = CreateWindow(
			m_WindowClass, "Noble Engine",
			m_WindowStyle,
			m_WindowConfig.WindowX, m_WindowConfig.WindowY,
			m_WindowConfig.WindowWidth, m_WindowConfig.WindowHeight,
			NULL, NULL,
			hInst, NULL);

#ifdef NE_OPENGL_WINDOW

		m_WindowDC = GetDC(m_Window);

		const int pixelAttribs[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_ALPHA_BITS_ARB, 8,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
			WGL_SAMPLES_ARB, 4,
			0
		};

		int pixFormatID;
		UINT numFormats;
		const bool status = wglChoosePixelFormatARB(m_WindowDC, pixelAttribs, NULL, 1, &pixFormatID, &numFormats);

		if (!status || numFormats == 0)
		{
			NE_LOG_FATAL("Failed to acquire ARB pixel format");
			return false;
		}

		PIXELFORMATDESCRIPTOR PFD;
		DescribePixelFormat(m_WindowDC, pixFormatID, sizeof(PFD), &PFD);
		SetPixelFormat(m_WindowDC, pixFormatID, &PFD);

		// Minimum version is OpenGL 4.0
		const int major_min = 4, minor_min = 0;
		const int contextAttribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
			WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		m_GLContext = wglCreateContextAttribsARB(m_WindowDC, 0, contextAttribs);
		if (m_GLContext == NULL)
		{
			NE_LOG_FATAL("Failed to create OpenGL Context");
			return false;
		}

		// Step 4: Kill the old window and bring this one forward

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(fakeRC);
		ReleaseDC(fakeWindow, fakeDC);
		DestroyWindow(fakeWindow);

		result = wglMakeCurrent(m_WindowDC, m_GLContext);
		if (!result)
		{
			NE_LOG_FATAL("Failed to assign OpenGL Context to window");
			return false;
		}

		// Load GLAD
		// Note that on failure, we can try GetProcAddress as well
		// Not sure why wgl behaves differently...
		//result = gladLoadGLLoader((GLADloadproc)wglGetProcAddress);
		result = gladLoadGL();
		if (!result)
		{
			NE_LOG_FATAL("Failed to acquire OpenGL extensions");
			return false;
		}

		//gladLoadGL();

		SetWindowText(m_Window, reinterpret_cast<LPCSTR>(glGetString(GL_VERSION)));
#endif

		ShowWindow(m_Window, SW_SHOW);

		return true;
	}

	void Window::ResizeWindow(I32 newWidth, I32 newHeight)
	{
		if (!m_Window)
		{
			NE_LOG_WARNING("Cannot resize window before creating it.");
			return;
		}

		m_WindowConfig.Width = newWidth;
		m_WindowConfig.Height = newHeight;

		AdjustSize();
		Center();

		MoveWindow(
			m_Window, 
			m_WindowConfig.WindowX, 
			m_WindowConfig.WindowY, 
			m_WindowConfig.WindowWidth, 
			m_WindowConfig.WindowHeight, 
			TRUE);
	}

	void Window::SetFullscreen(bool fullscreen)
	{
		if (!m_Window)
		{
			NE_LOG_WARNING("Cannot resize window before creating it.");
			return;
		}

		m_WindowConfig.Fullscreen = fullscreen;

		m_CurrentStyle = fullscreen ? m_BorderlessStyle : m_WindowStyle;
		SetWindowLongPtr(m_Window, GWL_STYLE, m_CurrentStyle);

		ResizeWindow(m_WindowConfig.Width, m_WindowConfig.Height);
	}

	void Window::Destroy()
	{
#ifdef NE_OPENGL_WINDOW
		wglMakeCurrent(NULL, NULL);
		if (m_GLContext)
		{
			wglDeleteContext(m_GLContext);
		}
		if (m_WindowDC)
		{
			ReleaseDC(m_Window, m_WindowDC);
		}
#endif
		if (m_Window)
		{
			DestroyWindow(m_Window);
		}
	}

	void Window::TestClearAndFlip()
	{
#ifdef NE_OPENGL_WINDOW
		glClearColor(0.0F, 0.0F, 0.2F, 1.0F);
		glClear(GL_COLOR_BUFFER_BIT);

		SwapBuffers(m_WindowDC);
#endif
	}

	void Window::AdjustSize()
	{
		m_CurrentStyle = m_WindowConfig.Fullscreen ? m_BorderlessStyle : m_WindowStyle;

		RECT rect = { 0, 0, m_WindowConfig.Width, m_WindowConfig.Height };
		AdjustWindowRect(&rect, m_CurrentStyle, false);
		NE_LOG_DEBUG("After adjustment, size: " << (rect.right - rect.left) << " x " << (rect.bottom - rect.top));

		m_WindowConfig.WindowWidth = rect.right - rect.left;
		m_WindowConfig.WindowHeight = rect.bottom - rect.top;
	}

	void Window::Center()
	{
		RECT displaySize;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &displaySize, 0);
		m_WindowConfig.WindowX = (displaySize.right - m_WindowConfig.WindowWidth) / 2;
		m_WindowConfig.WindowY = (displaySize.bottom - m_WindowConfig.WindowHeight) / 2;
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
		NE_LOG_DEBUG("After adjustment, size: " << (rect.right - rect.left) << " x " << (rect.bottom - rect.top));

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