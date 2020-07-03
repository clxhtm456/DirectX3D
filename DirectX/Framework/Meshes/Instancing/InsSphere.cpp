#include "Framework.h"
#include "InsSphere.h"

InsSphere * InsSphere::Create(float radius, UINT stackCount, UINT sliceCount)
{
	auto pRet = new InsSphere();
	if (pRet && pRet->Init(radius,stackCount, sliceCount))
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

InsSphere::InsSphere()
{

}

bool InsSphere::Init(float radius, UINT stackCount, UINT sliceCount)
{
	_radius = radius;
	_stackCount = stackCount;
	_sliceCount = sliceCount;

	CreateBuffer();
	return true;
}

InsSphere::~InsSphere()
{

}

void InsSphere::CreateMesh()
{
	vector<MeshVertex> v;
	v.push_back(MeshVertex(0, _radius, 0, 0, 0, 0, 1, 0, 1, 0, 0));

	float phiStep = Math::PI / _stackCount;
	float thetaStep = 2.0f * Math::PI / _sliceCount;

	for (UINT i = 1; i <= _stackCount - 1; i++)
	{
		float phi = i * phiStep;

		for (UINT j = 0; j <= _sliceCount; j++)
		{
			float theta = j * thetaStep;

			Vector3 p = Vector3
			(
				(_radius * sinf(phi) * cosf(theta)),
				(_radius * cosf(phi)),
				(_radius * sinf(phi) * sinf(theta))
			);

			Vector3 t = Vector3
			(
				-_radius * sinf(phi) * sinf(theta),
				0,
				_radius * sinf(phi) * cosf(theta)
			);

			XMVECTOR vT = XMLoadFloat3(&t);
			vT = XMVector3Normalize(vT);
			XMStoreFloat3(&t, vT);

			XMVECTOR vN;
			Vector3 n;
			XMVECTOR vP = XMLoadFloat3(&p);
			vN = XMVector3Normalize(vP);
			XMStoreFloat3(&n, vN);

			Vector2 uv = Vector2(theta / (Math::PI * 2), phi / Math::PI);
			v.push_back(MeshVertex(p.x, p.y, p.z, uv.x, uv.y, n.x, n.y, n.z, t.x, t.y, t.z));
		}
	}
	v.push_back(MeshVertex(0, -_radius, 0, 0, 0, 0, -1, 0, -1, 0, 0));

	vertices = new MeshVertex[v.size()];
	vertexCount = v.size();
	copy(v.begin(), v.end(), stdext::checked_array_iterator<MeshVertex*>(vertices, vertexCount));


	vector<UINT> indices;
	for (UINT i = 1; i <= _sliceCount; i++)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	UINT baseIndex = 1;
	UINT ringVertexCount = _sliceCount + 1;
	for (UINT i = 0; i < _stackCount - 2; i++)
	{
		for (UINT j = 0; j < _sliceCount; j++)
		{
			indices.push_back(baseIndex + i * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	UINT southPoleIndex = v.size() - 1;
	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT i = 0; i < _sliceCount; i++)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}

	this->indices = new UINT[indices.size()];
	indexCount = indices.size();
	copy(indices.begin(), indices.end(), stdext::checked_array_iterator<UINT *>(this->indices, indexCount));
}
