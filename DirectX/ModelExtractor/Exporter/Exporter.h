#pragma once


class Exporter
{
private:
	FbxManager* manager;
	FbxImporter* importer;
	FbxScene* scene;
	FbxIOSettings* ios;
	FbxGeometryConverter* converter;

	vector<struct FbxMaterial*> materials;
	vector<struct FbxBoneData*> bones;
	vector<struct FbxMeshData*> meshes;

public:
	Exporter(string file);
	~Exporter();

	void ExporterMaterial(string saveFile, string saveFolder = "");
	void ExporterMesh(string saveFile, string saveFolder = "");
	void ExporterAnimation(UINT clipNumber, string saveFile, string saveFolder = "");

private:
	void ReadMaterial();
	void WriteMaterial(string saveFile, string saveFolder);

	void ReadBone(FbxNode* node, int index, int parent);
	void ReadMesh(FbxNode* node, int parentBone);
	void ReadSkin();
	void WriteMesh(string saveFile, string saveFolder);

	struct FbxClip* ReadAnimation(UINT index);
	void ReadKeyFrameData(FbxClip* clip, FbxNode* node, int start, int end);
	void WriteClip(FbxClip* clip, string saveFile, string saveFolder);

	UINT GetBoneIndexByName(string name);
};