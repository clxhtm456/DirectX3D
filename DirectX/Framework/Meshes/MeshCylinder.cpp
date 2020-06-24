#include "Framework.h"
#include "MeshCylinder.h"

MeshCylinder * MeshCylinder::Create(float radius, float height, UINT sliceCount, UINT stackCount)
{
	auto pRet = new MeshCylinder();
	if (pRet && pRet->Init(radius,height,sliceCount,stackCount))
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

bool MeshCylinder::Init(float radius, float height, UINT sliceCount, UINT stackCount)
{
	_topRadius = radius;
	_bottomRadius = radius;
	_height = height;
	_sliceCount = sliceCount;
	_stackCount = stackCount;

	CreateBuffer();
	return true;
}

MeshCylinder::MeshCylinder()
{

}

MeshCylinder::~MeshCylinder()
{

}

void MeshCylinder::CreateMesh()
{
	vector<MeshVertex> vertices;

	float stackHeight = _height / (float)_stackCount;
	float radiusStep = (_topRadius - _bottomRadius) / (float)_stackCount;

	UINT ringCount = _stackCount + 1;
	for (UINT i = 0; i < ringCount; i++)
	{
		float y = -0.5f * _height + i * stackHeight;
		float r = _bottomRadius + i * radiusStep;
		float theta = 2.0f * Math::PI / (float)_sliceCount;

		for (UINT k = 0; k <= _sliceCount; k++)
		{
			float c = cosf(k * theta);
			float s = sinf(k * theta);


			MeshVertex vertex;
			vertex.Position = Vector3(r * c, y, r * s);
			vertex.Uv = Vector2((float)k / (float)_sliceCount, 1.0f - (float)i / (float)_stackCount);


			Vector3 tangent = Vector3(-s, 0.0f, c);

			float dr = _bottomRadius - _topRadius;
			Vector3 biTangent = Vector3(dr * c, -_height, dr * s);

			XMVECTOR vTangent =  XMLoadFloat3(&tangent);
			XMVECTOR vBiTangent = XMLoadFloat3(&biTangent);
			XMVECTOR vVertexNormal = XMVector3Cross(vTangent, vBiTangent);

			vVertexNormal = XMVector3Normalize(vVertexNormal);
			XMStoreFloat3(&vertex.Normal, vVertexNormal);

			vertex.Tangent = tangent;

			vertices.push_back(vertex);
		}
	}


	vector<UINT> indices;
	UINT ringVertexCount = _sliceCount + 1;
	for (UINT y = 0; y < _stackCount; y++)
	{
		for (UINT x = 0; x < _sliceCount; x++)
		{
			indices.push_back(y * ringVertexCount + x);
			indices.push_back((y + 1) * ringVertexCount + x);
			indices.push_back((y + 1) * ringVertexCount + (x + 1));

			indices.push_back(y * ringVertexCount + x);
			indices.push_back((y + 1) * ringVertexCount + x + 1);
			indices.push_back(y * ringVertexCount + x + 1);
		}
	}

	BuildTopCap(vertices, indices);
	BuildBottomCap(vertices, indices);


	this->vertices = new MeshVertex[vertices.size()];
	vertexCount = vertices.size();
	copy(vertices.begin(), vertices.end(), stdext::checked_array_iterator<MeshVertex *>(this->vertices, vertexCount));

	this->indices = new UINT[indices.size()];
	indexCount = indices.size();
	copy(indices.begin(), indices.end(), stdext::checked_array_iterator<UINT *>(this->indices, indexCount));
}

void MeshCylinder::BuildTopCap(vector<MeshVertex>& vertices, vector<UINT>& indices)
{
	float y = 0.5f * _height;
	float theta = 2.0f * Math::PI / (float)_sliceCount;

	for (UINT i = 0; i <= _sliceCount; i++)
	{
		float x = _topRadius * cosf(i * theta);
		float z = _topRadius * sinf(i * theta);

		float u = x / _height + 0.5f;
		float v = z / _height + 0.5f;

		vertices.push_back(MeshVertex(x, y, z, u, v, 0, 1, 0, 1, 0, 0));
	}
	vertices.push_back(MeshVertex(0, y, 0, 0.5f, 0.5f, 0, 1, 0, 1, 0, 0));


	UINT baseIndex = vertices.size() - _sliceCount - 2;
	UINT centerIndex = vertices.size() - 1;

	for (UINT i = 0; i < _sliceCount; i++)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i + 1);
		indices.push_back(baseIndex + i);
	}
}

void MeshCylinder::BuildBottomCap(vector<MeshVertex>& vertices, vector<UINT>& indices)
{
	float y = -0.5f * _height;
	float theta = 2.0f * Math::PI / (float)_sliceCount;

	for (UINT i = 0; i <= _sliceCount; i++)
	{
		float x = _bottomRadius * cosf(i * theta);
		float z = _bottomRadius * sinf(i * theta);

		float u = x / _height + 0.5f;
		float v = z / _height + 0.5f;

		vertices.push_back(MeshVertex(x, y, z, u, v, 0, -1, 0, -1, 0, 0));
	}
	vertices.push_back(MeshVertex(0, y, 0, 0.5f, 0.5f, 0, -1, 0, -1, 0, 0));


	UINT baseIndex = vertices.size() - _sliceCount - 2;
	UINT centerIndex = vertices.size() - 1;

	for (UINT i = 0; i < _sliceCount; i++)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}
}