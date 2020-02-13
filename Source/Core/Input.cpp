#include "Input.h"

#include "Logger.h"
#include "String.h"
#include "WindowsMinimal.h"

#define STRINGIZE(x) #x

namespace Noble
{
	const NIdentifier ActionStrings[] =
	{
		ID("KEY_A"),
		ID("KEY_B"),
		ID("KEY_C"),
		ID("KEY_D"),
		ID("KEY_E"),
		ID("KEY_F"),
		ID("KEY_G"),
		ID("KEY_H"),
		ID("KEY_I"),
		ID("KEY_J"),
		ID("KEY_K"),
		ID("KEY_L"),
		ID("KEY_M"),
		ID("KEY_N"),
		ID("KEY_O"),
		ID("KEY_P"),
		ID("KEY_Q"),
		ID("KEY_R"),
		ID("KEY_S"),
		ID("KEY_T"),
		ID("KEY_U"),
		ID("KEY_V"),
		ID("KEY_W"),
		ID("KEY_X"),
		ID("KEY_Y"),
		ID("KEY_Z"),
		ID("KEY_SPACE"),
		ID("KEY_TILDE"),
		ID("KEY_1"),
		ID("KEY_2"),
		ID("KEY_3"),
		ID("KEY_4"),
		ID("KEY_5"),
		ID("KEY_6"),
		ID("KEY_7"),
		ID("KEY_8"),
		ID("KEY_9"),
		ID("KEY_0"),
		ID("KEY_MINUS"),
		ID("KEY_EQUALS"),
		ID("KEY_BACKSPACE"),
		ID("KEY_TAB"),
		ID("KEY_LBRACKET"),
		ID("KEY_RBRACKET"),
		ID("KEY_BACKSLASH"),
		ID("KEY_SEMICOLON"),
		ID("KEY_APOSTROPHE"),
		ID("KEY_COMMA"),
		ID("KEY_PERIOD"),
		ID("KEY_SLASH"),
		ID("KEY_NUMPAD_0"),
		ID("KEY_NUMPAD_1"),
		ID("KEY_NUMPAD_2"),
		ID("KEY_NUMPAD_3"),
		ID("KEY_NUMPAD_4"),
		ID("KEY_NUMPAD_5"),
		ID("KEY_NUMPAD_6"),
		ID("KEY_NUMPAD_7"),
		ID("KEY_NUMPAD_8"),
		ID("KEY_NUMPAD_9"),
		ID("KEY_NUMPAD_ENTER"),
		ID("KEY_NUMPAD_PLUS"),
		ID("KEY_NUMPAD_MINUS"),
		ID("KEY_NUMPAD_DIV"),
		ID("KEY_NUMPAD_MULT"),
		ID("KEY_NUMPAD_DEC"),
		ID("KEY_UP"),
		ID("KEY_DOWN"),
		ID("KEY_LEFT"),
		ID("KEY_RIGHT"),
		ID("KEY_ESC"),
		ID("KEY_F1"),
		ID("KEY_F2"),
		ID("KEY_F3"),
		ID("KEY_F4"),
		ID("KEY_F5"),
		ID("KEY_F6"),
		ID("KEY_F7"),
		ID("KEY_F8"),
		ID("KEY_F9"),
		ID("KEY_F10"),
		ID("KEY_F11"),
		ID("KEY_F12"),
		ID("KEY_CAPSLOCK"),
		ID("KEY_LSHIFT"),
		ID("KEY_RETURN"),
		ID("KEY_RSHIFT"),
		ID("KEY_LCTRL"),
		ID("KEY_LALT"),
		ID("KEY_RALT"),
		ID("KEY_RCTRL"),
		ID("KEY_INSERT"),
		ID("KEY_DELETE"),
		ID("KEY_HOME"),
		ID("KEY_END"),
		ID("KEY_PAGEUP"),
		ID("KEY_PAGEDOWN"),
		ID("MOUSE_LEFT_BUTTON"),
		ID("MOUSE_RIGHT_BUTTON"),
		ID("MOUSE_MIDDLE_BUTTON"),
		ID("GAMEPAD_A"),
		ID("GAMEPAD_B"),
		ID("GAMEPAD_Y"),
		ID("GAMEPAD_X"),
		ID("GAMEPAD_DPAD_UP"),
		ID("GAMEPAD_DPAD_DOWN"),
		ID("GAMEPAD_DPAD_LEFT"),
		ID("GAMEPAD_DPAD_RIGHT"),
		ID("GAMEPAD_LEFT_STICK_BUTTON"),
		ID("GAMEPAD_RIGHT_STICK_BUTTON"),
		ID("GAMEPAD_START"),
		ID("GAMEPAD_LEFT_BUMPER"),
		ID("GAMEPAD_RIGHT_BUMPER"),
		ID("ACTION_UNKNOWN"),
		ID("ACTION_UNASSIGNED"),
		ID("ACTION_COUNT")
	};

	const NIdentifier AxisStrings[] =
	{
		ID("MOUSE_AXIS_X"),
		ID("MOUSE_AXIS_Y"),
		ID("MOUSE_AXIS_WHEEL"),
		
		ID("GAMEPAD_LEFT_STICK_AXIS_X"),
		ID("GAMEPAD_LEFT_STICK_AXIS_Y"),
		ID("GAMEPAD_RIGHT_STICK_AXIS_X"),
		ID("GAMEPAD_RIGHT_STICK_AXIS_Y"),
		ID("GAMEPAD_LEFT_TRIGGER"),
		ID("GAMEPAD_RIGHT_TRIGGER"),
		ID("AXIS_UNKNOWN"),
		ID("AXIS_UNASSIGNED"),
		ID("AXIS_COUNT")
	};

	namespace Input
	{
		namespace
		{
			char g_LastTextKey = 0;

			bool g_ActionStates[2][Action::ACTION_COUNT];
			float g_AxisStates[Axis::AXIS_COUNT];

			bool g_ActionBuffer[Action::ACTION_COUNT];
		}

		Action GetAction(const NIdentifier& id)
		{
			U16 ind = 0;
			while (ind < Action::ACTION_COUNT)
			{
				if (ActionStrings[ind] == id)
				{
					return (Action) ind;
				}
				++ind;
			}

			return Action::ACTION_UNKNOWN;
		}

		Action GetAction(const U32 id)
		{
			U16 ind = 0;
			while (ind < Action::ACTION_COUNT)
			{
				if (ActionStrings[ind].GetHash() == id)
				{
					return (Action)ind;
				}
				++ind;
			}

			return Action::ACTION_UNKNOWN;
		}

		Axis GetAxis(const NIdentifier& id)
		{
			U16 ind = 0;
			while (ind < Axis::AXIS_COUNT)
			{
				
				if (AxisStrings[ind] == id)
				{
					return (Axis) ind;
				}
				++ind;
			}

			return Axis::AXIS_UNKNOWN;
		}

		Axis GetAxis(const U32 id)
		{
			U16 ind = 0;
			while (ind < Axis::AXIS_COUNT)
			{

				if (AxisStrings[ind].GetHash() == id)
				{
					return (Axis)ind;
				}
				++ind;
			}

			return Axis::AXIS_UNKNOWN;
		}

		const NIdentifier& GetActionString(Action val)
		{
			return ActionStrings[(U16)val];
		}

		const NIdentifier& GetAxisString(Axis val)
		{
			return AxisStrings[(U16)val];
		}

		// Handles mapping a VKey to an Action
		Action MapKey(USHORT vkey)
		{
			switch (vkey)
			{
				// Letter keys
				case 0x41: return KEY_A;
				case 0x42: return KEY_B;
				case 0x43: return KEY_C;
				case 0x44: return KEY_D;
				case 0x45: return KEY_E;
				case 0x46: return KEY_F;
				case 0x47: return KEY_G;
				case 0x48: return KEY_H;
				case 0x49: return KEY_I;
				case 0x4A: return KEY_J;
				case 0x4B: return KEY_K;
				case 0x4C: return KEY_L;
				case 0x4D: return KEY_M;
				case 0x4E: return KEY_N;
				case 0x4F: return KEY_O;
				case 0x50: return KEY_P;
				case 0x51: return KEY_Q;
				case 0x52: return KEY_R;
				case 0x53: return KEY_S;
				case 0x54: return KEY_T;
				case 0x55: return KEY_U;
				case 0x56: return KEY_V;
				case 0x57: return KEY_W;
				case 0x58: return KEY_X;
				case 0x59: return KEY_Y;
				case 0x5A: return KEY_Z;
					// Number row keys
				case 0x30: return KEY_0;
				case 0x31: return KEY_1;
				case 0x32: return KEY_2;
				case 0x33: return KEY_3;
				case 0x34: return KEY_4;
				case 0x35: return KEY_5;
				case 0x36: return KEY_6;
				case 0x37: return KEY_7;
				case 0x38: return KEY_8;
				case 0x39: return KEY_9;
					// Arrow keys
				case VK_UP:    return KEY_UP;
				case VK_DOWN:  return KEY_DOWN;
				case VK_RIGHT: return KEY_RIGHT;
				case VK_LEFT:  return KEY_LEFT;
					// Shift
				case VK_LSHIFT: return KEY_LSHIFT;
				case VK_RSHIFT: return KEY_RSHIFT;
					// Navigator keys
				case VK_HOME:   return KEY_HOME;
				case VK_END:    return KEY_END;
				case VK_INSERT: return KEY_INSERT;
				case VK_DELETE: return KEY_DELETE;
				case VK_PRIOR:  return KEY_PAGEUP;
				case VK_NEXT:   return KEY_PAGEDOWN;
					// Utility keys
				case VK_SPACE:  return KEY_SPACE;
				case VK_ESCAPE: return KEY_ESC;
				case VK_F1:     return KEY_F1;
				case VK_F2:     return KEY_F2;
				case VK_F3:     return KEY_F3;
				case VK_F4:     return KEY_F4;
				case VK_F5:     return KEY_F5;
				case VK_F6:     return KEY_F6;
				case VK_F7:     return KEY_F7;
				case VK_F8:     return KEY_F8;
				case VK_F9:     return KEY_F9;
				case VK_F10:    return KEY_F10;
				case VK_F11:    return KEY_F11;
				case VK_F12:    return KEY_F12;
				case VK_TAB:    return KEY_TAB;
				case VK_RETURN: return KEY_RETURN;
				case VK_BACK:   return KEY_BACKSPACE;
					// Numpad keys
				case VK_NUMPAD0:  return KEY_NUMPAD_0;
				case VK_NUMPAD1:  return KEY_NUMPAD_1;
				case VK_NUMPAD2:  return KEY_NUMPAD_2;
				case VK_NUMPAD3:  return KEY_NUMPAD_3;
				case VK_NUMPAD4:  return KEY_NUMPAD_4;
				case VK_NUMPAD5:  return KEY_NUMPAD_5;
				case VK_NUMPAD6:  return KEY_NUMPAD_6;
				case VK_NUMPAD7:  return KEY_NUMPAD_7;
				case VK_NUMPAD8:  return KEY_NUMPAD_8;
				case VK_NUMPAD9:  return KEY_NUMPAD_9;
				case VK_ADD:      return KEY_NUMPAD_PLUS;
				case VK_SUBTRACT: return KEY_NUMPAD_MINUS;
				case VK_MULTIPLY: return KEY_NUMPAD_MULT;
				case VK_DIVIDE:   return KEY_NUMPAD_DIV;
				case VK_DECIMAL:  return KEY_NUMPAD_DEC;
					//case VK_NUMLOCK:  return KEY_Numlock;
					// Other keys
				case VK_OEM_MINUS:  return KEY_MINUS;
				case VK_OEM_PLUS:   return KEY_EQUALS;
				case VK_OEM_COMMA:  return KEY_COMMA;
				case VK_OEM_PERIOD: return KEY_PERIOD;
				case VK_OEM_1:      return KEY_SEMICOLON;
				case VK_OEM_2:      return KEY_SLASH;
				case VK_OEM_3:      return KEY_TILDE;
				case VK_OEM_4:      return KEY_LBRACKET;
				case VK_OEM_5:      return KEY_BACKSLASH;
				case VK_OEM_6:      return KEY_RBRACKET;
				case VK_OEM_7:      return KEY_APOSTROPHE;
				case VK_CAPITAL:    return KEY_CAPSLOCK;
					//case VK_SNAPSHOT:   return KEY_PRINTSCR;
					//case VK_PAUSE:     return KEY_BREAK;
					//case VK_SCROLL:     return KEY_SCROLL_LOCK;
					//case VK_RWIN:       return KEY_RWINDOWS;
					//case VK_LWIN:       return KEY_LWINDOWS;
					//case VK_APPS:       return KEY_MENU;

				default: return ACTION_UNKNOWN;
			}
		}

		void HandleKeyboardInput(USHORT vkey, USHORT scancode, USHORT flags)
		{
			Action key = ACTION_UNKNOWN;

			// bypass fake key messages
			if (vkey == 255)
			{
				return;
			}
			else if (vkey == VK_SHIFT)
			{
				// Map to L or R shift
				vkey = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
			}
			else if (vkey == VK_NUMLOCK)
			{
				// Adjust apparent oddity with numlock / pause / break
				scancode = (MapVirtualKey(vkey, MAPVK_VK_TO_VSC) | 0x100);
			}

			const bool isE0 = ((flags & RI_KEY_E0) != 0);
			const bool isE1 = ((flags & RI_KEY_E1) != 0);

			if (isE1)
			{
				if (vkey == VK_PAUSE)
				{
					scancode = 0x45;
				}
				else
				{
					scancode = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
				}
			}

			switch (vkey)
			{
				// L/R distinction
				case VK_CONTROL:
					if (isE0)
						key = KEY_RCTRL;
					else
						key = KEY_LCTRL;
					break;
				case VK_MENU:
					if (isE0)
						key = KEY_RALT;
					else
						key = KEY_LALT;
					break;
				case VK_RETURN:
					if (isE0)
						key = KEY_NUMPAD_ENTER;
					break;

					// Numpad vs ins/home/etc keys
				case VK_INSERT:
					if (!isE0)
						key = KEY_NUMPAD_0;
					break;
				case VK_DELETE:
					if (!isE0)
						key = KEY_NUMPAD_DEC;
					break;
				case VK_HOME:
					if (!isE0)
						key = KEY_NUMPAD_7;
					break;
				case VK_END:
					if (!isE0)
						key = KEY_NUMPAD_1;
					break;
				case VK_PRIOR:
					if (!isE0)
						key = KEY_NUMPAD_9;
					break;
				case VK_NEXT:
					if (!isE0)
						key = KEY_NUMPAD_3;
					break;

					// Numpad vs arrow key distinction
				case VK_LEFT:
					if (!isE0)
						key = KEY_NUMPAD_4;
					break;
				case VK_RIGHT:
					if (!isE0)
						key = KEY_NUMPAD_6;
					break;
				case VK_UP:
					if (!isE0)
						key = KEY_NUMPAD_8;
					break;
				case VK_DOWN:
					if (!isE0)
						key = KEY_NUMPAD_2;
					break;

					// Numpad 5 distinction
				case VK_CLEAR:
					if (!isE0)
						key = KEY_NUMPAD_5;
					break;
			}

			if (key == ACTION_UNKNOWN)
			{
				// If we still don't know the output, it should be mappable from its vkey
				key = MapKey(vkey);
			}

			// If the key was successfully mapped, log it
			if (key != ACTION_UNKNOWN)
			{
				bool newState = !(flags & RI_KEY_BREAK);
				g_ActionBuffer[key] = newState;
			}
		}

		void HandleMouseInput(LONG x, LONG y, USHORT buttonFlags, USHORT buttonData)
		{
			// Add mouse movement to this frame's delta
			g_AxisStates[Axis::MOUSE_AXIS_X] += (float)x;
			g_AxisStates[Axis::MOUSE_AXIS_Y] += (float)y;

			// Check for scroll wheel action
			if (buttonFlags & RI_MOUSE_WHEEL)
			{
				float sign = buttonData & 0b1000000000000000 ? -1.0F : 1.0F;
				g_AxisStates[MOUSE_AXIS_WHEEL] += sign;
			}

			// Mouse button presses
			if (buttonFlags & RI_MOUSE_LEFT_BUTTON_UP || buttonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
			{
				g_ActionBuffer[MOUSE_LEFT_BUTTON] = buttonFlags & RI_MOUSE_LEFT_BUTTON_DOWN;
			}

			if (buttonFlags & RI_MOUSE_MIDDLE_BUTTON_UP || buttonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
			{
				g_ActionBuffer[MOUSE_MIDDLE_BUTTON] = buttonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN;
			}

			if (buttonFlags & RI_MOUSE_RIGHT_BUTTON_UP || buttonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
			{
				g_ActionBuffer[MOUSE_RIGHT_BUTTON] = buttonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN;
			}

			// Todo: Find a way to handle non-standard mouse buttons (back/forward thumb buttons, etc)
		}

		bool Initialize(HWND hwnd)
		{
			// FIXME: HWND targets

			RAWINPUTDEVICE Rid[2];

			// Keyboard
			Rid[0].usUsagePage = 0x01;
			Rid[0].usUsage = 0x06;
			Rid[0].hwndTarget = hwnd;
			Rid[0].dwFlags = 0;

			// Mouse
			Rid[1].usUsagePage = 0x01;
			Rid[1].usUsage = 0x02;
			Rid[1].hwndTarget = hwnd;
			Rid[1].dwFlags = 0;
			
			if (!RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])))
			{
				return false;
			}

			memset(g_ActionStates, 0, sizeof(g_ActionStates));
			memset(g_ActionBuffer, 0, sizeof(g_ActionBuffer));

			return true;
		}

		/**
		 * Handles character input from WM_CHAR
		 * Called from Renderer.cpp WndProc
		 */
		void OnCharInput(WPARAM wp)
		{
			const char key = static_cast<char>(wp);
			if (key == 0x0A
				|| key == 0x1B
				|| key == 0x09
				|| key == 0x0D
				|| key > 255)
			{
				return;
			}

			if (key == 0x08)
			{
				// backspace
				// For now store it anyway
			}

			g_LastTextKey = key;
		}

		/**
		 * Handles raw input messages from WM_INPUT
		 * Called from Renderer.cpp WndProc
		 */
		void OnRawInput(WPARAM wp, LPARAM lp)
		{
			UINT dwSize = sizeof(RAWINPUT);
			LPBYTE lpb[sizeof(RAWINPUT)];

			GetRawInputData((HRAWINPUT)lp, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;
			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				HandleKeyboardInput(
					raw->data.keyboard.VKey,
					raw->data.keyboard.MakeCode,
					raw->data.keyboard.Flags);
			}
			else if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				HandleMouseInput(
					raw->data.mouse.lLastX,
					raw->data.mouse.lLastY,
					raw->data.mouse.usButtonFlags,
					raw->data.mouse.usButtonData);
			}
		}

		const char GetLastTextKey()
		{
			return g_LastTextKey;
		}

		void PrepFrame()
		{
			// Copy the last frame's input buffer into the second buffer
			memcpy(g_ActionStates[1], g_ActionStates[0], sizeof(g_ActionStates[0]));
			// Fill the first buffer with updated values
			memcpy(g_ActionStates[0], g_ActionBuffer, sizeof(g_ActionStates[0]));

			// Clear axis deltas
			memset(g_AxisStates, 0, sizeof(g_AxisStates));

			// Clear the last text key to avoid spamming that value into any text box
			g_LastTextKey = 0;
		}

		bool IsPressed(Action act)
		{
			return g_ActionStates[0][act];
		}

		bool IsJustPressed(Action act)
		{
			return g_ActionStates[0][act] && !g_ActionStates[1][act];
		}

		bool IsReleased(Action act)
		{
			return !g_ActionStates[0][act];
		}

		bool IsJustReleased(Action act)
		{
			return !g_ActionStates[0][act] && g_ActionStates[1][act];
		}

		float GetAxisState(Axis ax)
		{
			return g_AxisStates[ax];
		}
	}
}