#include "Framework.h"
#include "Camera.h"
#include "Perspective.h"
#include "Viewport.h"

Camera::Camera(CameraOption option) :
	Node()
{
	matView = XMMatrixIdentity();
	matRotation = XMMatrixIdentity();

	perspective = new Perspective(option.Width, option.Height,option.zn,option.zf,option.fov);
	viewport = new Viewport(option.Width, option.Height,option.x,option.y,option.minDepth,option.maxDepth);

	Rotate();
	Move();
}

Camera::~Camera()
{
	delete perspective;
	delete viewport;
}

void Camera::Update()
{
}


void Camera::Position(float x, float y, float z)
{
	Position(Vector3(x, y, z));
}

void Camera::Position(Vector3 & vec)
{
	position = vec;

	Move();
}

void Camera::Position(Vector3 * vec)
{
	*vec = position;
}

void Camera::Rotation(float x, float y, float z)
{
	Rotation(Vector3(x, y, z));
}

void Camera::Rotation(Vector3 & vec)
{
	rotation = vec;

	Rotation();
}

void Camera::Rotation(Vector3 * vec)
{
	*vec = rotation;
}

void Camera::RotationDegree(float x, float y, float z)
{
	RotationDegree(Vector3(x, y, z));
}

void Camera::RotationDegree(Vector3 & vec)
{
	rotation.x = vec.x * 0.01745328f; 
	rotation.y = vec.y * 0.01745328f;
	rotation.z = vec.z * 0.01745328f;// vec * Math::PI / 180.0f

	Rotation();
}

void Camera::RotationDegree(Vector3 * vec)
{
	vec->x = rotation.x * 57.295791f;
	vec->y = rotation.y * 57.295791f;
	vec->z = rotation.z * 57.295791f;
}

void Camera::GetMatrix(Matrix * matrix)
{
	memcpy(matrix, &matView, sizeof(Matrix));
}

void Camera::Move()
{
	View();
}

void Camera::Rotation()
{
	Matrix X, Y, Z;
	X = XMMatrixRotationX(rotation.x);
	Y = XMMatrixRotationX(rotation.y);
	Z = XMMatrixRotationX(rotation.z);

	matRotation = X * Y * Z;

	//저장될 값 <- ..의 방향 <- ..공간
	forward = XMVector3TransformNormal(XMVectorSet(0,0,1,0), matRotation);
	up = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), matRotation);
	right = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), matRotation);

	View();
	
}

void Camera::View()
{
	matView = XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&position) + (forward), up);
}

Matrix Camera::ViewMatrix()
{
	return Matrix();
}

Matrix Camera::ProjectionMatrix()
{
	return Matrix();
}
