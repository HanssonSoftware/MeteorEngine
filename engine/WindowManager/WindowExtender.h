/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
class IWindowExtender
{
public:

	IWindowExtender() = default;

	virtual ~IWindowExtender() noexcept = default;

	virtual void Init() = 0;

	virtual void Update() = 0;

	virtual void Remove() = 0;
};

