/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

template<typename T>
struct HashValue
{
	static_assert(sizeof(T) == 0, "HashValue just as is not supported!");
};

// You should use Implementation parameter for implementing the method of hashing ex. { ... code ... return xy; }
#define CREATE_HASH_FOR_TYPE(Type, Implementation) \
template<> \
struct HashValue<Type>\
{\
	static u64 Apply(Type& data) Implementation; \
}
