#include "Framework.h"
#include "Model.h"

#include "Objects/EmptyNode.h"

Model* Model::Create(string path)
{
	auto pRet = new Model();
	if (pRet && pRet->Init(path))
	{
		pRet->AutoRelease();
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}
	return pRet;
}

bool Model::Init(string path)
{
	if (!Super::Init())
		return false;

	assert(Path::ExistFile(path + ".mesh") && Path::ExistFile(path + ".material"));
	LoadModel(path);
	SetVSShader(Shader::VSAdd(L"InstModelVS"));
	SetPSShader(Shader::PSAdd(L"InstModelPS"));

	InitInstanceObject();

	return true;
}

Model::Model()
{
}

Model::~Model()
{
	/*for (auto iter = hierarchyNodes.begin(); iter != hierarchyNodes.end(); iter++)
	{
		hierarchyNodes.erase(iter);
		delete *iter;
	}*/

	/*for (auto iter = hierarchyMap.begin(); iter != hierarchyMap.end(); iter++)
	{
		hierarchyMap.erase(iter);
		delete *iter;
	}*/

	for (auto mat : materials)
		delete mat;

	for (auto mesh : meshs)
	{
		delete mesh.indexBuffer;
		delete mesh.vertexBuffer;
		mesh.meshBuffer->Release();
	}

	for (auto bone : bones)
	{
		delete bone;
	}

	materialMap.clear();
	
	/*for (auto& i : keyFrames)
	{
		for (UINT k = 0; k < i.keyframes.size(); k++)
			vector<XMFLOAT3X4>().swap(i.keyframes[k]);
		vector< vector<XMFLOAT3X4>>().swap(i.keyframes);
	}
	vector<KEYFRAME>().swap(keyFrames);*/
	//vector<Matrix>().swap(hierarchyMatrix);
	delete instancingBuffer;

	vector<Node*> releaseList;
	for (auto iter = instanceMatrixList.begin(); iter != instanceMatrixList.end(); iter++)
	{
		releaseList.push_back((*iter).first);
	}
	//메모리 꼬임방지용
	for (auto object : releaseList)
	{
		object->Release();
	}
}

Node* Model::CreateInstance()
{
	Node* object = EmptyNode::Create();

	IncreaseInstancing(object);

	return object;
}


void Model::InitInstanceObject()
{
	instancingBuffer = new VertexBuffer(worlds, MAX_MESH_INSTANCE, sizeof(Matrix), 1, true);

	instanceMatrixList.insert(std::pair<Node*, Matrix>(this, this->GetWorld()));

	instancingCount = 1;


	OnChangePosition = [&](Matrix matrix)->void
	{
		for (UINT i = 0; i < MAX_MESH_INSTANCE; i++)
			worlds[i] = XMMatrixIdentity();

		instanceMatrixList.at(this) = matrix;

		int i = 0;
		for (auto iter = instanceMatrixList.begin(); iter != instanceMatrixList.end(); iter++, i++)
		{
			memcpy(&worlds[i], &(*iter).second, sizeof(Matrix));
			worlds[i] = XMMatrixTranspose(worlds[i]);
		}

		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(instancingBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		{
			memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MESH_INSTANCE);
		}
		D3D::GetDC()->Unmap(instancingBuffer->Buffer(), 0);

	};
}

void Model::IncreaseInstancing(Node* object)
{
	instanceMatrixList.insert(std::pair<Node*, Matrix>(object, object->GetWorld()));

	instancingCount++;

	object->OnDestroy = std::bind(&Model::DecreaseInstancing, this, std::placeholders::_1);
	object->OnChangePosition = [&, object](Matrix matrix)->void
	{
		for (UINT i = 0; i < MAX_MESH_INSTANCE; i++)
			worlds[i] = XMMatrixIdentity();

		instanceMatrixList.at(object) = matrix;

		int i = 0;
		for (auto iter = instanceMatrixList.begin(); iter != instanceMatrixList.end(); iter++, i++)
		{
			memcpy(&worlds[i], &(*iter).second, sizeof(Matrix));
			worlds[i] = XMMatrixTranspose(worlds[i]);
		}

		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(instancingBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		{
			memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MESH_INSTANCE);
		}
		D3D::GetDC()->Unmap(instancingBuffer->Buffer(), 0);

	};
}

void Model::DecreaseInstancing(Node* object)
{
	assert(instancingCount > 0);

	instanceMatrixList.erase(object);
	instancingCount--;
}



void Model::LoadAnimation(const string path, float blendLoop, bool isLoop)
{
	assert(Path::ExistFile(path + ".anim"));
	BinaryReader* r = new BinaryReader();
	r->Open(String::ToWString(path + ".anim"));

	KEYFRAME clip;

	clip.tickPerSec = r->Float();
	clip.duration = r->Float();
	clip.blendLoop = blendLoop > clip.duration ? clip.duration : blendLoop;
	clip.isLoop = isLoop;
	
	UINT keyCount = r->UInt();
	for (UINT i = 0; i < keyCount; i++)
	{
		UINT count = r->UInt();
		vector<XMFLOAT3X4> Key;
		for (UINT j = 0; j < count; j++)
		{
			XMFLOAT3X4 data = { 0,0,0,0,0,0,0,1,1,1,1,1 };
			{
				void* ptr = (void*)&data.m[0];
				r->BYTE(&ptr, sizeof(XMFLOAT3));
			}
			data._14 = r->Float();
			{
				void* ptr = (void*)&data.m[1];
				r->BYTE(&ptr, sizeof(XMFLOAT4));
			}
			{
				void* ptr = (void*)&data.m[2];
				r->BYTE(&ptr, sizeof(XMFLOAT3));
			}
			Key.emplace_back(data);
		}
		clip.keyframes.emplace_back(Key);
	}
	clips.emplace_back(clip);

	delete r;

	//CreateAnimSRV();
}


void Model::Update()
{
	Super::Update();

	AnimUpdate();
}

void Model::ResourceBinding(Camera* viewer)
{
	Super::ResourceBinding(viewer);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::GetDC()->PSSetSamplers(3, 1, CommonStates::Get()->AnisotropicWrap());

	GetVSShader()->BindingVS();
	GetPSShader()->BindingPS();
	instancingBuffer->Binding();

	//D3D::GetDC()->VSSetShaderResources(1, 1, &hierarchySRV);
}

void Model::Render(Camera* viewer)
{
	
	for (int i = 0; i < 2; i++)
	{
		for (Mesh mesh : meshs)
		{
			if ((!materials[mesh.matrialID]->GetOpaque() && i == 0) || (materials[mesh.matrialID]->GetOpaque() && i == 1))
				continue;

			D3D::GetDC()->VSSetConstantBuffers(3, 1, &mesh.meshBuffer);

			materials[mesh.matrialID]->Binding();

			mesh.vertexBuffer->Binding();
			mesh.indexBuffer->Binding();
			
			D3D::GetDC()->DrawIndexedInstanced(mesh.indexCount, instancingCount, 0, 0, 0);
		}
	}
}


void Model::LoadModel(const string path)
{
	BinaryReader* r = new BinaryReader();
	r->Open(String::ToWString(path + ".mesh"));

	nodeCount = r->UInt();

	struct BoneData
	{
		XMFLOAT4 preQuat = { 0,0,0,1 };
		Matrix local = XMMatrixIdentity();
		Matrix world = XMMatrixIdentity();
		Matrix offset = XMMatrixIdentity();

		int parentID=-1;
		UINT enable = 0;
	};

	vector<BoneData> boneDatas(nodeCount);//버퍼용 초기셋팅 1번만
	for (UINT i = 0; i < nodeCount; i++)
	{
		BoneNode* node = new BoneNode();

		string name = r->String();
		{
			void* ptr = (void*)&node->translate;
			r->BYTE(&ptr, sizeof(XMFLOAT3));
		}
		{
			void* ptr = (void*)&node->preQuaternion;
			r->BYTE(&ptr, sizeof(XMFLOAT4));
		}
		{
			void* ptr = (void*)&node->quaternion;
			r->BYTE(&ptr, sizeof(XMFLOAT4));
		}
		{
			void* ptr = (void*)&node->scale;
			r->BYTE(&ptr, sizeof(XMFLOAT3));
		}
		{
			void* ptr = (void*)&node->local;
			r->BYTE(&ptr, sizeof(XMMATRIX));
		}
		{
			void* ptr = (void*)&node->world;
			r->BYTE(&ptr, sizeof(XMMATRIX));
		}
		{
			void* ptr = (void*)&node->offset;
			r->BYTE(&ptr, sizeof(XMMATRIX));
		}
		node->parentID = r->Int();

		/*hierarchyNodes.emplace_back(node);
		hierarchyMap.emplace(name, i);*/

		bones.push_back(node);
		/*boneDatas[i].preQuat= node.preQuaternion;
		boneDatas[i].local = XMMatrixTranspose(node.local);
		boneDatas[i].world =  XMMatrixTranspose(node.world);
		boneDatas[i].offset = XMMatrixTranspose(node.offset);
		boneDatas[i].parentID = node.parentID;
		boneDatas[i].enable = 1;*/
	}

	UINT count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		Mesh mesh;

		struct DATA
		{
			UINT id;
			float padding[3];
		}data;
		data.id = r->UInt();
		//하이어라키버퍼에서의 메쉬위치
		D3D11_BUFFER_DESC meshBufferDesc = {};
		meshBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		meshBufferDesc.ByteWidth = sizeof(DATA);
		meshBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		meshBufferDesc.CPUAccessFlags = 0;
		meshBufferDesc.MiscFlags = 0;
		meshBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA meshData;
		meshData.pSysMem = &data;
		meshData.SysMemPitch = 0;
		meshData.SysMemSlicePitch = 0;

		D3D::GetDevice()->CreateBuffer(&meshBufferDesc, &meshData, &mesh.meshBuffer);
		mesh.matrialID = r->UInt();

		UINT vertexCount = r->UInt();

		VertexType* vertices = new VertexType[vertexCount];
		{
			void* ptr = (void*)&vertices[0];
			r->BYTE(&ptr, sizeof(VertexType)* vertexCount);
		}

		mesh.vertexBuffer = new VertexBuffer(vertices, vertexCount,sizeof(VertexType));

		mesh.indexCount = r->UInt();
		indexCount = mesh.indexCount;

		UINT* indices = new UINT[mesh.indexCount];
		{
			void* ptr = (void*)&indices[0];
			r->BYTE(&ptr, sizeof(UINT)* mesh.indexCount);
		}

		mesh.indexBuffer = new IndexBuffer(indices, mesh.indexCount);

		meshs.emplace_back(mesh);

		delete[] vertices;
		delete[] indices;
	}

	delete r;

	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLError error = document->LoadFile((path + ".material").c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();
	Xml::XMLElement* materialNode = root->FirstChildElement();

	int index = 0;
	do
	{
		Material* mat = new Material();

		Xml::XMLElement* node = NULL;

		node = materialNode->FirstChildElement();

		string name = node->GetText();
		materialMap.emplace(name, index);

		//Diffuse
		node = node->NextSiblingElement();
		mat->SetDiffuse(
			node->FloatAttribute("R"),
			node->FloatAttribute("G"),
			node->FloatAttribute("B"),
			1.0f
		);

		//Ambient
		node = node->NextSiblingElement();
		mat->SetAmbient(
			node->FloatAttribute("R"),
			node->FloatAttribute("G"),
			node->FloatAttribute("B"),
			1.0f
		);

		//Specular
		node = node->NextSiblingElement();
		mat->SetSpecular(
			node->FloatAttribute("R"),
			node->FloatAttribute("G"),
			node->FloatAttribute("B"),
			1.0f
		);

		//Emissive
		node = node->NextSiblingElement();
		mat->SetEmissive(
			node->FloatAttribute("R"),
			node->FloatAttribute("G"),
			node->FloatAttribute("B"),
			1.0f
		);

		//Transparent
		node = node->NextSiblingElement();
		mat->SetTransparent(
			node->FloatAttribute("R"),
			node->FloatAttribute("G"),
			node->FloatAttribute("B"),
			1.0f
		);

		//Reflective
		node = node->NextSiblingElement();
		mat->SetReflective(
			node->FloatAttribute("R"),
			node->FloatAttribute("G"),
			node->FloatAttribute("B"),
			1.0f
		);

		//Opacity
		node = node->NextSiblingElement();
		mat->SetOpacity(node->FloatText());

		//Transparentfactor
		node = node->NextSiblingElement();
		mat->SetTransparentFactor(node->FloatText());

		//Bumpscaling
		node = node->NextSiblingElement();
		mat->SetBumpscaling(node->FloatText());

		//Shininess
		node = node->NextSiblingElement();
		mat->SetShininess(node->FloatText());

		//Reflectivity
		node = node->NextSiblingElement();
		mat->SetReflectivity(node->FloatText());

		//Shininessstrength
		node = node->NextSiblingElement();
		mat->SetShininessStrength(node->FloatText());

		//Refracti
		node = node->NextSiblingElement();
		mat->SetRefracti(node->FloatText());

		//DiffuseSRV
		node = node->NextSiblingElement();
		mat->SetDiffuseMap(node->GetText());

		//SpecularSRV
		node = node->NextSiblingElement();
		mat->SetSpecularMap(node->GetText());

		//AmbientSRV
		node = node->NextSiblingElement();
		mat->SetAmbientMap(node->GetText());

		//EmissiveSRV
		node = node->NextSiblingElement();
		mat->SetEmissiveMap(node->GetText());

		//HeightSRV
		node = node->NextSiblingElement();
		mat->SetHeightMap(node->GetText());

		//NormalSRV
		node = node->NextSiblingElement();
		mat->SetNormalMap(node->GetText());

		//ShininessSRV
		node = node->NextSiblingElement();
		mat->SetShinnessMap(node->GetText());

		//OpacitySRV
		node = node->NextSiblingElement();
		mat->SetOpacityMap(node->GetText());

		//DisplacementSRV
		node = node->NextSiblingElement();
		mat->SetDisplacementMap(node->GetText());

		//LightMapSRV
		node = node->NextSiblingElement();
		mat->SetLightMap(node->GetText());

		//ReflectionSRV
		node = node->NextSiblingElement();
		mat->SetReflectionMap(node->GetText());

		//BaseColorSRV
		node = node->NextSiblingElement();
		mat->SetBaseColorMap(node->GetText());

		//NormalCameraSRV
		node = node->NextSiblingElement();
		mat->SetNormalCameraMap(node->GetText());

		//EmissionColorSRV
		node = node->NextSiblingElement();
		mat->SetEmissionColorMap(node->GetText());

		//MatalnessSRV
		node = node->NextSiblingElement();
		mat->SetMetalnessMap(node->GetText());

		//DiffuseroughtnessSRV
		node = node->NextSiblingElement();
		mat->SetDiffuseroughnessMap(node->GetText());

		//AmbientocculsionSRV
		node = node->NextSiblingElement();
		mat->SetAmbientocculsionMap(node->GetText());

		materials.emplace_back(mat);

		materialNode = materialNode->NextSiblingElement();
		index++;
	} while (materialNode != NULL);

	delete document;
	
}

void Model::AnimUpdate()
{
	if (clips.empty())
		return;

	/*vector<ModelBone*>* bones = (modelData->GetBones());

	for (UINT i = 0; i < bones->size(); i++)
	{
		ModelBone* bone = bones->at(i);
		tweener->UpdateBlending(bone);
	}

	CalcWorldMatrix();*/
}

