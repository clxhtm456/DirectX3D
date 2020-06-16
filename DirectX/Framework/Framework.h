#pragma once

#include <Windows.h>
#include <assert.h>

//STL
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <functional>
#include <iterator>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <wrl.h>
#include <shellapi.h>
using namespace std;

#define Check(hr) { assert(SUCCEEDED(hr)); }
#define Super __super

#define SafeRelease(p) { if (p) { (p)->Release(); (p) = NULL; } }
#define SafeDelete(p){ if(p){ delete (p); (p) = NULL; } }
#define SafeDeleteArray(p){ if(p){ delete [] (p); (p) = NULL; } }

//Direct3D
#include <d3d11.h>
#include <DirectXPackedVector.h>
#include <dxgi1_4.h>

#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>


#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d11.lib")

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

typedef XMFLOAT2 Vector2;
typedef XMFLOAT3 Vector3;
typedef XMFLOAT4 Vector4;
typedef XMCOLOR Color;
typedef XMMATRIX Matrix;
typedef XMVECTOR Quaternion;
typedef XMVECTOR Plane;

#define DECVEC3(__result__,__vec1__,__vec2__)\
{\
	XMStoreFloat3(&__result__, XMLoadFloat3(&__vec1__) - XMLoadFloat3(&__vec2__));\
}


//ImGui
#include <ImGui_New/imgui.h>
#include <ImGui_New/imgui_impl_dx11.h>
#include <ImGui_New/imgui_impl_win32.h>
#pragma comment(lib, "ImGui_New/imgui.lib")

//D3D
#include "Systems/D3D.h"
#include "Systems/Keyboard.h"
#include "Systems/Mouse.h"
#include "Systems/Time.h"
#include "Systems/Gui.h"
#include "Systems/Context.h"
#include "Systems/Window.h"

#include "Utilities/Math.h"

#include "Objects/Node.h"