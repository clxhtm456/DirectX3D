#pragma once

typedef VertexTextureNormalTangentBlend ModelVertexType;

struct FbxMaterial
{
	string Name;

	Vector4 Ambient;
	Vector4 Diffuse;
	Vector4 Specular;
	Vector4 Emissive;

	string DiffuseFile;
	string SpecularFile;
	string NormalFile;
};

struct FbxBoneData
{
	int index;
	string name;

	int parent;

	Matrix local;
	Matrix global;
};

struct FbxVertex
{
	int controlPoint;
	string materialName;

	ModelVertexType vertex;
};

struct FbxMeshPartData
{
	string materialName;

	vector<ModelVertexType> vertices;
	vector<UINT> indices;


};

struct FbxMeshData
{
	string name;
	int parentBone;

	class FbxMesh* mesh;

	vector<FbxVertex*> vertices;
	vector<FbxMeshPartData*> meshParts;

	~FbxMeshData()
	{
		for (auto vertex : vertices)
			delete vertex;
		vertices.clear();

		/*for (auto mesh : meshParts)
			delete mesh;
		meshParts.clear();*/
	}
};


struct FbxKeyFrameData
{
	float time;

	Vector3 scale;
	Quaternion rotation;
	Vector3 position;
};

struct FbxKeyFrame
{
	string boneName;
	vector<FbxKeyFrameData> transforms;

	
};

struct FbxClip
{
	string name;

	int frameCount;
	float frameRate;
	float duration;

	vector<FbxKeyFrame*> keyFrames;

	~FbxClip()
	{
		/*for (auto object : keyFrames)
			delete object;*/
		keyFrames.clear();
	}
};

struct FbxAnimation
{
	string name;

	int totalFrame;
	float frameRate;

	vector<FbxKeyFrame*> keyFrames;

	~FbxAnimation()
	{
		for (auto object : keyFrames)
			delete object;
		keyFrames.clear();
	}
};

struct FbxBlendWeight
{
	Vector4 indices = Vector4(0, 0, 0, 0);
	Vector4 weights = Vector4(0, 0, 0, 0);

	void Set(UINT index, UINT boneIndex, float weight)
	{
		float i = (float)boneIndex;
		float w = weight;

		switch (index)
		{
		case 0:
			indices.x = i;
			weights.x = w;
			break;
		case 1:
			indices.y = i;
			weights.y = w;
			break;
		case 2:
			indices.z = i;
			weights.z = w;
			break;
		case 3:
			indices.w = i;
			weights.w = w;
			break;
		default:
			break;
		}
	}
};

struct FbxBoneWeights
{
private:
	typedef pair<UINT, float> Pair;
	vector<Pair> boneWeights;

public:
	void AddBoneWeight(UINT boneIndex, float boneWeight)
	{
		if (boneWeight <= 0.0f)
			return;

		bool isAdd = false;

		auto it = boneWeights.begin();
		while (it != boneWeights.end())
		{
			if (boneWeight > it->second)
			{
				boneWeights.insert(it, Pair(boneIndex, boneWeight));
				isAdd = true;
				break;
			}
			it++;
		}

		if (!isAdd)
			boneWeights.push_back(Pair(boneIndex, boneWeight));
	}

	void GetBlendWeiths(FbxBlendWeight& blendWeights)
	{
		for (UINT i = 0; i < boneWeights.size(); i++)
		{
			if (i >= 4)
				return;

			blendWeights.Set(i, boneWeights[i].first, boneWeights[i].second);
		}
	}

	void Normalize()
	{
		float totalWeight = 0.0f;

		int i = 0;

		auto it = boneWeights.begin();

		while (it != boneWeights.end())
		{
			if (i < 4)
			{
				totalWeight += it->second;

				i++;
				it++;
			}
			else
				it = boneWeights.erase(it);
		}

		float scale = 1.0f / totalWeight;

		it = boneWeights.begin();
		while (it != boneWeights.end())
		{
			it->second *= scale;
			it++;
		}
	}
};
