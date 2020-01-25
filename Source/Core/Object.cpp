#include "Object.h"

#include "Logger.h"

namespace Noble
{
	Map<NIdentifier, ObjectRegistration> Object::ObjectRegistry;

	Object* Object::CreateInstance(const NImmutableIdentifier& id, void* ptr)
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