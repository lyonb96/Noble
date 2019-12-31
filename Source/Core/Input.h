#pragma once

#include "Types.h"

namespace Noble
{
	namespace Input
	{

		/**
		 * Enum of all supported action inputs
		 * Actions only have an on/off state
		 */
		enum Action : U16
		{
			// Keyboard

			// Text keys

			// Letters
			KEY_A,
			KEY_B,
			KEY_C,
			KEY_D,
			KEY_E,
			KEY_F,
			KEY_G,
			KEY_H,
			KEY_I,
			KEY_J,
			KEY_K,
			KEY_L,
			KEY_M,
			KEY_N,
			KEY_O,
			KEY_P,
			KEY_Q,
			KEY_R,
			KEY_S,
			KEY_T,
			KEY_U,
			KEY_V,
			KEY_W,
			KEY_X,
			KEY_Y,
			KEY_Z,
			KEY_SPACE,
			// Numbers / top row
			KEY_TILDE,
			KEY_1,
			KEY_2,
			KEY_3,
			KEY_4,
			KEY_5,
			KEY_6,
			KEY_7,
			KEY_8,
			KEY_9,
			KEY_0,
			KEY_MINUS,
			KEY_EQUALS,
			KEY_BACKSPACE,
			// Symbols
			KEY_TAB,
			KEY_LBRACKET,
			KEY_RBRACKET,
			KEY_BACKSLASH,
			KEY_SEMICOLON,
			KEY_APOSTROPHE,
			KEY_COMMA,
			KEY_PERIOD,
			KEY_SLASH,
			// Numpad stuff
			KEY_NUMPAD_0,
			KEY_NUMPAD_1,
			KEY_NUMPAD_2,
			KEY_NUMPAD_3,
			KEY_NUMPAD_4,
			KEY_NUMPAD_5,
			KEY_NUMPAD_6,
			KEY_NUMPAD_7,
			KEY_NUMPAD_8,
			KEY_NUMPAD_9,
			KEY_NUMPAD_ENTER,
			KEY_NUMPAD_PLUS,
			KEY_NUMPAD_MINUS,
			KEY_NUMPAD_DIV,
			KEY_NUMPAD_MULT,
			KEY_NUMPAD_DEC,
			// Arrow keys
			KEY_UP,
			KEY_DOWN,
			KEY_LEFT,
			KEY_RIGHT,
			// Function keys
			KEY_ESC,
			KEY_F1,
			KEY_F2,
			KEY_F3,
			KEY_F4,
			KEY_F5,
			KEY_F6,
			KEY_F7,
			KEY_F8,
			KEY_F9,
			KEY_F10,
			KEY_F11,
			KEY_F12,
			// Modifier keys
			KEY_CAPSLOCK,
			KEY_LSHIFT,
			KEY_RETURN,
			KEY_RSHIFT,
			KEY_LCTRL,
			KEY_LALT,
			KEY_RALT,
			KEY_RCTRL,
			// Text navigation keys
			KEY_INSERT,
			KEY_DELETE,
			KEY_HOME,
			KEY_END,
			KEY_PAGEUP,
			KEY_PAGEDOWN,

			// Mouse
			MOUSE_LEFT_BUTTON,
			MOUSE_RIGHT_BUTTON,
			MOUSE_MIDDLE_BUTTON,

			// Gamepad (Labeled according to an Xbox controller)
			GAMEPAD_A,
			GAMEPAD_B,
			GAMEPAD_Y,
			GAMEPAD_X,
			GAMEPAD_DPAD_UP,
			GAMEPAD_DPAD_DOWN,
			GAMEPAD_DPAD_LEFT,
			GAMEPAD_DPAD_RIGHT,
			GAMEPAD_LEFT_STICK_BUTTON,
			GAMEPAD_RIGHT_STICK_BUTTON,
			GAMEPAD_START,
			GAMEPAD_LEFT_BUMPER,
			GAMEPAD_RIGHT_BUMPER,
			ACTION_UNKNOWN,
			ACTION_UNASSIGNED,
			ACTION_COUNT
		};

		/**
		 * Enum of all supported axis inputs
		 * Axis inputs can have varying values, and are represented by deltas per frame
		 */
		enum Axis : U16
		{
			// Mouse
			MOUSE_AXIS_X,
			MOUSE_AXIS_Y,
			MOUSE_AXIS_WHEEL,

			//Gamepad
			GAMEPAD_LEFT_STICK_AXIS_X,
			GAMEPAD_LEFT_STICK_AXIS_Y,
			GAMEPAD_RIGHT_STICK_AXIS_X,
			GAMEPAD_RIGHT_STICK_AXIS_Y,
			GAMEPAD_LEFT_TRIGGER,
			GAMEPAD_RIGHT_TRIGGER,
			AXIS_UNKNOWN,
			AXIS_UNASSIGNED,
			AXIS_COUNT
		};

		/**
		 * Returns the action that corresponds to the given string, or UNKNOWN if none
		 */
		Action GetAction(const char* str);

		/**
		 * Returns the axis that corresponds to the given string, or UNKNOWN if none
		 */
		Axis GetAxis(const char* str);

		/**
		 * Returns a string representation of the given Action
		 */
		const char* GetActionString(Action val);

		/**
		 * Returns a string representation of the given Axis
		 */
		const char* GetAxisString(Axis val);

		/**
		 * Initializes raw input code and prepares buffers
		 */
		//bool Initialize();

		/**
		 * Called at the start of the frame automatically, clears buffers and prepares input for the frame
		 */
		void PrepFrame();

		/**
		 * Returns true if the given action is currently pressed/true
		 */
		bool IsPressed(Action act);

		/**
		 * Returns true if the given action was false last frame and true this frame
		 */
		bool IsJustPressed(Action act);

		/**
		 * Returns true if the given action is released/false
		 */
		bool IsReleased(Action act);

		/**
		 * Returns true if the given action was true last frame and false this frame
		 */
		bool IsJustReleased(Action act);

		/**
		 * Returns this frame's delta for the given axis
		 */
		float GetAxisState(Axis ax);

		/**
		 * Returns the last typed character
		 * Caller needs to handle special cases:
		 * 0x00: Nothing pressed
		 * 0x08: Backspace pressed
		 *
		 * In the future I'd like to make this a string that concatenates each character.
		 * That way if exceptionally fast typers manage to type two characters in a single frame,
		 * the engine won't miss the first one. It also makes handling the text slightly easier.
		 */
		const char GetLastTextKey();
	}
}