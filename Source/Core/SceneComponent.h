#pragma once

#include "Component.h"

namespace Noble
{

	class SceneComponent : public Component
	{
		OBJECT_DECL(SceneComponent, Component);

	private:

		friend class Renderer;

		/**
		 * Overridden in base classes to handle drawing the component
		 */
		virtual void Draw() {}
	};
}