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
	for (IExecute* exe : executes)
	{
		exe->AutoDestroy();
		SafeDelete(exe);
	}
}

void Main::Update()
{
	for (IExecute* exe : executes)
		exe->AutoUpdate();
}

void Main::PreRender()
{
	for (IExecute* exe : executes)
		exe->AutoPreRender();
}

void Main::Render()
{
	for (IExecute* exe : executes)
		exe->AutoRender();
}

void Main::PostRender()
{
	for (IExecute* exe : executes)
		exe->AutoPostRender();
}

void Main::ResizeScreen()
{
	for (IExecute* exe : executes)
		exe->ResizeScreen();
}

void Main::Push(IExecute * execute)
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

	//_CrtSetBreakAlloc(258);
	Main* main = new Main();
	WPARAM wParam = Window::Run(main);


	SafeDelete(main);

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	return wParam;
}