#pragma once
class Math
{
public:
	static const float PI;
	static const float EPSILON;

	static float Modulo(float val1, float val2);

	static float ToRadian(float degree);
	static float ToDegree(float radian);

	static int Random(int r1, int r2);
	static float Random(float r1, float r2);
	
	static XMFLOAT2 RandomVec2(float r1, float r2);
	static XMFLOAT3 RandomVec3(float r1, float r2);
	static XMCOLOR RandomColor3();
	static XMCOLOR RandomColor4();


	static float Clamp(float value, float min, float max);

	static void LerpMatrix(OUT XMMATRIX& out, const XMMATRIX& m1, const XMMATRIX& m2, float amount);

	static XMFLOAT4 LookAt(const XMFLOAT3& origin, const XMFLOAT3& target, const XMFLOAT3& up);
	static float Gaussian(float val, UINT blurCount);

	static void MatrixDecompose(const XMMATRIX& m, OUT Vector3& S, OUT Vector3& R, OUT Vector3& T);
};