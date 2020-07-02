#include "Framework.h"
#include "Node.h"

Node::Node() : 
	_referenceCount(1),
	_position(XMVectorSet(0, 0,0,0)),
	_scale(XMVectorSet(1, 1,1,0)),
	_rotation(XMVectorSet(0, 0, 0,0)),
	_running(true),
	_reorderChildDirty(false),
	_visible(true),
	_scene(nullptr)
{
	_parent = nullptr;

	AddEvent([=]()->void
		{
			Start();
		});
}

Node::~Node()
{
	for (auto object : _childList)
		object->Release();
}


void Node::AutoPostUpdate()
{
	PostUpdate();

	for (auto object : _childList)
		object->AutoPostUpdate();
}

void Node::AutoUpdate()
{
	for (int i = 0; i < _eventList.size(); i++)
	{
		if (_eventList[i].second <= 0)
		{
			if (_eventList[i].first != nullptr)
				_eventList[i].first();
			_eventList.erase(_eventList.begin() + i);
		}
		else
		{
			_eventList[i].second -= Time::Delta();
		}
	}

	Update();

	for (auto object : _childList)
	{
		if (object->GetRunning())
			object->AutoUpdate();
	}
}

void Node::AutoLateUpdate()
{
	LateUpdate();

	for (auto object : _childList)
		object->AutoLateUpdate();
}

void Node::AutoRender(Camera* viewer)
{
	if (_visible == false)
		return;

	Render(viewer);

	Draw(viewer);

	auto list = _childList;
	for (auto object : list)
		object->AutoRender(viewer);
}

void Node::AutoPreRender(Camera* viewer)
{
	PreRender(viewer);
	auto list = _childList;
	for (auto object : list)
		object->AutoPreRender(viewer);
}

void Node::AutoPostRender(Camera* viewer)
{
	PostRender(viewer);
	auto list = _childList;
	for (auto object : list)
		object->AutoPostRender(viewer);
}

void Node::RemoveFromParent()
{
	if (_parent != nullptr)
	{
		_parent->DelChild(this);
	}
}


void Node::Draw(Camera* viewer)
{
}

Vector3 Node::GetPosition()
{
	Vector3 result;
	XMStoreFloat3(&result, _position);
	return result;
}

Vector3 Node::GetRotation()
{
	Vector3 result;
	XMStoreFloat3(&result, _rotation);
	return result;
}

Vector3 Node::GetRotationDegree()
{
	Vector3 result;
	XMStoreFloat3(&result, _rotation);
	result.x = Math::ToDegree(result.x);
	result.y = Math::ToDegree(result.y);
	result.z = Math::ToDegree(result.z);
	return result;
}

Vector3 Node::GetScale()
{
	Vector3 result;
	XMStoreFloat3(&result, _scale);
	return result;
}

void Node::SetPosition(Vector3 position)
{
	_position = XMLoadFloat3(&position);
}

void Node::SetRotation(Vector3 rotation)
{
	_rotation = XMLoadFloat3(&rotation);
}

void Node::SetRotationDegree(Vector3 rotation)
{
	rotation.x = Math::ToRadian(rotation.x);
	rotation.y = Math::ToRadian(rotation.y);
	rotation.z = Math::ToRadian(rotation.z);

	_rotation = XMLoadFloat3(&rotation);
}

void Node::SetScale(Vector3 scale)
{
	_scale = XMLoadFloat3(&scale);
}

void Node::SetPosition(float x, float y, float z)
{
	Vector3 temp = Vector3(x, y, z);
	SetPosition(temp);
}

void Node::SetRotation(float x, float y, float z)
{
	Vector3 temp = Vector3(x, y, z);
	SetRotation(temp);
}

void Node::SetRotationDegree(float x, float y, float z)
{
	Vector3 temp = Vector3(x, y, z);
	SetRotationDegree(temp);
}

void Node::SetScale(float x, float y, float z)
{
	Vector3 temp = Vector3(x, y, z);
	SetScale(temp);
}

void Node::AddChild(Node* child)
{
	for (auto _child : _childList)
	{
		if (_child == child)
			return;
	}
	_childList.push_back(child);
	_reorderChildDirty = true;

	child->_parent = this;
	child->_scene = _scene;
	child->Retain();
}

void Node::DelChild(Node* child)
{
	if (_childList.empty())
	{
		return;
	}

	int index;
	auto iter = std::find(_childList.begin(), _childList.end(), child);
	if (iter != _childList.end())
	{
		child->_parent = nullptr;

		auto it = std::next(_childList.begin(), index);
		(*it)->Release();
		_childList.erase(it);
	}
	_reorderChildDirty = true;
}

void Node::AddEvent(const std::function<void()>& func, float timer)
{
	auto event = pair<std::function<void()>, float>(func, timer);
	_eventList.push_back(event);
}

void Node::AutoRelease()
{
	Window::AddReleaseList(this);
}

void Node::Release()
{
	_referenceCount--;

	if (_referenceCount == 0)
	{
		delete this;
	}
}

void Node::Retain()
{
	_referenceCount++;
}
