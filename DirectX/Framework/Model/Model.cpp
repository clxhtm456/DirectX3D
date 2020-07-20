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

	assert(Path::ExistFile(path));
	LoadModel(path);
	SetVSShader(Shader::VSAdd(L"InstModelVS"));
	SetPSShader(Shader::PSAdd(L"InstModelPS"));

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
		delete mat.matBuffer;

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



//void Model::LoadAnimation(const string path, float blendLoop, bool isLoop)
//{
//	assert(Path::ExistFile(path));
//	BinaryReader* r = new BinaryReader();
//	r->Open(String::ToWString(path));
//
//	KEYFRAME keyFrame;
//
//	keyFrame.tickPerSec = r->Float();
//	keyFrame.duration = r->Float();
//	keyFrame.blendLoop = blendLoop > keyFrame.duration ? keyFrame.duration : blendLoop;
//	keyFrame.isLoop = isLoop;
//	
//	UINT keyCount = r->UInt();
//	for (UINT i = 0; i < keyCount; i++)
//	{
//		UINT count = r->UInt();
//		assert(count == nodeCount);
//		vector<XMFLOAT3X4> Key;
//		for (UINT j = 0; j < count; j++)
//		{
//			XMFLOAT3X4 data = { 0,0,0,0,0,0,0,1,1,1,1,1 };
//			r->BYTE((void**)&data.m[0], sizeof(XMFLOAT3));
//			data._14 = r->Float();
//			r->BYTE((void**)&data.m[1], sizeof(XMFLOAT4));
//			r->BYTE((void**)&data.m[2], sizeof(XMFLOAT3));
//			Key.emplace_back(data);
//		}
//		keyFrame.keyframes.emplace_back(Key);
//	}
//	keyFrames.emplace_back(keyFrame);
//
//	delete r;
//
//	CreateAnimSRV();
//}

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
}

void Model::Render(Camera* viewer)
{
	
	for (int i = 0; i < 2; i++)
	{
		for (Mesh mesh : meshs)
		{
			if ((!materials[mesh.matrialID].matData.opaque && i == 0) || (materials[mesh.matrialID].matData.opaque && i == 1))
				continue;

			mesh.meshBuffer->SetVSBuffer(3);

			materials[mesh.matrialID].matBuffer->SetPSBuffer(0);
			//D3D::GetDC()->PSSetConstantBuffers(0, 1, materials[mesh.matrialID].matBuffer.GetAddressOf());

			D3D::GetDC()->PSSetShaderResources(0, 1, &materials[mesh.matrialID].diffusesrv);
			D3D::GetDC()->PSSetShaderResources(1, 1, &materials[mesh.matrialID].specularsrv);
			D3D::GetDC()->PSSetShaderResources(2, 1, &materials[mesh.matrialID].normalsrv);
			D3D::GetDC()->PSSetShaderResources(3, 1, &materials[mesh.matrialID].emissivesrv);
			D3D::GetDC()->PSSetShaderResources(4, 1, &materials[mesh.matrialID].heightsrv);
			D3D::GetDC()->PSSetShaderResources(5, 1, &materials[mesh.matrialID].ambientsrv);
			D3D::GetDC()->PSSetShaderResources(6, 1, &materials[mesh.matrialID].shininesssrv);
			D3D::GetDC()->PSSetShaderResources(7, 1, &materials[mesh.matrialID].opacitysrv);
			D3D::GetDC()->PSSetShaderResources(8, 1, &materials[mesh.matrialID].displacementsrv);
			D3D::GetDC()->PSSetShaderResources(9, 1, &materials[mesh.matrialID].lightMapsrv);
			D3D::GetDC()->PSSetShaderResources(10, 1, &materials[mesh.matrialID].reflectionsrv);
			//pbr
			D3D::GetDC()->PSSetShaderResources(11, 1, &materials[mesh.matrialID].basecolorsrv);
			D3D::GetDC()->PSSetShaderResources(12, 1, &materials[mesh.matrialID].normalcamerasrv);
			D3D::GetDC()->PSSetShaderResources(13, 1, &materials[mesh.matrialID].emissioncolorsrv);
			D3D::GetDC()->PSSetShaderResources(14, 1, &materials[mesh.matrialID].metalnesssrv);
			D3D::GetDC()->PSSetShaderResources(15, 1, &materials[mesh.matrialID].diffuseroughnesssrv);
			D3D::GetDC()->PSSetShaderResources(16, 1, &materials[mesh.matrialID].ambientocculsionsrv);

			float blendFactor[4] = { 0, 0, 0, 0 };
			if (materials[mesh.matrialID].matData.opaque)//알파블랜딩 부분만 나중에 그려서 해결
			{
				D3D::GetDC()->OMSetDepthStencilState(CommonStates::Get()->DepthDefault(), 1);
				D3D::GetDC()->OMSetBlendState(CommonStates::Get()->Opaque(), blendFactor, 0xffffffff);
				D3D::GetDC()->RSSetState(CommonStates::Get()->CullCounterClockwise());
			}
			else
			{
				D3D::GetDC()->OMSetDepthStencilState(CommonStates::Get()->DepthRead(), 1);
				D3D::GetDC()->OMSetBlendState(CommonStates::Get()->AlphaBlend(), blendFactor, 0xffffffff);
				D3D::GetDC()->RSSetState(CommonStates::Get()->CullNone());
			}
			mesh.vertexBuffer->Binding();
			mesh.indexBuffer->Binding();
			D3D::GetDC()->DrawIndexedInstanced(mesh.indexCount, instancingCount, 0, 0, 0);
		}
	}
}

void Model::LoadModel(const string path)
{
	BinaryReader* r = new BinaryReader();
	r->Open(String::ToWString(path));

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

	vector<HierarchyDATA> hierarchyDatas(nodeCount);//버퍼용 초기셋팅 1번만
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

	/*ConstantBuffer* hierarchyBuffer = nullptr;
	hierarchyBuffer = new ConstantBuffer(hierarchyDatas.data(),sizeof(HierarchyDATA) * nodeCount);*/

	//하이어라키 오프셋 결과 매트릭스 생성
	/*for (UINT i = 0; i < 10; i++)
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
	vector<HierarchyDATA>().swap(hierarchyDatas);*/

	//GPU캐시대신 노드갯수에 맞는 언오더드맵. 갯수를 미리 정해 놓는 캐시 맵과 차이가 없음 더 적은량을 정해둬서 그런듯. 캐시는 갯수를 미리 여유있게 안하면 의미 없어짐
	D3D11_TEXTURE2D_DESC hierarchyMatrixTextureDesc;
	hierarchyMatrixTextureDesc.Width = 4 * nodeCount;
	hierarchyMatrixTextureDesc.Height = 1;//인스턴싱갯수
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
	//srv와 uav는 데이터를 공유한다. 하이어라키srv는 애니메이션과 offset을 계산한 결과값 메트릭스만 가진다.
	//매트릭스구조는 U = 메트릭스 행 * 하이어라키넘버 V=10프레임 이전것까지 기록한다
	//srv라 트랜스포즈 불필요
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

	D3D11_SUBRESOURCE_DATA* hierarchyTextureData= new D3D11_SUBRESOURCE_DATA[hierarchyTextureDesc.ArraySize];//하이어라키srv는 초기 하이어라키 메트릭스 및 offset적용 매트릭스 넣어주기
	for (UINT i = 0; i < hierarchyTextureDesc.ArraySize; i++)
	{
		hierarchyTextureData[i].pSysMem = hierarchyMatrix.data();
		hierarchyTextureData[i].SysMemPitch = nodeCount * sizeof(XMFLOAT4X4);
		hierarchyTextureData[i].SysMemSlicePitch = sizeof(XMFLOAT4X4) * nodeCount * 10;
	}

	ID3D11Texture2D* hierarchyTex = nullptr;
	D3D::GetDevice()->CreateTexture2D(&hierarchyTextureDesc, hierarchyTextureData, &hierarchyTex);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;//버텍스 셰이더에서 사용할 셰이더 리소스
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = hierarchyTextureDesc.ArraySize;
	D3D::GetDevice()->CreateShaderResourceView(hierarchyTex, &srvDesc, &hierarchySRV);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;//컴퓨트 셰이더에서 쓰기를 할 셰이더 리소스, 버텍스 셰이더 리소스랑 값이 공유된다.
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

	count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		string name = r->String();
		materialMap.emplace(name, i);

		Material mat;
		{
			void* ptr = (void*)&mat.matData.diffuse;
			r->BYTE(&ptr, sizeof(XMFLOAT3));
		}
		{
			void* ptr = (void*)&mat.matData.ambient;
			r->BYTE(&ptr, sizeof(XMFLOAT3));
		}
		{
			void* ptr = (void*)&mat.matData.specular;
			r->BYTE(&ptr, sizeof(XMFLOAT3));
		}
		{
			void* ptr = (void*)&mat.matData.emissive;
			r->BYTE(&ptr, sizeof(XMFLOAT3));
		}
		{
			void* ptr = (void*)&mat.matData.tranparent;
			r->BYTE(&ptr, sizeof(XMFLOAT3));
		}
		{
			void* ptr = (void*)&mat.matData.reflective;
			r->BYTE(&ptr, sizeof(XMFLOAT3));
		}

		mat.matData.opacity = r->Float();
		mat.matData.transparentfactor = r->Float();
		mat.matData.bumpscaling = r->Float();
		mat.matData.shininess = r->Float();
		mat.matData.reflectivity = r->Float();
		mat.matData.shininessstrength = r->Float();
		mat.matData.refracti = r->Float();
		
		mat.diffusesrv = Texture::LoadSRV(r->String());
		mat.specularsrv = Texture::LoadSRV(r->String());
		mat.ambientsrv = Texture::LoadSRV(r->String());
		mat.emissivesrv = Texture::LoadSRV(r->String());
		mat.heightsrv = Texture::LoadSRV(r->String());
		mat.normalsrv = Texture::LoadSRV(r->String());
		mat.shininesssrv = Texture::LoadSRV(r->String());
		mat.opacitysrv = Texture::LoadSRV(r->String());
		mat.displacementsrv = Texture::LoadSRV(r->String());
		mat.lightMapsrv = Texture::LoadSRV(r->String());
		mat.reflectionsrv = Texture::LoadSRV(r->String());
		//pbr
		mat.basecolorsrv = Texture::LoadSRV(r->String());
		mat.normalcamerasrv = Texture::LoadSRV(r->String());
		mat.emissioncolorsrv = Texture::LoadSRV(r->String());
		mat.metalnesssrv = Texture::LoadSRV(r->String());
		mat.diffuseroughnesssrv = Texture::LoadSRV(r->String());
		mat.ambientocculsionsrv = Texture::LoadSRV(r->String());


		if (mat.opacitysrv != NULL || mat.matData.tranparent.x > 0.f || mat.matData.tranparent.y > 0.f || mat.matData.tranparent.z > 0.f)
			mat.matData.opaque = 0;
		if (mat.reflectionsrv != NULL || mat.matData.reflective.x > 0.f || mat.matData.reflective.y > 0.f || mat.matData.reflective.z > 0.f)
			mat.matData.reflector = 1;
		if (mat.diffusesrv != NULL)
			mat.matData.hasDiffuseMap = 1;
		if (mat.specularsrv != NULL)
			mat.matData.hasSpecularMap = 1;
		if (mat.ambientsrv != NULL)
			mat.matData.hasAmbientMap = 1;
		if (mat.emissivesrv != NULL)
			mat.matData.hasEmissiveMap = 1;
		if (mat.opacitysrv != NULL)
			mat.matData.hasOpacityMap = 1;


		mat.matBuffer = new ConstantBuffer(&mat.matData, sizeof(MaterialData));

		materials.emplace_back(mat);
	}

	delete r;
}

//void Model::CreateAnimSRV()
//{
//	UINT animCount = (UINT)keyFrames.size();
//	UINT maxKeyCount = 0;
//	for (KEYFRAME i : keyFrames)
//	{
//		if (i.keyframes.size() > maxKeyCount)
//			maxKeyCount = (UINT)i.keyframes.size();
//	}
//
//	D3D11_TEXTURE2D_DESC tex2Ddesc = {};
//	tex2Ddesc.Width = nodeCount * 3;
//	tex2Ddesc.Height = maxKeyCount;
//	tex2Ddesc.ArraySize = animCount;
//	tex2Ddesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//	tex2Ddesc.Usage = D3D11_USAGE_IMMUTABLE;
//	tex2Ddesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//	tex2Ddesc.MipLevels = 1;
//	tex2Ddesc.SampleDesc.Count = 1;
//
//	UINT pageSize = sizeof(XMFLOAT3X4) * nodeCount * maxKeyCount;
//	vector< vector<XMFLOAT3X4>> keyData;
//	for (UINT anim = 0; anim < animCount; anim++)
//	{
//		vector<XMFLOAT3X4> data;
//		for (UINT key = 0; key < keyFrames[anim].keyframes.size(); key++)
//		{
//			data.insert(data.begin() + (size_t)nodeCount * key, keyFrames[anim].keyframes[key].begin(), keyFrames[anim].keyframes[key].end());
//		}
//		data.resize((size_t)nodeCount * maxKeyCount, { 0,0,0,0,0,0,0,1,1,1,1,1 });
//		keyData.emplace_back(data);
//	}
//
//	D3D11_SUBRESOURCE_DATA* subResource = new D3D11_SUBRESOURCE_DATA[animCount];
//	for (UINT anim = 0; anim < animCount; anim++)
//	{
//		subResource[anim].pSysMem = keyData[anim].data();
//		subResource[anim].SysMemPitch = nodeCount * sizeof(XMFLOAT3X4);
//		subResource[anim].SysMemSlicePitch = pageSize;
//	}
//
//	ComPtr<ID3D11Texture2D> animTex = nullptr;
//	D3D::GetDevice()->CreateTexture2D(&tex2Ddesc, subResource, animTex.ReleaseAndGetAddressOf());
//	delete[] subResource;
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourcedesc = {};
//	shaderResourcedesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//	shaderResourcedesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
//	shaderResourcedesc.Texture2DArray.MipLevels = 1;
//	shaderResourcedesc.Texture2DArray.ArraySize = animCount;
//
//	D3D::GetDevice()->CreateShaderResourceView(animTex.Get(), &shaderResourcedesc, &animSRV);
//}

//void Model::AnimUpdate()
//{
//	//TODO : 애니메이션 타임 방식 정리해야함 AddtionalAnim적용 생각해봐야함.
//	if (keyFrames.empty())
//		return;
//
//	for (UINT i = 0; i < inatanceAnims.size(); i++)
//	{
//		inatanceAnims[i].cur.curtime += Time::Delta() * keyFrames[(UINT)inatanceAnims[i].cur.clip].tickPerSec * inatanceAnimCtrl[i].speed;
//		if (inatanceAnims[i].cur.curtime > inatanceAnims[i].cur.duration)
//		{
//			if(keyFrames[(UINT)inatanceAnims[i].cur.clip].isLoop)
//				inatanceAnims[i].cur.curtime = fmod(inatanceAnims[i].cur.curtime, inatanceAnims[i].cur.duration);
//			else
//			{
//				//	if(event!=nullptr)
//				//		event;
//				ANIMATION tmp;
//				tmp = inatanceAnims[i].cur;
//				inatanceAnims[i].cur = inatanceAnims[i].next;
//				inatanceAnims[i].next = tmp;
//				inatanceAnimCtrl[i].fadeOut = 0.f;
//			}
//			inatanceAnimCtrl[i].fadeIn = 0.f;
//		}
//		if (inatanceAnims[i].cur.curtime < inatanceAnimCtrl[i].fadeIn)
//			inatanceAnims[i].blendFactor = 1.f - (inatanceAnims[i].cur.curtime / inatanceAnimCtrl[i].fadeIn);
//		else if (inatanceAnims[i].cur.duration - inatanceAnims[i].cur.curtime < inatanceAnimCtrl[i].fadeOut)
//		{
//			inatanceAnims[i].blendFactor = 1.f - ((inatanceAnims[i].cur.duration - inatanceAnims[i].cur.curtime) / inatanceAnimCtrl[i].fadeOut);
//		}
//		else
//			inatanceAnims[i].blendFactor = 0.f;
//		if (inatanceAnims[i].blendFactor > 0.f)
//		{
//			inatanceAnims[i].next.curtime += Time::Delta() * keyFrames[(UINT)inatanceAnims[i].next.clip].tickPerSec * inatanceAnimCtrl[i].speed;
//			if (inatanceAnims[i].next.curtime > inatanceAnims[i].next.duration)
//			{
//				if (keyFrames[(UINT)inatanceAnims[i].next.clip].isLoop)
//					inatanceAnims[i].next.curtime = fmod(inatanceAnims[i].next.curtime, inatanceAnims[i].next.duration);
//				else
//					inatanceAnims[i].next.curtime = inatanceAnims[i].next.duration;
//			}
//		}
//		else
//			inatanceAnims[i].next.curtime = 0.f;
//	}
//
//	//셰이더 연결 끊고 해줘야함
//	ComPtr<ID3D11ShaderResourceView> emptySRV = nullptr;
//	D3D::GetDC()->VSSetShaderResources(1, 1, emptySRV.GetAddressOf());
//
//	D3D11_MAPPED_SUBRESOURCE subResource;
//	D3D::GetDC()->Map(inatanceAnimBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
//	memcpy(subResource.pData, inatanceAnims.data(), sizeof(INSTANCEANIMATION) * inatanceAnims.size());
//	D3D::GetDC()->Unmap(inatanceAnimBuffer.Get(), 0);
//
//	computeShader->BindingCS();
//	//하이어라키버퍼: preQuat, local(키없이 메트릭스 반환시), world(상위에 키가없을시 페어런트메트릭스), offset, parentID, enable(컴퓨트 불필요한 스레드계산제거시)
//	D3D::GetDC()->CSSetShaderResources(0, 1, hierarchyBufferSrv.GetAddressOf());//컴퓨트 셰이더에서 쓰일 하이어라키 정보
//	D3D::GetDC()->CSSetShaderResources(1, 1, inatanceAnimSRV.GetAddressOf());//인스턴스애니메이션컨트롤 리소스뷰
//	D3D::GetDC()->CSSetShaderResources(2, 1, &animSRV);//애니메이션 리소스뷰
//	D3D::GetDC()->CSSetUnorderedAccessViews(0, 1, &hierarchyMatrixUAV, nullptr);//컴퓨트중간 하이어라키 매트릭스용 UAV
//	D3D::GetDC()->CSSetUnorderedAccessViews(1, 1, &hierarchyUAV, nullptr);//결과 UAV
//	UINT groupSize = (UINT)ceil(1.0 / 32.0f);//그룹은 인스턴싱갯수로
//	D3D::GetDC()->Dispatch(instancingCount, 1, 1);
//
//	D3D::GetDC()->CSSetShaderResources(0, 1, emptySRV.GetAddressOf());
//	D3D::GetDC()->CSSetShaderResources(1, 1, emptySRV.GetAddressOf());
//	D3D::GetDC()->CSSetShaderResources(2, 1, emptySRV.GetAddressOf());
//	ComPtr<ID3D11UnorderedAccessView> emptyUAV = nullptr;
//	D3D::GetDC()->CSSetUnorderedAccessViews(0, 1, emptyUAV.GetAddressOf(), nullptr);
//	D3D::GetDC()->CSSetUnorderedAccessViews(1, 1, emptyUAV.GetAddressOf(), nullptr);
//	D3D::GetDC()->CSSetShader(nullptr, nullptr, 0);
//}


