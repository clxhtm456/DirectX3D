#include "Framework.h"
#include "Camera.h"
#include "Perspective.h"
#include "Viewport.h"
#include "Renders/Render2D.h"

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
	CreateRender2DOption();

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

	if (default.useGBuffer == false)
		return;

	delete renderTarget;
	delete depthStencil;
	delete renderViewport;
	renderImage->Release();
	//delete renderImage;
}

void Camera::Update()
{
	GetVPBuffer()->SetView(ViewMatrix());
	GetVPBuffer()->SetProjection(ProjectionMatrix());

	if (default.useGBuffer == false)
		return;

	renderImage->Update();
}


void Camera::GetMatrix(Matrix * matrix)
{
	memcpy(matrix, &matView, sizeof(Matrix));
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
}

void Camera::CreateRender2DOption()
{
	if (default.useGBuffer == false)
		return;

	renderTarget = new RenderTarget();
	depthStencil = new DepthStencil();
	renderViewport = new Viewport(D3D::Width(), D3D::Height(), default.x, default.y, default.minDepth, default.maxDepth);

	renderImage = Render2D::Create();
	renderImage->Retain();

	renderImage->SetPosition(D3D::Width()*0.5f, D3D::Height()*0.5f, 0);
	renderImage->SetScale(D3D::Width()*0.9f, D3D::Height()*0.9f, 1);

	renderImage->SetSRV(renderTarget->SRV());
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

	//����� �� <- ..�� ���� <- ..����
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

void Camera::Draw(Camera* viewr)
{
}

RenderTarget* Camera::GetRenderTarget()
{
	if (default.useGBuffer == false)
		return nullptr;

	return renderTarget;
}

Matrix Camera::ViewMatrix()
{
	return matView;
}

Matrix Camera::ProjectionMatrix()
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

	if (default.useGBuffer == false)
	{
		viewport->RSSetViewport();
	}
	else
	{
		renderTarget->Set(depthStencil);
		viewport->RSSetViewport();
	}
}

void Camera::Render(Camera* viewer)
{
	if (default.useGBuffer == false)
		return;

	D3D::Get()->SetRenderTarget();
	D3D::Get()->Clear(Color(0,0,1,1));

	viewport->RSSetViewport();

	renderImage->PostRender(this);
}


void Camera::PostRender(Camera* viewer)
{
}
