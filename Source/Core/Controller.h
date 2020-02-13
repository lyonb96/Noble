#pragma once

#include "Object.h"
#include "Types.h"

namespace Noble
{
	// Dummy game object
	class GameObject;

	/**
	 * Controllers are the core type for controlling a GameObject in the world
	 * The controller can be said to represent the will of the player or an AI
	 */
	class Controller : public Object
	{
		OBJECT_DECL(Controller, Object);
	public:

		/**
		 * Default constructor
		 */
		Controller();

		// Cannot be moved or copied
		Controller(const Controller&) = delete;
		Controller(Controller&&) = delete;
		Controller& operator=(const Controller&) = delete;
		Controller& operator=(Controller&&) = delete;

		/**
		 * Returns the object that is being controlled, or nullptr if none
		 */
		GameObject* GetControlledObject() const { return m_Object; }

		/**
		 * Called each frame to handle input and other per-frame actions
		 */
		virtual void Update() = 0;

		/**
		 * Called at each fixed step to handle any logic that requires a fixed step rate
		 */
		virtual void FixedUpdate() = 0;

	protected:

		// The possessed object
		GameObject* m_Object;

	};
}