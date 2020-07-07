#include "stdafx.h"
#include "ModelLoader.h"
#include "Model/FbxType.h"
//#include "model\Animation.h"


//assimp�� �̿��� �� ������ �о�´�.
void ModelLoader::LoadModel(string path, UINT flag)
{
	//Read Model
	Assimp::Importer	importer;
	string				strPath	= path;
	const aiScene*		pScene	= importer.ReadFile(strPath, flag);
	
	if (!pScene) return;

	//������ ���� ����
	VertexData				vertices;
	vector<UINT>			indices;
	vector<FbxMeshData>		meshes;
	vector<MaterialSrc>		materials;
	vector<FbxBoneData*>	bones;

	//�� �̸� ����
	wstring modelName = String::ToWString(Path::GetFileName(path));

	//�޽� ���� ����
	for (UINT i = 0; i < pScene->mNumMeshes; i++)
		ProcessMesh(pScene->mMeshes[i], vertices, indices, meshes);

	//���� ���� ����
	ProcessMaterial( pScene, materials, Path::GetDirectoryName(path));
	
	//��������(��) ���� ����
	ProcessNode(pScene->mRootNode, bones);
	//������ tm ������Ʈ�� ���� ���� ���� ���� ����
	sort(model->GetNodeList().begin(), model->GetNodeList().end(),
		[](const NodeInfo* a, const NodeInfo* b)->bool {	return a->depth < b->depth; });


	//��Ű�� ����
	for (UINT i = 0; i < pScene->mNumMeshes; i++) {
		aiMesh* aiMesh = pScene->mMeshes[i];

		if (!aiMesh->HasBones()) 
			continue;

		FbxMeshData* mesh = meshes[i];
		ProcessSkin(aiMesh, mesh, vertices, indices, model);
	}

	//�ִϸ��̼� ���� ����
	if (pScene->HasAnimations())
		ProcessAnimation(pScene, model);
	
	//���� �� �ε��� ���� ���� ���н� ������ �� ��ü ���� �� ��ȯ
	if (!model->CreateModel(D3D::GetDevice(), vertices, indices)) {
		SafeRelease(model);
		return;
	}

	//���׸��� ������ �޽� ���� ������Ʈ
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

	//�ִϸ��̼� ���� ����
	if (pScene->HasAnimations())
		ProcessAnimation(pScene, clips);

	return clips;
}



void ModelLoader::ProcessNode(aiNode * aiNodeInfo, SkinModel* skModel, NodeInfo* parent, int depth)
{
	//�������� ���� ���� �� �߰�
	wstring nodeName = MYUTIL::ConvertToWString((CString)aiNodeInfo->mName.C_Str());
	XMMATRIX tm = XMMatrixTranspose(XMMATRIX(aiNodeInfo->mTransformation[0]));
	NodeInfo* node = new NodeInfo(parent, nodeName, tm, depth);
	skModel->GetNodeList().emplace_back(node);
	
	//�޽��� ����� ���̸� ��������
	if (aiNodeInfo->mNumMeshes > 0) {
		HierarchyMesh* hiMesh = (HierarchyMesh*)skModel->GetMeshList()[aiNodeInfo->mMeshes[0]];
		hiMesh->linkNode = node;
	}

	//���� ��� Ž��
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
	
	//���� ���� ����

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		ModelVertex vertex;
		//Position
		if (!mesh->mVertices)	return;
		memcpy_s(&position, sizeof(position), &mesh->mVertices[i], sizeof(mesh->mVertices[i]));
		vertex.Position = position;
		
		//���������� ���� ��.
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

	//�ε��� ���� ����
	for (UINT i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++) {
			indices.emplace_back(face.mIndices[j]);
			indices.back() += startVert;
		}
	}

	//�޽� ����
	
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

			//assimp �����Ǵ� ���� Ȯ���� ���� ���� �� ����
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

	//���� ������ ���� ���̾ƿ����� �ѱ�� ���� ����ġ ������ �� �ε��� ������ ������ ��ġ ������ ��ġ��Ų��.
	if (listBoneId.size() < vertices.vertex.size()) {
		listBoneId.resize(vertices.vertex.size(), XMUINT4(0,0,0,0));
		listWeight.resize(vertices.vertex.size(), XMFLOAT4(0.f, 0.f, 0.f, 0.f));
	}
	
	//��Ű�� ���� ����
	for (UINT i = 0; i < aiMesh->mNumBones; i++) {
		auto aibone = aiMesh->mBones[i];
		
		//������ 
		AsBoneData bone;
		bone.matOffset = XMMatrixTranspose(XMMATRIX(aibone->mOffsetMatrix[0]));
		
		//����� ��� ã��
		bone = *FindNode(aibone->mName, node);
		mesh->boneList.emplace_back(bone);
	

		//����ġ ����
		for (UINT j = 0; j < aibone->mNumWeights; j++) {
			auto vertId = aibone->mWeights[j].mVertexId + mesh->startVert;
			auto weight = aibone->mWeights[j].mWeight;

			//����ġ ���� ���� ��ġ�� �Է�
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

	//�ִϸ��̼��� ���� ��ŭ...
	for (UINT i = 0; i < pScene->mNumAnimations; i++) {
		aiAnimation* aiAni = pScene->mAnimations[i];
		
		float lastTime = 0.f;
		AsClip aniInfo;
		aniInfo.duration = ((float)aiAni->mDuration);
		aniInfo.frameRate = ((float)aiAni->mTicksPerSecond);

		string aniName = aiAni->mName.C_Str();

		if(aniName != "")
			aniInfo.name = aniName;

		//����� ��� ��ŭ...
		for (UINT j = 0; j < aiAni->mNumChannels; j++) {
			auto aiAniNode = aiAni->mChannels[j];

			AsClipNode aniNodeInfo;
			aniNodeInfo.name = aiAniNode->mNodeName.C_Str();
			UINT keyCnt = max(aiAniNode->mNumPositionKeys, aiAniNode->mNumRotationKeys);
			keyCnt		= max(keyCnt, aiAniNode->mNumScalingKeys);


			//Ű ������ ������ �����Ѵ�.
			XMFLOAT3 translation	= XMFLOAT3(0.f, 0.f, 0.f);
			XMFLOAT3 scale			= XMFLOAT3(0.f,0.f,0.f);
			XMFLOAT4 rotation		= XMFLOAT4(0.f,0.f,0.f,0.f);
			float	 time			= 0.f;

			//Ű ���� ���� ���� ������, �������� �ִ�.
			//���� ��� ���� ������ ä���.
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


//�̸��� ��ġ�ϴ� ��带 ã�´�.
AsBoneData * ModelLoader::FindNode(aiString name, vector<AsBoneData*> nodeList)
{
	AsBoneData* findNode = NULL;
	string bName = name.C_Str();

	//��� Ž��
	auto nodeInfo = find_if(nodeList.begin(), nodeList.end(), [bName](const AsBoneData* a)->bool { return a->name == bName; });

	//ã���� ��� ���� ��ȯ
	if (nodeInfo != nodeList.end())
		findNode = *nodeInfo;

	return findNode;
}
