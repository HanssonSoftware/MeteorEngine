/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

class Application;

template<typename InApp, typename InConfigurationType>
Application* BootstrapApplication()
{

	return new InApp();
}