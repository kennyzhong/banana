#include <math.h>
#include "math.h"

Vector2 Vector2::operator+(const Vector2 &other)
{
	Vector2 result;
	result.x = other.x + x;
	result.y = other.y + y;
	return result;
}

Vector2 Vector2::operator-(const Vector2 &other)
{
	Vector2 result;
	result.x = x - other.x;
	result.y = y - other.y;
	return result;
}

void Vector2::operator+=(const Vector2 &other)
{
	x += other.x;
	y += other.y;
}

void Vector2::operator-=(const Vector2 &other)
{
	x -= other.x;
	y -= other.y;
}

Vector2 Vector2::operator*(float f)
{
	Vector2 result;
	result.x = x * f;
	result.y = y * f;
	return result;
}

Vector2 Vector2::operator/(float f)
{
	Vector2 result;
	result.x = x / f;
	result.y = y / f;
	return result;
}

void Vector2::operator*=(float f)
{
	x *= f;
	y *= f;
}
void Vector2::operator/=(float f)
{
	x /= f;
	y /= f;
}

Matrix4 Matrix4::operator*(const Matrix4 &m2)
{
	Matrix4 m = { 0 };

	for (int i = 0; i < 4; ++i)
	{
		m.data[i * 4 + 0] =
			(data[i * 4 + 0] * m2.data[0 * 4 + 0]) +
			(data[i * 4 + 1] * m2.data[1 * 4 + 0]) +
			(data[i * 4 + 2] * m2.data[2 * 4 + 0]) +
			(data[i * 4 + 3] * m2.data[3 * 4 + 0]);

		m.data[i * 4 + 1] =
			(data[i * 4 + 0] * m2.data[0 * 4 + 1]) +
			(data[i * 4 + 1] * m2.data[1 * 4 + 1]) +
			(data[i * 4 + 2] * m2.data[2 * 4 + 1]) +
			(data[i * 4 + 3] * m2.data[3 * 4 + 1]);

		m.data[i * 4 + 2] =
			(data[i * 4 + 0] * m2.data[0 * 4 + 2]) +
			(data[i * 4 + 1] * m2.data[1 * 4 + 2]) +
			(data[i * 4 + 2] * m2.data[2 * 4 + 2]) +
			(data[i * 4 + 3] * m2.data[3 * 4 + 2]);

		m.data[i * 4 + 3] =
			(data[i * 4 + 0] * m2.data[0 * 4 + 3]) +
			(data[i * 4 + 1] * m2.data[1 * 4 + 3]) +
			(data[i * 4 + 2] * m2.data[2 * 4 + 3]) +
			(data[i * 4 + 3] * m2.data[3 * 4 + 3]);
	}

	return m;
}

Matrix4 Matrix4_identity()
{
	Matrix4 result = { 0 };
	result.m00 = 1.0f;
	result.m11 = 1.0f;
	result.m22 = 1.0f;
	result.m33 = 1.0f;
	return result;
}

Matrix4 Matrix4_ortho(float left, float right, float top, float bottom, float znear, float zfar)
{
	Matrix4 m = { 0 };
	m.m00 = +2.0f / (right - left);
	m.m30 = -(right + left) / (right - left);
	m.m11 = +2.0f / (top - bottom);
	m.m31 = -(top + bottom) / (top - bottom);
	m.m22 = -2.0f / (zfar - znear);
	m.m32 = -(zfar + znear) / (zfar - znear);
	m.m33 = 1.0f;
	return m;
}

Matrix4 Matrix4_translate(float x, float y, float z)
{
	Matrix4 result = Matrix4_identity();
	result.m30 = x;
	result.m31 = y;
	result.m32 = z;
	return result;
}

Matrix4 Matrix4_rotate(float angle, float x, float y, float z)
{
	float c, s, norm;
	Matrix4 m = Matrix4_identity();

	c = cosf(M_PI*angle / 180.0f);
	s = sinf(M_PI*angle / 180.0f);
	norm = sqrtf(x*x + y*y + z*z);

	x /= norm;
	y /= norm;
	z /= norm;

	m.m00 = x*x*(1 - c) + c;
	m.m10 = y*x*(1 - c) - z*s;
	m.m20 = z*x*(1 - c) + y*s;

	m.m01 = x*y*(1 - c) + z*s;
	m.m11 = y*y*(1 - c) + c;
	m.m21 = z*y*(1 - c) - x*s;

	m.m02 = x*z*(1 - c) - y*s;
	m.m12 = y*z*(1 - c) + x*s;
	m.m22 = z*z*(1 - c) + c;

	return m;
}

Matrix4 Matrix4_scale(float x, float y, float z)
{
	Matrix4 m = Matrix4_identity();
	m.m00 = x;
	m.m11 = y;
	m.m22 = z;
	return m;
}

float ToDegrees(float radians)
{
	return tanf(radians*M_PI / 180.0f);
}

float Clamp(float input, float  min, float max)
{
	if (input < min) return min;
	if (input > max) return max;
	return input;
}