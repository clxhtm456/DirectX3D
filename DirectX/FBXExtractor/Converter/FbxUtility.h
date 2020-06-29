#pragma once

#define FBXSDK_SHARED
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")

using namespace fbxsdk;


class FbxUtility
{
public:
	static Matrix Negative();

	static Vector4 ToColor(FbxPropertyT<FbxDouble3>& vec, FbxPropertyT<FbxDouble>& factor);
	static Matrix ToMatrix(FbxAMatrix& value);
	static Vector3 ToVector3(FbxVector4& value);

	static string GetTextureFile(FbxProperty& prop);
	static string GetMaterialName(FbxMesh* mesh, int polygonIndex, int cpIndex);
	static Vector2 GetUV(FbxMesh* mesh, int cpIndex, int uvIndex);
};