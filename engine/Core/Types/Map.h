/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Array.h>
#include <Platform/DataTypes.h>

template<typename T>
static u64 Hash(const T& str)
{
	u64 final = 0xcbf29ce484222325;
	
	for (char val : str)
	{
		final *= 1099511628211;
		final ^= static_cast<u64>(val);
	}

	return final;
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
		u64 id = Hash(Input);
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
		u64 id = Hash(Input);
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

