#include "framework.h"
#include "IExecute.h"
#include "Viewer/Camera.h"

void IExecute::AutoInitialize()
{
	D3DDesc desc = D3D::GetDesc();

	CameraOption option;
	option.Width = desc.Width;
	option.Height = desc.Height;

	_mainCamera = new Camera(option);

	Context::Get()->SetMainCamera(_mainCamera);

	Initialize();

	for (auto object : _childList)
	{
		if (object->GetRunning())
			object->AutoInit();
	}
}

void IExecute::AutoDestroy()
{
	delete _mainCamera;

	Destroy();

	for (auto object : _childList)
	{
		object->Release();
	}
}

void IExecute::AutoUpdate()
{
	Update();

	for (auto object : _childList)
	{
		if (object->GetRunning())
			object->AutoUpdate();
	}
}

void IExecute::AutoPreRender()
{
	PreRender();

	for (auto object : _childList)
	{
		if (object->GetRunning())
			object->AutoPreRender();
	}
}

void IExecute::AutoRender()
{
	Render();

	for (auto object : _childList)
	{
		if (object->GetRunning())
			object->AutoRender();
	}
}

void IExecute::AutoPostRender()
{
	PostRender();

	for (auto object : _childList)
	{
		if (object->GetRunning())
			object->AutoPostRender();
	}
}

void IExecute::AddChild(Node* node)
{
	for (auto _child : _childList)
	{
		if (_child == node)
			return;
	}
	_childList.push_back(node);

	node->Retain();
}

void IExecute::DelChild(Node* child)
{
	if (_childList.empty())
	{
		return;
	}

	int index;
	auto iter = std::find(_childList.begin(), _childList.end(), child);
	if (iter != _childList.end())
	{
		auto it = std::next(_childList.begin(), index);
		(*it)->Release();
		_childList.erase(it);
	}
}