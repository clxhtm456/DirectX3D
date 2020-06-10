#pragma once
#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500
#define MAX_MODEL_INSTANCE 500

class ModelBone;
class ModelMesh;
class ModelClip;

class Model
{
public:
	typedef VertexTextureNormalTangentBlend ModelVertex;

	Model();
	~Model();

	UINT MaterialCount() { return materials.size(); }
	vector<Material*>& Materials() { return materials; }
	Material* MaterialByIndex(UINT index) { return materials[index]; }
	Material* MaterialByName(wstring name);

	UINT BoneCount() { return bones.size(); }
	vector<ModelBone*>& Bones() { return bones; }
	ModelBone* BoneByIndex(UINT index) { return bones[index]; }
	ModelBone* BoneByName(wstring name);

	UINT MeshCount() { return meshes.size(); }
	vector<ModelMesh*>& Meshes() { return meshes; }
	ModelMesh* MeshByIndex(UINT index) { return meshes[index]; }
	ModelMesh* MeshByName(wstring name);

	UINT ClipCount() { return clips.size(); }
	vector<ModelClip*>& Clips() { return clips; }
	ModelClip* ClipByIndex(UINT index) { return clips[index]; }
	ModelClip* ClipByName(wstring name);

	void Ambient(Color& color);
	void Ambient(float r, float g, float b, float a = 1.0);

	void Diffuse(Color& color);
	void Diffuse(float r, float g, float b, float a = 1.0);

	void Specular(Color& color);
	void Specular(float r, float g, float b, float a = 1.0);

	void Emissive(Color& color);
	void Emissive(float r, float g, float b, float a = 1.0);

	void Attach(Shader* shader, Model* model, int parentBoneIndex, Transform* transform = NULL);

public:
	void ReadMaterial(wstring file);
	void ReadMesh(wstring file);
	void ReadClip(wstring file);

private:
	void BindBone();
	void BindMesh();

private:
	vector<Material*> materials;

	ModelBone* root;
	vector<ModelBone*> bones;
	vector<ModelMesh*> meshes;
	vector<ModelClip*> clips;
};