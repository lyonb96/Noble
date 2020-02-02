#include "Object.h"

#include "Logger.h"

namespace Noble
{
	Map<NIdentifier, NClass> Object::ObjectRegistry;

	Object* Object::CreateInstance(NClass* const type, void* ptr)
	{
		CHECK(type && ptr);

		return CreateInstanceFromClass(type, ptr);
	}

	Object* Object::CreateInstance(const NIdentifier& id, void* ptr)
	{
		CHECK(ptr);
		if (ObjectRegistry.ContainsKey(id))
		{
			return CreateInstanceFromClass(&ObjectRegistry[id], ptr);
		}

		NE_LOG_ERROR("Requested class %s is not registered!", id.GetString());

		return nullptr;
	}

	Object* Object::CreateInstanceFromClass(NClass* cls, void* ptr)
	{
		CHECK(ptr);

		if (cls->IsAbstract)
		{
			NE_LOG_ERROR("Cannot create instance of abstract type %s", cls->ObjectID.GetString());
			return nullptr;
		}

		Object* inst = cls->CreateInstance(ptr);
		inst->m_Class = cls;

		return inst;
	}
}