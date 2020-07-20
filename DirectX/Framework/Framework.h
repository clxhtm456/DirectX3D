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

#define MAX_MODEL_BONE 256
#define MAX_POINTLIGHT 32


#define ForIterator(__TYPE__,__ARRAY__,__NAME__,__ITER__)	for (std::list<__TYPE__*>::iterator __ITER__ = __ARRAY__.begin(), (__TYPE__*) __NAME__ = (*__ITER__); __ITER__ != __ARRAY__.end(); __ITER__++)

//Direct3D
#include <d3d11.h>
#include <DirectXPackedVector.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <DirectXCollision.h>

#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

//fxShader
//#include <d3dx11effect.h>
//#pragma comment(lib, "Effects11d.lib")




using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

typedef XMFLOAT2 Vector2;
typedef XMFLOAT3 Vector3;
typedef XMFLOAT4 Vector4;
typedef XMFLOAT4 Color;
typedef XMMATRIX Matrix;
typedef XMFLOAT4 Quaternion;
typedef XMVECTOR Plane;

#define LERP(s, e, t) (s + (e - s)*t)

#define TYPE_NODE	(1<<1)
#define TYPE_VIEWER	(1<<2)
#define TYPE_LIGHT	(1<<3)
#define TYPE_REFLECTION	(1<<4)

#define TEXTURE_DIR(texture)	"../../_Textures/" + texture

#define CAMERA_MASK1	(1<<1)
#define CAMERA_MASK2	(1<<2)
#define CAMERA_MASK3	(1<<3)
#define CAMERA_MASK4	(1<<4)
#define CAMERA_MASK5	(1<<5)
#define CAMERA_MASK6	(1<<6)
#define CAMERA_MASK7	(1<<7)
#define CAMERA_MASK8	(1<<8)
#define CAMERA_MASK9	(1<<9)
#define CAMERA_MASK10	(1<<10)
#define CAMERA_MASK11	(1<<11)
#define CAMERA_MASK12	(1<<12)

typedef enum _TYPEMASK
{
	DEFAULT = CAMERA_MASK1,
	ALL = CAMERA_MASK1 | CAMERA_MASK2 | CAMERA_MASK3 | CAMERA_MASK4 | CAMERA_MASK5 | CAMERA_MASK6 | CAMERA_MASK7 | CAMERA_MASK8 | CAMERA_MASK9 | CAMERA_MASK10 | CAMERA_MASK11 | CAMERA_MASK12
}TYPEMASK;

//ImGui
#include <ImGui_New/imgui.h>
#include <ImGui_New/imgui_impl_dx11.h>
#include <ImGui_New/imgui_impl_win32.h>
#pragma comment(lib, "ImGui_New/imgui.lib")

#include "Observer/IRenderObserver.h"

//D3D
#include "Systems/D3D.h"
#include "Systems/Keyboard.h"
#include "Systems/Mouse.h"
#include "Systems/Time.h"
#include "Systems/Gui.h"
#include "Systems/Context.h"
#include "Systems/Window.h"
#include "Systems/AlignAllocationPolicy.h"

//
#include "Renders/ShaderResourceDefine.h"

//Framework

#include "Renders/Shader.h"
#include "Renders/VertexLayouts.h"
#include "Renders/Buffers.h"
#include "Renders/GlobalBuffer.h"
#include "Renders/DebugLine.h"
#include "Renders/Texture.h"
#include "Renders/Material.h"
#include "Renders/ShaderState/RasterizerState.h"
#include "Renders/ShaderState/DepthStencilState.h"
#include "Renders/ShaderState/BlendState.h"
#include "Renders/ShaderState/SamplerState.h"
#include "Renders/CommonStates.h"


#include "Utilities/Math.h"
#include "Utilities/Path.h"
#include "Utilities/String.h"
#include "Utilities/Xml.h"
#include "Utilities/BinaryFile.h"

#include "Objects/Node.h"
#include "Objects/RenderingNode.h"
#include "Objects/ReflectionNode.h"

#include "Light/Light.h"
#include "Light/DirectLight.h"

#include "Viewer/Camera.h"
#include "Viewer/Perspective.h"
#include "Viewer/Viewport.h"
#include "Viewer/DepthStencil.h"
#include "Viewer/RenderTarget.h"

#include "Meshes/MeshCube.h"
#include "Meshes/MeshCylinder.h"
#include "Meshes/MeshGrid.h"
#include "Meshes/MeshQuad.h"
#include "Meshes/MeshSphere.h"
#include "Terrain/Terrain.h"

#include "Model/Model.h"

#include "Systems/Scene.h"