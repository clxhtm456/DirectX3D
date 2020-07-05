#pragma once

#include "FbxType.h"

class ModelMeshPart;

struct ModelBone
{
	int index;
	string name;
	int parentIndex;
	ModelBone* parent;

	vector<ModelBone*> children;

	Matrix local;
	Matrix global;
};

///////////////////////////////////////////////////////////////////////////////

class ModelMesh
{
private:
	friend class ModelData;

	string name;

	int parentBoneIndex;
	ModelBone* parentBone;

	vector<ModelMeshPart*> meshParts;

	ModelMesh();
	~ModelMesh();

	void Binding();

public:
	void Render();

	int GetParentBoneIndex() { return parentBoneIndex; }
};



class ModelMeshPart
{
private:
	friend class ModelData;
	friend class ModelMesh;

	Material* material;
	string materialName;

	ModelMesh* parent;


	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
public:
	vector<ModelVertexType> vertices;

	vector<UINT> indices;

	ModelMeshPart();
	~ModelMeshPart();

	void Render();

	void Binding();
};

class ModelData
{
public:
	ModelData(string modelDir);
	~ModelData();

public:
	void ReadMaterial(string file);
	void ReadMesh(string file);

	void CopyGlobalBoneTo(vector<Matrix>& transforms);
	void CopyGlobalBoneTo(vector<Matrix>& transforms, Matrix& w);

	void BindingBone();
	void BindingMesh();

	void SetBones(Matrix* m, UINT count);

	vector<ModelBone*>* GetBones() { return &bones; }
	vector<ModelMesh*>* GetMeshes() { return &meshes; }
	ModelBone* BoneByName(string name);

	BoneBuffer* GetBuffer() { return boneBuffer; }
private:
	BoneBuffer* boneBuffer;

	ModelBone* root;

	vector<Material*> materials;
	vector<ModelBone*> bones;
	vector<ModelMesh*> meshes;

};

class ModelKeyFrame
{
private:
	friend class ModelClip;

	string boneName;

	float duration;
	float frameRate;
	int frameCount;

	vector<FbxKeyFrameData> transforms;

public:
	ModelKeyFrame();
	~ModelKeyFrame();

	Matrix GetInterpolatedMatrix(float time);
	Matrix GetInterpolatedMatrix(UINT index1, UINT index2, float t);

	UINT GetKeyFrameIndex(float time);
	void CalcKeyFrameIndex(float time, OUT UINT& index1, OUT UINT& index2,
		OUT float& interpolatedTime);
};

class ModelClip
{
private:
	string name;

	float duration;
	float frameRate;
	int frameCount;

	bool isLockRoot;
	bool isRepeat;
	float speed;
	float playTime;

	unordered_map<string, ModelKeyFrame*> keyFrameMap;

	function<void()> EndEvent;
public:
	ModelClip(string file);
	~ModelClip();

	Matrix GetKeyFrameMatrix(ModelBone* bone);
	void UpdateKeyFrame(ModelBone* bone);

	void ReadAnimation(string file);

	void Reset();

	void LockRoot(bool val) { isLockRoot = val; }
	void Repeat(bool val) { isRepeat = val; }
	void Speed(float val) { speed = val; }
	void StartTime(float val) { playTime = val; }

	void SetEndEvent(function<void()> Event) { EndEvent = Event; }
};