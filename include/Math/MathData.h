#pragma once

#include <cmath>
#include <cassert>
#include <cfloat>
#include <iostream>



//--------------------Vectors-----------------------
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
		float data[2];
	};
	Vec2()
		: x(0.0f), y(0.0f)
	{
	};
	Vec2(float v1, float v2)
		: x(v1), y(v2)
	{
	};

	// Static Memeber declaration 
	static const Vec2 Zero;
	static const Vec2 One;
	static const Vec2 Up;

	// Vector addition
	Vec2 operator+(const Vec2& other) const
	{
		return Vec2(x + other.x, y + other.y);
	}

	// Vector Subtraction
	Vec2 operator-(const Vec2& other) const
	{
		return Vec2(x - other.x, y - other.y);
	}

	// Vector scalar multiplication
	Vec2 operator*(float scalar) const
	{
		return Vec2(x * scalar, y * scalar);
	}

	// Vector multiplicaton
	float operator*(const Vec2& other) const
	{
		return (x * other.x + y * other.y);
	}

	// Vector scalar division
	Vec2 operator/(float scalar) const
	{
		return Vec2(x / scalar, y / scalar);
	}

	Vec2& operator+=(const Vec2& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	Vec2& operator-=(const Vec2& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vec2& operator*=(const float scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	// Divides this Vec3's components by a scalar
	Vec2& operator/=(float scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}
	Vec2 operator-() const
	{
		return Vec2(-x, -y);
	}

	bool operator==(const Vec2& other) const
	{
		return (fabsf(x - other.x) < FLT_EPSILON) &&
			(fabsf(y - other.y) < FLT_EPSILON);
	}

	bool operator!=(const Vec2& other) const
	{
		return !(*this == other);
	}

	const float operator[](int index) const
	{
		assert(index >= 0 && index < 2);
		return data[index];
	}

	float& operator[](int index)
	{
		assert(index >= 0 && index < 2);
		return data[index];
	}
};

std::ostream& operator<<(std::ostream& os, const Vec2& vec4)
{
	os << "X: " << vec4.x << ", Y: " << vec4.y;
	return os;
}

// For Scalar multiplication scalar * Vectord
inline Vec2 operator*(float scalar, const Vec2& vec)
{
	return vec * scalar; // Reuses the member operator*
}

// Default Vec2`s
inline const Vec2 Vec2::Zero = Vec2(0.0f, 0.0f);
inline const Vec2 Vec2::One = Vec2(1.0f, 1.0f);
inline const Vec2 Vec2::Up = Vec2(0.0f, 1.0f);

inline float Dot(const Vec2& v1, const Vec2& v2)
{
	return(v1.x * v2.x + v1.y * v2.y);
}

inline float Dist(const Vec2& v1, const Vec2& v2)
{
	return (sqrt((v1.x - v2.x) * (v1.x - v2.x)
		+ (v1.y - v2.y) * (v1.y - v2.y)));
}

inline float DistanceSquared(const Vec2& v1, const Vec2& v2)
{
	return ((v1.x - v2.x) * (v1.x - v2.x)
		+ (v1.y - v2.y) * (v1.y - v2.y));
}

inline float Length(const Vec2& v1)
{
	return (sqrt((v1.x * v1.x) + (v1.y * v1.y)));
}

inline float LengthSquared(const Vec2& v1)
{
	return ((v1.x * v1.x) + (v1.y * v1.y));
}

inline Vec2 Normalize(const Vec2& v1)
{
	float temp = Length(v1);
	if (temp > FLT_EPSILON)
	{
		return Vec2(v1.x / temp, v1.y / temp);
	}
	else
	{
		return Vec2(0.0f, 0.0f);
	}
}

// Projects v1 onto v2
inline Vec2 Project(const Vec2& v1, const Vec2& v2)
{
	float v2LengthSquared = LengthSquared(v2);
	if (v2LengthSquared < FLT_EPSILON)
	{
		return Vec2::Zero;
	}
	float scale = Dot(v1, v2) / v2LengthSquared;
	return v2 * scale;
}

Vec2 reflect(const Vec2& incident, const Vec2& normal)
{
	return incident - normal * 2.0f * Dot(incident, normal);
}

// Incident and Normal Vectors must be normalized
Vec2 refract(const Vec2& incident, const Vec2& normal, float eta)
{
	float dot_ni = Dot(normal, incident);
	float k = 1.0f - eta * eta * (1.0f - dot_ni * dot_ni);

	if (k < 0.0f)
	{
		// Total internal reflection
		return Vec2::Zero;
	}
	else
	{
		return incident * eta - normal * (eta * dot_ni + sqrt(k));
	}
}


// Vec3 Implimentation
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
		float data[3];
	};

	Vec3()
		: x(0.0f), y(0.0f), z(0.0f)
	{
	};
	Vec3(float v1, float v2, float v3)
		: x(v1), y(v2), z(v3)
	{
	};

	// Static Memeber declaration 
	static const Vec3 Zero;
	static const Vec3 One;
	static const Vec3 Up;

	// Vector addition
	Vec3 operator+(const Vec3& other) const
	{
		return Vec3(x + other.x, y + other.y, z + other.z);
	}

	// Vector Subtraction
	Vec3 operator-(const Vec3& other) const
	{
		return Vec3(x - other.x, y - other.y, z - other.z);
	}

	// Vector scalar multiplication
	Vec3 operator*(float scalar) const
	{
		return Vec3(x * scalar, y * scalar, z * scalar);
	}

	// Vector multiplicaton
	float operator*(const Vec3& other) const
	{
		return (x * other.x + y * other.y + z * other.z);
	}

	// Vector scalar division
	Vec3 operator/(float scalar) const
	{
		return Vec3(x / scalar, y / scalar, z / scalar);
	}

	Vec3& operator+=(const Vec3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vec3& operator-=(const Vec3& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Vec3& operator*=(const float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

    // Divides this Vec3's components by a scalar
    Vec3& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

	Vec3 operator-() const
	{
		return Vec3(-x, -y, -z);
	}

	bool operator==(const Vec3& other) const
	{
		return (fabsf(x - other.x) < FLT_EPSILON) &&
			(fabsf(y - other.y) < FLT_EPSILON) &&
			(fabsf(z - other.z) < FLT_EPSILON);
	}

	bool operator!=(const Vec3& other) const
	{
		return !(*this == other);
	}

	const float operator[](int index) const
	{
		assert(index >= 0 && index < 3);
		return data[index];
	}

	float& operator[](int index)
	{
		assert(index >= 0 && index < 3);
		return data[index];
	}
};

std::ostream& operator<<(std::ostream& os, const Vec3& vec4)
{
	os << "X: " << vec4.x << ", Y: " << vec4.y << ", Z: " << vec4.z;
	return os;
}

// For Scalar multiplication scalar * Vectord
inline Vec3 operator*(float scalar, const Vec3& vec)
{
	return vec * scalar; // Reuses the member operator*
}

// Default Vec3`s
inline const Vec3 Vec3::Zero = Vec3(0.0f, 0.0f, 0.0f);
inline const Vec3 Vec3::One = Vec3(1.0f, 1.0f, 1.0f);
inline const Vec3 Vec3::Up = Vec3(0.0f, 1.0f, 0.0f);

inline float Dot3D(const Vec3& v1, const Vec3& v2)
{
	return(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

inline Vec3 Cross(const Vec3& v1, const Vec3& v2)
{
	return Vec3((v1.y * v2.z - v1.z * v2.y),
		(v1.z * v2.x - v1.x * v2.z),
		(v1.x * v2.y - v1.y * v2.x));
}

inline float Dist(const Vec3& v1, const Vec3& v2)
{
	return (sqrt((v1.x - v2.x) * (v1.x - v2.x)
					+ (v1.y - v2.y) * (v1.y - v2.y)
					+ (v1.z - v2.z) * (v1.z - v2.z)));
}

inline float DistanceSquared(const Vec3& v1, const Vec3& v2)
{
	return ((v1.x - v2.x) * (v1.x - v2.x)
		+ (v1.y - v2.y) * (v1.y - v2.y)
		+ (v1.z - v2.z) * (v1.z - v2.z));
}

inline float Length(const Vec3& v1)
{
	return (sqrt((v1.x * v1.x) + (v1.y * v1.y) + (v1.z * v1.z)));
}

inline float LengthSquared(const Vec3& v1)
{
	return ((v1.x * v1.x) + (v1.y * v1.y) + (v1.z * v1.z));
}

inline Vec3 Normalize(const Vec3& v1)
{
	float temp = Length(v1);
	if (temp > FLT_EPSILON)
	{
		return Vec3(v1.x / temp, v1.y / temp, v1.z / temp);
	}
	else
	{
		return Vec3(0.0f, 0.0f, 0.0f);
	}
}

// Projects v1 onto v2
inline Vec3 Project(const Vec3& v1, const Vec3& v2)
{
	float v2LengthSquared = LengthSquared(v2);
	if (v2LengthSquared < FLT_EPSILON)
	{
		return Vec3::Zero;
	}
	float scale = Dot3D(v1, v2) / v2LengthSquared;
	return v2 * scale;
}

Vec3 reflect(const Vec3& incident, const Vec3& normal)
{
	return incident - normal * 2.0f * Dot3D(incident, normal);
}

// Incident and Normal Vectors must be normalized
Vec3 refract(const Vec3& incident, const Vec3& normal, float eta)
{
	float dot_ni = Dot3D(normal, incident);
	float k = 1.0f - eta * eta * (1.0f - dot_ni * dot_ni);

	if (k < 0.0f)
	{
		// Total internal reflection
		return Vec3::Zero;
	}
	else
	{
		return incident * eta - normal * (eta * dot_ni + sqrt(k));
	}
}


// Vec4 Implementation
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
		float data[4];
	};

	Vec4()
		: x(0.0f), y(0.0f), z(0.0f), w(0.0)
	{
	};
	Vec4(float v1, float v2, float v3, float v4)
		: x(v1), y(v2), z(v3), w(v4)
	{
	};

	// Satic Member Declaration
	static const Vec4 Zero;
	static const Vec4 One;
	static const Vec4 Up;

	// Vector addition
	Vec4 operator+(const Vec4& other) const
	{
		return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
	}

	// Vector Subtraction
	Vec4 operator-(const Vec4& other) const
	{
		return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
	}

	// Vector scalar multiplication
	Vec4 operator*(float scalar) const
	{
		return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
	}

	// Vector multiplicaton (Dot3D Product)
	float operator*(const Vec4& other) const
	{
		return (x * other.x + y * other.y + z * other.z + w * other.w);
	}

	// Vector scalar division
	Vec4 operator/(float scalar)const 
	{
		return Vec4(x / scalar, y / scalar, z / scalar, w / scalar);
	}

	Vec4& operator+=(const Vec4& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
	}

	Vec4& operator-=(const Vec4& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
		return *this;
	}

	Vec4& operator*=(const float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
	}

	// Divides this Vec3's components by a scalar
	Vec4& operator/=(float scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;
		return *this;
	}

	Vec4 operator-() const
	{
		return Vec4(-x, -y, -z, -w);
	}

	bool operator==(const Vec4& other) const
	{
		return (fabsf(x - other.x) < FLT_EPSILON) &&
				(fabsf(y - other.y) < FLT_EPSILON) &&
				(fabsf(z - other.z) < FLT_EPSILON) &&
				(fabsf(w - other.w) < FLT_EPSILON);
	}

	bool operator!=(const Vec4& other) const
	{
		return !(*this == other);
	}

	const float operator[](int index) const
	{
		assert(index >= 0 && index < 4);
		return data[index];
	}
	
	float& operator[](int index)
	{
		assert(index >= 0 && index < 4); 
		return data[index];
	}

};

std::ostream& operator<<(std::ostream& os, const Vec4& vec4)
{
	os << "X: " << vec4.x << ", Y: " << vec4.y << ", Z: " << vec4.z << ", W: " << vec4.w;
	return os;
}

// For Scalar multiplication scalar * Vectord
inline Vec4 operator*(float scalar, const Vec4& vec)
{
	return vec * scalar; // Reuses the member operator*
}

// Default Vec4`s
inline const Vec4 Vec4::Zero = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
inline const Vec4 Vec4::One = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
inline const Vec4 Vec4::Up = Vec4(0.0f, 1.0f, 0.0f, 0.0f);

inline float Dot3D(const Vec4& v1, const Vec4& v2)
{
	return(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

inline Vec4 Cross(const Vec4& v1, const Vec4& v2)
{
	return Vec4((v1.y * v2.z - v1.z * v2.y),
		(v1.z * v2.x - v1.x * v2.z),
		(v1.x * v2.y - v1.y * v2.x),
		0.0f);
}

inline float Dist(const Vec4& v1, const Vec4& v2)
{
	return (sqrt((v1.x - v2.x) * (v1.x - v2.x)
		+ (v1.y - v2.y) * (v1.y - v2.y)
		+ (v1.z - v2.z) * (v1.z - v2.z)));
}

inline float DistanceSquared(const Vec4& v1, const Vec4& v2)
{
	return ((v1.x - v2.x) * (v1.x - v2.x)
		+ (v1.y - v2.y) * (v1.y - v2.y)
		+ (v1.z - v2.z) * (v1.z - v2.z));
}

inline float Length(const Vec4& v1)
{
	return sqrt((v1.x * v1.x) + (v1.y * v1.y) + (v1.z * v1.z));
}

inline float LengthSquared(const Vec4& v1)
{
	return (v1.x * v1.x) + (v1.y * v1.y) + (v1.z * v1.z);
}

inline Vec4 Normalize(const Vec4& v1)
{
	float temp = Length(v1);
	if (temp > FLT_EPSILON)
	{
		return Vec4(v1.x / temp, v1.y / temp, v1.z / temp, 0.0f);
	}
	else 
	{
		return Vec4(0.0f,0.0f,0.0f,0.0f);
	}
}

// Projects v1 onto v2
inline Vec4 Project(const Vec4& v1, const Vec4& v2)
{
	float v2LengthSquared = LengthSquared(v2);
	if (v2LengthSquared < FLT_EPSILON)
	{
		return Vec4::Zero;
	}
	float scale = Dot3D(v1, v2) / v2LengthSquared;
	return v2 * scale;
}

// Normal vector must be normalized
inline Vec4 Reflect(const Vec4& incident, const Vec4& normal)
{
	Vec4 temp = incident - normal * 2.0f * Dot3D(incident, normal);
	temp.w = 0.0f;
	return temp;
}

// Incident and Normal Vectors must be normalized
inline Vec4 refract(const Vec4& incident, const Vec4& normal, float eta)
{
	float dot_ni = Dot3D(normal, incident);
	float k = 1.0f - eta * eta * (1.0f - dot_ni * dot_ni);

	if (k < 0.0f)
	{
		// Total internal reflection
		return Vec4::Zero;
	}
	else
	{
		Vec4 temp = incident * eta - normal * (eta * dot_ni + sqrt(k));
		temp.w = 0.0f;
		return temp;
	}
}

//------------------------- Matrices------------------------

struct Mat4
{
	union
	{
		float m[4][4];

		Vec4 columns[4];

		float data[16];
	};

	Mat4(float diagonal = 1.0f)
	{
		for (int i = 0; i < 16; i++)
		{
			data[i] = 0.0f;
		}

		m[1][1] = diagonal;
		m[2][2] = diagonal;
		m[3][3] = diagonal;
		m[3][3] = diagonal;	
	}

	Mat4(Vec4& col1, Vec4 col2, Vec4& col3, Vec4& col4)
	{
		columns[0] = col1;
		columns[1] = col2;
		columns[2] = col3;
		columns[3] = col4;
	}

	Mat4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
		m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
		m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
		m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
	}

	Mat4 operator*(const Mat4& other)
	{

	}
};