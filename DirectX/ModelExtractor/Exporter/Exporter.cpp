#include "stdafx.h"
#include "Exporter.h"

#include "FbxUtility.h"
#include "FbxType.h"



Exporter::Exporter(string file)
{
	file = "../../_Assets/" + file;
	manager = FbxManager::Create();
	scene = FbxScene::Create(manager, "");


	ios = FbxIOSettings::Create(manager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	manager->SetIOSettings(ios);

	(*(manager->GetIOSettings())).SetBoolProp(EXP_FBX_EMBEDDED, true);

	importer = FbxImporter::Create(manager, "");
	bool lExportStatus = importer->Initialize(file.c_str(), -1, manager->GetIOSettings());

	assert(lExportStatus);

	importer->Import(scene);

	converter = new FbxGeometryConverter(manager);
}

Exporter::~Exporter()
{
	delete converter;

	ios->Destroy();
	importer->Destroy();
	scene->Destroy();
	manager->Destroy();
}

void Exporter::ExporterMaterial(string saveFile, string saveFolder)
{
	ReadMaterial();
	saveFolder = "../../_Assets/Materials/" + saveFolder;
	saveFile += ".mat";
	WriteMaterial(saveFile, saveFolder);
}

void Exporter::ExporterMesh(string saveFile, string saveFolder)
{
	ReadBone(scene->GetRootNode(), -1, -1);
	ReadSkin();
	saveFolder = "../../_Assets/Meshes/" + saveFolder;
	saveFile += ".mesh";
	WriteMesh(saveFile, saveFolder);
}

void Exporter::ExporterAnimation(UINT clipNumber, string saveFile, string saveFolder)
{
	FbxClip* clip = ReadAnimation(clipNumber);
	saveFolder = "../../_Assets/Clips/" + saveFolder;
	WriteClip(clip, saveFile, saveFolder);
}

void Exporter::ReadMaterial()
{
	int count = scene->GetMaterialCount();

	for (int i = 0; i < count; i++)
	{
		FbxSurfaceMaterial* fbxMaterial = scene->GetMaterial(i);

		FbxMaterial* material = new FbxMaterial();
		material->Name = fbxMaterial->GetName();

		if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{
			FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)fbxMaterial;
			material->Diffuse = FbxUtility::ToColor(lambert->Diffuse, lambert->DiffuseFactor);

			material->Ambient = FbxUtility::ToColor(lambert->Ambient, lambert->AmbientFactor);

			material->Emissive = FbxUtility::ToColor(lambert->Emissive, lambert->EmissiveFactor);

			FbxProperty prop = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuse);
			material->DiffuseFile = FbxUtility::GetTextureFile(prop);

			prop = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sSpecular);
			material->SpecularFile = FbxUtility::GetTextureFile(prop);

			prop = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sNormalMap);
			material->NormalFile = FbxUtility::GetTextureFile(prop);
		}

		materials.push_back(material);
	}
}

void Exporter::WriteMaterial(string saveFile, string saveFolder)
{
	string folder = Path::GetDirectoryName(saveFolder);
	string file = Path::GetFileName(saveFile);

	Path::CreateFolders(folder);

	Xml::XMLDocument* document = new Xml::XMLDocument();

	Xml::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Materials");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	for (FbxMaterial* material : materials)
	{
		XmlElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		XmlElement* element = document->NewElement("Name");
		element->SetText(material->Name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		string temp = Path::GetFileName(material->DiffuseFile);
		element->SetText(temp.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		temp = Path::GetFileName(material->SpecularFile);
		element->SetText(temp.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		temp = Path::GetFileName(material->NormalFile);
		element->SetText(temp.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->Ambient.x);
		element->SetAttribute("G", material->Ambient.y);
		element->SetAttribute("B", material->Ambient.z);
		element->SetAttribute("A", material->Ambient.w);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->Diffuse.x);
		element->SetAttribute("G", material->Diffuse.y);
		element->SetAttribute("B", material->Diffuse.z);
		element->SetAttribute("A", material->Diffuse.w);
		node->LinkEndChild(element);

		element = document->NewElement("Emissive");
		element->SetAttribute("R", material->Emissive.x);
		element->SetAttribute("G", material->Emissive.y);
		element->SetAttribute("B", material->Emissive.z);
		element->SetAttribute("A", material->Emissive.w);
		node->LinkEndChild(element);

		SafeDelete(material);
	}

	document->SaveFile((folder + file).c_str());

	
	SafeDelete(document);
}

void Exporter::ReadBone(FbxNode* node, int index, int parent)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute != nullptr)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();

		bool b = false;
		b |= (nodeType == FbxNodeAttribute::eSkeleton);
		b |= (nodeType == FbxNodeAttribute::eMesh);
		b |= (nodeType == FbxNodeAttribute::eNull);
		b |= (nodeType == FbxNodeAttribute::eMarker);

		if (b)
		{
			FbxBoneData* bone = new FbxBoneData();
			bone->index = index;
			bone->parent = parent;
			bone->name = node->GetName();

			bone->local = FbxUtility::ToMatrix(node->EvaluateLocalTransform());
			bone->global = FbxUtility::ToMatrix(node->EvaluateGlobalTransform());

			bones.push_back(bone);

			if (nodeType == FbxNodeAttribute::eMesh)
			{
				converter->Triangulate(attribute, true, true);
				ReadMesh(node, index);
			}
		}
	}

	for (int i = 0; i < node->GetChildCount(); i++)
		ReadBone(node->GetChild(i), bones.size(), index);
}

void Exporter::ReadMesh(FbxNode* node, int parentBone)
{
	FbxMesh* mesh = node->GetMesh();

	vector<FbxVertex*> vertices;
	for (int p = 0; p < mesh->GetPolygonCount(); p++)
	{
		for (int vi = 2; vi >= 0; vi--)
		{
			FbxVertex* vertex = new FbxVertex();

			int cpIndex = mesh->GetPolygonVertex(p, vi);
			vertex->controlPoint = cpIndex;

			FbxVector4 position = mesh->GetControlPointAt(cpIndex);
			Vector3 temp = FbxUtility::ToVector3(position);
			XMStoreFloat3(&vertex->vertex.Position, XMVector3TransformCoord(XMLoadFloat3(&temp), FbxUtility::Negative()));

			FbxVector4 normal;
			mesh->GetPolygonVertexNormal(p, vi, normal);
			normal.Normalize();
			temp = FbxUtility::ToVector3(normal);
			XMStoreFloat3(&vertex->vertex.Normal, XMVector3TransformCoord(XMLoadFloat3(&temp), FbxUtility::Negative()));

			vertex->materialName = FbxUtility::GetMaterialName(mesh, p, cpIndex);

			int uvIndex = mesh->GetTextureUVIndex(p, vi);
			vertex->vertex.Uv = FbxUtility::GetUV(mesh, cpIndex, uvIndex);

			vertices.push_back(vertex);
		}

		UINT index0 = vertices.size() - 3;
		UINT index1 = vertices.size() - 2;
		UINT index2 = vertices.size() - 1;

		ModelVertexType vertex0 = vertices[index0]->vertex;
		ModelVertexType vertex1 = vertices[index1]->vertex;
		ModelVertexType vertex2 = vertices[index2]->vertex;

		Vector3 p0 = vertex0.Position;
		Vector3 p1 = vertex1.Position;
		Vector3 p2 = vertex2.Position;

		Vector2 uv0 = vertex0.Uv;
		Vector2 uv1 = vertex1.Uv;
		Vector2 uv2 = vertex2.Uv;

		XMVECTOR e0 = XMLoadFloat3(&p1) - XMLoadFloat3(&p0);
		XMVECTOR e1 =XMLoadFloat3(&p2) - XMLoadFloat3(&p0);

		float u0 = uv1.x - uv0.x;
		float u1 = uv2.x - uv0.x;
		float v0 = uv1.y - uv0.y;
		float v1 = uv2.y - uv0.y;

		float d = 1.0f / (u0 * v1 - v0 * u1);

		XMVECTOR tangent;		
		tangent = (v1 * e0 - v0 * e1) * d;

		XMStoreFloat3(&vertices[index0]->vertex.Tangent, XMLoadFloat3(&vertices[index0]->vertex.Tangent) + tangent);
		XMStoreFloat3(&vertices[index1]->vertex.Tangent, XMLoadFloat3(&vertices[index1]->vertex.Tangent) + tangent);
		XMStoreFloat3(&vertices[index2]->vertex.Tangent, XMLoadFloat3(&vertices[index2]->vertex.Tangent) + tangent);
	}

	for (FbxVertex* vertex : vertices)
	{
		XMVECTOR t = XMLoadFloat3(&vertex->vertex.Tangent);
		XMVECTOR n = XMLoadFloat3(&vertex->vertex.Normal);

		XMVECTOR temp = (t - n * XMVector3Dot(n, t));
		temp = XMVector3Normalize(temp);

		XMStoreFloat3(&vertex->vertex.Tangent, temp);
	}

	FbxMeshData* data = new FbxMeshData();
	data->name = node->GetName();
	data->parentBone = parentBone;
	data->vertices = vertices;
	data->mesh = mesh;
	meshes.push_back(data);
}

void Exporter::ReadSkin()
{
	for (FbxMeshData* data : meshes)
	{
		FbxMesh* mesh = data->mesh;

		int deformerCount = mesh->GetDeformerCount();
		vector<FbxBoneWeights> boneWeights(mesh->GetControlPointsCount(), FbxBoneWeights());

		for (int i = 0; i < deformerCount; i++)
		{
			FbxDeformer* deformer = mesh->GetDeformer(i, FbxDeformer::eSkin);

			FbxSkin* skin = reinterpret_cast<FbxSkin*>(deformer);
			if (skin == nullptr)
				continue;

			for (int clusterIndex = 0; clusterIndex < skin->GetClusterCount(); clusterIndex++)
			{
				FbxCluster* cluster = skin->GetCluster(clusterIndex);

				string linkName = cluster->GetLink()->GetName();
				UINT boneIndex = GetBoneIndexByName(linkName);

				FbxAMatrix transform;
				FbxAMatrix linkTransform;

				cluster->GetTransformMatrix(transform);
				cluster->GetTransformLinkMatrix(linkTransform);

				bones[boneIndex]->local = FbxUtility::ToMatrix(transform);
				bones[boneIndex]->global = FbxUtility::ToMatrix(linkTransform);

				for (int indexCount = 0; indexCount < cluster->GetControlPointIndicesCount();
					indexCount++)
				{
					int temp = cluster->GetControlPointIndices()[indexCount];
					double* weights = cluster->GetControlPointWeights();

					boneWeights[temp].AddBoneWeight(boneIndex, (float)weights[indexCount]);
				}
			}
		}

		for (FbxBoneWeights& boneWeight : boneWeights)
			boneWeight.Normalize();

		for (FbxVertex* vertex : data->vertices)
		{
			int cpIndex = vertex->controlPoint;

			FbxBlendWeight weights;
			boneWeights[cpIndex].GetBlendWeiths(weights);
			vertex->vertex.BlendIndices = weights.indices;
			vertex->vertex.BlendWeights = weights.weights;
		}

		for (int i = 0; i < scene->GetMaterialCount(); i++)
		{
			fbxsdk::FbxSurfaceMaterial* material = scene->GetMaterial(i);
			string materialName = material->GetName();

			vector<FbxVertex*> gather;
			for (FbxVertex* vertex : data->vertices)
			{
				if (vertex->materialName == materialName)
					gather.push_back(vertex);
			}
			if (gather.size() == 0)
				continue;

			FbxMeshPartData* meshPart = new FbxMeshPartData();
			meshPart->materialName = materialName;

			for (FbxVertex* temp : gather)
			{
				ModelVertexType vertex;
				vertex = temp->vertex;

				meshPart->vertices.push_back(vertex);
				meshPart->indices.push_back(meshPart->indices.size());
			}

			data->meshParts.push_back(meshPart);
		}
	}
}

void Exporter::WriteMesh(string saveFile, string saveFolder)
{
	Path::CreateFolders(saveFolder);

	wstring path = String::ToWString(saveFolder + saveFile);
	BinaryWriter* w = new BinaryWriter();
	w->Open(path);

	w->UInt(bones.size());
	for (FbxBoneData* bone : bones)
	{
		w->Int(bone->index);
		w->String(bone->name);
		w->Int(bone->parent);

		w->Matrix(bone->local);
		w->Matrix(bone->global);

		delete bone;
	}

	w->UInt(meshes.size());
	for (FbxMeshData* data : meshes)
	{
		w->String(data->name);
		w->Int(data->parentBone);

		w->UInt(data->meshParts.size());
		for (FbxMeshPartData* part : data->meshParts)
		{
			w->String(part->materialName);

			w->UInt(part->vertices.size());
			w->BYTE(part->vertices.data(), sizeof(ModelVertexType) * part->vertices.size());

			w->UInt(part->indices.size());
			w->BYTE(part->indices.data(), sizeof(UINT) * part->indices.size());

			delete part;
		}
		delete data;
	}
	w->Close();
	delete w;
}

FbxClip* Exporter::ReadAnimation(UINT index)
{
	UINT stackCount = (UINT)importer->GetAnimStackCount();

	FbxTime::EMode mode = scene->GetGlobalSettings().GetTimeMode();
	float frameRate = (float)FbxTime::GetFrameRate(mode);

	FbxClip* clip = new FbxClip();
	clip->frameRate = frameRate;

	FbxTakeInfo* takeInfo = importer->GetTakeInfo(index);
	clip->name = takeInfo->mName.Buffer();

	FbxTimeSpan span = takeInfo->mLocalTimeSpan;
	FbxTime tempDuration = span.GetDuration();

	int start = (int)span.GetStart().GetFrameCount();
	int end = (int)span.GetStop().GetFrameCount();

	float duration = (float)tempDuration.GetMilliSeconds();

	if (start < end)
		ReadKeyFrameData(clip, scene->GetRootNode(), start, end);

	clip->duration = duration;
	clip->frameCount = (end - start) + 1;

	return clip;
}

void Exporter::ReadKeyFrameData(FbxClip* clip, FbxNode* node, int start, int end)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute != nullptr)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();

		if (nodeType == FbxNodeAttribute::eSkeleton)
		{
			FbxKeyFrame* keyFrame = new FbxKeyFrame();
			keyFrame->boneName = node->GetName();

			for (int i = start ; i <= end; i++)
			{
				FbxTime animationTime;
				animationTime.SetFrame(i);

				FbxAMatrix matrix = node->EvaluateLocalTransform(animationTime);
				Matrix transform = FbxUtility::ToMatrix(matrix);

				FbxKeyFrameData data;
				data.time = (float)animationTime.GetMilliSeconds();

				XMVECTOR scale = XMLoadFloat3(&data.scale);
				XMVECTOR position = XMLoadFloat3(&data.position);

				XMMatrixDecompose(&scale, &data.rotation, &position, transform);
				XMStoreFloat3(&data.scale, scale);
				XMStoreFloat3(&data.position, position);
				keyFrame->transforms.push_back(data);
			}

			clip->keyFrames.push_back(keyFrame);
		}
	}

	for (int i = 0; i < node->GetChildCount(); i++)
		ReadKeyFrameData(clip, node->GetChild(i), start, end);
}

void Exporter::WriteClip(FbxClip* clip, string saveFile, string saveFolder)
{
	Path::CreateFolders(saveFolder);

	BinaryWriter* w = new BinaryWriter();
	w->Open(String::ToWString(saveFolder + saveFile));

	w->String(clip->name);
	w->Float(clip->duration);
	w->Float(clip->frameRate);
	w->Int(clip->frameCount);

	w->UInt(clip->keyFrames.size());
	for (FbxKeyFrame* frame : clip->keyFrames)
	{
		w->String(frame->boneName);

		w->UInt(frame->transforms.size());
		w->BYTE(frame->transforms.data(), sizeof(FbxKeyFrameData) * frame->transforms.size());

		delete frame;
	}

	w->Close();
	delete clip;
	delete w;
}

UINT Exporter::GetBoneIndexByName(string name)
{
	for (UINT i = 0; i < bones.size(); i++)
	{
		if (bones[i]->name == name)
			return i;
	}

	return -1;
}
