#include "Framework.h"
#include "Math.h"

const float Math::PI = 3.14159265f;
const float Math::EPSILON = 0.000001f;

float Math::Modulo(float val1, float val2)
{
	while (val1 - val2 >= 0)
		val1 -= val2;

	return val1;
}

float Math::ToRadian(float degree)
{
	return degree * PI / 180.0f;
}

float Math::ToDegree(float radian)
{
	return radian * 180.0f / PI;
}

float Math::Random(float r1, float r2)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = r2 - r1;
	float val = random * diff;

	return r1 + val;
}

XMFLOAT2 Math::RandomVec2(float r1, float r2)
{
	XMFLOAT2 result;
	result.x = Random(r1, r2);
	result.y = Random(r1, r2);

	return result;
}

XMFLOAT3 Math::RandomVec3(float r1, float r2)
{
	XMFLOAT3 result;
	result.x = Random(r1, r2);
	result.y = Random(r1, r2);
	result.z = Random(r1, r2);

	return result;
}

XMCOLOR Math::RandomColor3()
{
	XMCOLOR result;
	result.r = Math::Random(0.0f, 1.0f);
	result.g = Math::Random(0.0f, 1.0f);
	result.b = Math::Random(0.0f, 1.0f);
	result.a = 1.0f;

	return result;
}

XMCOLOR Math::RandomColor4()
{
	XMCOLOR result;
	result.r = Math::Random(0.0f, 1.0f);
	result.g = Math::Random(0.0f, 1.0f);
	result.b = Math::Random(0.0f, 1.0f);
	result.a = Math::Random(0.0f, 1.0f);

	return result;
}

float Math::Clamp(float value, float min, float max)
{
	value = value > max ? max : value;
	value = value < min ? min : value;

	return value;
}

void Math::LerpMatrix(OUT XMMATRIX & out, const XMMATRIX& m1, const XMMATRIX& m2, float amount)
{
	XMFLOAT4X4 tout, tm1, tm2;
	XMStoreFloat4x4(&tout, out);
	XMStoreFloat4x4(&tm1, m1);
	XMStoreFloat4x4(&tm2, m2);

	tout._11 = tm1._11 + (tm2._11 - tm1._11) * amount;
	tout._12 = tm1._12 + (tm2._12 - tm1._12) * amount;
	tout._13 = tm1._13 + (tm2._13 - tm1._13) * amount;
	tout._14 = tm1._14 + (tm2._14 - tm1._14) * amount;

	tout._21 = tm1._21 + (tm2._21 - tm1._21) * amount;
	tout._22 = tm1._22 + (tm2._22 - tm1._22) * amount;
	tout._23 = tm1._23 + (tm2._23 - tm1._23) * amount;
	tout._24 = tm1._24 + (tm2._24 - tm1._24) * amount;

	tout._31 = tm1._31 + (tm2._31 - tm1._31) * amount;
	tout._32 = tm1._32 + (tm2._32 - tm1._32) * amount;
	tout._33 = tm1._33 + (tm2._33 - tm1._33) * amount;
	tout._34 = tm1._34 + (tm2._34 - tm1._34) * amount;

	tout._41 = tm1._41 + (tm2._41 - tm1._41) * amount;
	tout._42 = tm1._42 + (tm2._42 - tm1._42) * amount;
	tout._43 = tm1._43 + (tm2._43 - tm1._43) * amount;
	tout._44 = tm1._44 + (tm2._44 - tm1._44) * amount;

	out = XMLoadFloat4x4(&tout);
}

XMFLOAT4 Math::LookAt(const XMFLOAT3& origin, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR f;
	f =  XMLoadFloat3(&origin) - XMLoadFloat3(&target);
	f = XMVector4Normalize(f);

	XMVECTOR s;
	s = XMVector3Cross(XMLoadFloat3(&up), f);
	s = XMVector4Normalize(s);

	XMVECTOR u;
	u = XMVector3Cross(f, s);

	Vector3 fF;
	Vector3 fS;
	Vector3 fU;

	XMStoreFloat3(&fF, f);
	XMStoreFloat3(&fS, s);
	XMStoreFloat3(&fU, u);

	float z = 1.0f + fS.x + fU.y + fF.z;
	float fd = 2.0f * sqrtf(z);

	XMFLOAT4 result;

	if (z > Math::EPSILON)
	{
		result.w = 0.25f * fd;
		result.x = (fF.y - fU.z) / fd;
		result.y = (fS.z - fF.x) / fd;
		result.z = (fU.x - fS.y) / fd;
	}
	else if (fS.x > fU.y && fS.x > fF.z)
	{
		fd = 2.0f * sqrtf(1.0f + fS.x - fU.y - fF.z);
		result.w = (fF.y - fU.z) / fd;
		result.x = 0.25f * fd;
		result.y = (fU.x + fS.y) / fd;
		result.z = (fS.z + fF.x) / fd;
	}
	else if (fU.y > fF.z)
	{
		fd = 2.0f * sqrtf(1.0f + fU.y - fS.x - fF.z);
		result.w = (fS.z - fF.x) / fd;
		result.x = (fU.x - fS.y) / fd;
		result.y = 0.25f * fd;
		result.z = (fF.y + fU.z) / fd;
	}
	else
	{
		fd = 2.0f * sqrtf(1.0f + fF.z - fS.x - fU.y);
		result.w = (fU.x - fS.y) / fd;
		result.x = (fS.z + fF.x) / fd;
		result.y = (fF.y + fU.z) / fd;
		result.z = 0.25f * fd;
	}

	return result;
}

int Math::Random(int r1, int r2)
{
	return (int)(rand() % (r2 - r1 + 1)) + r1;
}

float Math::Gaussian(float val, UINT blurCount)
{
	float a = 1.0f / sqrtf(2 * PI * (float)blurCount * (float)blurCount);
	float c = 2.0f * (float)blurCount * (float)blurCount;
	float b = exp(-(val * val) / c);

	return a * b;
}

void Math::MatrixDecompose(const XMMATRIX & m, OUT Vector3 & S, OUT Vector3 & R, OUT Vector3 & T)
{
	XMVECTOR rotation;
	XMVECTOR vScale = XMLoadFloat3(&S);
	XMVECTOR vTranslation = XMLoadFloat3(&T);
	XMMatrixDecompose(&vScale, &rotation, &vTranslation, m);


	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, XMMatrixRotationQuaternion(rotation));

	R.x = asin(-temp._32);
	R.y = atan2(temp._31, temp._33);
	R.z = atan2(temp._12, temp._22);
}
