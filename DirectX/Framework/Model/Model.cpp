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
	csShader = Shader::CSAdd(L"InstModelCS");

	instancingCount = 0;
	bInstancingMode = false;
	for (UINT i = 0; i < MAX_MESH_INSTANCE; i++)
		worlds[i] = XMMatrixIdentity();
	instancingBuffer = new VertexBuffer(worlds, MAX_MESH_INSTANCE, sizeof(Matrix), 1, true);
	instancingCount = 1;

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
		delete mesh.meshBuffer;
	}

	materialMap.clear();
	
	for (auto& i : keyFrames)
	{
		for (UINT k = 0; k < i.keyframes.size(); k++)
			vector<XMFLOAT3X4>().swap(i.keyframes[k]);
		vector< vector<XMFLOAT3X4>>().swap(i.keyframes);
	}
	vector<KEYFRAME>().swap(keyFrames);
	//vector<Matrix>().swap(hierarchyMatrix);
	delete instancingBuffer;

	vector<Node*> releaseList;
	for (auto iter = instanceMatrixList.begin(); iter != instanceMatrixList.end(); iter++)
	{
		releaseList.push_back((*iter).first);
	}
	//�޸� ���ӹ�����
	for (auto object : releaseList)
	{
		object->Release();
	}
}

Node* Model::CreateInstance()
{
	StartInstancingMode();

	Node* object = EmptyNode::Create();

	IncreaseInstancing(object);

	return object;
}

void Model::StartInstancingMode()
{
	if (bInstancingMode == false)
	{
		bInstancingMode = true;
		instancingCount = 0;
	}
	else
		return;
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

	KEYFRAME keyFrame;

	keyFrame.tickPerSec = r->Float();
	keyFrame.duration = r->Float();
	keyFrame.blendLoop = blendLoop > keyFrame.duration ? keyFrame.duration : blendLoop;
	keyFrame.isLoop = isLoop;
	
	UINT keyCount = r->UInt();
	for (UINT i = 0; i < keyCount; i++)
	{
		UINT count = r->UInt();
		assert(count == nodeCount);
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
				r->BYTE(&ptr, sizeof(XMFLOAT3));
			}
			{
				void* ptr = (void*)&data.m[2];
				r->BYTE(&ptr, sizeof(XMFLOAT3));
			}
			Key.emplace_back(data);
		}
		keyFrame.keyframes.emplace_back(Key);
	}
	keyFrames.emplace_back(keyFrame);

	delete r;

	CreateAnimSRV();
}

void Model::Update()
{
	Super::Update();

	//AnimUpdate();
}

void Model::ResourceBinding(Camera* viewer)
{
	Super::ResourceBinding(viewer);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::GetDC()->PSSetSamplers(3, 1, CommonStates::Get()->AnisotropicWrap());

	GetVSShader()->BindingVS();
	GetPSShader()->BindingPS();
	instancingBuffer->Binding();

	
}

void Model::Render(Camera* viewer)
{
	
	for (int i = 0; i < 2; i++)
	{
		for (Mesh mesh : meshs)
		{
			if ((!materials[mesh.matrialID]->GetOpaque() && i == 0) || (materials[mesh.matrialID]->GetOpaque() && i == 1))
				continue;

			mesh.meshBuffer->SetVSBuffer(3);

			materials[mesh.matrialID]->Binding();

			mesh.vertexBuffer->Binding();
			mesh.indexBuffer->Binding();
			
			D3D::GetDC()->DrawIndexedInstanced(mesh.indexCount, instancingCount, 0, 0, 0);
		}
	}
}

void Model::CalcWorldMatrix()
{
	if (bInstancingMode == true)
		return;

	worlds[0] = GetWorld();
	worlds[0] = XMMatrixTranspose(worlds[0]);

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instancingBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MESH_INSTANCE);
	}
	D3D::GetDC()->Unmap(instancingBuffer->Buffer(), 0);
}

void Model::LoadModel(const string path)
{
	BinaryReader* r = new BinaryReader();
	r->Open(String::ToWString(path + ".mesh"));

	nodeCount = r->UInt();

	struct HierarchyDATA
	{
		XMFLOAT4 preQuat = { 0,0,0,1 };
		Matrix local = XMMatrixIdentity();
		Matrix world = XMMatrixIdentity();
		Matrix offset = XMMatrixIdentity();

		int parentID=-1;
		UINT enable = 0;
	};

	vector<HierarchyDATA> hierarchyDatas(nodeCount);//���ۿ� �ʱ���� 1����
	for (UINT i = 0; i < nodeCount; i++)
	{
		HierarchyNode node;

		string name = r->String();
		{
			void* ptr = (void*)&node.translate;
			r->BYTE(&ptr, sizeof(XMFLOAT3));
		}
		{
			void* ptr = (void*)&node.preQuaternion;
			r->BYTE(&ptr, sizeof(XMFLOAT4));
		}
		{
			void* ptr = (void*)&node.quaternion;
			r->BYTE(&ptr, sizeof(XMFLOAT4));
		}
		{
			void* ptr = (void*)&node.scale;
			r->BYTE(&ptr, sizeof(XMFLOAT3));
		}
		{
			void* ptr = (void*)&node.local;
			r->BYTE(&ptr, sizeof(XMMATRIX));
		}
		{
			void* ptr = (void*)&node.world;
			r->BYTE(&ptr, sizeof(XMMATRIX));
		}
		{
			void* ptr = (void*)&node.offset;
			r->BYTE(&ptr, sizeof(XMMATRIX));
		}
		node.parentID = r->Int();

		hierarchyNodes.emplace_back(node);
		hierarchyMap.emplace(name, i);

		hierarchyDatas[i].preQuat= node.preQuaternion;
		hierarchyDatas[i].local = XMMatrixTranspose(node.local);
		hierarchyDatas[i].world =  XMMatrixTranspose(node.world);
		hierarchyDatas[i].offset = XMMatrixTranspose(node.offset);
		hierarchyDatas[i].parentID = node.parentID;
		hierarchyDatas[i].enable = 1;
	}

	ConstantBuffer* hierarchyBuffer = nullptr;
	hierarchyBuffer = new ConstantBuffer(hierarchyDatas.data(),sizeof(HierarchyDATA) * nodeCount);

	//���̾��Ű ������ ��� ��Ʈ���� ����
	for (UINT i = 0; i < 10; i++)
	{
		for (UINT j = 0; j < nodeCount; j++)
		{
			Matrix temp;
			if (i == 0)
			{
				temp = XMMatrixMultiply(hierarchyNodes[j].offset, hierarchyNodes[j].world);
			}
			else
			{
				temp = hierarchyMatrix[j];
			}
			hierarchyMatrix.emplace_back(temp);
		}
	}
	vector<HierarchyDATA>().swap(hierarchyDatas);

	//GPUĳ�ô�� ��尹���� �´� ��������. ������ �̸� ���� ���� ĳ�� �ʰ� ���̰� ���� �� �������� ���صּ� �׷���. ĳ�ô� ������ �̸� �����ְ� ���ϸ� �ǹ� ������
	D3D11_TEXTURE2D_DESC hierarchyMatrixTextureDesc;
	hierarchyMatrixTextureDesc.Width = 4 * nodeCount;
	hierarchyMatrixTextureDesc.Height = 1;//�ν��Ͻ̰���
	hierarchyMatrixTextureDesc.ArraySize = 1;
	hierarchyMatrixTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	hierarchyMatrixTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	hierarchyMatrixTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	hierarchyMatrixTextureDesc.MipLevels = 1;
	hierarchyMatrixTextureDesc.CPUAccessFlags = 0;
	hierarchyMatrixTextureDesc.MiscFlags = 0;
	hierarchyMatrixTextureDesc.SampleDesc.Count = 1;
	hierarchyMatrixTextureDesc.SampleDesc.Quality = 0;

	ID3D11Texture2D* hierarchyMatrixTex = nullptr;
	D3D::GetDevice()->CreateTexture2D(&hierarchyMatrixTextureDesc, nullptr, &hierarchyMatrixTex);

	D3D11_UNORDERED_ACCESS_VIEW_DESC hierarchyMatrixUavDesc;
	hierarchyMatrixUavDesc.Format = DXGI_FORMAT_UNKNOWN;
	hierarchyMatrixUavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	hierarchyMatrixUavDesc.Texture2D.MipSlice = 0;
	D3D::GetDevice()->CreateUnorderedAccessView(hierarchyMatrixTex, &hierarchyMatrixUavDesc, &hierarchyMatrixUAV);
	//srv�� uav�� �����͸� �����Ѵ�. ���̾��Űsrv�� �ִϸ��̼ǰ� offset�� ����� ����� ��Ʈ������ ������.
	//��Ʈ���������� U = ��Ʈ���� �� * ���̾��Ű�ѹ� V=10������ �����ͱ��� ����Ѵ�
	//srv�� Ʈ�������� ���ʿ�
	D3D11_TEXTURE2D_DESC hierarchyTextureDesc;
	hierarchyTextureDesc.Width = 4 * nodeCount;
	hierarchyTextureDesc.Height = 10;
	hierarchyTextureDesc.ArraySize = 1;
	hierarchyTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	hierarchyTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	hierarchyTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	hierarchyTextureDesc.MipLevels = 1;
	hierarchyTextureDesc.CPUAccessFlags = 0;
	hierarchyTextureDesc.MiscFlags = 0;
	hierarchyTextureDesc.SampleDesc.Count = 1;
	hierarchyTextureDesc.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA* hierarchyTextureData= new D3D11_SUBRESOURCE_DATA[hierarchyTextureDesc.ArraySize];//���̾��Űsrv�� �ʱ� ���̾��Ű ��Ʈ���� �� offset���� ��Ʈ���� �־��ֱ�
	for (UINT i = 0; i < hierarchyTextureDesc.ArraySize; i++)
	{
		hierarchyTextureData[i].pSysMem = hierarchyMatrix.data();
		hierarchyTextureData[i].SysMemPitch = nodeCount * sizeof(XMFLOAT4X4);
		hierarchyTextureData[i].SysMemSlicePitch = sizeof(XMFLOAT4X4) * nodeCount * 10;
	}

	ID3D11Texture2D* hierarchyTex = nullptr;
	D3D::GetDevice()->CreateTexture2D(&hierarchyTextureDesc, hierarchyTextureData, &hierarchyTex);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;//���ؽ� ���̴����� ����� ���̴� ���ҽ�
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = hierarchyTextureDesc.ArraySize;
	D3D::GetDevice()->CreateShaderResourceView(hierarchyTex, &srvDesc, &hierarchySRV);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;//��ǻƮ ���̴����� ���⸦ �� ���̴� ���ҽ�, ���ؽ� ���̴� ���ҽ��� ���� �����ȴ�.
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.MipSlice = 0;
	uavDesc.Texture2DArray.ArraySize = hierarchyTextureDesc.ArraySize;
	uavDesc.Texture2DArray.FirstArraySlice = 0;
	D3D::GetDevice()->CreateUnorderedAccessView(hierarchyTex, &uavDesc, &hierarchyUAV);
	delete[] hierarchyTextureData;

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

		mesh.meshBuffer = new ConstantBuffer(&data,sizeof(DATA));
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

void Model::CreateAnimSRV()
{
	UINT animCount = (UINT)keyFrames.size();
	UINT maxKeyCount = 0;
	for (KEYFRAME i : keyFrames)
	{
		if (i.keyframes.size() > maxKeyCount)
			maxKeyCount = (UINT)i.keyframes.size();
	}

	D3D11_TEXTURE2D_DESC tex2Ddesc = {};
	tex2Ddesc.Width = nodeCount * 3;
	tex2Ddesc.Height = maxKeyCount;
	tex2Ddesc.ArraySize = animCount;
	tex2Ddesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex2Ddesc.Usage = D3D11_USAGE_IMMUTABLE;
	tex2Ddesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex2Ddesc.MipLevels = 1;
	tex2Ddesc.SampleDesc.Count = 1;

	UINT pageSize = sizeof(XMFLOAT3X4) * nodeCount * maxKeyCount;
	vector< vector<XMFLOAT3X4>> keyData;
	for (UINT anim = 0; anim < animCount; anim++)
	{
		vector<XMFLOAT3X4> data;
		for (UINT key = 0; key < keyFrames[anim].keyframes.size(); key++)
		{
			data.insert(data.begin() + (size_t)nodeCount * key, keyFrames[anim].keyframes[key].begin(), keyFrames[anim].keyframes[key].end());
		}
		data.resize((size_t)nodeCount * maxKeyCount, { 0,0,0,0,0,0,0,1,1,1,1,1 });
		keyData.emplace_back(data);
	}

	D3D11_SUBRESOURCE_DATA* subResource = new D3D11_SUBRESOURCE_DATA[animCount];
	for (UINT anim = 0; anim < animCount; anim++)
	{
		subResource[anim].pSysMem = keyData[anim].data();
		subResource[anim].SysMemPitch = nodeCount * sizeof(XMFLOAT3X4);
		subResource[anim].SysMemSlicePitch = pageSize;
	}

	ComPtr<ID3D11Texture2D> animTex = nullptr;
	D3D::GetDevice()->CreateTexture2D(&tex2Ddesc, subResource, animTex.ReleaseAndGetAddressOf());
	delete[] subResource;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourcedesc = {};
	shaderResourcedesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	shaderResourcedesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	shaderResourcedesc.Texture2DArray.MipLevels = 1;
	shaderResourcedesc.Texture2DArray.ArraySize = animCount;

	D3D::GetDevice()->CreateShaderResourceView(animTex.Get(), &shaderResourcedesc, &animSRV);
}

void Model::AnimUpdate()
{
	//TODO : �ִϸ��̼� Ÿ�� ��� �����ؾ��� AddtionalAnim���� �����غ�����.
	if (keyFrames.empty())
		return;

	for (UINT i = 0; i < inatanceAnims.size(); i++)
	{
		inatanceAnims[i].cur.curtime += Time::Delta() * keyFrames[(UINT)inatanceAnims[i].cur.clip].tickPerSec * inatanceAnimCtrl[i].speed;
		if (inatanceAnims[i].cur.curtime > inatanceAnims[i].cur.duration)
		{
			if(keyFrames[(UINT)inatanceAnims[i].cur.clip].isLoop)
				inatanceAnims[i].cur.curtime = fmod(inatanceAnims[i].cur.curtime, inatanceAnims[i].cur.duration);
			else
			{
				//	if(event!=nullptr)
				//		event;
				ANIMATION tmp;
				tmp = inatanceAnims[i].cur;
				inatanceAnims[i].cur = inatanceAnims[i].next;
				inatanceAnims[i].next = tmp;
				inatanceAnimCtrl[i].fadeOut = 0.f;
			}
			inatanceAnimCtrl[i].fadeIn = 0.f;
		}
		if (inatanceAnims[i].cur.curtime < inatanceAnimCtrl[i].fadeIn)
			inatanceAnims[i].blendFactor = 1.f - (inatanceAnims[i].cur.curtime / inatanceAnimCtrl[i].fadeIn);
		else if (inatanceAnims[i].cur.duration - inatanceAnims[i].cur.curtime < inatanceAnimCtrl[i].fadeOut)
		{
			inatanceAnims[i].blendFactor = 1.f - ((inatanceAnims[i].cur.duration - inatanceAnims[i].cur.curtime) / inatanceAnimCtrl[i].fadeOut);
		}
		else
			inatanceAnims[i].blendFactor = 0.f;
		if (inatanceAnims[i].blendFactor > 0.f)
		{
			inatanceAnims[i].next.curtime += Time::Delta() * keyFrames[(UINT)inatanceAnims[i].next.clip].tickPerSec * inatanceAnimCtrl[i].speed;
			if (inatanceAnims[i].next.curtime > inatanceAnims[i].next.duration)
			{
				if (keyFrames[(UINT)inatanceAnims[i].next.clip].isLoop)
					inatanceAnims[i].next.curtime = fmod(inatanceAnims[i].next.curtime, inatanceAnims[i].next.duration);
				else
					inatanceAnims[i].next.curtime = inatanceAnims[i].next.duration;
			}
		}
		else
			inatanceAnims[i].next.curtime = 0.f;
	}

	//���̴� ���� ���� �������
	ComPtr<ID3D11ShaderResourceView> emptySRV = nullptr;
	D3D::GetDC()->VSSetShaderResources(1, 1, emptySRV.GetAddressOf());

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(inatanceAnimBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy(subResource.pData, inatanceAnims.data(), sizeof(INSTANCEANIMATION) * inatanceAnims.size());
	D3D::GetDC()->Unmap(inatanceAnimBuffer, 0);

	csShader->BindingCS();
	//���̾��Ű����: preQuat, local(Ű���� ��Ʈ���� ��ȯ��), world(������ Ű�������� ��Ʈ��Ʈ����), offset, parentID, enable(��ǻƮ ���ʿ��� �����������Ž�)
	D3D::GetDC()->CSSetShaderResources(0, 1, &hierarchyBufferSrv);//��ǻƮ ���̴����� ���� ���̾��Ű ����
	D3D::GetDC()->CSSetShaderResources(1, 1, &inatanceAnimSRV);//�ν��Ͻ��ִϸ��̼���Ʈ�� ���ҽ���
	D3D::GetDC()->CSSetShaderResources(2, 1, &animSRV);//�ִϸ��̼� ���ҽ���
	D3D::GetDC()->CSSetUnorderedAccessViews(0, 1, &hierarchyMatrixUAV, nullptr);//��ǻƮ�߰� ���̾��Ű ��Ʈ������ UAV
	D3D::GetDC()->CSSetUnorderedAccessViews(1, 1, &hierarchyUAV, nullptr);//��� UAV
	UINT groupSize = (UINT)ceil(1.0 / 32.0f);//�׷��� �ν��Ͻ̰�����
	D3D::GetDC()->Dispatch(instancingCount, 1, 1);

	D3D::GetDC()->CSSetShaderResources(0, 1, emptySRV.GetAddressOf());
	D3D::GetDC()->CSSetShaderResources(1, 1, emptySRV.GetAddressOf());
	D3D::GetDC()->CSSetShaderResources(2, 1, emptySRV.GetAddressOf());
	ComPtr<ID3D11UnorderedAccessView> emptyUAV = nullptr;
	D3D::GetDC()->CSSetUnorderedAccessViews(0, 1, emptyUAV.GetAddressOf(), nullptr);
	D3D::GetDC()->CSSetUnorderedAccessViews(1, 1, emptyUAV.GetAddressOf(), nullptr);
	D3D::GetDC()->CSSetShader(nullptr, nullptr, 0);
}


