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
		Color ambient;
		Color Specular;

		Vector3 direction;
		Vector3 position;

		float padding[2];
	}data;

	LightBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.ambient = Color(0, 0, 0, 1);
		data.Specular = Color(1, 1, 1, 1);
		data.direction = Vector3(0, -0.2, 0);

		data.position = Vector3(0,0,0);

	}
};

class MaterialBuffer : public ConstantBuffer
{
public:
	struct Data
	{
		Vector3 diffuse ;
		Vector3 ambient ;
		Vector3 specular;
		Vector3 emissive;
		Vector3 transparent;
		Vector3 reflective;

		FLOAT bumpscaling;
		FLOAT opacity;
		FLOAT shininess;
		FLOAT shininessstrength;
		FLOAT transparentfactor;
		FLOAT reflectivity;

		FLOAT refracti;
		INT opaque;
		INT reflector;

		INT hasDiffuseMap;
		INT hasSpecularMap;
		INT hasAmbientMap;
		INT hasEmissiveMap;
		INT hasHeightMap;
		INT hasNormalMap;
		INT hasShininessMap;
		INT hasOpacityMap;
		INT hasDisplacementMap;
		INT hasLightMapMap;
		INT hasReflectionMap;
		INT hasBasecolorMap;
		INT hasNormalcameraMap;
		INT hasEmissioncolorMap;
		INT hasMetalnessMap;
		INT hasDiffuseroughnessMap;
		INT hasAmbientocculsionMap;
	}data;

	MaterialBuffer() : ConstantBuffer(&data, sizeof(Data))
	{
		data.diffuse		= Vector3(1, 1, 1);
		data.ambient		= Vector3(0, 0, 0);
		data.specular		= Vector3(0, 0, 0);
		data.emissive		= Vector3(0, 0, 0);

		data.transparent	= Vector3(0, 0, 0);
		data.reflective		= Vector3(0, 0, 0);

		data.bumpscaling	= 0;
		data.opacity		= 0;
		data.shininess		= 0;
		data.shininessstrength = 0;
		data.transparentfactor = 0;
		data.reflectivity	= 0;

		data.refracti		= 0;
		data.opaque			= 0;
		data.reflector		= 0;

		data.hasDiffuseMap	= 0;
		data.hasSpecularMap = 0;
		data.hasAmbientMap = 0;
		data.hasEmissiveMap = 0;
		data.hasHeightMap = 0;
		data.hasNormalMap = 0;
		data.hasShininessMap = 0;
		data.hasOpacityMap = 0;
		data.hasDisplacementMap = 0;
		data.hasLightMapMap = 0;
		data.hasReflectionMap = 0;
		data.hasBasecolorMap = 0;
		data.hasNormalcameraMap = 0;
		data.hasEmissioncolorMap = 0;
		data.hasMetalnessMap = 0;
		data.hasDiffuseroughnessMap = 0;
		data.hasAmbientocculsionMap = 0;
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
