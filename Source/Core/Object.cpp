#include "Object.h"

#include "Logger.h"

namespace Noble
{
	Map<U32, ObjectRegistration> Object::ObjectRegistry;

	Object* Object::CreateInstance(const U32 id, void* ptr)
	{
		CHECK(ptr);
		if (ObjectRegistry.ContainsKey(id))
		{
			ObjectRegistration& reg = ObjectRegistry[id];
			return reg.CreateInstance(ptr);
		}

		NE_LOG_ERROR("Requested object ID %u is not registered!", id);

		return nullptr;
	}
}