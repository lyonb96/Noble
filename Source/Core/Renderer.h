#pragma once

#include "CameraManager.h"
#include "Types.h"
#include "WindowsMinimal.h"

namespace Noble
{

	/**
	 * Encapsulates render commands
	 */
	class Renderer
	{
	public:

		// Copy/Move deleted
		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) = delete;

		// Default constructor to init members
		Renderer();

		/**
		 * Creates the game window and initializes the renderer
		 */
		bool Initialize(const char* windowName);

		/**
		 * Shuts down the renderer and destroys the game window
		 */
		void Shutdown();
		
		/**
		 * Placeholder for advancing the frame
		 */
		void Frame();

	public:

		/**
		 * Returns the handle for the game's window
		 */
		HWND GetWindowHandle() const { return m_WindowHandle; }

	private:

		/**
		 * Creates the game's window with the stored width and height
		 */
		bool CreateGameWindow(const char* windowName);

		/**
		 * Adjusts the window's size and centers it
		 */
		void AdjustAndCenterWindow();

	private:

		// Window handle
		HWND m_WindowHandle;
		// Window class
		LPSTR m_WindowClass;
		// Window style
		DWORD m_CurrentStyle;

		// Width of the rendered frame
		I32 m_RenderWidth;
		// Height of the rendered frame
		I32 m_RenderHeight;
		// Width of the game window
		I32 m_WindowWidth;
		// Height of the game window
		I32 m_WindowHeight;
		// Window X position
		I32 m_WindowX;
		// Window Y position
		I32 m_WindowY;
		// Handles camera positioning for the main game scene
		CameraManager* m_CamMgr;

	};
}