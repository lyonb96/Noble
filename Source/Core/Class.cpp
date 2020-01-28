#include "Class.h"

#include "Object.h"

namespace Noble
{
	NClass::NClass(const NImmutableIdentifier& id, ObjectCreator fn, Size size, Size align, bool abstr)
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
}