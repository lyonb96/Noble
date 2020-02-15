#include "Object.h"

#include "Logger.h"

namespace Noble
{
	Map<NIdentifier, NClass*> Object::ObjectRegistry;
	U64 Object::ObjectCount = 0;

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
			return CreateInstanceFromClass(ObjectRegistry[id], ptr);
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
		inst->m_UID = ObjectCount++;

		return inst;
	}

	NClass* Object::GetClassByID(const NIdentifier& id)
	{
		for (auto cls : ObjectRegistry)
		{
			if (cls.Key == id)
			{
				return cls.Value;
			}
		}

		return nullptr;
	}

	NClass* Object::GetClassByID(const U32 id)
	{
		for (auto cls : ObjectRegistry)
		{
			if (cls.Key.GetHash() == id)
			{
				return cls.Value;
			}
		}

		return nullptr;
	}
}