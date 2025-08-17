#pragma once
#include <cmath> 
#include <iostream>


struct Vec2
{
	union
	{
		struct
		{
			float x;
			float y;
		};
		struct
		{
			float r;
			float g;
		};
		struct
		{
			float s;
			float t;
		};
	};
	Vec2()
		: x(0.0f), y(0.0f)
	{
	};
	Vec2(float v1, float v2)
		: x(v1), y(v2)
	{
	};
	// Vector addition
	Vec2 operator+(const Vec2& other) const
	{
		return Vec2(x + other.x, y + other.y);
	}

	// Vector scalar multiplication
	Vec2 operator*(float scalar) const
	{
		return Vec2(x * scalar, y * scalar);
	}

	// Vector multiplicaton
	float operator*(const Vec2& other) const
	{
		return float(x * other.x + y * other.y);
	}
};

std::ostream& operator<<(std::ostream& os, const Vec2& vec4)
{
	os << "X: " << vec4.x << ", Y: " << vec4.y;
	return os;
}


struct Vec3
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		struct
		{
			float r;
			float g;
			float b;
		};
		struct
		{
			float s;
			float t;
			float p;
		};
	};

	Vec3()
		: x(0.0f), y(0.0f), z(0.0f)
	{
	};
	Vec3(float v1, float v2, float v3)
		: x(v1), y(v2), z(v3)
	{
	};
	// Vector addition
	Vec3 operator+(const Vec4& other) const
	{
		return Vec3(x + other.x, y + other.y, z + other.z);
	}

	// Vector scalar multiplication
	Vec3 operator*(float scalar) const
	{
		return Vec3(x * scalar, y * scalar, z * scalar);
	}

	// Vector multiplicaton
	float operator*(const Vec3& other) const
	{
		return float(x * other.x + y * other.y + z * other.z);
	}
};
std::ostream& operator<<(std::ostream& os, const Vec3& vec4)
{
	os << "X: " << vec4.x << ", Y: " << vec4.y << ", Z: " << vec4.z;
	return os;
}

inline Vec3 Cross(Vec3& v1, Vec3& v2)
{
	return Vec3((v1.y * v2.z - v1.z * v2.y),
		(v1.z * v2.x - v1.x * v2.z),
		(v1.x * v2.y - v1.y * v2.x));
}


struct Vec4
{ 
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		struct
		{
			float r;
			float g;
			float b;
			float a;
		};
		struct
		{
			float s;
			float t;
			float p;
			float q;
		};
	};

	Vec4() 
		: x(0.0f), y(0.0f), z(0.0f), w(0.0) {};
	Vec4(float v1, float v2, float v3, float v4)
		: x(v1), y(v2), z(v3), w(v4) {};

	// Vector addition
	Vec4 operator+(const Vec4& other) const
	{
		return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
	}

	// Vector scalar multiplication
	Vec4 operator*(float scalar) const
	{
		return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
	}

	// Vector multiplicaton
	float operator*(const Vec4& other) const
	{
		return float(x * other.x + y * other.y + z * other.z + w * other.w);
	}
	
	
};
std::ostream& operator<<(std::ostream& os, const Vec4& vec4) 
{
	os << "X: " << vec4.x << ", Y: " << vec4.y << ", Z: " << vec4.z << ", W: " << vec4.w;
	return os;
}

inline Vec4 Cross(Vec4& v1, Vec4& v2)
{
	return Vec4((v1.y * v2.z - v1.z * v2.y),
				(v1.z * v2.x - v1.x * v2.z),
				(v1.x * v2.y - v1.y * v2.x),
				0);
}
