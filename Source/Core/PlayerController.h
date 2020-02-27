#pragma once

#include "Controller.h"
#include "CameraComponent.h"

namespace Noble
{

	/**
	 * The PlayerController is the representation of the player's input in the game
	 */
	class PlayerController : public Controller
	{
		OBJECT_DECL(PlayerController, Controller)
	public:

		/**
		 * Default constructor
		 */
		PlayerController();

		/**
		 * Called each frame to check bindings and forward changes to the possessed object
		 */
		virtual void Update() override;

		/**
		 * Called each fixed frame
		 */
		virtual void FixedUpdate() override;

		/**
		 * Overridden to handle camera selection
		 */
		virtual void Possess(GameObject* obj) override;

	private:

	};
}