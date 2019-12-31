#pragma once

#include <type_traits>

#include "GameObject.h"

namespace Noble
{
	/**
	 * The World class encompasses a "map" or area of play.
	 * Worlds hold all of the currently spawned GameObjects.
	 */
	class World
	{
	public:

		World();

		/**
		 * Creates a new GameObject, but does not immediately spawn it
		 */
		//template <class T,
		//	typename std::enable_if_t<std::is_base_of<GameObject, T>::value>* = nullptr>
		//T* CreateGameObject()
		//{

		//}

		template <class T>
		T* CreateGameObject()
		{
			Size objSize = sizeof(T);

		}

	private:

		
	};
}