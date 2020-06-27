#pragma once

#include "Framework.h"
#include "Buffers.h"

class ViewProjectionBuffer : public ConstantBuffer
{
private:
	struct Data
	{
		Matrix view;
		Matrix projection;
		Matrix invView;
	}data;

public:
	ViewProjectionBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.view = XMMatrixIdentity();
		data.projection = XMMatrixIdentity();
	}

	void SetView(Matrix value)
	{
		data.invView = XMMatrixInverse(nullptr, value);
		data.view = XMMatrixTranspose(value);
		data.invView = XMMatrixTranspose(data.invView);
	}

	void SetProjection(Matrix value)
	{
		data.projection = XMMatrixTranspose(value);
	}

	Matrix GetProjection() { return data.projection; }
};

class WorldBuffer : public ConstantBuffer
{
private:
	struct Data
	{
		Matrix world;
	}data;

public:
	WorldBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.world = XMMatrixIdentity();
	}

	void SetWorld(Matrix value)
	{
		data.world = XMMatrixTranspose(value);
	}

	Matrix GetWorld() { return data.world; }
};

class ColorBuffer : public ConstantBuffer
{
public:
	struct Data
	{
		Color color;
	}data;

	ColorBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.color = Color(1, 1, 1, 1);
	}
};

class LightBuffer : public ConstantBuffer
{
public:
	struct Data
	{
		Vector3 direction;
		float specExp;

		Color ambient;

		int isSpecularMap;
		int isNormalMap;

		float padding;
	}data;

	LightBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.direction = Vector3(-1, -1, 1);
		data.specExp = 8.0f;

		data.ambient = Color(0.1f, 0.1f, 0.1f, 1.0f);
		data.isSpecularMap = 0;
		data.isNormalMap = 0;
	}
};

class MaterialBuffer : public ConstantBuffer
{
public:
	struct Data
	{
		Color diffuse;
		Color specular;
		Color ambient;
	}data;

	MaterialBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.diffuse = Color(1, 1, 1, 1);
		data.specular = Color(1, 1, 1, 1);
		data.ambient = Color(1, 1, 1, 1);
	}
};



class BoneBuffer : public ConstantBuffer
{
private:
	struct Data
	{
		Matrix bones[MAX_MODEL_BONE];

		UINT index;
		int isUseBlend;
		float padding[2];
	}data;
public:
	BoneBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		for (int i = 0; i < MAX_MODEL_BONE; i++)
		{
			data.bones[i] = XMMatrixIdentity();
		}

		data.index = 0;
		data.isUseBlend = 0;
	}

	void Bones(Matrix* m, UINT count)
	{
		memcpy(data.bones, m, sizeof(Matrix) * count);

		for (UINT i = 0; i < count; i++)
			data.bones[i] = XMMatrixTranspose(data.bones[i]);
	}

	void SetIndex(UINT value)
	{
		data.index = value;
	}

	void SetBlend(bool value)
	{
		data.isUseBlend = value;
	}
};

class IntBuffer : public ConstantBuffer
{
public:
	struct Data
	{
		int index;		

		float padding[3];
	}data;

	IntBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.index = 0;		
	}
};

class FloatBuffer : public ConstantBuffer
{
public:
	struct Data
	{
		float index;

		float padding[3];
	}data;

	FloatBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.index = 0;
	}
};

class RayBuffer : public ConstantBuffer
{
public:
	struct Data
	{
		Vector3 position;
		UINT size;

		Vector3 direction;
		float padding;
	}data;

	RayBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.position = Vector3(0, 0, 0);
		data.size = 0;
		data.direction = Vector3(0, 0, 0);
	}
};

class AmbientBuffer : public ConstantBuffer
{
public:
	struct Data
	{
		Color floorColor;
		Color ceilColor;
	}data;
	
	AmbientBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.floorColor = Color(0, 0, 0.1f, 1);
		data.ceilColor = Color(0.1f, 0.1f, 0, 1);
	}
};

struct PointLightData
{
	Vector3 position;
	float range;
	Color color;

	PointLightData()
	{
		position = Vector3(0, 0, 0);
		range = 20.0f;
		color = Color(1, 1, 1, 1);
	}
};

class PointBuffer : public ConstantBuffer
{
public:
	struct Data
	{
		PointLightData lights[MAX_POINTLIGHT];
		int lightCount;

		float padding[3];
	}data;

	PointBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.lightCount = 0;
	}

	void Add(PointLightData lightData)
	{
		data.lights[data.lightCount] = lightData;
		data.lightCount++;
	}
};

class SpotBuffer : public ConstantBuffer
{
public:
	struct Data
	{
		Color color;
		Vector3 position;
		float range;
		Vector3 direction;
		float outer;
		float inner;

		float padding[3];
	}data;

	SpotBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.color = Color(1, 1, 1, 1);
		data.position = Vector3(0, 8, 0);
		data.range = 60.0f;
		data.direction = Vector3(0, -1, 0);
		data.outer = 65;
		data.inner = 55;
	}
};

class CapsuleBuffer : public ConstantBuffer
{
public:
	struct Data
	{
		Color color;
		Vector3 position;
		float range;
		Vector3 direction;
		float length;
	}data;

	CapsuleBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.color = Color(1, 1, 1, 1);
		data.position = Vector3(0, 8, 0);
		data.range = 60.0f;
		data.direction = Vector3(0, -1, 0);
		data.length = 50.0f;
	}
};


//class BrushBuffer : public ConstantBuffer
//{
//public:
//	struct Data
//	{
//		Color Color;
//		Vector3 Location;
//		UINT Type;
//		UINT Range;
//	}data;
//
//	BrushBuffer() : ConstantBuffer(&data, sizeof(Data))
//	{
//		data.Color = XMCOLOR(0, 1, 0, 1);
//		data.Location = Vector3(0, 0, 0);
//
//		data.Type = 0;
//		data.Range = 1;
//	}
//};
