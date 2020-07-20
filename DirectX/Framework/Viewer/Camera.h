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

	bool useGBuffer = false;
};

class Camera : public Node
{
public:
	static Camera* Create(CameraOption option);
	bool Init(CameraOption option);
public:
	Camera();
	virtual ~Camera();

	virtual void Update();
	virtual void PostUpdate() override;
	virtual void LateUpdate() override;
	void SetUpRender();
	virtual void Render(Camera* viewer) override;
	virtual void PostRender(Camera* viewer) override;

public:
	Matrix GetMatrix();
	void SetPosition(Vector3 position) override;
	void SetRotation(Vector3 rotation) override;
	void SetRotationDegree(Vector3 rotation) override;
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetRotationDegree(float x, float y, float z);
	void Resize();

	XMVECTOR Foward() { return forward; }
	XMVECTOR Right() { return right; }
	XMVECTOR Up() { return up; }
	ViewProjectionBuffer* GetVPBuffer() { return viewProjection; }

	Matrix GetViewMatrix();
	Matrix GetProjectionMatrix();
	class Perspective* GetPerspective() { return perspective; }
	class Viewport* GetViewport() { return viewport; }
	class Frustum* GetFrustum() { return m_Frustum; }
	UINT ChildType() override
	{
		return TYPE_VIEWER;
	}
	RenderTarget* GetRenderTarget();
protected :
	void CreateCameraDefault();
	void CreateRender2DOption();
	virtual void Move();
	virtual void Rotate();
	virtual void View();
public:
	virtual void SetRNShader2Depth(RenderingNode* node);
	virtual void SetRNShader2Origin(RenderingNode* node);
private:
	Shader* psShaderSlot;
private:
	XMVECTOR forward = XMVectorSet(0, 0, 1,0);
	XMVECTOR up = XMVectorSet(0, 1, 0,0);
	XMVECTOR right = XMVectorSet(1, 0, 0,0);

	Matrix matRotation;
	Matrix matView;
public:
	bool CheckMask(UINT targetMask);
	void AddMask(UINT mask);
	void DelMask(UINT mask);
	UINT GetCameraMask() { return cameraMask; }
	void SetAllCameraMask();
private:
	UINT cameraMask = 0;
protected:
	class Perspective* perspective;
	class Viewport* viewport;

	class Frustum* m_Frustum;
	
	CameraOption default;
	
	ViewProjectionBuffer* viewProjection;
private:
	class DepthStencil* depthStencil;
	class GBuffer* gBuffer;

	class RenderTarget* renderTarget;
	class Render2D* renderImage;
	

};