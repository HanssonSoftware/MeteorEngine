/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once

template<typename T>
struct Vector2
{
	T x, y;

	Vector2();

	Vector2(const Vector2& Copy)
	{
		x = Copy.x;
		y = Copy.y;
	}

	Vector2(T x, T y)
		:x(x), y(y)
	{

	}

	Vector2(T all)
		:x(all), y(all)
	{

	}

	~Vector2()
	{

	}

	const Vector2 operator+(Vector2& Add)
	{
		Vector2 Temp;
		Temp.x = this->x + Add.x;
		Temp.y = this->y + Add.y;

		return Temp;
	}	
	
	const void operator+=(Vector2& Add)
	{
		this->x += Add.x;
		this->y += Add.y;
	}	
	
	Vector2& operator=(Vector2 Equal)
	{
		this->x = Equal.x;
		this->y = Equal.y;
		
		return *this;
	}		
	
	bool operator!=(Vector2& Equal)
	{
		bool A = this->x != Equal.x;
		bool B = this->y != Equal.y;
		return A && B;
	}		
	
	const bool operator==(T& Equal)
	{
		bool A = (T)this->x == Equal;
		bool B = (T)this->y == Equal;
		return A && B;
	}	
	
	bool operator>(Vector2<T>& Equal)
	{
		bool A = this->x > Equal.x;
		bool B = this->y > Equal.y;
		return A && B;
	}		
	
	bool operator>(T Equal)
	{
		bool A = this->x > Equal;
		bool B = this->y > Equal;
		return A && B;
	}			
	
	bool operator<(T Equal) const
	{
		return x < Equal && y < Equal;
	}		
	
	bool operator==(const Vector2<T>& Equal)
	{
		bool A = this->x == Equal.x;
		bool B = this->y == Equal.y;
		return A && B;
	}	

	bool operator<(const Vector2<T>& Equal)
	{
		bool A = this->x < Equal.x;
		bool B = this->y < Equal.y;
		return A && B;
	}
};

template<typename T>
struct Vector3
{
	Vector3();

	Vector3(T x, T y, T z)
		:x(x), y(y), z(z)
	{

	}	
	
	Vector3(T all)
		:x(all), y(all), z(all)
	{

	}

	~Vector3();

	const Vector3 operator+(const Vector3& Add)
	{
		Vector3 Temp;
		Temp.x = this->x + Add.x;
		Temp.y = this->y + Add.y;
		Temp.z = this->z + Add.z;

		return Temp;
	}	
	
	const Vector3 operator-(const Vector3& Add)
	{
		Vector3 Temp;
		Temp.x = Add.x - this->x;
		Temp.y = Add.y - this->y;
		Temp.z = Add.z - this->z;

		return Temp;
	}

	const void operator+=(const Vector3& Add)
	{
		this->x += Add.x;
		this->y += Add.y;
		this->z += Add.z;
	}

	const void operator=(const Vector3& Equal)
	{
		this->x = Equal.x;
		this->y = Equal.y;
		this->z = Equal.z;
	}	
	
	const bool operator!=(const Vector3& Equal)
	{
		bool A = Equal.x != this->x;
		bool B = Equal.y != this->y;
		bool C = Equal.z != this->z;
		return A && B && C;
	}	
	
	const bool operator==(const Vector3& Equal)
	{
		bool A = Equal.x == this->x;
		bool B = Equal.y == this->y;
		bool C = Equal.z == this->z;
		return A && B && C;
	}

	T x, y, z;
};

template<typename T>
struct Vector4
{
	T x, y, z, w;

	Vector4();

	Vector4(const Vector4& Copy)
	{
		x = Copy.x;
		y = Copy.y;		
		z = Copy.z;
		w = Copy.w;
	}

	Vector4(T x, T y, T z, T w)
		:x(x), y(y), z(z), w(w)
	{

	}

	Vector4(T all)
		:x(all), y(all), z(all), w(all)
	{

	}

	~Vector4();

	const Vector4 operator+(const Vector4& Add)
	{
		Vector4 Temp;
		Temp.x = this->x + Add.x;
		Temp.y = this->y + Add.y;		
		Temp.z = this->z + Add.z;
		Temp.w = this->w + Add.w;

		return Temp;
	}

	const void operator+=(const Vector4& Add)
	{
		this->x += Add.x;
		this->y += Add.y;
		this->z += Add.z;
		this->w += Add.w;
	}

	const void operator=(const Vector4& Equal)
	{
		this->x = Equal.x;
		this->y = Equal.y;
		this->z = Equal.z;
		this->w = Equal.w;
	}

	constexpr bool operator!=(const Vector4& Equal)
	{
		bool A = this->x != Equal.x;
		bool B = this->y != Equal.y;
		return A && B;
	}

	constexpr bool operator>(const Vector4<T>& Equal)
	{
		bool A = this->x > Equal.x;
		bool B = this->y > Equal.y;
		return A && B;
	}

	constexpr bool operator==(const Vector4<T>& Equal)
	{
		bool A = this->x == Equal.x;
		bool B = this->y == Equal.y;
		bool C = this->z == Equal.z;
		bool D = this->w == Equal.w;
		return A && B && C && D;
	}

	template<typename F>
	constexpr bool operator==(const F Equal)
	{
		bool A = this->x == Equal;
		bool B = this->y == Equal;
		bool C = this->z == Equal;
		bool D = this->w == Equal;
		return A && B && C && D;
	}

};

template<typename T>
inline Vector2<T>::Vector2()
{
	x = 0;
	y = 0;
}

template<typename T>
inline Vector3<T>::Vector3()
{
	x = 0;
	y = 0;
	z = 0;
}

template<typename T>
inline Vector3<T>::~Vector3()
{
}

template<typename T>
inline Vector4<T>::Vector4()
{
}

template<typename T>
inline Vector4<T>::~Vector4()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}
