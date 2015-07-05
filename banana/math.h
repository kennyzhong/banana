#ifndef MATH_H
#define MATH_H

struct Vector2
{
	Vector2(float x, float y) : x(x), y(y) {};
	Vector2() : x(0.0f), y(0.0f) {};

	float x;
	float y;

	Vector2 operator+(const Vector2 &other);
	Vector2 operator-(const Vector2 &other);

	bool operator<(const Vector2 &other);
	bool operator>(const Vector2 &other);

	bool operator<=(const Vector2 &other);
	bool operator>=(const Vector2 &other);

	bool operator==(const Vector2 &other);
	bool operator!=(const Vector2 &other);

	void operator+=(const Vector2 &other);
	void operator-=(const Vector2 &other);
	
	Vector2 operator*(float f);
	Vector2 operator/(float f);

	void operator*=(float f);
	void operator/=(float f);
};

struct Vector3
{
	Vector3(float x, float y, float z) : x(x), y(y) , z(z){};
	Vector3() : x(0.0f), y(0.0f), z(0.0f) {};
	Vector3(Vector2 i) : x(i.x), y(i.y), z(0.0f){};

	float x;
	float y;
	float z;

	Vector3 operator+(const Vector3 &other);
	Vector3 operator-(const Vector3 &other);

	bool operator<(const Vector3 &other);
	bool operator>(const Vector3 &other);

	bool operator<=(const Vector3 &other);
	bool operator>=(const Vector3 &other);

	bool operator!=(const Vector3 &other);
	bool operator==(const Vector3 &other);

	void operator+=(const Vector3 &other);
	void operator-=(const Vector3 &other);

	Vector3 operator*(float f);
	Vector3 operator/(float f);

	void operator*=(float f);
	void operator/=(float f);
};


#ifndef M_PI
#    define M_PI 3.14159265358979323846f
#endif

struct Matrix4
{
	union
	{
		float data[16];
		struct
		{
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};
	};

	Matrix4 operator*(const Matrix4 &other);
};

Matrix4 Matrix4_identity();
Matrix4 Matrix4_ortho(float left, float right, float top, float bottom, float znear, float zfar);
Matrix4 Matrix4_translate(float x, float y, float z);
Matrix4 Matrix4_rotate(float angle, float x, float y, float z);
Matrix4 Matrix4_scale(float x, float y, float z);

float ToDegrees(float radians);
float ToRadians(float degrees);
float Clamp(float input, float min, float max);

#endif // MATH_H