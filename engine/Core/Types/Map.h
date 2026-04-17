/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Array.h>
#include <Platform/DataTypes.h>

static u64 Hash(const char* str)
{
	static constexpr const u64 offset = 0xcbf29ce484222325;
	static constexpr const u64 PRIME = 0x100000001b3;

	const u32 length = (u32)strlen(str);

	u64 value = offset;
	const u8* bytes = reinterpret_cast<const u8*>(str);
	for (u64 i = 0; i < length; ++i)
	{
		value ^= static_cast<u64>(bytes[i]);
		value *= PRIME;
	}

	return value;
}

u64 operator ""_h(const char* str, u64 len)
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
		u64 id = Hash(&Input);
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
		u64 id = Hash(&Input);
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

