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
	CreateCameraDefault(option);

	CreateRender2DOption();

	Rotate();
	Move();

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


void Camera::CreateCameraDefault(CameraOption option)
{
	default = option;
	matView = XMMatrixIdentity();
	matRotation = XMMatrixIdentity();

	perspective = new Perspective(option.Width, option.Height, option.zn, option.zf, option.fov);
	viewport = new Viewport(option.Width, option.Height, option.x, option.y, option.minDepth, option.maxDepth);
	viewProjection = new ViewProjectionBuffer();

	viewProjection->SetProjection(perspective->GetMatrix());
}

void Camera::CreateRender2DOption()
{
	renderTarget = new RenderTarget();
	depthStencil = new DepthStencil();
	renderViewport = new Viewport(D3D::Width(), D3D::Height(), default.x, default.y, default.minDepth, default.maxDepth);

	renderImage = Render2D::Create();
	renderImage->Retain();

	renderImage->SetPosition(D3D::Width()*0.5f, D3D::Height()*0.5f, 0);
	renderImage->SetScale(D3D::Width(), D3D::Height(), 1);
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

Matrix Camera::ViewMatrix()
{
	return matView;
}

Matrix Camera::ProjectionMatrix()
{
	return perspective->GetMatrix();
}

void Camera::Start()
{
}

void Camera::PostUpdate()
{
}

void Camera::LateUpdate()
{
}

void Camera::Render(Camera* viewer)
{
	D3D::Get()->SetRenderTarget();
	D3D::Get()->Clear(Color(0,0,1,1));

	viewport->RSSetViewport();

	auto srv = renderTarget->SRV();
	renderImage->SetSRV(srv);
	renderImage->Draw(this);
}

void Camera::PreRender(Camera* viewer)
{
	//D3D::Get()->SetRenderTarget();
	//D3D::Get()->Clear(D3D::GetDesc().Background);

	renderTarget->Set(depthStencil);
	viewport->RSSetViewport();
}

void Camera::PostRender(Camera* viewer)
{
}

void Camera::RemoveFromParent()
{
}
