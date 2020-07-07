#include "stdafx.h"
#include "ModelLoader.h"
#include "Model/FbxType.h"
//#include "model\Animation.h"


//assimp를 이용해 모델 정보를 읽어온다.
void ModelLoader::LoadModel(string path, UINT flag)
{
	//Read Model
	Assimp::Importer	importer;
	string				strPath	= path;
	const aiScene*		pScene	= importer.ReadFile(strPath, flag);
	
	if (!pScene) return;

	//정보를 담을 공간
	VertexData				vertices;
	vector<UINT>			indices;
	vector<FbxMeshData>		meshes;
	vector<MaterialSrc>		materials;
	vector<FbxBoneData*>	bones;

	//모델 이름 정의
	wstring modelName = String::ToWString(Path::GetFileName(path));

	//메쉬 정보 구성
	for (UINT i = 0; i < pScene->mNumMeshes; i++)
		ProcessMesh(pScene->mMeshes[i], vertices, indices, meshes);

	//재질 정보 생성
	ProcessMaterial( pScene, materials, Path::GetDirectoryName(path));
	
	//계층구조(본) 정보 구성
	ProcessNode(pScene->mRootNode, bones);
	//순차적 tm 업데이트를 위해 깊이 값에 맞춰 정렬
	sort(model->GetNodeList().begin(), model->GetNodeList().end(),
		[](const NodeInfo* a, const NodeInfo* b)->bool {	return a->depth < b->depth; });


	//스키닝 정보
	for (UINT i = 0; i < pScene->mNumMeshes; i++) {
		aiMesh* aiMesh = pScene->mMeshes[i];

		if (!aiMesh->HasBones()) 
			continue;

		FbxMeshData* mesh = meshes[i];
		ProcessSkin(aiMesh, mesh, vertices, indices, model);
	}

	//애니메이션 정보 구성
	if (pScene->HasAnimations())
		ProcessAnimation(pScene, model);
	
	//정점 및 인덱스 정보 생성 실패시 생성한 모델 객체 삭제 후 반환
	if (!model->CreateModel(D3D::GetDevice(), vertices, indices)) {
		SafeRelease(model);
		return;
	}

	//마테리얼 단위로 메쉬 정보 업데이트
	model->UpdateMeshByMaterial();

	return model;
}

vector<AsClip> ModelLoader::LoadAnimation(string path, UINT flag)
{

	//Read Model
	Assimp::Importer	importer;
	string				strPath = path;
	const aiScene*		pScene = importer.ReadFile(strPath, flag);

	vector<AsClip>		clips;
	if (!pScene) return;

	//애니메이션 정보 구성
	if (pScene->HasAnimations())
		ProcessAnimation(pScene, clips);

	return clips;
}



void ModelLoader::ProcessNode(aiNode * aiNodeInfo, SkinModel* skModel, NodeInfo* parent, int depth)
{
	//계층구조 정보 생성 후 추가
	wstring nodeName = MYUTIL::ConvertToWString((CString)aiNodeInfo->mName.C_Str());
	XMMATRIX tm = XMMatrixTranspose(XMMATRIX(aiNodeInfo->mTransformation[0]));
	NodeInfo* node = new NodeInfo(parent, nodeName, tm, depth);
	skModel->GetNodeList().emplace_back(node);
	
	//메쉬와 연결된 본이면 정보연결
	if (aiNodeInfo->mNumMeshes > 0) {
		HierarchyMesh* hiMesh = (HierarchyMesh*)skModel->GetMeshList()[aiNodeInfo->mMeshes[0]];
		hiMesh->linkNode = node;
	}

	//하위 노드 탐색
	for (UINT i = 0; i < aiNodeInfo->mNumChildren; i++) {
		this->ProcessNode(aiNodeInfo->mChildren[i], skModel, node, depth+1);
	}


}

void ModelLoader::ProcessMesh(aiMesh * mesh, VertexData& vertices, vector<UINT>& indices, vector<FbxMeshData*>& meshList)
{
	UINT startIdx	= (UINT)indices.size();
	UINT startVert	= (UINT)vertices.vertex.size();

	XMFLOAT3 position, normal, bitangent, tangent;
	//XMFLOAT4 color;
	
	//정점 정보 구성

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		ModelVertex vertex;
		//Position
		if (!mesh->mVertices)	return;
		memcpy_s(&position, sizeof(position), &mesh->mVertices[i], sizeof(mesh->mVertices[i]));
		vertex.Position = position;
		
		//색상정보는 제외 함.
		/*if (mesh->mColors) {	
			memcpy_s(&color, sizeof(color), &mesh->mColors[i], sizeof(mesh->mColors[i]));
			vertices.color.emplace_back(color);
		}*/

		//Normal
		if (mesh->mNormals) {
			memcpy_s(&normal, sizeof(normal), &mesh->mNormals[i], sizeof(mesh->mNormals[i]));
			vertex.Normal=normal;
		}

		////Bitangent
		//if (mesh->mTangents) {
		//	memcpy_s(&bitangent, sizeof(bitangent), &mesh->mBitangents[i], sizeof(mesh->mBitangents[i]));
		//	vertices.bitangent.emplace_back(bitangent);
		//}

		//Tangent
		if (mesh->mTangents) {
			memcpy_s(&tangent, sizeof(tangent), &mesh->mTangents[i], sizeof(mesh->mTangents[i]));
			vertex.Tangent = tangent;
		}

		//UV
		if (mesh->mTextureCoords[0]) {
			vertex.Uv = XMFLOAT2((float)mesh->mTextureCoords[0][i].x, (float)mesh->mTextureCoords[0][i].y);
		}


		vertices.vertex.emplace_back(vertex);
	}

	//인덱스 정보 구성
	for (UINT i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++) {
			indices.emplace_back(face.mIndices[j]);
			indices.back() += startVert;
		}
	}

	//메쉬 생성
	
	wstring meshName = String::ToWString((mesh->mName.C_Str()));
	meshList.emplace_back(new FbxMeshData(meshName, startIdx, (int)indices.size() - startIdx, mesh->mMaterialIndex, startVert));
	
}

void ModelLoader::ProcessMaterial(const aiScene * pScene, vector<MaterialSrc>& matList, string directoryPath)
{
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		MaterialSrc newMat;

		// extract material info if material exists.
		if (pScene->mMaterials[i] != NULL) {

			// variables for materials
			/*aiColor3D color(0.0f, 0.0f, 0.0f);
			aiColor3D ambient(0.0f, 0.0f, 0.0f);
			aiColor3D specular(0.0f, 0.0f, 0.0f);
			aiColor3D transparent(0.0f, 0.0f, 0.0f);

			float opacity = 0.0f;
			float shininess = 0.0f;*/

			aiString  texture_path[3];
			pScene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path[0], NULL, NULL, NULL, NULL, NULL);
			string texPath = directoryPath + Path::GetFileName(texture_path[0].C_Str());
			newMat.diffuseMap = texPath;

			pScene->mMaterials[i]->GetTexture(aiTextureType_OPACITY, 0, &texture_path[1], NULL, NULL, NULL, NULL, NULL);
			texPath = directoryPath + Path::GetFileName(texture_path[1].C_Str());
			newMat.alphaMap = texPath;

			pScene->mMaterials[i]->GetTexture(aiTextureType_HEIGHT, 0, &texture_path[2], NULL, NULL, NULL, NULL, NULL);
			texPath = directoryPath + Path::GetFileName(texture_path[2].C_Str());
			newMat.normalMap = texPath;

			//assimp 제공되는 정보 확인을 위해 나열 해 놓음
			{
				// set diffuse color.
				//pScene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, color);
				//newMat->setDiffuseColour(glm::vec3(color[0], color[1], color[2]));

				//// set ambient color.
				//pScene->mMaterials[i]->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
				//newMat->setAmbientColour(glm::vec3(ambient[0], ambient[2], ambient[3]));

				//// set specular color.
				//pScene->mMaterials[i]->Get(AI_MATKEY_COLOR_SPECULAR, specular);
				//newMat->setSpecularColour(glm::vec3(specular[0], specular[1], specular[2]));

				//// set transparency of materials.
				//pScene->mMaterials[i]->Get(AI_MATKEY_COLOR_TRANSPARENT, transparent);
				//newMat->setTransparentColour(glm::vec3(transparent[0], transparent[1], transparent[2]));

				//// set shininess.
				//pScene->mMaterials[i]->Get(AI_MATKEY_SHININESS, shininess);
				//newMat->setShininess(shininess);

				//// set opacity of material
				//pScene->mMaterials[i]->Get(AI_MATKEY_OPACITY, opacity);
				//newMat->setOpacity(opacity);

				/*		pScene->mMaterials[i]->GetTexture(aiTextureType_NORMALS, 0, &texture_path[2], NULL, NULL, NULL, NULL, NULL);
				pScene->mMaterials[i]->GetTexture(aiTextureType_NONE, 0, &texture_path[2], NULL, NULL, NULL, NULL, NULL);
				pScene->mMaterials[i]->GetTexture(aiTextureType_UNKNOWN, 0, &texture_path[2], NULL, NULL, NULL, NULL, NULL);
				pScene->mMaterials[i]->GetTexture(aiTextureType_AMBIENT, 0, &texture_path[2], NULL, NULL, NULL, NULL, NULL);
				pScene->mMaterials[i]->GetTexture(aiTextureType_DISPLACEMENT, 0, &texture_path[2], NULL, NULL, NULL, NULL, NULL);
				pScene->mMaterials[i]->GetTexture(aiTextureType_EMISSIVE, 0, &texture_path[2], NULL, NULL, NULL, NULL, NULL);
				pScene->mMaterials[i]->GetTexture(aiTextureType_HEIGHT, 0, &texture_path[2], NULL, NULL, NULL, NULL, NULL);
				pScene->mMaterials[i]->GetTexture(aiTextureType_LIGHTMAP, 0, &texture_path[2], NULL, NULL, NULL, NULL, NULL);
				pScene->mMaterials[i]->GetTexture(aiTextureType_REFLECTION, 0, &texture_path[2], NULL, NULL, NULL, NULL, NULL);
				pScene->mMaterials[i]->GetTexture(aiTextureType_SHININESS, 0, &texture_path[2], NULL, NULL, NULL, NULL, NULL);
				pScene->mMaterials[i]->GetTexture(aiTextureType_SPECULAR, 0, &texture_path[2], NULL, NULL, NULL, NULL, NULL);
				pScene->mMaterials[i]->GetTexture(aiTextureType_UNKNOWN, 0, &texture_path[2], NULL, NULL, NULL, NULL, NULL);*/
			}
		}
		matList.emplace_back(newMat);
	}
}

void ModelLoader::ProcessSkin(aiMesh * aiMesh, HierarchyMesh * mesh, VertexData& vertices, vector<UINT>& indices, SkinModel* skModel)
{
	auto  node			= skModel->GetNodeList();
	auto& listBoneId	= vertices.boneidx;
	auto& listWeight	= vertices.weight;

	//정점 정보를 묶어 레이아웃으로 넘기기 위해 가중치 정보와 본 인덱스 정보를 정점의 위치 갯수와 일치시킨다.
	if (listBoneId.size() < vertices.vertex.size()) {
		listBoneId.resize(vertices.vertex.size(), XMUINT4(0,0,0,0));
		listWeight.resize(vertices.vertex.size(), XMFLOAT4(0.f, 0.f, 0.f, 0.f));
	}
	
	//스키닝 정보 구성
	for (UINT i = 0; i < aiMesh->mNumBones; i++) {
		auto aibone = aiMesh->mBones[i];
		
		//오프셋 
		AsBoneData bone;
		bone.matOffset = XMMatrixTranspose(XMMATRIX(aibone->mOffsetMatrix[0]));
		
		//연결된 노드 찾기
		bone = *FindNode(aibone->mName, node);
		mesh->boneList.emplace_back(bone);
	

		//가중치 정보
		for (UINT j = 0; j < aibone->mNumWeights; j++) {
			auto vertId = aibone->mWeights[j].mVertexId + mesh->startVert;
			auto weight = aibone->mWeights[j].mWeight;

			//가중치 값이 없는 위치에 입력
			if (listWeight[vertId].x == 0) {
				listBoneId[vertId].x = i;
				listWeight[vertId].x = weight;
			}
			else if (listWeight[vertId].y == 0) {
				listBoneId[vertId].y = i;
				listWeight[vertId].y = weight;
			}
			else if (listWeight[vertId].z == 0) {
				listBoneId[vertId].z = i;
				listWeight[vertId].z = weight;
			}
			else if (listWeight[vertId].w == 0) {
				listBoneId[vertId].w = i;
				listWeight[vertId].w = weight;
			}
		}
	}
}

void ModelLoader::ProcessAnimation(const aiScene * pScene, vector<AsClip>& skModel)
{

	//애니메이션의 갯수 만큼...
	for (UINT i = 0; i < pScene->mNumAnimations; i++) {
		aiAnimation* aiAni = pScene->mAnimations[i];
		
		float lastTime = 0.f;
		AsClip aniInfo;
		aniInfo.duration = ((float)aiAni->mDuration);
		aniInfo.frameRate = ((float)aiAni->mTicksPerSecond);

		string aniName = aiAni->mName.C_Str();

		if(aniName != "")
			aniInfo.name = aniName;

		//연결된 노드 만큼...
		for (UINT j = 0; j < aiAni->mNumChannels; j++) {
			auto aiAniNode = aiAni->mChannels[j];

			AsClipNode aniNodeInfo;
			aniNodeInfo.name = aiAniNode->mNodeName.C_Str();
			UINT keyCnt = max(aiAniNode->mNumPositionKeys, aiAniNode->mNumRotationKeys);
			keyCnt		= max(keyCnt, aiAniNode->mNumScalingKeys);


			//키 프레임 정보를 구성한다.
			XMFLOAT3 translation	= XMFLOAT3(0.f, 0.f, 0.f);
			XMFLOAT3 scale			= XMFLOAT3(0.f,0.f,0.f);
			XMFLOAT4 rotation		= XMFLOAT4(0.f,0.f,0.f,0.f);
			float	 time			= 0.f;

			//키 값은 있을 수도 있으며, 없을수도 있다.
			//없는 경우 이전 값으로 채운다.
			for (UINT k = 0; k < keyCnt; k++) {
				if (aiAniNode->mNumPositionKeys > k) {
					auto posKey = aiAniNode->mPositionKeys[k];
					memcpy_s(&translation, sizeof(translation), &posKey.mValue, sizeof(posKey.mValue));
					time = (float)aiAniNode->mPositionKeys[k].mTime;
				}

				if (aiAniNode->mNumRotationKeys > k) {
					auto rotKey = aiAniNode->mRotationKeys[k];
					rotation = XMFLOAT4(rotKey.mValue.x, rotKey.mValue.y, rotKey.mValue.z, rotKey.mValue.w);
					time = (float)aiAniNode->mRotationKeys[k].mTime;
				}

				if (aiAniNode->mNumScalingKeys > k) {
					auto scaleKey = aiAniNode->mScalingKeys[k];
					memcpy_s(&scale, sizeof(scale), &scaleKey.mValue, sizeof(scaleKey.mValue));
					time = (float)aiAniNode->mScalingKeys[k].mTime;
				}

				aniNodeInfo.keyframe.emplace_back(AsKeyFrameData{ time, translation, rotation, scale });
			}

			lastTime = max(aniNodeInfo.keyframe.back().time, lastTime);
			aniInfo.keyFrames.emplace_back(aniNodeInfo);
		}

		//aniInfo.SetLastFrame(lastTime);
		skModel.emplace_back(aniInfo);
	}
}


//이름과 일치하는 노드를 찾는다.
AsBoneData * ModelLoader::FindNode(aiString name, vector<AsBoneData*> nodeList)
{
	AsBoneData* findNode = NULL;
	string bName = name.C_Str();

	//노드 탐색
	auto nodeInfo = find_if(nodeList.begin(), nodeList.end(), [bName](const AsBoneData* a)->bool { return a->name == bName; });

	//찾으면 노드 정보 반환
	if (nodeInfo != nodeList.end())
		findNode = *nodeInfo;

	return findNode;
}
