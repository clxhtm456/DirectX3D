#pragma once

struct CameraOption
{
	float Width = 500;
	float Height = 500;
	float x = 0;
	float y = 0;
	float minDepth = 0;
	float maxDepth = 1;

	float zn = 0.1f;
	float zf = 1000.0f;
	float fov = (float)3.141592f * 0.25f;
};

class Camera : public Node
{
public:
	Camera(CameraOption option);
	virtual ~Camera();

	virtual void Update();

public:
	void GetMatrix(Matrix* matrix);

	XMVECTOR Foward() { return forward; }
	XMVECTOR Right() { return right; }
	XMVECTOR Up() { return up; }

protected :
	virtual void Move();
	virtual void Rotate();
	virtual void View();
private:
	Matrix ViewMatrix();
	Matrix ProjectionMatrix();
	class Perspective* GetPerspective() { return perspective; }
	class Viewport* GetViewport() { return viewport; }

private:
	XMVECTOR forward = XMVectorSet(0, 0, 1,0);
	XMVECTOR up = XMVectorSet(0, 1, 0,0);
	XMVECTOR right = XMVectorSet(1, 0, 0,0);

	Matrix matRotation;

protected:
	class Perspective* perspective;
	class Viewport* viewport;
	Matrix matView;

};