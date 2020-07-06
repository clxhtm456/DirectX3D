#include "Framework.h"
#include "ModelData.h"

//////////////////////////////////////////////////////
/*

				ModelMesh

*/
//////////////////////////////////////////////////////

ModelMesh::ModelMesh()
{
}

ModelMesh::~ModelMesh()
{
	for (ModelMeshPart* part : meshParts)
		delete part;
}

void ModelMesh::Binding()
{
	for (ModelMeshPart* part : meshParts)
		part->Binding();
}

void ModelMesh::Render()
{
	for (ModelMeshPart* part : meshParts)
		part->Render();
}

//////////////////////////////////////////////////////
/*

				ModelMeshPart

*/
//////////////////////////////////////////////////////

ModelMeshPart::ModelMeshPart()
{
}

ModelMeshPart::~ModelMeshPart()
{
	delete indexBuffer;
	delete vertexBuffer;

}

void ModelMeshPart::Render()
{
	vertexBuffer->Render();
	indexBuffer->Render();

	material->Render();


	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D::GetDC()->DrawIndexed(indices.size(), 0, 0);
}

void ModelMeshPart::Binding()
{
	/*Vertex tvertices[4];
	tvertices[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
	tvertices[1].Position = Vector3(-0.5f, +0.5f, 0.0f);
	tvertices[2].Position = Vector3(+0.5f, -0.5f, 0.0f);
	tvertices[3].Position = Vector3(+0.5f, +0.5f, 0.0f);

	UINT tindices[6];
	tindices[0] = 0;
	tindices[1] = 1;
	tindices[2] = 2;
	tindices[3] = 2;
	tindices[4] = 1;
	tindices[5] = 3;

	vertexBuffer = new VertexBuffer(tvertices, 4,sizeof(Vertex));
	indexBuffer = new IndexBuffer(tindices, 6);*/
	vertexBuffer = new VertexBuffer(vertices.data(), vertices.size(),sizeof(ModelVertexType));
	indexBuffer = new IndexBuffer(indices.data(), indices.size());
}

//////////////////////////////////////////////////////
/*

				ModelData

*/
//////////////////////////////////////////////////////

ModelData::ModelData(string modelDir)
{
	boneBuffer = new BoneBuffer();

	ReadMaterial(modelDir);
	ReadMesh(modelDir);
}

ModelData::~ModelData()
{
	delete boneBuffer;

	for (Material* material : materials)
		delete material;

	for (ModelBone* bone : bones)
		delete bone;

	for (ModelMesh* mesh : meshes)
		delete mesh;
}

void ModelData::ReadMaterial(string file)
{
	file = "../../_Assets/Materials/" + file + ".mat";

	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLError error = document->LoadFile(file.c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();
	Xml::XMLElement* materialNode = root->FirstChildElement();

	do
	{
		Material* material = new Material();

		Xml::XMLElement* node = NULL;

		node = materialNode->FirstChildElement();
		material->SetName(String::ToWString(node->GetText()));

		wstring texture = L"";

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->SetDiffuseMap(texture);

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->SetSpecularMap(texture);

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->SetNormalMap(texture);

		Color color;

		node = node->NextSiblingElement();
		color.x = node->FloatAttribute("R");
		color.y = node->FloatAttribute("G");
		color.z = node->FloatAttribute("B");
		color.w = node->FloatAttribute("A");
		material->SetAmbient(color);

		node = node->NextSiblingElement();
		color.x = node->FloatAttribute("R");
		color.y = node->FloatAttribute("G");
		color.z = node->FloatAttribute("B");
		color.w = node->FloatAttribute("A");
		material->SetDiffuse(color);

		node = node->NextSiblingElement();
		color.x = node->FloatAttribute("R");
		color.y = node->FloatAttribute("G");
		color.z = node->FloatAttribute("B");
		color.w = node->FloatAttribute("A");
		material->SetEmissive(color);

		materials.push_back(material);

		materialNode = materialNode->NextSiblingElement();
	} while (materialNode != NULL);

	delete document;
}

void ModelData::ReadMesh(string file)
{
	file = "../../_Assets/Meshes/" + file + ".mesh";

	BinaryReader* r = new BinaryReader();
	r->Open(String::ToWString(file));

	UINT count = r->UInt();

	for (UINT i = 0; i < count; i++)
	{
		ModelBone* bone = new ModelBone();

		bone->index = r->Int();
		bone->name = r->String();
		bone->parentIndex = r->Int();

		bone->local = r->Matrix();
		bone->global = r->Matrix();

		bones.push_back(bone);
	}

	count = r->UInt();

	for (UINT i = 0; i < count; i++)
	{
		ModelMesh* mesh = new ModelMesh();
		mesh->name = r->String();
		mesh->parentBoneIndex = r->Int();

		UINT partCount = r->UInt();
		for (UINT j = 0; j < partCount; j++)
		{
			ModelMeshPart* meshPart = new ModelMeshPart();
			meshPart->parent = mesh;
			meshPart->materialName = r->String();

			{
				UINT count = r->UInt();
				meshPart->vertices.resize(count);

				void* ptr = (void*)meshPart->vertices.data();
				r->BYTE(&ptr, sizeof(ModelVertexType) * count);
			}

			{
				UINT count = r->UInt();
				meshPart->indices.resize(count);

				void* ptr = (void*)meshPart->indices.data();
				r->BYTE(&ptr, sizeof(UINT) * count);
			}

			mesh->meshParts.push_back(meshPart);
		}
		meshes.push_back(mesh);
	}

	delete r;

	BindingBone();
	BindingMesh();
}

void ModelData::CopyGlobalBoneTo(vector<Matrix>& transforms)
{
	Matrix w;
	w = XMMatrixIdentity();

	CopyGlobalBoneTo(transforms, w);
}

void ModelData::CopyGlobalBoneTo(vector<Matrix>& transforms, Matrix& w)
{
	transforms.clear();
	transforms.resize(bones.size());

	for (UINT i = 0; i < bones.size(); i++)
	{
		ModelBone* bone = bones[i];

		if (bone->parent != nullptr)
		{
			int index = bone->parent->index;
			transforms[i] = bone->local * transforms[index];
		}
		else
			transforms[i] = bone->local * w;
	}
}

void ModelData::BindingBone()
{
	root = bones.front();

	for (ModelBone* bone : bones)
	{
		if (bone->parentIndex > -1)
		{
			bone->parent = bones[bone->parentIndex];
			bone->parent->children.push_back(bone);
		}
		else
			bone->parent = nullptr;
	}
}

void ModelData::BindingMesh()
{
	for (ModelMesh* mesh : meshes)
	{
		for (ModelBone* bone : bones)
		{
			if (mesh->parentBoneIndex == bone->index)
			{
				mesh->parentBone = bone;
				break;
			}
		}

		for (ModelMeshPart* part : mesh->meshParts)
		{
			for (Material* material : materials)
			{
				if (material->GetName() == String::ToWString(part->materialName))
				{
					part->material = material;
					break;
				}
			}
		}

		mesh->Binding();
	}
}


void ModelData::SetBones(Matrix* m, UINT count)
{
	boneBuffer->Bones(m, count);
	boneBuffer->SetVSBuffer(2);
}


ModelBone* ModelData::BoneByName(string name)
{
	for (ModelBone* bone : bones)
	{
		if (bone->name == name)
			return bone;
	}

	return nullptr;
}

//////////////////////////////////////////////////////
/*

				ModelKeyFrame

*/
//////////////////////////////////////////////////////

ModelKeyFrame::ModelKeyFrame()
{
}

ModelKeyFrame::~ModelKeyFrame()
{
}

Matrix ModelKeyFrame::GetInterpolatedMatrix(float time)
{

	UINT index1 = 0;
	UINT index2 = 0;
	float interpolatedTime = 0.0f;

	CalcKeyFrameIndex(time, index1, index2, interpolatedTime);

	return GetInterpolatedMatrix(index1, index2, interpolatedTime);
}

Matrix ModelKeyFrame::GetInterpolatedMatrix(UINT index1, UINT index2, float t)
{
	XMVECTOR scale;
	
	scale = XMVectorLerp(XMLoadFloat3(&transforms[index1].scale), XMLoadFloat3(&transforms[index2].scale), t);

	XMVECTOR rotation;
	rotation = XMQuaternionSlerp(XMLoadFloat4(&transforms[index1].rotation), XMLoadFloat4(&transforms[index2].rotation), t);

	XMVECTOR position;
	position = XMVectorLerp(XMLoadFloat3(&transforms[index1].position), XMLoadFloat3(&transforms[index2].position), t);

	Matrix S, R, T;
	S = XMMatrixScaling(XMVectorGetX(scale), XMVectorGetY(scale), XMVectorGetZ(scale));
	R = XMMatrixRotationQuaternion(rotation);
	T = XMMatrixTranslation(XMVectorGetX(position), XMVectorGetY(position), XMVectorGetZ(position));

	return S * R * T;
}

UINT ModelKeyFrame::GetKeyFrameIndex(float time)
{
	UINT start = 0, end = frameCount - 1;
	if (time >= transforms[end].time)
		return end;

	do
	{
		UINT middle = (start + end) / 2;

		if (end - start <= 1)
			break;
		else if (transforms[middle].time < time)
			start = middle;
		else if (transforms[middle].time > time)
			end = middle;
		else
		{
			start = middle;
			break;
		}
	} while ((end - start) > 1);

	return start;
}

void ModelKeyFrame::CalcKeyFrameIndex(float time, OUT UINT& index1, OUT UINT& index2, OUT float& interpolatedTime)
{
	index1 = GetKeyFrameIndex(time);
	index2 = index1 + 1;

	if (index1 >= frameCount - 1)
	{
		index1 = index2 = frameCount - 1;
		interpolatedTime = 1.0f;
	}
	else
	{
		float time1 = time - transforms[index1].time;
		float time2 = transforms[index2].time - transforms[index1].time;

		interpolatedTime = time1 / time2;
	}
}
//////////////////////////////////////////////////////
/*

				ModelClip

*/
//////////////////////////////////////////////////////
ModelClip::ModelClip(string file) : 
	playTime(0.0f), 
	isRepeat(false), 
	speed(1.0f), 
	isLockRoot(false),
	EndEvent(nullptr)
{
	ReadAnimation(file);
}


ModelClip::~ModelClip()
{
	for (auto key : keyFrameMap)
		delete key.second;
}

Matrix ModelClip::GetKeyFrameMatrix(ModelBone* bone)
{
	string boneName = bone->name;

	if (keyFrameMap.find(boneName) == keyFrameMap.end())
	{
		return XMMatrixIdentity();
	}

	ModelKeyFrame* keyFrame = keyFrameMap[boneName];

	playTime += speed * Time::Delta();

	if (isRepeat)
	{
		if (playTime >= duration)
		{
			while (playTime - duration >= 0)
				playTime -= duration;
		}
	}
	else
	{
		if (playTime >= duration)
		{
			playTime = duration;
			if (EndEvent != nullptr)
			{
				playTime = 0.0f;
				EndEvent();
			}
		}
	}

	Matrix invGlobal = bone->global;
	invGlobal = XMMatrixInverse(nullptr, invGlobal);

	Matrix animation = keyFrame->GetInterpolatedMatrix(playTime);

	Matrix parent;
	int parentIndex = bone->parentIndex;
	if (parentIndex < 0)
	{
		if (isLockRoot)
			parent = XMMatrixIdentity();
		else
			parent = animation;
	}
	else
	{
		parent = animation * bone->parent->global;
	}

	return invGlobal * parent;
}

void ModelClip::UpdateKeyFrame(ModelBone* bone)
{
	bone->local = GetKeyFrameMatrix(bone);
}

void ModelClip::ReadAnimation(string file)
{
	if (Path::ExistFile(file) == false)
	{

	}
	BinaryReader* r = new BinaryReader();
	r->Open(String::ToWString(file));

	name = r->String();
	duration = r->Float();
	frameRate = r->Float();
	frameCount = r->Int();

	UINT count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		ModelKeyFrame* key = new ModelKeyFrame();
		key->boneName = r->String();
		key->duration = duration;
		key->frameCount = frameCount;
		key->frameRate = frameRate;

		UINT size = r->UInt();
		if (size > 0)
		{
			key->transforms.resize(size);

			void* ptr = (void*)key->transforms.data();
			r->BYTE(&ptr, sizeof(FbxKeyFrameData) * size);

		}
		keyFrameMap.insert({ key->boneName, key });
	}

	delete r;
}

void ModelClip::Reset()
{
	isRepeat = false;
	speed = 1.0f;
	playTime = 0.0f;
}
