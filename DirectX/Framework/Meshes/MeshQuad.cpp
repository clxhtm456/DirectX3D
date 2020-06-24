#include "Framework.h"
#include "MeshQuad.h"

MeshQuad * MeshQuad::Create()
{
	auto pRet = new MeshQuad();
	if (pRet && pRet->Init())
	{
		pRet->AutoRelease();
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}
	return pRet;
}

bool MeshQuad::Init()
{
	CreateBuffer();
	return true;
}

MeshQuad::MeshQuad()
{

}

MeshQuad::~MeshQuad()
{

}

void MeshQuad::CreateMesh()
{
	float w = 0.5f;
	float h = 0.5f;

	vector<MeshVertex> v;
	v.push_back(MeshVertex(-w, -h, 0, 0, 1, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(-w, +h, 0, 0, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(+w, -h, 0, 1, 1, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(+w, +h, 0, 1, 0, 0, 0, -1, 1, 0, 0));

	vertices = new MeshVertex[v.size()];
	vertexCount = v.size();

	copy
	(
		v.begin(), v.end(),
		stdext::checked_array_iterator<MeshVertex *>(vertices, vertexCount)
	);

	indexCount = 6;
	indices = new UINT[indexCount]{ 0, 1, 2, 2, 1, 3 };
}
