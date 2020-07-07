//==================================================================
//		## ModelLoader ## (assimp ���̺귯���� �̿��� �� ������ �ε��Ѵ�.)
//==================================================================

#pragma once
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

typedef VertexTextureNormalTangentBlend ModelVertex;

struct VertexData
{
	vector<ModelVertex> vertex;

	vector<XMFLOAT4> color;
	vector<XMUINT4>	 boneidx;
	vector<XMFLOAT4> weight;
};

struct MaterialSrc
{
	string diffuseMap;
	string alphaMap;
	string normalMap;
};

struct AsBoneData
{
	int index;
	string name;

	int parent;

	Matrix local;
	Matrix global;
	Matrix matOffset = XMMatrixIdentity();
};


struct AsKeyFrameData
{
	float time;

	Vector3 scale;
	Quaternion rotation;
	Vector3 position;
};

struct AsKeyFrame
{
	string boneName;
	vector<AsKeyFrameData> transforms;
};

struct AsClipNode
{
	vector<AsKeyFrameData> keyframe;
	aiString name;
};


struct AsClip
{
	string name;

	int frameCount;
	float frameRate;
	float duration;

	vector<AsKeyFrame*> keyFrames;

	~AsClip()
	{
		/*for (auto object : keyFrames)
			delete object;*/
		keyFrames.clear();
	}
};

struct AsAnimation
{
	string name;

	int totalFrame;
	float frameRate;

	vector<AsKeyFrame*> keyFrames;

	~AsAnimation()
	{
		for (auto object : keyFrames)
			delete object;
		keyFrames.clear();
	}
};

class ModelLoader
{
public:
	ModelLoader() {};
	~ModelLoader() {};

	//�� ���� �ε�
	void	LoadModel(string path, UINT flag);
	vector<AsClip>	LoadAnimation(string path, UINT flag);
protected:
	//��� ���� ����(�� ����)
	void ProcessNode(aiNode* aiNodeInfo, SkinModel* skModel, NodeInfo* parent = NULL, int depth = 0);
	//�޽� ���� ����(��������, �ε��� ����, �⺻ �޽� ����)
	void ProcessMesh(aiMesh* mesh, VertexData& vertices, vector<UINT>& indices, vector<FbxMeshData*>& meshList);
	//���� ���� ����
	void ProcessMaterial(const aiScene* pScene, vector<MaterialSrc>& matList, string directoryPath);
	//��Ű�� ���� ����(offset_mat, weight)
	void ProcessSkin(aiMesh* aiMesh, HierarchyMesh* mesh, VertexData& vertices, vector<UINT>& indices, SkinModel* skModel);
	//�ִϸ��̼� ���� ����
	void ProcessAnimation(const aiScene* pScene, vector<AsClip>& skModel);

	//�̸��� ���� ��带 ã�´�.
	AsBoneData* FindNode(aiString name, vector<AsBoneData*> nodeList);
};
