/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Layers/Layer.h>

class EditorLayer : public Layer
{
public:
	EditorLayer(const String& newName) noexcept;

	virtual void Init() override;

	virtual void Attached() override;

	virtual void Removed() override;

	virtual void Update() override;
};

