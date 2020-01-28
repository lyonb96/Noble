#include "ReflectionTesting.h"

void TestClass::TestReflectCreation()
{
	myMember = 5.0F;
	RefProps.Add(CREATE_REFLECTED(TestClass, myMember));
}