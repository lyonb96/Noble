#pragma once

#include <type_traits>

#include "Functional.h"
#include "Map.h"
#include "String.h"
#include "Types.h"

namespace Noble
{
	class Object;
	// Provides some real-time class information
	struct NClass;

	// Pointer to a function that creates an object instance via placement new
	typedef Function<Object*(void*)> ObjectCreator;

	/**
	 * The Object class provides facilities for runtime creation by ID,
	 * and will provide network sync and replication features in the future
	 */
	class Object
	{
	public:

		/**
		 * Root level of the IsA() comparison
		 * Since any compatible object for this function is an Object derivative,
		 * it always returns true.
		 */
		template <class T>
		bool IsA() const
		{
			return true;
		}

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
		static Object* CreateInstance(NClass* const type, void* ptr);

		/**
		 * Creates an Object subclass of the given ID, or returns nullptr if ID is invalid
		 */
		static Object* CreateInstance(const NImmutableIdentifier& id, void* ptr);

		/**
		 * Returns a pointer to the NClass that this Object is based on
		 */
		NClass* const GetClass() const { return m_Class; }

	private:

		// Map of all object registrations
		static Map<NIdentifier, NClass> ObjectRegistry;

		/**
		 * Uses the given NClass instance to create a new Object subclass at the given address
		 */
		static Object* CreateInstanceFromClass(NClass& cls, void* ptr);

	};

	/**
	 * Handles registration data for Object subclasses
	 */
	struct NClass
	{
	private:

		/**
		 * Fills out members and automatically adds the registration data to the map
		 */
		NClass(const NImmutableIdentifier& id, ObjectCreator fn, Size size, Size align, bool abstr)
			: ObjectID(id), 
			CreateInstance(fn), 
			ObjectSize(size), 
			ObjectAlign(align), 
			IsAbstract(abstr)
		{
			// Make sure the registry doesn't already contain the key (avoid duplicates or hash collisions)
			CHECK(!Object::ObjectRegistry.ContainsKey(id));
			Object::ObjectRegistry.Insert(id, *this);
		}

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
		}

		/**
		 * Creates a registration based on the given Object subclass and automatically registers it
		 */
		template <class T>
		static NClass RegisterNClass()
		{
			NClass reg(
				T::ClassName,
				&T::CreateInstance,
				sizeof(T),
				alignof(T),
				std::is_abstract_v<T>);
			return reg;
		}

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
		if (T::ClassName == ClassName)\
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