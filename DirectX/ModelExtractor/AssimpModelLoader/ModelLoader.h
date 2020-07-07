//==================================================================
//		## ModelLoader ## (assimp 라이브러리를 이용해 모델 정보를 로드한다.)
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

	//모델 정보 로드
	void	LoadModel(string path, UINT flag);
	vector<AsClip>	LoadAnimation(string path, UINT flag);
protected:
	//노드 정보 생성(본 정보)
	void ProcessNode(aiNode* aiNodeInfo, SkinModel* skModel, NodeInfo* parent = NULL, int depth = 0);
	//메쉬 정보 생성(정점정보, 인덱스 정보, 기본 메쉬 정보)
	void ProcessMesh(aiMesh* mesh, VertexData& vertices, vector<UINT>& indices, vector<FbxMeshData*>& meshList);
	//재질 정보 생성
	void ProcessMaterial(const aiScene* pScene, vector<MaterialSrc>& matList, string directoryPath);
	//스키닝 정보 생성(offset_mat, weight)
	void ProcessSkin(aiMesh* aiMesh, HierarchyMesh* mesh, VertexData& vertices, vector<UINT>& indices, SkinModel* skModel);
	//애니메이션 정보 생성
	void ProcessAnimation(const aiScene* pScene, vector<AsClip>& skModel);

	//이름을 통해 노드를 찾는다.
	AsBoneData* FindNode(aiString name, vector<AsBoneData*> nodeList);
};
