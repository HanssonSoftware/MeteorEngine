/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include "WindowExtender.h"

class IWindowMenu : public IWindowExtender
{
public:

	IWindowMenu() = default;

	virtual ~IWindowMenu() noexcept = default;
};

