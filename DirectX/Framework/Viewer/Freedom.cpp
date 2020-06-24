#include "Framework.h"
#include "Freedom.h"

Freedom * Freedom::Create(CameraOption option)
{
	auto pRet = new Freedom();
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

Freedom::Freedom() : Camera()
{
}

Freedom::~Freedom()
{
}

void Freedom::Update()
{
	Super::Update();
	if (Mouse::Get()->Press(1) == false) return;

	XMVECTOR f = Foward();
	XMVECTOR u = Up();
	XMVECTOR r = Right();

	//이동
	{
		XMVECTOR P;
		XMFLOAT3 tempP = GetPosition();
		P = DirectX::XMLoadFloat3(&tempP);

		if (Keyboard::Get()->Press('W'))
			P = P + f * move * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			P = P - f * move * Time::Delta();

		if (Keyboard::Get()->Press('D'))
			P = P + r * move * Time::Delta();
		else if (Keyboard::Get()->Press('A'))
			P = P - r * move * Time::Delta();

		if (Keyboard::Get()->Press('E'))
			P = P + u * move * Time::Delta();
		else if (Keyboard::Get()->Press('Q'))
			P = P - u * move * Time::Delta();

		DirectX::XMStoreFloat3(&tempP, P);
		SetPosition(tempP);
	}

	//회전
	{
		Vector3 R = GetRotation();

		Vector3 val =  Mouse::Get()->GetMoveValue();

		R.x += val.y * rotation * 0.001f;
		R.y += val.x * rotation *  0.001f;
		R.z = 0.0f;

		SetRotation(R);
	}
	
}

void Freedom::Speed(float move, float rotation)
{
	this->move = move;
	this->rotation = rotation;
}
