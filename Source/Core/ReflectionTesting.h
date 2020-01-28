#pragma once

#include "Array.h"
#include "String.h"
#include "Types.h"

#include <cstddef>
#include <type_traits>

using namespace Noble;

/**
 * A header for playing around with various ideas for Object member reflection and replication
 */

struct Reflected {};

struct ReflectedVar : public Reflected
{
    template <typename Object, typename T>
    static ReflectedVar Create(const NImmutableIdentifier& id, Size off)
    {
        ReflectedVar v;
        v.VarID = id;
        v.VarSize = sizeof(T);
        v.Offset = off;
        return v;
    }

    NIdentifier VarID;
    Size VarSize;
    Size Offset;
};



class TestClass
{
public:

    void TestReflectCreation();

    float ReadValue() const { return myMember; }

    template <typename T>
    void SetReflectedValue(const U32 id, T newVal)
    {
        for (auto var : RefProps)
        {
            if (var.VarID.GetHash() == id)
            {
                // The most sinful line of C++ ever written
                *((T*)(((U8*)this) + var.Offset)) = newVal;
                return;
            }
        }
    }

private:

    float myMember;
    Array<ReflectedVar> RefProps;
};

#define CREATE_REFLECTED(CLASS_NAME, MEMBER) \
ReflectedVar::Create<CLASS_NAME, std::decay_t<decltype(MEMBER)>>(#MEMBER, offsetof(CLASS_NAME, MEMBER))