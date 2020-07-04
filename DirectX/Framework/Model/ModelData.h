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

	vector<ModelBone*> GetBones() { return bones; }
	vector<ModelMesh*> GetMeshes() { return meshes; }
	ModelBone* BoneByName(string name);

	BoneBuffer* GetBuffer() { return boneBuffer; }
private:
	BoneBuffer* boneBuffer;

	ModelBone* root;

	vector<Material*> materials;
	vector<ModelBone*> bones;
	vector<ModelMesh*> meshes;

};