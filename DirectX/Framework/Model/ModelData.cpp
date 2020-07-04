#include "Framework.h"
#include "ModelData.h"

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
