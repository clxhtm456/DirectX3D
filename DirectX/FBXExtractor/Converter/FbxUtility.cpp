#include "stdafx.h"
#include "FbxUtility.h"

Matrix FbxUtility::Negative()
{
	Matrix scaleX;
	scaleX = XMMatrixScaling(-1, 1, 1);

	Matrix rotation;
	rotation = XMMatrixRotationY(Math::PI);

	return scaleX * rotation;
}

Vector4 FbxUtility::ToColor(FbxPropertyT<FbxDouble3>& vec, FbxPropertyT<FbxDouble>& factor)
{
	Vector4 result;

	FbxDouble3 color = vec;
	result.x = (float)color.mData[0];
	result.y = (float)color.mData[1];
	result.z = (float)color.mData[2];
	result.w = (float)factor;

	return result;
}

Matrix FbxUtility::ToMatrix(FbxAMatrix& value)
{
	FbxVector4 r0 = value.GetRow(0);
	FbxVector4 r1 = value.GetRow(1);
	FbxVector4 r2 = value.GetRow(2);
	FbxVector4 r3 = value.GetRow(3);

	Matrix origin = Matrix(
		(float)r0.mData[0], (float)r0.mData[1], (float)r0.mData[2], (float)r0.mData[3],
		(float)r1.mData[0], (float)r1.mData[1], (float)r1.mData[2], (float)r1.mData[3],
		(float)r2.mData[0], (float)r2.mData[1], (float)r2.mData[2], (float)r2.mData[3],
		(float)r3.mData[0], (float)r3.mData[1], (float)r3.mData[2], (float)r3.mData[3]
	);

	return Negative() * origin * Negative();
}

Vector3 FbxUtility::ToVector3(FbxVector4& value)
{
	return Vector3((float)value.mData[0], (float)value.mData[1], (float)value.mData[2]);
}

string FbxUtility::GetTextureFile(FbxProperty& prop)
{
	if (prop.IsValid())
	{
		if (prop.GetSrcObjectCount() > 0)
		{
			FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>();

			if (texture != nullptr)
				return string(texture->GetFileName());
		}
	}

	return "";
}

string FbxUtility::GetMaterialName(FbxMesh* mesh, int polygonIndex, int cpIndex)
{
	FbxNode* node = mesh->GetNode();

	if (node == nullptr)
		return "";

	FbxLayerElementMaterial* material = mesh->GetLayer(0)->GetMaterials();
	if (material == nullptr)
		return "";

	FbxLayerElement::EMappingMode mappingMode = material->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = material->GetReferenceMode();

	int mappingIndex = -1;

	switch (mappingMode)
	{	
	case fbxsdk::FbxLayerElement::eByControlPoint:
		mappingIndex = cpIndex;
		break;
	case fbxsdk::FbxLayerElement::eByPolygonVertex:
		mappingIndex = polygonIndex * 3;
		break;
	case fbxsdk::FbxLayerElement::eByPolygon:
		mappingIndex = polygonIndex;
		break;	
	case fbxsdk::FbxLayerElement::eAllSame:
		mappingIndex = 0;
		break;	
	}

	fbxsdk::FbxSurfaceMaterial* findMaterial = nullptr;
	if (refMode == FbxLayerElement::eDirect)
	{
		if (mappingIndex < node->GetMaterialCount())
			findMaterial = node->GetMaterial(mappingIndex);
	}
	else if (refMode == FbxLayerElement::eIndexToDirect)
	{
		FbxLayerElementArrayTemplate<int>& indexArr = material->GetIndexArray();
		if (mappingIndex < indexArr.GetCount())
		{
			int tempIndex = indexArr.GetAt(mappingIndex);

			if (tempIndex < node->GetMaterialCount())
				findMaterial = node->GetMaterial(tempIndex);
		}
	}

	if (findMaterial == nullptr)
		return "";

	return findMaterial->GetName();
}

Vector2 FbxUtility::GetUV(FbxMesh* mesh, int cpIndex, int uvIndex)
{
	Vector2 result = Vector2(0, 0);

	FbxLayerElementUV* uv = mesh->GetLayer(0)->GetUVs();
	if (uv == nullptr)
		return result;

	FbxLayerElement::EMappingMode mappingMode = uv->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = uv->GetReferenceMode();

	switch (mappingMode)
	{	
	case fbxsdk::FbxLayerElement::eByControlPoint:
	{
		if (refMode == FbxLayerElement::eDirect)
		{
			result.x = (float)uv->GetDirectArray().GetAt(cpIndex).mData[0];
			result.y = (float)uv->GetDirectArray().GetAt(cpIndex).mData[1];
		}
		else if (refMode == FbxLayerElement::eIndexToDirect)
		{
			int index = uv->GetIndexArray().GetAt(cpIndex);

			result.x = (float)uv->GetDirectArray().GetAt(index).mData[0];
			result.y = (float)uv->GetDirectArray().GetAt(index).mData[1];
		}
	}
		break;
	case fbxsdk::FbxLayerElement::eByPolygonVertex:
	{
		result.x = (float)uv->GetDirectArray().GetAt(uvIndex).mData[0];
		result.y = (float)uv->GetDirectArray().GetAt(uvIndex).mData[1];
	}
		break;	
	}

	result.y = 1.0f - result.y;

	return result;
}
