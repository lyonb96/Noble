#pragma once

#include "Controller.h"

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
		PlayerController(bool local = false);

		/**
		 * Overrides possession to handle camera selection
		 */
		virtual void Possess(GameObject* obj) override;

		/**
		 * Called each frame to check bindings and forward changes to the possessed object
		 */
		virtual void Update() override;

		/**
		 * Called each fixed frame
		 */
		virtual void FixedUpdate() override;

	private:

		// True if this PlayerController is the local player, not a networked player
		bool m_IsLocal;
	};
}