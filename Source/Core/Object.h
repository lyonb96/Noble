#pragma once

#include "Functional.h"
#include "Map.h"
#include "Types.h"

namespace Noble
{
	class Object;
	struct ObjectRegistration;

	// Pointer to a function that creates an object instance via placement new
	typedef Function<Object*(void*)> ObjectCreator;

	/**
	 * The Object class provides facilities for runtime creation by ID,
	 * and will provide network sync and replication featuers in the future
	 */
	class Object
	{
		friend struct ObjectRegistration;

	protected:

	public:

		static Object* CreateInstance(const U32 id, void* ptr);

	private:

		// Map of all object registrations
		static Map<U32, ObjectRegistration> ObjectRegistry;

	};

	/**
	 * Handles registration data for Object subclasses
	 */
	struct ObjectRegistration
	{
	private:
		ObjectRegistration(U32 id, ObjectCreator fn, Size size, Size align)
			: ObjectID(id), CreateInstance(fn), ObjectSize(size), ObjectAlign(align)
		{
			Object::ObjectRegistry.Insert(id, *this);
		}

	public:

		ObjectRegistration()
			: CreateInstance(nullptr)
		{
			ObjectID = 0;
			ObjectSize = 0;
			ObjectAlign = 0;
		}

		/**
		 * Creates a registration based on the given Object subclass and automatically registers it
		 */
		template <class T>
		static ObjectRegistration CreateRegistration()
		{
			ObjectRegistration reg(T::ClassName, &T::CreateInstance, sizeof(T), alignof(T));
			return reg;
		}

		// Unique ID of the Object subclass (Hash of the class name)
		U32 ObjectID;
		// Function that uses placement new to create an instance of the Object subclass
		ObjectCreator CreateInstance;
		// Size of an instance of the Object subclass, in bytes
		Size ObjectSize;
		// Minimum alignment required by an instance of the Object subclass
		Size ObjectAlign;

	};
}

#define OBJECT_DECL(CLASS_NAME, PARENT_NAME)\
public:\
	typedef PARENT_NAME Super;\
	static constexpr ::Noble::NIdentifier ClassName = #CLASS_NAME;\
	static ::Noble::Object* CreateInstance(void* ptr);\
private:\
	static ::Noble::ObjectRegistration Registration;

#define OBJECT_DEF(CLASS_NAME)\
::Noble::ObjectRegistration CLASS_NAME::Registration = ::Noble::ObjectRegistration::CreateRegistration<CLASS_NAME>();\
::Noble::Object* CLASS_NAME::CreateInstance(void* ptr) { return new (ptr) CLASS_NAME; }