#include "Framework.h"
#include "Scene.h"


void Scene::AutoInitialize()
{
	D3DDesc desc = D3D::GetDesc();

	CameraOption option;
	option.Width = desc.Width;
	option.Height = desc.Height;

	auto cam = Camera::Create(option);


	SetMainCamera(cam);
	CreateMainLight();

	

	Initialize();
}

void Scene::AutoDestroy()
{
	if (_directionLight != nullptr)
		_directionLight->Release();

	Destroy();

	for (Node* object : _cameraList)
	{
		object->Release();
	}

	for (auto object : _childList)
	{
		object->Release();
	}
}

void Scene::AutoUpdate()
{
	Update();

	for (Node* object : _cameraList)
	{
		if (object->GetRunning())
			object->AutoUpdate();
	}

	for (auto object : _childList)
	{
		if (object->GetRunning())
			object->AutoUpdate();
	}

	for (auto object : _lightList)
	{
		if (object->GetRunning())
			object->AutoUpdate();
	}
}

void Scene::AutoPreRender()
{
	PreRender();

	for (Camera* camera : _cameraList)
	{
		if (camera->GetRunning())
		{
			camera->AutoPreRender(camera);

			for (auto object : _childList)
			{
				if (object->GetRunning())
					object->AutoPreRender(camera);
			}
		}
	}
}

void Scene::AutoRender()
{
	Render();

	for (Camera* camera : _cameraList)
	{
		if (camera->GetRunning())
		{
			camera->SetUpRender();
			for (auto object : _childList)
			{
				if (object->GetRunning())
					object->AutoRender(camera);
			}

			camera->AutoRender(camera);
		}
	}
}

void Scene::AutoPostRender()
{
	PostRender();

	for (Camera* camera : _cameraList)
	{
		if (camera->GetRunning())
		{
			camera->AutoPostRender(camera);

			for (auto object : _childList)
			{
				if (object->GetRunning())
					object->AutoPostRender(camera);
			}
		}
	}
}

void Scene::AddChild(Node* node)
{
	node->_scene = this;
	node->Retain();

	switch (node->ChildType())
	{
	case TYPE_NODE:
	{
		for (auto _child : _childList)
		{
			if (_child == node)
				return;
		}
		_childList.push_back(node);
		break;
	}
	case TYPE_VIEWER:
	{
		for (auto _child : _cameraList)
		{
			if (_child == node)
				return;
		}
		_cameraList.push_back((Camera*)node);
		break;
	}
	case TYPE_LIGHT:
	{
		for (auto _child : _lightList)
		{
			if (_child == node)
				return;
		}
		_lightList.push_back((Light*)node);
		break;
	}
	}
}


void Scene::DelChild(Node* child)
{
	if (_childList.empty() && _cameraList.empty() && _lightList.empty())
	{
		return;
	}

	switch (child->ChildType())
	{
	case TYPE_NODE:
	{
		int index = 0;
		auto iter = std::find(_childList.begin(), _childList.end(), child);
		if (iter != _childList.end())
		{
			auto it = std::next(_childList.begin(), index);
			(*it)->Release();
			_childList.erase(it);
		}
	}
	case TYPE_VIEWER:
	{
		int index = 0;
		auto iter = std::find(_cameraList.begin(), _cameraList.end(), child);
		if (iter != _cameraList.end())
		{
			auto it = std::next(_cameraList.begin(), index);
			(*it)->Release();
			_cameraList.erase(it);
		}
	}
	case TYPE_LIGHT:
	{
		int index = 0;
		auto iter = std::find(_lightList.begin(), _lightList.end(), child);
		if (iter != _lightList.end())
		{
			auto it = std::next(_lightList.begin(), index);
			(*it)->Release();
			_lightList.erase(it);
		}
	}
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

void Scene::CreateMainLight()
{
	auto mainLight = DirectionLight::Create();
	AddChild(mainLight);
	//mainLight->Retain();

	_directionLight = mainLight;
}

DirectionLight* Scene::GetDirectionLight()
{
	return _directionLight;
}
