/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once

template<typename T, typename U>
struct same { static constexpr bool value = false; };

template<typename T>
struct same<T, T> { static constexpr bool value = true; };

template<typename T, typename U>
static constexpr bool same_v = same<T, U>::value;