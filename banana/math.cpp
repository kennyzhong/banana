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

bool Vector2::operator<(const Vector2 &other)
{
	if (x < other.x && y < other.y)
		return true;
	return false;
}

bool Vector2::operator>(const Vector2 &other)
{
	if (x > other.x && y > other.y)
		return true;
	return false;
}

bool Vector2::operator<=(const Vector2 &other)
{
	if (x <= other.x && y <= other.y)
		return true;
	return false;
}

bool Vector2::operator>=(const Vector2 &other)
{
	if (x >= other.x && y >= other.y)
		return true;
	return false;
}

bool Vector2::operator==(const Vector2 &other)
{
	if (x == other.x && y == other.y)
		return true;
	return false;
}

bool Vector2::operator!=(const Vector2 &other)
{
	if (x != other.x || y != other.y)
		return true;
	return false;
}

// Vector3

Vector3 Vector3::operator+(const Vector3 &other)
{
	Vector3 result;
	result.x = other.x + x;
	result.y = other.y + y;
	result.z = other.z + z;
	return result;
}

Vector3 Vector3::operator-(const Vector3 &other)
{
	Vector3 result;
	result.x = x - other.x;
	result.y = y - other.y;
	result.z = z - other.z;
	return result;
}

void Vector3::operator+=(const Vector3 &other)
{
	x += other.x;
	y += other.y;
	z += other.z;
}

void Vector3::operator-=(const Vector3 &other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
}

Vector3 Vector3::operator-()
{
	Vector3 result;
	result.x = -x;
	result.y = -y;
	result.z = -z;
	return result;
}

Vector3 Vector3::operator*(float f)
{
	Vector3 result;
	result.x = x * f;
	result.y = y * f;
	result.z = z * f;
	return result;
}

Vector3 Vector3::operator/(float f)
{
	Vector3 result;
	result.x = x / f;
	result.y = y / f;
	result.z = z / f;
	return result;
}

void Vector3::operator*=(float f)
{
	x *= f;
	y *= f;
	z *= f;
}
void Vector3::operator/=(float f)
{
	x /= f;
	y /= f;
	z /= f;
}

bool Vector3::operator<(const Vector3 &other)
{
	if (x < other.x && y < other.y && z < other.z)
		return true;
	return false;
}

bool Vector3::operator>(const Vector3 &other)
{
	if (x > other.x && y > other.y && z > other.z)
		return true;
	return false;
}

bool Vector3::operator<=(const Vector3 &other)
{
	if (x <= other.x && y <= other.y && z <= other.z)
		return true;
	return false;
}

bool Vector3::operator>=(const Vector3 &other)
{
	if (x >= other.x && y >= other.y && z >= other.z)
		return true;
	return false;
}

bool Vector3::operator==(const Vector3 &other)
{
	if (x == other.x && y == other.y && z == other.z)
		return true;
	return false;
}

bool Vector3::operator!=(const Vector3 &other)
{
	if (x != other.x || y != other.y || z != other.z)
		return true;
	return false;
}

float Length(Vector3 input)
{
	return (float)sqrtf(input.x * input.x + input.y * input.y + input.z * input.z);
}

void Normalize(Vector3 &input)
{
	float l = Length(input);
	if (l != 0)
	{
		input.x = input.x / l;
		input.y = input.y / l;
		input.z = input.z / l;
	}
}

void Rotate(Vector3 &input, float angle, Vector3 axis)
{
	Rotate(input, Quaternion(angle, axis));
}

void Rotate(Vector3 &input, Quaternion quat)
{
	Quaternion conjugate = quat;
	Conjugate(conjugate);

	Quaternion w = quat * input * conjugate;

	input.x = w.x;
	input.y = w.y;
	input.z = w.z;
}

Vector3 Cross(Vector3 a, Vector3 b)
{
	return Vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}

float Dot(Vector3 a, Vector3 b)
{
	return a.x * b.x + a.y * b.y + a.x * b.z;
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

Matrix4 Matrix4_perspective(float fovy, float aspect, float znear, float zfar)
{
	Matrix4 result = { 0 }; 
	float h, w;
	h = (float)tanf(fovy / 360.0f * M_PI) * znear;
	w = h * aspect;
	result = Matrix4_frustum(-w, w, -h, h, znear, zfar);
	return result;
}

Matrix4 Matrix4_frustum(float left, float right, float bottom, float top, float znear, float zfar)
{
	Matrix4 result = { 0 };
	result.m00 = (2.0f * znear) / (right - left);
	result.m20 = (right + left) / (right - left);
	result.m11 = (2.0f*znear) / (top - bottom);
	result.m21 = (top + bottom) / (top - bottom);
	result.m22 = -(zfar + znear) / (zfar - znear);
	result.m32 = -(2.0f*zfar*znear) / (zfar - znear);
	result.m23 = -1.0f;
	return result;
}

Matrix4 Matrix4_rotate(float angle, float x, float y, float z)
{
	float c, s, norm;
	Matrix4 m = Matrix4_identity();

	c = cosf(M_PI*angle / 180.0f);
	s = sinf(M_PI*angle / 180.0f);
	norm = Length(Vector3(x, y, z));

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

Matrix4 Matrix4_rotate(Quaternion quat)
{
	Vector3 forward = Vector3(2.0f * (quat.x * quat.z - quat.w * quat.y), 2.0f *
		(quat.y * quat.z + quat.w * quat.x), 1.0f - 2.0f * (quat.x * quat.x + quat.y * quat.y));
	Vector3 up = Vector3(2.0f * (quat.x * quat.y + quat.w * quat.z), 1.0f - 2.0f *
		(quat.x * quat.x + quat.z * quat.z), 2.0f * (quat.y * quat.z - quat.w * quat.x));
	Vector3 right = Vector3(1.0f - 2.0f * (quat.y * quat.y + quat.z * quat.z), 2.0f * 
		(quat.x * quat.y - quat.w * quat.z), 2.0f * (quat.x * quat.z + quat.w * quat.y));
	return Matrix4_lookat(forward, up, right);
}

Matrix4 Matrix4_scale(float x, float y, float z)
{
	Matrix4 m = Matrix4_identity();
	m.m00 = x;
	m.m11 = y;
	m.m22 = z;
	return m;
}

Matrix4 Matrix4_lookat(Vector3 f, Vector3 up, Vector3 r)
{
	
	Matrix4 m = Matrix4_identity();

	m.m00 = r.x;		m.m01 = r.y;		m.m02 = r.z;
	m.m10 = up.x,		m.m11 = up.y;		m.m12 = up.z;
	m.m20 = -f.x;		m.m21 = -f.y;		m.m22 = -f.z;
	return m;
}

float ToDegrees(float radians)
{
	return tanf(radians*M_PI / 180.0f);
}

float ToRadians(float degrees)
{
	return ((180.0f * degrees) / M_PI);
}

float Clamp(float input, float  min, float max)
{
	if (input < min) return min;
	if (input > max) return max;
	return input;
}

// Quaternion
float Length(Quaternion quat)
{
	return (float)sqrtf(quat.x * quat.x + quat.y * quat.y + quat.z * quat.z + quat.w * quat.w);
}

void Normalize(Quaternion &quat)
{
	float length = Length(quat);
	quat.x /= length;
	quat.y /= length;
	quat.z /= length;
	quat.w /= length;
}

void Conjugate(Quaternion &quat)
{
	quat.x = -quat.x;
	quat.y = -quat.y;
	quat.z = -quat.z;
}

Quaternion Quaternion::operator*(const Quaternion &other)
{
	Quaternion result;

	result.w = w * other.w - x * other.x - y * other.y - z * other.z;
	result.x = x * other.w + w * other.x + y * other.z - z * other.y;
	result.y = y * other.w + w * other.y + z * other.x - x * other.z;
	result.z = z * other.w + w * other.z + x * other.y - y * other.x;

	return result;
}

Quaternion Quaternion::operator*(const Vector3 &other)
{
	Quaternion result;

	result.w = -x * other.x - y * other.y - z * other.z;
	result.x = w * other.x + y * other.z - z * other.y;
	result.y = w * other.y + z * other.x - x * other.z;
	result.z = w * other.z + x * other.y - y * other.x;

	return result;
}

Vector3 GetForward(Quaternion quat)
{
	Vector3 result = Vector3(0.0f, 0.0f, 1.0f);
	Rotate(result, quat);
	return result;
}

Vector3 GetBackward(Quaternion quat)
{
	Vector3 result = Vector3(0.0f, 0.0f, -1.0f);
	Rotate(result, quat);
	return result;
}

Vector3 GetRight(Quaternion quat)
{
	Vector3 result = Vector3(1.0f, 0.0f, 0.0f);
	Rotate(result, quat);
	return result;
}

Vector3 GetLeft(Quaternion quat)
{
	Vector3 result = Vector3(-1.0f, 0.0f, 0.0f);
	Rotate(result, quat);
	return result;
}