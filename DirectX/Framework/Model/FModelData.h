#pragma once

#include "FbxType.h"

class FModelBone
{
public:
	friend class FModelData;

private:
	FModelBone();
	~FModelBone();

public:
	int Index() { return index; }

	int ParentIndex() { return parentIndex; }
	FModelBone* Parent() { return parent; }

	wstring Name() { return name; }

	Matrix& Transform() { return transform; }
	void Transform(Matrix& matrix) { transform = matrix; }

	vector<FModelBone*>& Childs() { return childs; }

private:
	int index;
	wstring name;

	int parentIndex;
	FModelBone* parent;

	Matrix transform;
	vector<FModelBone*> childs;
};

///////////////////////////////////////////////////////////////////////////////
class FModelMesh
{
public:
	friend class FModelData;

private:
	FModelMesh();
	~FModelMesh();

	void Binding(FModelData* model);

public:

	void Update();
	void Render();

	wstring Name() { return name; }

	int BoneIndex() { return boneIndex; }
	class FModelBone* Bone() { return bone; }

	void Transforms(Matrix* transform);

	void TransformsSRV(ID3D11ShaderResourceView* srv);

private:
	wstring name;

	Shader* shader;

	wstring materialName = L"";
	Material* material = NULL;

	int boneIndex;
	class FModelBone* bone;

	VertexBuffer* vertexBuffer;
	UINT vertexCount;
	ModelVertexType* vertices;

	IndexBuffer* indexBuffer;
	UINT indexCount;
	UINT* indices;

	IntBuffer* intBuffer;

	ID3D11ShaderResourceView* SRV;
};

class FModelData
{
public:
	typedef VertexTextureNormalTangentBlend ModelVertex;

	FModelData();
	~FModelData();

	UINT MaterialCount() { return materials.size(); }
	vector<Material*>& Materials() { return materials; }
	Material* MaterialByIndex(UINT index) { return materials[index]; }
	Material* MaterialByName(wstring name);

	UINT BoneCount() { return bones.size(); }
	vector<FModelBone*>& Bones() { return bones; }
	FModelBone* BoneByIndex(UINT index) { return bones[index]; }
	FModelBone* BoneByName(wstring name);

	UINT MeshCount() { return meshes.size(); }
	vector<FModelMesh*>& Meshes() { return meshes; }
	FModelMesh* MeshByIndex(UINT index) { return meshes[index]; }
	FModelMesh* MeshByName(wstring name);


	void Ambient(Color& color);
	void Ambient(float r, float g, float b, float a = 1.0);

	void Diffuse(Color& color);
	void Diffuse(float r, float g, float b, float a = 1.0);

	void Specular(Color& color);
	void Specular(float r, float g, float b, float a = 1.0);

	void Emissive(Color& color);
	void Emissive(float r, float g, float b, float a = 1.0);

public:
	void ReadMaterial(wstring file);
	void ReadMesh(wstring file);
	void ReadClip(wstring file);

private:
	void BindBone();
	void BindMesh();

private:
	vector<Material*> materials;

	FModelBone* root;
	vector<FModelBone*> bones;
	vector<FModelMesh*> meshes;
};