/***
*ehveccvb.cpp - EH c-tor iterator helper function for class w/ virtual bases
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       EH-aware version of constructor iterator helper function for class
*       with virtual bases
*
*       These functions are called when constructing and destructing arrays of
*       objects.  Their purpose is to assure that constructed elements get
*       destructed if the constructor for one of the elements throws.
*
*       These functions are called when constructing and destructing arrays of
*       objects.  Their purpose is to assure that constructed elements get
*       destructed if the constructor for one of the elements throws.
*
*       Must be compiled using "-d1Binl" to be able to specify __thiscall
*       at the user level
****/

#include <eh.h>

#define CALLEETYPE __stdcall
#define __RELIABILITY_CONTRACT

#if defined _M_IX86
#define CALLTYPE __thiscall
#else
#define CALLTYPE __stdcall
#endif

using constructor_type = void (CALLTYPE*)(void*);
using destructor_type  = void (CALLTYPE*)(void*);

__RELIABILITY_CONTRACT
void CALLEETYPE __ArrayUnwind(
    void*           ptr,            // Pointer to array to destruct
    size_t          size,           // Size of each element (including padding)
    size_t          count,          // Number of elements in the array
    destructor_type destructor      // The destructor to call
    );

__RELIABILITY_CONTRACT
void CALLEETYPE __ehvec_ctor_vb(
    void*            ptr,           // Pointer to array to destruct
    size_t           size,          // Size of each element (including padding)
    size_t           count,         // Number of elements in the array
    constructor_type constructor,   // Constructor to call
    destructor_type  destructor     // Destructor to call should exception be thrown
    )
{
    size_t i{0};
    bool success{false};

    __try
    {
        for (; i != count; ++i)
        {
            #pragma warning(push)
            #pragma warning(disable: 4191) // unsafe conversion
            reinterpret_cast<void (CALLTYPE*)(void*, int)>(constructor)(ptr, 1);
            #pragma warning(pop)

            ptr = static_cast<char*>(ptr) + size;
        }

        success = true;
    }
    __finally
    {
        if (!success)
        {
            __ArrayUnwind(ptr, size, i, destructor);
        }
    }
}

__RELIABILITY_CONTRACT
void CALLEETYPE __ehvec_ctor_vb(
    void*            ptr,           // Pointer to array to destruct
    size_t           size,          // Size of each element (including padding)
    int              count,         // Number of elements in the array
    constructor_type constructor,   // Constructor to call
    destructor_type  destructor     // Destructor to call should exception be thrown
    )
{
    __ehvec_ctor_vb(ptr, size, static_cast<size_t>(count), constructor, destructor);
}
