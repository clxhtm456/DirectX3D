#pragma once

class BinaryWriter
{
public:
	BinaryWriter();
	~BinaryWriter();

	void Open(wstring filePath, UINT openOption = CREATE_ALWAYS);
	void Open(string filePath, UINT openOption = CREATE_ALWAYS);
	void Close();

	void Bool(bool data);
	void Word(WORD data);
	void Int(int data);
	void UInt(UINT data);
	void Float(float data);
	void Double(double data);

	void Vector2(const Vector2& data);
	void Vector3(const Vector3& data);
	void Vector4(const Vector4& data);
	void Color3f(const Color& data);
	void Color4f(const Color& data);
	void Matrix(const Matrix& data);

	void String(const string& data);
	void BYTE(void* data, UINT dataSize);

protected:
	HANDLE fileHandle;
	DWORD size;
};

//////////////////////////////////////////////////////////////////////////

class BinaryReader
{
public:
	BinaryReader();
	~BinaryReader();

	void Open(wstring filePath);
	void Close();

	bool Bool();
	WORD Word();
	int Int();
	UINT UInt();
	float Float();
	double Double();

	XMFLOAT2 Vector2();
	XMFLOAT3 Vector3();
	XMFLOAT4 Vector4();
	XMCOLOR Color3f();
	XMCOLOR Color4f();
	XMMATRIX Matrix();

	string String();
	void BYTE(void** data, UINT dataSize);

protected:
	HANDLE fileHandle;
	DWORD size;
};