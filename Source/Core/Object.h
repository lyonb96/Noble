#pragma once

#include <type_traits>

#include "AreTypesEqual.h"
#include "Class.h"
#include "Functional.h"
#include "Map.h"
#include "String.h"
#include "Types.h"

namespace Noble
{
	class Object;
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
		 * Root level of the IsA() comparison
		 */
		template <class T>
		bool IsA() const
		{
			return ARE_TYPES_EQUAL(T, Object);
		}

		/**
		 * Returns a pointer to the NClass that this Object is based on
		 */
		NClass* GetClass() const { return m_Class; }

	private:

		// Class that this Object instance is based on
		NClass* m_Class;

	public:

		// -----------------------------------------------------------
		// Object subclass registration and creation
		// -----------------------------------------------------------
		
		friend struct NClass;

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
		static Object* CreateInstance(const NImmutableIdentifier& id, void* ptr);

	private:

		// Map of all object registrations
		static Map<NIdentifier, NClass> ObjectRegistry;

		/**
		 * Uses the given NClass instance to create a new Object subclass at the given address
		 */
		static Object* CreateInstanceFromClass(NClass* cls, void* ptr);

	};
}

/**
 * Declares all required members of an Object subclass
 */
#define OBJECT_DECL(CLASS_NAME, PARENT_NAME)\
public:\
	friend struct NClass;\
	typedef PARENT_NAME Super;\
	static constexpr ::Noble::NImmutableIdentifier ClassName = #CLASS_NAME;\
	static ::Noble::NClass* GetStaticClass() { return &StaticClass; }\
	template <class T>\
	bool IsA() const\
	{\
		if (::Noble::AreTypesEqual<T, CLASS_NAME>::Value)\
		{\
			return true;\
		}\
		else\
		{\
			return Super::IsA<T>();\
		}\
	}\
private:\
	static ::Noble::Object* CreateInstance(void* ptr);\
	static ::Noble::NClass StaticClass;

/**
 * Defines all required members of an Object subclass
 */
#define OBJECT_DEF(CLASS_NAME)\
::Noble::NClass CLASS_NAME::StaticClass = ::Noble::NClass::RegisterNClass<CLASS_NAME>();\
::Noble::Object* CLASS_NAME::CreateInstance(void* ptr) { return new (ptr) CLASS_NAME; }

/**
 * Defines all required members of an abstract Object subclass
 * Abstract classes cannot be created via CreateInstance!
 */
#define OBJECT_DEF_ABSTRACT(CLASS_NAME)\
::Noble::NClass CLASS_NAME::StaticClass = ::Noble::NClass::RegisterNClass<CLASS_NAME>();\
::Noble::Object* CLASS_NAME::CreateInstance(void* ptr) { return nullptr; }