/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>
#include <Logging/Log.h>


LOG_ADDCATEGORY(Layers);

class Layer
{
	friend class LayerManager;
public:
	Layer(const String& Name) noexcept;

	Layer() = delete;

	virtual ~Layer() = default;

	virtual void Init() = 0;
	
	virtual void Attached();

	virtual void Removed();

	virtual void Update() = 0;

	String GetName() const noexcept { return name; }

	void SetName(const String NewName) noexcept { name = NewName; }

	void operator=(const Layer&) = delete;

	const bool operator==(const Layer& Layer) noexcept
	{
		return this->name == Layer.name;
	}

protected:

	String name;
};

