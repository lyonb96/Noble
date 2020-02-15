#pragma once

#include "AreTypesEqual.h"
#include "Functional.h"
#include "HelperMacros.h"
#include "String.h"

namespace Noble
{
	// Forward declare Object
	class Object;

	// -----------------------------------------------------------
	//         NClass
	// -----------------------------------------------------------

	// Pointer to a function that creates an object instance via placement new
	typedef Function<Object*(void*)> ObjectCreator;

	/**
	 * Handles registration data for Object subclasses
	 */
	struct NClass
	{
	private:

		/**
		 * Fills out members and automatically adds the registration data to the map
		 */
		NClass(const NIdentifier& id, ObjectCreator fn, Size size, Size align, bool abstr);

	public:

		/**
		 * Empty-init so it can live in a map
		 */
		NClass()
			: CreateInstance(nullptr), ObjectID()
		{
			ObjectSize = 0;
			ObjectAlign = 0;
			IsAbstract = false;
			Parent = nullptr;
		}

		/**
		 * Creates a registration based on the given Object subclass and automatically registers it
		 */
		template <class T>
		static NClass RegisterNClass()
		{
			// Constructor also stores it to the Object registry
			NClass reg(
				T::ClassName,
				FactoryFunc<T>,
				sizeof(T),
				alignof(T),
				std::is_abstract_v<T>);

			reg.Parent = T::Super::GetStaticClass();

			return reg;
		}

		/**
		 * Returns true if this NClass can be safely casted to the requested type
		 */
		template <typename T>
		bool IsA()
		{
			NClass* Tclass = T::GetStaticClass();
			NClass* myCls = this;
			while (myCls)
			{
				if (myCls == Tclass)
				{
					return true;
				}

				myCls = myCls->Parent;
			}

			return false;
		}

		/**
		 * Comparison operator uses only the ID since it should be unique for every class and is a quick integer compare
		 */
		FORCEINLINE friend bool operator==(const NClass& rhs, const NClass& lhs) { return rhs.ObjectID == lhs.ObjectID; }

		/**
		 * Comparison operator uses only the ID since it should be unique for every class and is a quick integer compare
		 */
		FORCEINLINE friend bool operator!=(const NClass& rhs, const NClass& lhs) { return rhs.ObjectID != lhs.ObjectID; }

		// Parent class
		NClass* Parent;
		// Unique ID of the Object subclass
		NIdentifier ObjectID;
		// Function that uses placement new to create an instance of the Object subclass
		ObjectCreator CreateInstance;
		// Size of an instance of the Object subclass, in bytes
		Size ObjectSize;
		// Minimum alignment required by an instance of the Object subclass
		Size ObjectAlign;
		// Class type
		bool IsAbstract;
	};
}