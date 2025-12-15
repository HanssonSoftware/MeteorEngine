/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Object.h"

Object::Object()
{
	//int32_t ia = 0;
}

void Object::Frame(float deltaTime)
{
	if (!bCanBeTicked)
		return;

}

void Object::Init()
{
}

void Object::Shutdown()
{
	delete this;
}

void Object::setCanTick(bool Has)
{
	bCanBeTicked = Has;
}
