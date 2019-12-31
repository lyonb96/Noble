#pragma once

#include "Types.h"
#include "WindowsMinimal.h"

namespace Noble
{
	/**
	 * Encapsulates functions related to creating, resizing, and destroying the game window
	 */
	class Window
	{
	public:

		/**
		 * Builds a window with the default size of 1280x720 Windowed
		 */
		Window();

		/**
		 * Builds a window with the given configuration
		 */
		Window(I32 width, I32 height, bool windowed);

		/**
		 * Resizes the window to the given values
		 * Note that the given values will be for the client area.
		 * The full window's size will thus be larger than the given value
		 */
		void ResizeWindow(I32 newWidth, I32 newHeight);

		/**
		 * Changes the fullscreen state for the window
		 * If true, the window will cover the entire screen (borderless fullscreen)
		 * If false, the window will have borders and will occupy the original width/height requested
		 */
		void SetFullscreen(bool fullscreen);

		/**
		 * Creates the window and a corresponding OpenGL 4.0 context
		 * if fromConfig is true, it will load width and height from config
		 */
		bool Create(bool fromConfig);

		/**
		 * Destroys the window and its OpenGL context
		 */
		void Destroy();

		/**
		 * GL test
		 */
		void TestClearAndFlip();

		/**
		 * Returns the handle for the game window
		 * No guarantee that the handle is valid!
		 */
		inline HWND GetWindowHandle() const
		{
			return m_Window;
		}

		/**
		 * Returns the width of the client area of the window
		 */
		I32 GetClientAreaWidth() const { return m_WindowConfig.Width; }

		/**
		 * Returns the height of the client area of the window
		 */
		I32 GetClientAreaHeight() const { return m_WindowConfig.Height; }

	private:

		/**
		 * Called internally to adjust the size of the window
		 */
		void AdjustSize();

		/**
		 * Called internally to center the window on the screen
		 */
		void Center();

	private:

		/**
		 * Struct describing the configuration of the game window
		 */
		struct WindowConfig
		{
			// Client area size
			I32 Width;
			I32 Height;

			// Fullscreen state
			bool Fullscreen;

			// Separate values for "actual" width and height of window (including borders)
			I32 WindowWidth;
			I32 WindowHeight;

			// Window coordinates on screen
			I32 WindowX;
			I32 WindowY;
		} m_WindowConfig;

		// Windows 10 window setup
#ifdef NOBLE_WINDOWS
		LPSTR m_WindowClass;
		DWORD m_WindowStyle;
		DWORD m_BorderlessStyle;
		DWORD m_CurrentStyle;
		HWND m_Window;
#endif
	};

	/**
	 * Encapsulates render commands
	 */
	class Renderer
	{
	public:

		// Copy constructors deleted
		Renderer(const Renderer&) = delete;
		Renderer(const Renderer&&) = delete;

		// Default constructor to init members
		Renderer();

		/**
		 * Creates the game window and initializes the renderer
		 */
		bool Initialize();

		/**
		 * Shuts down the renderer and destroys the game window
		 */
		void Shutdown();
		
		/**
		 * Placeholder for advancing the frame
		 */
		void Frame();

		/**
		 * Returns the handle for the game's window
		 */
		HWND GetWindowHandle() const { return m_WindowHandle; }

	private:

		/**
		 * Creates the game's window with the stored width and height
		 */
		bool CreateGameWindow();

		/**
		 * Adjusts the window's size and centers it
		 */
		void AdjustAndCenterWindow();

	private:

		HWND m_WindowHandle;
		LPSTR m_WindowClass;
		DWORD m_CurrentStyle;

		I32 m_RenderWidth;
		I32 m_RenderHeight;
		I32 m_WindowWidth;
		I32 m_WindowHeight;
		I32 m_WindowX;
		I32 m_WindowY;
	};
}