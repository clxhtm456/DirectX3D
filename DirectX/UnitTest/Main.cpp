#include "stdafx.h"
#include "Main.h"
#include "Systems/Window.h"

#include "TestScene.h"

void Main::Initialize()
{
	Push(new TestScene());
}

void Main::Ready()
{

}

void Main::Destroy()
{
	for (Scene* exe : executes)
	{
		exe->AutoDestroy();
		SafeDelete(exe);
	}
}

void Main::Update()
{
	for (Scene* exe : executes)
		exe->AutoUpdate();
}

void Main::PreRender()
{
	for (Scene* exe : executes)
		exe->AutoPreRender();
}

void Main::Render()
{
	for (Scene* exe : executes)
		exe->AutoRender();
}

void Main::PostRender()
{
	for (Scene* exe : executes)
		exe->AutoPostRender();
}

void Main::ResizeScreen()
{
	for (Scene* exe : executes)
		exe->ResizeScreen();
}

void Main::Push(Scene * execute)
{
	executes.push_back(execute);

	execute->AutoInitialize();
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
	D3DDesc desc;
	desc.AppName = L"D3D Game";
	desc.Instance = instance;
	desc.bFullScreen = false;
	desc.bVsync = false;
	desc.Handle = NULL;
	desc.Width = 1280;
	desc.Height = 720;
	desc.Background = Color(0.3f, 0.3f, 0.3f, 1.0f);	
	D3D::SetDesc(desc);

	//_CrtSetBreakAlloc(376);
	Main* main = new Main();
	WPARAM wParam = Window::Run(main);


	SafeDelete(main);

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	return wParam;
}