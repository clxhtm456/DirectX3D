#include "Framework.h"
#include "Scene.h"
#include "Viewer/Camera.h"

void Scene::AutoInitialize()
{
	D3DDesc desc = D3D::GetDesc();

	CameraOption option;
	option.Width = desc.Width;
	option.Height = desc.Height;

	auto cam = Camera::Create(option);


	SetMainCamera(cam);

	

	Initialize();
}

void Scene::AutoDestroy()
{

	Destroy();

	for (auto object : _childList)
	{
		object->Release();
	}
}

void Scene::AutoUpdate()
{
	Update();

	_mainCamera->AutoUpdate();

	for (auto object : _childList)
	{
		if (object->GetRunning())
			object->AutoUpdate();
	}
}

void Scene::AutoPreRender()
{
	PreRender();

	_mainCamera->AutoPreRender();
	for (auto object : _childList)
	{
		if (object->GetRunning())
			object->AutoPreRender();
	}
}

void Scene::AutoRender()
{
	Render();

	_mainCamera->AutoRender();

	for (auto object : _childList)
	{
		if (object->GetRunning())
			object->AutoRender();
	}
}

void Scene::AutoPostRender()
{
	PostRender();

	_mainCamera->AutoPostRender();
	for (auto object : _childList)
	{
		if (object->GetRunning())
			object->AutoPostRender();
	}
}

void Scene::AddChild(Node* node)
{
	for (auto _child : _childList)
	{
		if (_child == node)
			return;
	}
	_childList.push_back(node);

	node->Retain();
}

void Scene::DelChild(Node* child)
{
	if (_childList.empty())
	{
		return;
	}

	int index = 0;
	auto iter = std::find(_childList.begin(), _childList.end(), child);
	if (iter != _childList.end())
	{
		auto it = std::next(_childList.begin(), index);
		(*it)->Release();
		_childList.erase(it);
	}
}

void Scene::SetMainCamera(Camera* cam)
{
	if(_mainCamera != nullptr)
		DelChild(_mainCamera);

	Context::Get()->SetMainCamera(cam);
	AddChild(cam);

	_mainCamera = cam;
}
