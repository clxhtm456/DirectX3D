#pragma once
#include "FbxUtility.h"

//struct Vertex
//{
//	Vector4 position;
//	Vector3 normal;
//	Vector2 uv;
//	int boneIndex[4] = { -1,-1,-1,-1 };
//	float boneWeight[4];
//};
//
//struct SkeletonAnim
//{
//	enum { MAX_BONE = 500 };
//	//std::vector<std::array<glm::mat4x4, MAX_BONE>> bones;
//	std::vector < std::array<Matrix, MAX_BONE>> bones;
//	uint32_t m_currentFrameIndex = 0;
//	uint32_t m_totalFrameCount = 0;
//};

class XmlExtractor
{
public:
	void Reset();
	static XmlExtractor* Create();
	~XmlExtractor();
private:
	XmlExtractor();
	

	void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
	bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);

	FbxAMatrix GetGeometry(FbxNode* pNode);
	FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition = nullptr);
	FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex);
public:
	void LoadModel(string filePath);
	void LoadAnimation(UINT index, string filePath);
private:
	std::vector<FbxAnimStack*> m_animStacks;

	FbxManager* m_sdkManager;

	FbxScene* m_scene;

	FbxImporter* m_importer;

	FbxAnimLayer* m_currentAnimLayer;

	FbxArray<FbxString*> m_animStackNameArray;

	FbxArray<FbxNode*> m_cameraArray;

	FbxArray<FbxPose*> m_poseArray;

	FbxTime mFrameTime, mStart, mStop, mCurrentTime;
	FbxTime mCache_Start, mCache_Stop;

	bool m_supportVBO;
public:
	void WriteMaterial(wstring savePath,bool bOverWrite);
	void WriteMesh(wstring savePath, bool bOverWrite);
	void WriteClip(class FbxClip* clip, wstring savePath);

	class FbxClip* ReadAnimation(UINT index);
	void ReadMesh(FbxNode* node, int parentBone);
	void ReadBone(FbxNode* node, int index, int parent);
	void ReadSkin();

	string WriteTexture(FbxProperty& pProperty);
	UINT GetBoneIndexByName(string name);
	void ReadKeyFrameData(class FbxClip* clip, FbxNode* node, int start, int end);
private:
	vector<class FbxBoneData*> bones;
	vector<class FbxMeshData*> meshes;
};