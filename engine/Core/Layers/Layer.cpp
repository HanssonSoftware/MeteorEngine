/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Layer.h"
#include <Application.h>
#include <Layers/LayerManager.h>


Layer::Layer(const String& Name) noexcept
	: name(Name)
{
}

void Layer::Attached()
{
	MR_LOG(LogLayers, Verbose, "%ls has been attached to the engine!", name.IsEmpty() ? L"Unnamed Layer" : name.Chr());
}

void Layer::Removed()
{
	MR_LOG(LogLayers, Verbose, "%ls has been removed from the engine!", name.IsEmpty() ? L"Unnamed Layer" : name.Chr());
}
