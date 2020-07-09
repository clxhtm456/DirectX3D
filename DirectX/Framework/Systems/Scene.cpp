#include "Framework.h"
#include "Scene.h"


void Scene::AutoInitialize()
{
	D3DDesc desc = D3D::GetDesc();

	CameraOption option;
	option.Width = desc.Width;
	option.Height = desc.Height;
	//option.useGBuffer = true;

	auto cam = Camera::Create(option);
	cam->SetPosition(20, 37, -68);
	cam->SetRotationDegree(30, -20, 0);

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

	for (auto object : _lightList)
	{
		object->Release();
	}

	for (auto object : _reflectionList)
	{
		object->Release();
	}
}

void Scene::AutoUpdate()
{
	Update();

	for (auto citer = _cameraList.begin(); citer != _cameraList.end(); citer++)
	{
		Camera* camera = *citer;
		if (camera->GetRunning())
			camera->AutoUpdate();
	}

	for (auto niter = _childList.begin(); niter != _childList.end(); niter++)
	{
		Node* object = *niter;
		if (object->GetRunning())
			object->AutoUpdate();
	}

	for (auto liter = _lightList.begin(); liter != _lightList.end(); liter++)
	{
		Light* light = *liter;
		if (light->GetRunning())
			light->AutoUpdate();
	}

	for (auto riter = _reflectionList.begin(); riter != _reflectionList.end(); riter++)
	{
		ReflectionNode* reflect = *riter;
		if (reflect->GetRunning())
			reflect->AutoUpdate();
	}
}

void Scene::AutoPreRender()
{
	PreRender();

	for (auto citer = _cameraList.begin(); citer != _cameraList.end(); citer++)
	{
		Camera* camera = *citer;
		for (auto riter = _reflectionList.begin(); riter != _reflectionList.end(); riter++)
		{
			ReflectionNode* reflect = *riter;
			reflect->SetUpRender();
			for (auto niter = _childList.begin(); niter != _childList.end(); niter++)
			{
				Node* object = *niter;
				RenderingNode* renderObject = dynamic_cast<RenderingNode*>(object);

				if (renderObject == NULL)
					continue;
				reflect->SetRNShader2Depth(renderObject);
				if (camera->CheckMask(object->GetObjectMask()))
					object->AutoRender(camera);

				reflect->SetRNShader2Origin(renderObject);
			}
			reflect->AutoRender(camera);
		}
	}
}

void Scene::AutoRender()
{
	Render();

	for (auto citer = _cameraList.begin(); citer != _cameraList.end(); citer++)
	{
		Camera* camera = *citer;
		camera->SetUpRender();
		for (auto niter = _childList.begin(); niter != _childList.end(); niter++)
		{
			Node* object = *niter;
			if (camera->CheckMask(object->GetObjectMask()))
				object->AutoRender(camera);
		}
		camera->AutoRender(camera);
	}
}

void Scene::AutoPostRender()
{
	PostRender();

	for (auto citer = _cameraList.begin(); citer != _cameraList.end(); citer++)
	{
		Camera* camera = *citer;
		camera->AutoPostRender(camera);

		for (auto niter = _childList.begin(); niter != _childList.end(); niter++)
		{
			Node* object = *niter;
			if (camera->CheckMask(object->GetObjectMask()))
				object->AutoPostRender(camera);
		}

		for (auto riter = _reflectionList.begin(); riter != _reflectionList.end(); riter++)
		{
			ReflectionNode* reflect = *riter;
			if (reflect->GetRunning())
				reflect->AutoPostRender(camera);
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
	case TYPE_REFLECTION :
	{
		for (auto _child : _reflectionList)
		{
			if (_child == node)
				return;
		}
		_reflectionList.push_back((ReflectionNode*)node);
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
	case TYPE_REFLECTION:
	{
		int index = 0;
		auto iter = std::find(_reflectionList.begin(), _reflectionList.end(), child);
		if (iter != _reflectionList.end())
		{
			auto it = std::next(_reflectionList.begin(), index);
			(*it)->Release();
			_reflectionList.erase(it);
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
	mainLight->UseShadow(true);
	AddChild(mainLight);
	//mainLight->Retain();

	_directionLight = mainLight;
}

DirectionLight* Scene::GetDirectionLight()
{
	return _directionLight;
}

Scene::Scene()
{
}

Scene::~Scene()
{
}
