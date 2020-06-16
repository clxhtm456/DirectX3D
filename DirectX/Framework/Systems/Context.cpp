#include "Framework.h"
#include "Context.h"
#include "Viewer/Camera.h"

Context* Context::instance = NULL;

Context * Context::Get()
{	
	return instance;
}

void Context::Create()
{
	assert(instance == NULL);

	instance = new Context();
}

void Context::Delete()
{
	SafeDelete(instance);
}

Context::Context()
{
	D3DDesc desc = D3D::GetDesc();

	CameraOption option;
	option.Width = desc.Width;
	option.Height = desc.Height;

	camera = new Camera(option);
}

Context::~Context()
{
	SafeDelete(camera);
}

void Context::Update()
{
	camera->Update();
}

void Context::Render()
{
	camera->AutoRender();

	string str = string("FPS : ") + to_string(ImGui::GetIO().Framerate);
	Gui::Get()->RenderText(5, 5, 1, 1, 1, str);

	Vector3 camPos;
	camPos = camera->GetPosition();

	Vector3 camDir;
	camDir = camera->GetRotationDegree();

	str = "camera(P) : ";
	str += to_string((int)camPos.x) + ", " + to_string((int)camPos.y) + ", " + to_string((int)camPos.z);
	Gui::Get()->RenderText(5, 20, 1, 1, 1, str);

	str = "camera(R) : ";
	str += to_string((int)camDir.x) + ", " + to_string((int)camDir.y);
	Gui::Get()->RenderText(5, 35, 1, 1, 1, str);
}

void Context::ResizeScreen()
{
	camera->Resize();
}

//
//UINT Context::PointLights(OUT PointLight * lights)
//{
//	memcpy(lights, pointLights, sizeof(PointLight) * pointLightCount);
//
//	return pointLightCount;
//}
//
//void Context::AddPointLight(PointLight & light)
//{
//	pointLights[pointLightCount] = light;
//	pointLightCount++;
//}
//
//PointLight & Context::GetPointLight(UINT index)
//{
//	return pointLights[index];
//}
//
//UINT Context::SpotLights(OUT SpotLight * lights)
//{
//	memcpy(lights, spotLights, sizeof(SpotLight) * spotLightCount);
//
//	return spotLightCount;
//}
//
//void Context::AddSpotLight(SpotLight & light)
//{
//	spotLights[spotLightCount] = light;
//	spotLightCount++;
//}
//
//SpotLight & Context::GetSpotLight(UINT index)
//{
//	return spotLights[index];
//}
