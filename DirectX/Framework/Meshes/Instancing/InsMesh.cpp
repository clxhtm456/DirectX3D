#include "Framework.h"
#include "InsMesh.h"

UINT InsMesh::instancingIndex = 0;

InsMesh::InsMesh() : RenderingNode()
{
	//rasterizerState->FillMode(D3D11_FILL_MODE::D3D11_FILL_WIREFRAME);
}

InsMesh::~InsMesh()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

}


bool InsMesh::CreateBuffer()
{
	return true;
}

void InsMesh::Update()
{
}

void InsMesh::Render(Camera* viewer)
{
	
}
