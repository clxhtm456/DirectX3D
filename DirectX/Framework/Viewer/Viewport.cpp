#include "Framework.h"
#include "Viewport.h"

Viewport::Viewport(float width, float height, float x, float y, float minDepth, float maxDepth)
{
	Set(width, height, x, y, minDepth, maxDepth);
}

Viewport::~Viewport()
{
	
}

void Viewport::Set(float width, float height, float x, float y, float minDepth, float maxDepth)
{
	viewport.TopLeftX = this->x = x;
	viewport.TopLeftY = this->y = y;
	viewport.Width = this->width = width;
	viewport.Height = this->height = height;
	viewport.MinDepth = this->minDepth = minDepth;
	viewport.MaxDepth = this->maxDepth = maxDepth;

	RSSetViewport();
}

void Viewport::Project(Vector3 * outPosition, Vector3 source, Matrix & W, Matrix & V, Matrix & P)
{
	Vector3 position = source;

	Matrix wvp = W * V * P;
	XMStoreFloat3(outPosition,XMVector3TransformCoord(XMLoadFloat3(&position), wvp));

	//viewport ��ǥ
	outPosition->x = ((outPosition->x + 1.0f) * 0.5f) * width + x;
	outPosition->y = ((-outPosition->y + 1.0f) * 0.5f) * height + y;
	outPosition->z = (outPosition->z * (maxDepth - minDepth)) + minDepth;
}

void Viewport::UnProject(Vector3 * outPosition, Vector3 source, Matrix & W, Matrix & V, Matrix & P)
{
	Vector3 position = source;
	
	//NDC ��ǥ
	outPosition->x = ((position.x - x) / width) * 2.0f - 1.0f;
	outPosition->y = (((position.y - y) / height) * 2.0f - 1.0f) * -1.0f;
	outPosition->z = (position.z - minDepth) / (maxDepth - minDepth);

	Matrix wvp = W * V * P;
	wvp = XMMatrixInverse(NULL, wvp);
	XMStoreFloat3(outPosition, XMVector3TransformCoord(XMLoadFloat3(outPosition), wvp));
}

/*
void Viewport::Project(XMVECTOR& outPosition, Vector3 source, Matrix & W, Matrix & V, Matrix & P)
{
	Vector3 position = source;

	Matrix wvp = W * V * P;
	outPosition = XMVector3TransformCoord(XMLoadFloat3(&position), wvp);

	//viewport ��ǥ
	XMVectorSetX(outPosition, ((XMVectorGetX(outPosition) + 1.0f) * 0.5f) * width + x);
	XMVectorSetY(outPosition, (-1.0f*(XMVectorGetY(outPosition) + 1.0f) * 0.5f) * height + y);
	XMVectorSetZ(outPosition, ((XMVectorGetZ(outPosition) * (maxDepth - minDepth) + minDepth)));

}

void Viewport::UnProject(XMVECTOR & outPosition, Vector3 source, Matrix & W, Matrix & V, Matrix & P)
{
	Vector3 position = source;
	XMVECTOR tempPos = XMLoadFloat3(&position);
	//NDC ��ǥ
	XMVectorSetX(outPosition, ((XMVectorGetX(tempPos) -x) / width) * 2.0f - 1.0f);
	XMVectorSetY(outPosition, (((XMVectorGetY(tempPos) -y) / height) * 2.0f -1.0f)*-1.0f);
	XMVectorSetZ(outPosition, ((XMVectorGetZ(tempPos) -minDepth)/ (maxDepth - minDepth)));

	Matrix wvp = W * V * P;
	wvp = XMMatrixInverse(NULL, wvp);
	outPosition = XMVector3TransformCoord(outPosition, wvp);
}
*/



void Viewport::RSSetViewport()
{
	D3D::GetDC()->RSSetViewports(1, &viewport);
}