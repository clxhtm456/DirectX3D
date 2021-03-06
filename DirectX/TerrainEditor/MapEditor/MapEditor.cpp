#include "stdafx.h"
#include "MapEditor.h"

#define DEFAULT_HORIZONTAL	512
#define DEFAULT_VERTICAL	512
#define DEFAULT_DISTANCE	DEFAULT_VERTICAL*540/512

MapEditor::MapEditor(Scene* scene):
	m_scene(scene)
{
	terrain = NULL;
}

MapEditor::~MapEditor()
{
}

void MapEditor::Update()
{

	ImGui::Separator();
	if (ImGui::Button("LoadMapFile"))
	{
		LoadMapDialog();
	}
	ImGui::SameLine();
	if (terrain == NULL)
	{
		if (ImGui::Button("CreateNewMap"))
		{
			CreateNewMap();
		}
	}
	else
	{
		if (ImGui::Button("SaveMapFile"))
		{
			SaveMapDialog();
		}
	}

	if (terrain != NULL)
	{
		auto posX = mapWidth * 0.5f;
		auto posZ = mapHeight * 0.5f;
		XMVECTOR terrainPos = DirectX::XMVectorSet(posX, 0.0f, posZ,0.0f);
		XMVECTOR camPos = XMLoadFloat3( &m_scene->GetMainCamera()->GetPosition());
		XMVECTOR length = DirectX::XMVectorSubtract(terrainPos, camPos);
		float distance;
		XMStoreFloat(&distance, DirectX::XMVector3Length(length));

		ImGui::LabelText("CameraDistance", "%f", distance);
	}
	

}

void MapEditor::Render()
{
}

void MapEditor::LoadMapDialog()
{
	std::function<void(wstring)> function = std::bind(&MapEditor::LoadMapFile, this, std::placeholders::_1);
	Path::OpenFileDialog(L"", L"", L"../../_Assets/", function, D3D::GetHandle());
}

void MapEditor::LoadMapFile(wstring fileDir)
{
	/*BinaryReader* r = new BinaryReader();
	r->Open(fileDir);

	UINT horizontal = r->UInt();
	UINT vertical = r->UInt();

	terrain = Terrain::Create(horizontal, vertical);

	for (UINT x = 0; x < horizontal; x++)
	{
		for (UINT z = 0; z < vertical; z++)
		{
			UINT height = r->UInt();
			terrain->SetHeight(x, z, height);
		}
	}

	for (UINT i = 0; i < count; i++)
	{
		ModelBone* bone = new ModelBone();

		bone->index = r->Int();
		bone->name = r->String();
		bone->parentIndex = r->Int();

		bone->local = r->Matrix();
		bone->global = r->Matrix();

		bones.push_back(bone);
	}

	count = r->UInt();

	for (UINT i = 0; i < count; i++)
	{
		ModelMesh* mesh = new ModelMesh();
		mesh->name = r->String();
		mesh->parentBoneIndex = r->Int();

		UINT partCount = r->UInt();
		for (UINT j = 0; j < partCount; j++)
		{
			ModelMeshPart* meshPart = new ModelMeshPart();
			meshPart->parent = mesh;
			meshPart->materialName = r->String();

			{
				UINT count = r->UInt();
				meshPart->vertices.resize(count);

				void* ptr = (void*)meshPart->vertices.data();
				r->BYTE(&ptr, sizeof(ModelVertexType) * count);
			}

			{
				UINT count = r->UInt();
				meshPart->indices.resize(count);

				void* ptr = (void*)meshPart->indices.data();
				r->BYTE(&ptr, sizeof(UINT) * count);
			}

			mesh->meshParts.push_back(meshPart);
		}
		meshes.push_back(mesh);
	}

	delete r;*/
}

void MapEditor::CreateNewMapFile(wstring fileDir)
{
	if(Path::GetExtension(fileDir) != L"map")
		fileDir = fileDir + L".map";
	Path::CreateFolders(Path::GetDirectoryName(fileDir));

	BinaryWriter* w = new BinaryWriter();
	w->Open(fileDir);

	//Save MapFile//-----

	w->UInt(mapWidth);
	w->UInt(mapHeight);
	for (UINT x = 0; x < mapWidth; x++)
	{
		for (UINT z = 0; z < mapHeight; z++)
		{
			w->UInt(0);
		}
	}
	//--------------------

	
	w->Close();
	delete w;
}
void MapEditor::SaveMapDialog()
{
}

void MapEditor::SaveMapFile()
{
	assert(terrain != NULL);

	const std::function<void(wstring)>& function = std::bind(&MapEditor::CreateNewMapFile, this, std::placeholders::_1);
	Path::SaveFileDialog(L"Default.map", L"MapFile\0 * .map", L"../../_Assets/", function, D3D::GetHandle());
}

void MapEditor::CreateNewMap()
{
	terrain = Terrain::Create(DEFAULT_HORIZONTAL, DEFAULT_VERTICAL,16);
	//terrain->SetScale(1, 1, 1);
	terrain->BaseMap(L"Terrain/Ground01_D.tga");

	auto posX = DEFAULT_HORIZONTAL * 0.5f;
	auto posZ = (DEFAULT_VERTICAL + (DEFAULT_VERTICAL * sin(50))) * -1.0f;

	m_scene->AddChild(terrain);
	m_scene->GetMainCamera()->SetPosition(posX, DEFAULT_DISTANCE, posZ);
	m_scene->GetMainCamera()->SetRotationDegree(40, 0, 0);

	mapWidth = DEFAULT_HORIZONTAL;
	mapHeight = DEFAULT_VERTICAL;

	/*terrain = Terrain::Create(DEFAULT_HORIZONTAL, DEFAULT_VERTICAL);
	terrain->SetShader(L"TerrainEdit");

	auto posX = DEFAULT_HORIZONTAL * 0.5f;
	auto posY = DEFAULT_DISTANCE * sinf(40.0f);
	auto posZ = DEFAULT_DISTANCE * cosf(40.0f);

	m_scene->AddChild(terrain);
	m_scene->GetMainCamera()->SetPosition(posX, posY, posZ);*/
}
