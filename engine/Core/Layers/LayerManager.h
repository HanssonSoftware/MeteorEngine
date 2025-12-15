/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Array.h>
#include <Object/Object.h>

class Layer;
class String;


class LayerManager : public Object
{
	friend class Layer;
public:
	LayerManager();

	virtual ~LayerManager();

	void PushLayer(Layer* Instance);

	void RemoveLayer(const Layer* Instance);

	void RemoveLayer(const String Name);

	void UpdateLayer();

	virtual void Init() override;

	virtual void Shutdown() override;

protected:
	Array<Layer*> layers;
};

