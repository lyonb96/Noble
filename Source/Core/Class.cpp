#include "Class.h"

#include "Object.h"

namespace Noble
{
	NClass::NClass(const NIdentifier& id, ObjectCreator fn, Size size, Size align, bool abstr)
		: ObjectID(id),
		CreateInstance(fn),
		ObjectSize(size),
		ObjectAlign(align),
		IsAbstract(abstr),
		Parent(nullptr)
	{
		// Make sure the registry doesn't already contain the key (avoid duplicates or hash collisions)
		CHECK(!Object::ObjectRegistry.ContainsKey(id));
		Object::ObjectRegistry.Insert(id, *this);
	}
}