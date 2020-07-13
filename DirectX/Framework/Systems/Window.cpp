#include "Framework.h"
#include "Window.h"
#include "Scene.h"
#include "Systems/IExecute.h"

IExecute* Window::mainExecute = NULL;
vector<Node*> Window::releaseList;


//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

WPARAM Window::Run(IExecute* main)
{
	mainExecute = main;
	Create();

	D3DDesc desc = D3D::GetDesc();

	D3D::Create();
	Keyboard::Create();
	Mouse::Create();

	Time::Create();
	Time::Get()->Start();

	Gui::Create();

	Context::Create();	
	DebugLine::Create();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; //
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     //도킹시스템(imgui)붙이기
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; //이설정이 멀티뷰 허용 권한(밖으로빼는 함수)
	io.ConfigWindowsResizeFromEdges = true;
	ImGui_ImplWin32_Init(D3D::GetHandle());
	ImGui_ImplDX11_Init(D3D::GetDevice(), D3D::GetDC());

	mainExecute->Initialize();


	MSG msg = { 0 };
	while (true)

	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			MainRender();
		}
	}
	mainExecute->Destroy();

	DebugLine::Delete();
	Shader::Delete();
	Texture::Delete();
	Context::Delete();
	Gui::Delete();
	Time::Delete();
	Mouse::Delete();
	Keyboard::Delete();
	D3D::Delete();

	Destroy();

	return msg.wParam;
}

void Window::Create()
{
	D3DDesc desc = D3D::GetDesc();


	WNDCLASSEX wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hIconSm = wndClass.hIcon;
	wndClass.hInstance = desc.Instance;
	wndClass.lpfnWndProc = (WNDPROC)WndProc;
	wndClass.lpszClassName = desc.AppName.c_str();
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbSize = sizeof(WNDCLASSEX);

	WORD wHr = RegisterClassEx(&wndClass);
	assert(wHr != 0);

	if (desc.bFullScreen == true)
	{
		DEVMODE devMode = { 0 };
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmPelsWidth = (DWORD)desc.Width;
		devMode.dmPelsHeight = (DWORD)desc.Height;
		devMode.dmBitsPerPel = 32;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
	}

	desc.Handle = CreateWindowEx
	(
		WS_EX_APPWINDOW
		, desc.AppName.c_str()
		, desc.AppName.c_str()
		, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, NULL
		, (HMENU)NULL
		, desc.Instance
		, NULL
	);
	assert(desc.Handle != NULL);
	D3D::SetDesc(desc);


	RECT rect = { 0, 0, (LONG)desc.Width, (LONG)desc.Height };

	UINT centerX = (GetSystemMetrics(SM_CXSCREEN) - (UINT)desc.Width) / 2;
	UINT centerY = (GetSystemMetrics(SM_CYSCREEN) - (UINT)desc.Height) / 2;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	MoveWindow
	(
		desc.Handle
		, centerX, centerY
		, rect.right - rect.left, rect.bottom - rect.top
		, TRUE
	);
	ShowWindow(desc.Handle, SW_SHOWNORMAL);
	SetForegroundWindow(desc.Handle);
	SetFocus(desc.Handle);

	ShowCursor(true);
}

void Window::Destroy()
{
	D3DDesc desc = D3D::GetDesc();

	if (desc.bFullScreen == true)
		ChangeDisplaySettings(NULL, 0);

	DestroyWindow(desc.Handle);

	UnregisterClass(desc.AppName.c_str(), desc.Instance);
}

LRESULT CALLBACK Window::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	Mouse::Get()->InputProc(message, wParam, lParam);

	if (Gui::Get()->MsgProc(handle, message, wParam, lParam))
		return true;

	if (message == WM_SIZE)
	{
		if (mainExecute != NULL)
		{
			float width = (float)LOWORD(lParam);
			float height = (float)HIWORD(lParam);

			if (D3D::Get() != NULL)
				D3D::Get()->ResizeScreen(width, height);

			if (Context::Get() != NULL)
				Context::Get()->ResizeScreen();

			mainExecute->ResizeScreen();
		}
	}

	if (message == WM_CLOSE || message == WM_DESTROY)
	{
		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(handle, message, wParam, lParam);
}

void Window::MainRender()
{
	Time::Get()->Update();

	if (ImGui::IsMouseHoveringAnyWindow() == false)
	{
		Keyboard::Get()->Update();
		Mouse::Get()->Update();
	}

	Gui::Get()->Update();
	Context::Get()->Update();

	mainExecute->Update();

	mainExecute->PreRender();
	
	D3D::Get()->SetRenderTarget();
	D3D::Get()->Clear(Color(0.2, 0.2, 0.2, 1));
	{
		Context::Get()->Render();
		mainExecute->Render();
		//DebugLine::Get()->Render();

		mainExecute->PostRender();
		Gui::Get()->Render();
	}

	ReleasePoolClear();
	D3D::Get()->Present();
}

void Window::AddReleaseList(Node* b)
{
	releaseList.push_back(b);
}

void Window::ReleasePoolClear()
{
	if (releaseList.size() == 0)
		return;

	std::vector<Node*> releasings;
	releasings.swap(releaseList);
	for (const auto& obj : releasings)
	{
		obj->Release();
	}
}
