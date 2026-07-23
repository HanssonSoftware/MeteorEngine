/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Array.h>
#include <HAL/DataTypes.h>

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

static constexpr u64 Hash(const wchar_t* key)
{
	u64 hash = 0xcbf29ce484222325;
	constexpr u64 prime = 0x100000001b3;

	while (*key && *key != L'\0')
	{
		hash = hash ^ *key;
		hash *= prime;
		key++;
	}

	return hash;
}

static constexpr u64 operator ""_h(const char* str, u64 len)
{
	return Hash(str);
}

template <typename Key, typename Value>
class Map
{
	struct InternalContainerForMap
	{
		u64 hashKey;
		Value data;
	};

public:
	Map() = default;
	~Map()
	{
		container.Clear();
	}

	Map(const Map& other)
	{
		container = other.container;
	};

	Map(Map&& other)
	{
		container = std::move(other.container);
	};

	Map& operator=(const Map& other)
	{
		if (*this != other)
		{
			container = other.container;
		}

		return *this;
	}

	Map& operator=(Map&& other)
	{
		if (*this != other)
		{
			container = std::move(other.container);
		}

		return *this;
	}

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

	Value* begin() { return &container.Data()->data; }
	Value* end() { return &container.Data()->data + container.GetSize(); }
	const Value* begin() const { return &container.Data()->data; }
	const Value* end() const { return &container.Data()->data + container.GetSize(); }

	bool operator!=(const Map& other)
	{
		return container != other.container;
	}
protected:
	Array<InternalContainerForMap> container;
};

