/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Array.h>
#include <Platform/DataTypes.h>

static constexpr u64 Hash(const char* key)
{
	u64 hash = 0xcbf29ce484222325;
	constexpr u64 prime = 0x100000001b3;

	while (*key && *key != '\0')
	{
		hash = hash ^ *key;
		hash *= prime;
		key++;
	}

	return hash;
}

static u64 operator ""_h(const char* str, u64 len)
{
	return Hash(str);
}

template <typename Key, typename Value>
class Map final
{
	struct InternalContainerForMap
	{
		u64 hashKey;
		Value data;
	};

public:
	Map() = default;
	~Map() noexcept = default;

	Value& operator[](Key Input)
	{
		u64 id = Hash(Input, strlen(Input));
		for (auto& ctr : container)
		{
			if (id == ctr.hashKey)
				return ctr.data;
		}

		container.Add({ id, Value()});
		return container[container.GetSize() - 1].data;
	};

	const Value& operator[](Key Input) const
	{
		u64 id = Hash(Input, strlen(Input));
		for (const auto& ctr : container)
		{
			if (id == ctr.hashKey)
				return ctr.data;
		}

		container.Add({ id, Value()});
		return container[container.GetSize() - 1].data;
	};

protected:
	Array<InternalContainerForMap> container;
};

