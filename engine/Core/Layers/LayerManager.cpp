/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "LayerManager.h"
#include "Layer.h"

LayerManager::LayerManager()
{

}

void LayerManager::Init()
{

}

void LayerManager::Shutdown()
{
	for (Layer*& indexed : layers)
	{
		if (indexed)
		{
			indexed->Removed();
			indexed = nullptr;
		}
	}
}


void LayerManager::PushLayer(Layer* Instance)
{
	if (!Instance)
		return;

	Instance->Attached();
	layers.Add(Instance);
}

void LayerManager::RemoveLayer(const Layer* Instance)
{
	const uint32_t size = layers.GetSize();
	for (uint32_t i = 0; i < size; i++)
	{
		if (layers[i] == Instance)
		{
			layers[i]->Removed();
			layers[i] = nullptr;
		}
	}
}

void LayerManager::RemoveLayer(const String Name)
{
	const uint32_t size = layers.GetSize();
	for (uint32_t i = 0; i < size; i++)
	{
		if (layers[i]->GetName() == Name)
		{
			layers[i]->Removed();
			layers[i] = nullptr;
		}
	}
}

void LayerManager::UpdateLayer()
{
	for (Layer*& indexed : layers)
	{
		if (indexed != nullptr) indexed->Update();
	}
}

LayerManager::~LayerManager()
{
	layers.Clear();
}
