#pragma once

#include <type_traits>

#include "AreTypesEqual.h"
#include "BitStream.h"
#include "Class.h"
#include "Functional.h"
#include "Map.h"
#include "String.h"
#include "Types.h"

namespace Noble
{
	// Provides some real-time class information
	struct NClass;

	/**
	 * The Object class provides facilities for runtime creation by ID,
	 * and will provide network sync and replication features in the future
	 */
	class Object
	{
	public:

		/**
		 * Checks if this Object is an instance of the given type
		 * Returns the Object casted to the given type if true,
		 * or nullptr if false
		 *
		 * Template argument must be a registered subclass of Object
		 */
		template <class T>
		T* IsA()
		{
			NClass* Tclass = T::GetStaticClass();
			NClass* myClass = GetClass();

			while (myClass)
			{
				if (*myClass == *Tclass)
				{
					return static_cast<T*>(this);
				}
				myClass = myClass->Parent;
			}

			return nullptr;
		}

		/**
		 * Returns a pointer to the NClass that this Object is based on
		 */
		NClass* GetClass() const { return m_Class; }

		/**
		 * Returns the Unique ID assigned to this Object instance
		 */
		const U64 GetUID() const { return m_UID; }

		/**
		 * Here to act as an end-point for iterating through parent classes
		 */
		static NClass* GetStaticClass() { return nullptr; }

	private:

		// Class that this Object instance is based on
		NClass* m_Class;
		// The Unique ID assigned to this Object instance
		U64 m_UID;

	public:

		// -----------------------------------------------------------
		// Object subclass registration and creation
		// -----------------------------------------------------------
		
		friend struct NClass;

		/**
		 * Returns the NClass that corresponds to the requested ID
		 */
		static NClass* GetClassByID(const NIdentifier& id);

		/**
		 * Returns the NClass that corresponds to the requested ID
		 */
		static NClass* GetClassByID(const U32 id);

		/**
		 * Creates a class instance from a template parameter
		 * Template parameter must be a registered Object subclass
		 */
		template <class T>
		static Object* CreateInstance(void* ptr)
		{
			return CreateInstance(T::GetStaticClass(), ptr);
		}

		/**
		 * Creates an Object subclass of the NClass type
		 */
		static Object* CreateInstance(NClass* type, void* ptr);

		/**
		 * Creates an Object subclass of the given ID, or returns nullptr if ID is invalid
		 */
		static Object* CreateInstance(const NIdentifier& id, void* ptr);

	private:

		/**
		 * Uses the given NClass instance to create a new Object subclass at the given address
		 */
		static Object* CreateInstanceFromClass(NClass* cls, void* ptr);

		// Map of all object registrations
		static Map<NIdentifier, NClass*> ObjectRegistry;

		// Counter for Objects to assign unique IDs
		static U64 ObjectCount;

	};

	/**
	 * Uses templates to create a unique function definition for each Object subclass
	 */
	template <typename T>
	Object* FactoryFunc(void* ptr)
	{
		if constexpr (std::is_abstract_v<T>)
		{
			// abstract classes can't be constructed
			return nullptr;
		}
		else
		{
			// Otherwise, placement-new the type
			return new (ptr) T;
		}
	}
}

/**
 * Declares all required members of an Object subclass
 */
#define OBJECT_DECL(CLASS_NAME, PARENT_NAME)\
public:\
	friend struct NClass;\
	typedef PARENT_NAME Super;\
	static constexpr ::Noble::NIdentifier ClassName = ID(#CLASS_NAME);\
	static ::Noble::NClass* GetStaticClass() { return &StaticClass; }\
private:\
	static ::Noble::NClass StaticClass;

/**
 * Defines all required members of an Object subclass
 */
#define OBJECT_DEF(CLASS_NAME)\
::Noble::NClass CLASS_NAME::StaticClass = ::Noble::NClass::RegisterNClass<CLASS_NAME>();