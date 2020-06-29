#include "Framework.h"
#include "FModelData.h"

FModelBone::FModelBone()
{
}

FModelBone::~FModelBone()
{
}

FModelMesh::FModelMesh()
{
	intBuffer = new IntBuffer();
}

FModelMesh::~FModelMesh()
{
	delete[] vertices;
	delete[] indices;

	delete vertexBuffer;
	delete indexBuffer;

	delete intBuffer;
}

void FModelMesh::Binding(FModelData* model)
{
	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(ModelVertexType));
	indexBuffer = new IndexBuffer(indices, indexCount);

	Material* srcMaterial = model->MaterialByName(materialName);

	material = new Material();
	material->SetAmbient(srcMaterial->GetAmbient());
	material->SetDiffuse(srcMaterial->GetDiffuse());
	material->SetSpecular(srcMaterial->GetSpecular());
	material->SetEmissive(srcMaterial->GetEmissive());

	if (srcMaterial->GetDiffuseMap() != nullptr)
		material->SetDiffuseMap(srcMaterial->GetDiffuseMap()->GetFile());

	if (srcMaterial->GetSpecularMap() != nullptr)
		material->SetSpecularMap(srcMaterial->GetSpecularMap()->GetFile());

	if (srcMaterial->GetNormalMap() != nullptr)
		material->SetNormalMap(srcMaterial->GetNormalMap()->GetFile());
}

void FModelMesh::Update()
{
}

void FModelMesh::Render()
{
	intBuffer->data.index = boneIndex;
	intBuffer->SetVSBuffer(3);

	D3D::GetDC()->PSSetShaderResources(3, 1, &SRV);
}

void FModelMesh::Transforms(Matrix* transform)
{
}

void FModelMesh::TransformsSRV(ID3D11ShaderResourceView* srv)
{
}

FModelData::FModelData()
{
}

FModelData::~FModelData()
{
}

Material* FModelData::MaterialByName(wstring name)
{
	return nullptr;
}

FModelBone* FModelData::BoneByName(wstring name)
{
	return nullptr;
}

FModelMesh* FModelData::MeshByName(wstring name)
{
	return nullptr;
}

void FModelData::Ambient(Color& color)
{
}

void FModelData::Ambient(float r, float g, float b, float a)
{
}

void FModelData::Diffuse(Color& color)
{
}

void FModelData::Diffuse(float r, float g, float b, float a)
{
}

void FModelData::Specular(Color& color)
{
}

void FModelData::Specular(float r, float g, float b, float a)
{
}

void FModelData::Emissive(Color& color)
{
}

void FModelData::Emissive(float r, float g, float b, float a)
{
}

void FModelData::ReadMaterial(wstring file)
{
}

void FModelData::ReadMesh(wstring file)
{
}

void FModelData::ReadClip(wstring file)
{
}

void FModelData::BindBone()
{
}

void FModelData::BindMesh()
{
}
