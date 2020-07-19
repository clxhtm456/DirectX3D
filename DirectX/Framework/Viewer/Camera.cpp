#include "Framework.h"
#include "Camera.h"
#include "Perspective.h"
#include "Viewport.h"
#include "Renders/Render2D.h"
#include "DefferedRender/GBuffer.h"
#include "Frustum.h"

Camera * Camera::Create(CameraOption option)
{
	auto pRet = new Camera();
	if (pRet && pRet->Init(option))
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

bool Camera::Init(CameraOption option)
{
	default = option;
	CreateCameraDefault();

	if (default.useGBuffer == true)
	{
		CreateRender2DOption();
	}
	Rotate();
	Move();

	SetAllCameraMask();

	return true;
}

Camera::Camera() :
	Node()
{
	
}

Camera::~Camera()
{
	delete perspective;
	delete viewport;
	delete viewProjection;
	delete m_Frustum;

	if (default.useGBuffer == false)
		return;

	delete renderTarget;
	delete depthStencil;
	delete renderViewport;
	delete gBuffer;
	renderImage->Release();
	//delete renderImage;
}

void Camera::Update()
{
	GetVPBuffer()->SetView(GetViewMatrix());
	GetVPBuffer()->SetProjection(GetProjectionMatrix());

	if (default.useGBuffer == false)
		return;

	renderImage->Update();
}


Matrix Camera::GetMatrix()
{
	return matView;
}

void Camera::SetPosition(Vector3 position)
{
	Node::SetPosition(position);

	Move();
}

void Camera::SetRotation(Vector3 rotation)
{
	Node::SetRotation(rotation);

	Rotate();
}

void Camera::SetRotationDegree(Vector3 rotation)
{
	Node::SetRotationDegree(rotation);

	Rotate();
}

void Camera::SetPosition(float x, float y, float z)
{
	Vector3 temp = Vector3(x, y, z);
	SetPosition(temp);
}

void Camera::SetRotation(float x, float y, float z)
{
	Vector3 temp = Vector3(x, y, z);
	SetRotation(temp);
}

void Camera::SetRotationDegree(float x, float y, float z)
{
	Vector3 temp = Vector3(x, y, z);
	SetRotationDegree(temp);
}

void Camera::Resize()
{
	perspective->Set(D3D::Width(), D3D::Height(), default.zn, default.zf, default.fov);
	viewport->Set(D3D::Width(), D3D::Height(), default.x, default.y, default.minDepth, default.maxDepth);
}


void Camera::CreateCameraDefault()
{
	matView = XMMatrixIdentity();
	matRotation = XMMatrixIdentity();

	perspective = new Perspective(default.Width, default.Height, default.zn, default.zf, default.fov);
	viewport = new Viewport(default.Width, default.Height, default.x, default.y, default.minDepth, default.maxDepth);
	viewProjection = new ViewProjectionBuffer();

	viewProjection->SetProjection(perspective->GetMatrix());

	m_Frustum = new Frustum();
	m_Frustum->ConstructFrustum(default.zf, GetProjectionMatrix(), GetViewMatrix());
}

void Camera::CreateRender2DOption()
{
	renderTarget = new RenderTarget();
	depthStencil = new DepthStencil();
	renderViewport = new Viewport(D3D::Width(), D3D::Height(), default.x, default.y, default.minDepth, default.maxDepth);

	renderImage = Render2D::Create();
	renderImage->Retain();

	renderImage->SetPosition(D3D::Width()*0.5f, D3D::Height()*0.5f, 0);
	renderImage->SetScale(D3D::Width()*0.9f, D3D::Height()*0.9f, 1);
	renderImage->SetSRV(renderTarget->SRV());

	gBuffer = new GBuffer(D3D::Width(), D3D::Height());
}

void Camera::Move()
{
	View();
}

void Camera::Rotate()
{
	Matrix X, Y, Z;
	Vector3 rotation;
	XMStoreFloat3(&rotation ,_rotation);
	X = DirectX::XMMatrixRotationX(rotation.x);
	Y = DirectX::XMMatrixRotationY(rotation.y);
	Z = DirectX::XMMatrixRotationZ(rotation.z);

	matRotation = X * Y * Z;

	//저장될 값 <- ..의 방향 <- ..공간
	forward = XMVector3TransformNormal(XMVectorSet(0.0f,0.0f,1.0f,0.0f), matRotation);
	up = XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), matRotation);
	right = XMVector3TransformNormal(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), matRotation);

	Vector3 tempUp;
	XMStoreFloat3(&tempUp, up);
	View();
	
}

void Camera::View()
{
	matView = XMMatrixLookAtLH(_position, _position + (forward), up);
}

void Camera::SetRNShader2Depth(RenderingNode * node)
{
	if (default.useGBuffer == false)
		return;
	psShaderSlot = node->GetPSShader();
	//psShaderSlot = node->GetVSShader();
	//node->SetShader(depthShader);
	node->SetPSShader(gBuffer->GetShader());
}

void Camera::SetRNShader2Origin(RenderingNode * node)
{
	if (default.useGBuffer == false)
		return;
	node->SetPSShader(psShaderSlot);
	//node->SetPSShader(psShaderSlot);
}

bool Camera::CheckMask(UINT targetMask)
{
	return cameraMask & targetMask;
}

void Camera::AddMask(UINT mask)
{
	cameraMask |= mask;
}

void Camera::DelMask(UINT mask)
{

	cameraMask &= ~mask;
}

void Camera::SetAllCameraMask()
{
	AddMask(TYPEMASK::ALL);
}

RenderTarget* Camera::GetRenderTarget()
{
	if (default.useGBuffer == false)
		return nullptr;

	return renderTarget;
}

Matrix Camera::GetViewMatrix()
{
	return matView;
}

Matrix Camera::GetProjectionMatrix()
{
	return perspective->GetMatrix();
}

void Camera::PostUpdate()
{
}

void Camera::LateUpdate()
{
}

void Camera::SetUpRender()
{
	m_Frustum->ConstructFrustum(default.zf*0.5f, GetProjectionMatrix(), GetViewMatrix());
	if (default.useGBuffer == false)
	{
		viewport->RSSetViewport();
	}
	else
	{
		/*renderTarget->Set(depthStencil);
		viewport->RSSetViewport();*/
		gBuffer->PackGBuffer();
	}
}

void Camera::Render(Camera* viewer)
{
	if (default.useGBuffer == false)
		return;

	D3D::Get()->SetRenderTarget();
	D3D::Get()->Clear(Color(0,0,1,1));

	viewport->RSSetViewport();

	//renderImage->PostRender(this);
	gBuffer->Render(viewer);
}


void Camera::PostRender(Camera* viewer)
{
	if (default.useGBuffer == false)
		return;
	gBuffer->DebugRender(viewer);
}
