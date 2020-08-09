#include "stdafx.h"
#include "AssimpConverter.h"

#include "Utilities/Xml.h"

AssimpConverter::AssimpConverter()
{
}

AssimpConverter::~AssimpConverter()
{
	ResetMeshs();
	ResetAnimations();
}

void AssimpConverter::ConvertMesh(const string path, const string outPath)
{
	assert(Path::ExistFile(path));
	meshPath = path;
	name = Path::GetFileNameWithoutExtension(Path::GetFileName(path));
	string outName= Path::GetFileNameWithoutExtension(Path::GetFileName(outPath));
	if (outPath == "")
	{
		this->outPath = "../../_Assets/Meshes/" + name + "/" + name;
	}
	else
	{
		if(outName == "")
			this->outPath = outPath + name;
		else if(outName == outPath)
			this->outPath = "../../_Assets/Meshes/" + outName + "/" + outName;
		else
			this->outPath = outPath;
	}

	LoadMesh();

	if (!meshs.size())
		return;

	SaveMesh();
}


void AssimpConverter::LoadAnimation(const string path)
{
	if (!hierarchyNodes.size())
		return;

	assert(Path::ExistFile(path));

	ResetAnimations();

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_MakeLeftHanded);

	if (scene->HasAnimations())
	{
		animName = Path::GetFileNameWithoutExtension(Path::GetFileName(path));
		AddAnimation(scene);
	}
}

void AssimpConverter::SaveAnimation(string path, UINT takeNum)
{
	if (takeNum >= animations.size())
		return;

	if (path == "")
	{
		if(animations.size() == 1)
			path = "../../_Assets/Meshes/" + name + "/anims/" + animName + ".anim";
		else
			path = "../../_Assets/Meshes/" + name + "/anims/" + animName + animations[takeNum].name + ".anim";
	}

	Path::CreateFolders(Path::GetDirectoryName(path));

	BinaryWriter* w = new BinaryWriter();
	w->Open(path);
	w->String(animations[takeNum].name);
	w->Float(animations[takeNum].tickPerSec);
	w->Float(animations[takeNum].duration);
	w->UInt((UINT)animations[takeNum].keyframes.size());

	for (auto keyframe : animations[takeNum].keyframes)
	{
		w->String(keyframe.boneName);
		w->UInt((UINT)keyframe.keys.size());
		for (auto key : keyframe.keys)
		{
			w->BYTE(&key.translate, sizeof(XMFLOAT4));
			w->BYTE(&key.quaternion, sizeof(XMFLOAT4));
			w->BYTE(&key.scale, sizeof(XMFLOAT4));
			w->Float(key.time);
		}
	}
	delete w;
}


void AssimpConverter::LoadMesh()
{
	ResetMeshs();

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(meshPath, aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality);
	assert(scene != NULL);

	NodeHeirarchy(scene->mRootNode, -1);
	InitBones(scene);
	InitMesh(scene);

	if (scene->HasAnimations())
	{
		animName = name;
		AddAnimation(scene);
	}
}

void AssimpConverter::SaveMesh()
{
	Path::CreateFolders(Path::GetDirectoryName(outPath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(outPath + ".mesh");

	w->UInt((UINT)hierarchyNodes.size());
	for (auto node : hierarchyNodes)
	{
		w->String(node.name);
		w->BYTE(&node.translate, sizeof(XMFLOAT3));
		w->BYTE(&node.preQuaternion, sizeof(XMFLOAT4));
		w->BYTE(&node.quaternion, sizeof(XMFLOAT4));
		w->BYTE(&node.scale, sizeof(XMFLOAT3));
		w->BYTE(&node.local, sizeof(XMMATRIX));
		w->BYTE(&node.world, sizeof(XMMATRIX));
		w->BYTE(&node.offset, sizeof(XMMATRIX));
		w->Int(node.parentID);
	}

	w->UInt((UINT)meshs.size());
	for (auto mesh : meshs)
	{
		w->UInt(mesh.ID);
		w->UInt(mesh.matrialID);
		w->UInt((UINT)mesh.vertices.size());
		w->BYTE(mesh.vertices.data(), sizeof(VertexType) * (UINT)mesh.vertices.size());
		w->UInt((UINT)mesh.indices.size());
		w->BYTE(mesh.indices.data(), sizeof(UINT) * (UINT)mesh.indices.size());
	}

	delete w;

	Xml::XMLDocument* document = new Xml::XMLDocument();

	Xml::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Materials");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	for (auto mat : materials)
	{
		Xml::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		Xml::XMLElement* element = NULL;

		element = document->NewElement("Name");
		element->SetText(mat.name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", mat.diffuse.x);
		element->SetAttribute("G", mat.diffuse.y);
		element->SetAttribute("B", mat.diffuse.z);
		element->SetAttribute("A", 0);
		node->LinkEndChild(element);

		element = document->NewElement("Ambient");
		element->SetAttribute("R", mat.ambient.x);
		element->SetAttribute("G", mat.ambient.y);
		element->SetAttribute("B", mat.ambient.z);
		element->SetAttribute("A", 0);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", mat.specular.x);
		element->SetAttribute("G", mat.specular.y);
		element->SetAttribute("B", mat.specular.z);
		element->SetAttribute("A", 0);
		node->LinkEndChild(element);

		element = document->NewElement("Emissive");
		element->SetAttribute("R", mat.emissive.x);
		element->SetAttribute("G", mat.emissive.y);
		element->SetAttribute("B", mat.emissive.z);
		element->SetAttribute("A", 0);
		node->LinkEndChild(element);

		element = document->NewElement("Tranparent");
		element->SetAttribute("R", mat.tranparent.x);
		element->SetAttribute("G", mat.tranparent.y);
		element->SetAttribute("B", mat.tranparent.z);
		element->SetAttribute("A", 0);
		node->LinkEndChild(element);

		element = document->NewElement("Reflective");
		element->SetAttribute("R", mat.reflective.x);
		element->SetAttribute("G", mat.reflective.y);
		element->SetAttribute("B", mat.reflective.z);
		element->SetAttribute("A", 0);
		node->LinkEndChild(element);

		element = document->NewElement("Opacity");
		element->SetText(mat.opacity);
		node->LinkEndChild(element);

		element = document->NewElement("TransparentFactor");
		element->SetText(mat.transparentfactor);
		node->LinkEndChild(element);

		element = document->NewElement("Bumpscaling");
		element->SetText(mat.bumpscaling);
		node->LinkEndChild(element);

		element = document->NewElement("Shininess");
		element->SetText(mat.shininess);
		node->LinkEndChild(element);

		element = document->NewElement("Reflectivity");
		element->SetText(mat.reflectivity);
		node->LinkEndChild(element);

		element = document->NewElement("Shininessstrength");
		element->SetText(mat.shininessstrength);
		node->LinkEndChild(element);

		element = document->NewElement("Refracti");
		element->SetText(mat.refracti);
		node->LinkEndChild(element);


		element = document->NewElement("Diffusefile");
		element->SetText(mat.diffusefile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Specularfile");
		element->SetText(mat.specularfile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambientfile");
		element->SetText(mat.ambientfile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Emissivefile");
		element->SetText(mat.emissivefile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Heightfile");
		element->SetText(mat.heightfile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Normalfile");
		element->SetText(mat.normalfile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Shininessfile");
		element->SetText(mat.shininessfile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Opacityfile");
		element->SetText(mat.opacityfile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Displacementfile");
		element->SetText(mat.displacementfile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("LightMapfile");
		element->SetText(mat.lightMapfile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Reflectionfile");
		element->SetText(mat.reflectionfile.c_str());
		node->LinkEndChild(element);

		w->String(mat.basecolorfile);
		w->String(mat.normalcamerafile);
		w->String(mat.emissioncolorfile);
		w->String(mat.metalnessfile);
		w->String(mat.diffuseroughnessfile);
		w->String(mat.ambientocculsionfile);

		element = document->NewElement("Basecolorfile");
		element->SetText(mat.basecolorfile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Normalcamerafile");
		element->SetText(mat.normalcamerafile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Emissioncolorfile");
		element->SetText(mat.emissioncolorfile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Metalnessfile");
		element->SetText(mat.metalnessfile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Diffuseroughnessfile");
		element->SetText(mat.diffuseroughnessfile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambientocculsionfile");
		element->SetText(mat.ambientocculsionfile.c_str());
		node->LinkEndChild(element);
	}

	document->SaveFile((outPath + ".material").c_str());
	delete document;
}

void AssimpConverter::ResetMeshs()
{
	vector<HierarchyNode>().swap(hierarchyNodes);
	//map<string,UINT>().swap(hierarchyMap);
	hierarchyMap.clear();
	//bones.clear();
	//boneMap.clear();
	for (Mesh& i : meshs)
	{
		vector<VertexType>().swap(i.vertices);
		vector<UINT>().swap(i.indices);
	}
	vector<Mesh>().swap(meshs);
	vector<Material>().swap(materials);
}

void AssimpConverter::NodeHeirarchy(const aiNode* node, int value)
{
	string nodeName(node->mName.data);
	int parentID = value;
	size_t idx = nodeName.find("_$AssimpFbx$_");
	string subName;
	if (string::npos != idx)
	{
		subName = nodeName.substr(0, idx);
		if (hierarchyMap.count(subName) == 0)
		{
			HierarchyNode newNode;
			newNode.name = subName;
			newNode.parentID = value;
			hierarchyNodes.emplace_back(newNode);
			parentID = (int)hierarchyNodes.size() - 1;
			for (UINT i = 0; i < node->mNumMeshes; i++)
			{
				meshMap[node->mMeshes[i]] = (UINT)parentID;
			}
			hierarchyMap.emplace(subName, parentID);
		}
		XMVECTOR trans, quat, scale;
		XMMatrixDecompose(&scale, &quat, &trans, XMMatrixTranspose(XMMATRIX(node->mTransformation[0])));
		if (nodeName.find("$_Translation") > idx && string::npos != nodeName.find("$_Translation"))
			XMStoreFloat4(&hierarchyNodes[hierarchyMap[subName]].translate, trans);
		if (nodeName.find("$_PreRotation") > idx && string::npos != nodeName.find("$_PreRotation"))
			XMStoreFloat4(&hierarchyNodes[hierarchyMap[subName]].preQuaternion, quat);
		if (nodeName.find("$_Rotation") > idx && string::npos != nodeName.find("$_Rotation"))
			XMStoreFloat4(&hierarchyNodes[hierarchyMap[subName]].quaternion, quat);
		if (nodeName.find("$_Scaling") > idx && string::npos != nodeName.find("$_Scaling"))
			XMStoreFloat4(&hierarchyNodes[hierarchyMap[subName]].scale, scale);
	}
	else
	{
		if(hierarchyMap.count(nodeName) == 0)
		{
			HierarchyNode newNode;
			newNode.name = nodeName;
			newNode.parentID = value;
			XMVECTOR trans, quat, scale;
			XMMatrixDecompose(&scale, &quat, &trans, XMMatrixTranspose(XMMATRIX(node->mTransformation[0])));
			XMStoreFloat4(&newNode.translate, trans);
			XMStoreFloat4(&newNode.quaternion, quat);
			XMStoreFloat4(&newNode.scale, scale);
			hierarchyNodes.emplace_back(newNode);
			parentID = (int)hierarchyNodes.size() - 1;
			for (UINT i = 0; i < node->mNumMeshes; i++)
			{
				meshMap[node->mMeshes[i]] = (UINT)parentID;
			}
			hierarchyMap.emplace(nodeName, parentID);
		}
		else
		{
			XMVECTOR trans, quat, scale, pretrans, prequat, prescale;
			XMMatrixDecompose(&scale, &quat, &trans, XMMatrixTranspose(XMMATRIX(node->mTransformation[0])));
			pretrans = XMLoadFloat4(&hierarchyNodes[hierarchyMap[nodeName]].translate);
			prequat = XMLoadFloat4(&hierarchyNodes[hierarchyMap[nodeName]].quaternion);
			prescale = XMLoadFloat4(&hierarchyNodes[hierarchyMap[nodeName]].scale);
			XMStoreFloat4(&hierarchyNodes[hierarchyMap[nodeName]].translate, trans + pretrans);
			XMStoreFloat4(&hierarchyNodes[hierarchyMap[nodeName]].quaternion, XMQuaternionMultiply(quat, prequat));
			XMStoreFloat4(&hierarchyNodes[hierarchyMap[nodeName]].scale, scale * prescale);
			parentID = (int)hierarchyNodes.size() - 1;
		}
		hierarchyNodes[hierarchyMap[nodeName]].local = XMMatrixTransformation(XMVectorZero(), XMQuaternionIdentity(), 
			XMLoadFloat4(&hierarchyNodes[hierarchyMap[nodeName]].scale), XMVectorZero(), XMQuaternionMultiply(XMLoadFloat4(&hierarchyNodes[hierarchyMap[nodeName]].quaternion),
			XMLoadFloat4(&hierarchyNodes[hierarchyMap[nodeName]].preQuaternion)), XMLoadFloat4(&hierarchyNodes[hierarchyMap[nodeName]].translate));
		hierarchyNodes[hierarchyMap[nodeName]].world = hierarchyNodes[hierarchyMap[nodeName]].local;
		if (hierarchyNodes[hierarchyMap[nodeName]].parentID > -1)
		{
			hierarchyNodes[hierarchyMap[nodeName]].world *= hierarchyNodes[hierarchyNodes[hierarchyMap[nodeName]].parentID].world;
		}
	}

	for (UINT i = 0; i < node->mNumChildren; i++) {
		NodeHeirarchy(node->mChildren[i], parentID);
	}
}

//���̾��Ű�� �޽��� ����Ǵ� ���� �ִ� ��� �޽����� �ٸ� �����¸�Ʈ������ �޽��� ���� �ι��� ��Ʈ������ ����ؼ� 1���� ���������� �����
void AssimpConverter::InitBones(const aiScene* scene)
{
	for (UINT m = 0; m < scene->mNumMeshes; m++)
	{
		const aiMesh* mesh = scene->mMeshes[m];

		if (mesh->mNumBones == 0 || mesh->mNumVertices == 0)
			continue;

		XMMATRIX invWorld = XMMatrixIdentity();
		if (meshMap.count(m) > 0)
			invWorld = XMMatrixInverse(NULL, hierarchyNodes[meshMap[m]].world);

		for (UINT i = 0; i < mesh->mNumBones; i++) {
			string name(mesh->mBones[i]->mName.data);

			if (hierarchyMap.count(name) > 0)
			{
				//hierarchyNodes[hierarchyMap[name]].isBone = 1;
				hierarchyNodes[hierarchyMap[name]].offset = XMMatrixMultiply(invWorld, XMMatrixTranspose(XMMATRIX(mesh->mBones[i]->mOffsetMatrix[0])));
			}
		}
	}
}

void AssimpConverter::InitMesh(const aiScene* scene)
{
	for (UINT m = 0; m < scene->mNumMeshes; m++) {
		const aiMesh* mesh = scene->mMeshes[m];
		
		if (mesh->mNumVertices == 0 && meshMap.count(m) == 0)//�޽��̸���� ���̾��Ű�� �޽� ���̵�� ��ġ�ϴ°��� �ִ��� �з�
			continue;

		Mesh meshData;

		meshData.ID = meshMap[m];
		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{
			VertexType vertex;

			if (!mesh->mVertices)	break;
			memcpy(&vertex.position, &mesh->mVertices[i], sizeof(XMFLOAT3));

			if (mesh->mNormals)
				memcpy(&vertex.normal, &mesh->mNormals[i], sizeof(XMFLOAT3));
			

			if (mesh->mTangents)
				memcpy(&vertex.tangent, &mesh->mTangents[i], sizeof(XMFLOAT3));
			

			if (mesh->mTextureCoords[0])
				memcpy(&vertex.uv, &mesh->mTextureCoords[0][i], sizeof(XMFLOAT2));
			
			vertex.weight = { 0.f, 0.f, 0.f, 0.f };
			vertex.boneid.x = 0;
			vertex.boneid.y = 0;
			vertex.boneid.z = 0;
			vertex.boneid.w = 0;

			meshData.vertices.emplace_back(vertex);
		}
		if (meshData.vertices.empty())
			continue;

		BindSkin(mesh, meshData.vertices);

		for (UINT i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (UINT j = 0; j < face.mNumIndices; j++) {
				meshData.indices.emplace_back((UINT)face.mIndices[j]);
			}
		}
		meshData.matrialID = InitMaterials(scene, mesh->mMaterialIndex);

		meshs.emplace_back(meshData);
	}
}

//���̾��Ű���� �ε����� �����̵� �ο�
void AssimpConverter::BindSkin(const aiMesh* mesh, vector<VertexType>& vertices)
{
	bool noWeight=true;

	for (UINT i = 0; i < mesh->mNumBones; i++) {
		string boneName(mesh->mBones[i]->mName.data);

		if (hierarchyMap.count(boneName) == 0)
			continue;
		
		for (UINT j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
			UINT vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;

			if (vertices[vertexID].weight.x == 0) {
				noWeight = false;
				vertices[vertexID].boneid.x = hierarchyMap[boneName];
				vertices[vertexID].weight.x = weight;
			}
			else if (vertices[vertexID].weight.y == 0) {
				vertices[vertexID].boneid.y = hierarchyMap[boneName];
				vertices[vertexID].weight.y = weight;
			}
			else if (vertices[vertexID].weight.z == 0) {
				vertices[vertexID].boneid.z = hierarchyMap[boneName];
				vertices[vertexID].weight.z = weight;
			}
			else if (vertices[vertexID].weight.w == 0) {
				vertices[vertexID].boneid.w = hierarchyMap[boneName];
				vertices[vertexID].weight.w = weight;
			}
		}
	}
	//��ֶ����� �� ��Ż����Ʈ 1�� ���߱�
	if (noWeight)
		return;
	for (VertexType v : vertices)
	{
		int wSize = 0;
		float wList[4] = { v.weight.x,v.weight.y,v.weight.z,v.weight.w };
		float sumWeight = 0.f;
		for (UINT i = 0; i < 4; i++)
		{
			if (wList[i] == 0) break;
			sumWeight += wList[i];
			wSize++;
		}
		float lastWeight = 0.f;
		for (int i = 0; i < wSize - 1; i++)
		{
			wList[i] = wList[i] / sumWeight;
			lastWeight += wList[i];
		}
		if (wSize > 0)
			wList[wSize - 1] = 1.f - lastWeight;
		v.weight.x = wList[0];
		v.weight.y = wList[1];
		v.weight.z = wList[2];
		v.weight.w = wList[3];
	}
}

//initmesh���� �޽��� �����ɶ� �ش� �޽��� ���͸����� ���� ��� �ߺ����� �ʰ� ���� �޽��� ���͸����� ���� ��츸 �����ؼ� ���� ���ʿ��� ���͸����� ����
UINT AssimpConverter::InitMaterials(const aiScene* scene, UINT index)
{
	if (scene->mNumMaterials == 0)
		return 0;

	//���͸����� ��������� ������ �޽����� �����̱⶧���� �������� ���͸��� ���̵� �������� ���Ѵ�
	auto nodeit = find_if(materials.begin(), materials.end(), [index](const Material mat)->bool { return mat.ID == index; });
	if (nodeit != materials.end())
		return (UINT)distance(materials.begin(), nodeit);

	Material newMat;
	aiString  texture_path;
	aiColor3D color;
	float value;

	string name(scene->mMaterials[index]->GetName().data);
	newMat.name = name;
	newMat.ID = index;

	scene->mMaterials[index]->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	string texPath(texture_path.C_Str());
	if (texPath.size() > 0)
	{
		newMat.diffusefile = SaveTexture(scene, texPath);
	}
	scene->mMaterials[index]->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	newMat.diffuse = { color[0], color[1], color[2] };

	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_SPECULAR, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.specularfile = SaveTexture(scene, texPath);
	}
	scene->mMaterials[index]->Get(AI_MATKEY_COLOR_SPECULAR, color);
	newMat.specular = { color[0], color[1], color[2] };

	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_AMBIENT, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.ambientfile = SaveTexture(scene, texPath);
	}
	scene->mMaterials[index]->Get(AI_MATKEY_COLOR_AMBIENT, color);
	newMat.ambient = { color[0], color[1], color[2] };

	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_EMISSIVE, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.emissivefile = SaveTexture(scene, texPath);
	}
	scene->mMaterials[index]->Get(AI_MATKEY_COLOR_EMISSIVE, color);
	newMat.emissive = { color[0], color[1], color[2] };

	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_HEIGHT, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.heightfile = SaveTexture(scene, texPath);
	}

	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_NORMALS, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.normalfile = SaveTexture(scene, texPath);
	}
	scene->mMaterials[index]->Get(AI_MATKEY_BUMPSCALING, value);
	newMat.bumpscaling = value;

	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_SHININESS, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.shininessfile = SaveTexture(scene, texPath);
	}
	scene->mMaterials[index]->Get(AI_MATKEY_SHININESS, value);
	newMat.shininess = value;
	scene->mMaterials[index]->Get(AI_MATKEY_SHININESS_STRENGTH, value);
	newMat.shininessstrength = value;

	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_OPACITY, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.opacityfile = SaveTexture(scene, texPath);
	}
	scene->mMaterials[index]->Get(AI_MATKEY_COLOR_TRANSPARENT, color);
	newMat.tranparent= { color[0], color[1], color[2] };

	scene->mMaterials[index]->Get(AI_MATKEY_OPACITY, value);
	newMat.opacity = value;
	scene->mMaterials[index]->Get(AI_MATKEY_TRANSPARENCYFACTOR, value);
	newMat.transparentfactor = value;

	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_DISPLACEMENT, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.displacementfile = SaveTexture(scene, texPath);
	}

	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_LIGHTMAP, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.lightMapfile = SaveTexture(scene, texPath);
	}

	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_REFLECTION, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.reflectionfile = SaveTexture(scene, texPath);
	}
	scene->mMaterials[index]->Get(AI_MATKEY_COLOR_REFLECTIVE, color);
	newMat.reflective = { color[0], color[1], color[2] };

	scene->mMaterials[index]->Get(AI_MATKEY_REFLECTIVITY, value);//�ݻ���
	newMat.reflectivity = value;
	scene->mMaterials[index]->Get(AI_MATKEY_REFRACTI, value);//������
	newMat.refracti = value;

	//maya PBR start - FXShader : Stingray PBS
	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_BASE_COLOR, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.basecolorfile = SaveTexture(scene, texPath);
	}

	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_NORMAL_CAMERA, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.normalcamerafile = SaveTexture(scene, texPath);
	}

	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_EMISSION_COLOR, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.emissioncolorfile = SaveTexture(scene, texPath);
	}

	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_METALNESS, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.metalnessfile = SaveTexture(scene, texPath);
	}

	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.diffuseroughnessfile = SaveTexture(scene, texPath);
	}

	texture_path.Clear();
	scene->mMaterials[index]->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texture_path, NULL, NULL, NULL, NULL, NULL);
	texPath = texture_path.C_Str();
	if (texPath.size() > 0)
	{
		newMat.ambientocculsionfile = SaveTexture(scene, texPath);
	}
	//maya PBR end

	materials.emplace_back(newMat);

	return (UINT)(materials.size() - 1);
}

string AssimpConverter::SaveTexture(const aiScene* scene, string file)
{
	if (file.length() == 0)
		return "";

	const aiTexture* texture = scene->GetEmbeddedTexture(file.c_str());

	string path = "";
	if (texture)
	{
		path = Path::GetDirectoryName(outPath) + "Textures/" + Path::GetFileNameWithoutExtension(file) + ".png";

		if (texture->mHeight < 1)
		{
			Path::CreateFolders(Path::GetDirectoryName(path));

			BinaryWriter w;
			w.Open(String::ToWString(path));
			w.BYTE(texture->pcData, texture->mWidth);
			w.Close();
		}
		else
		{
			Image image;

			image.width = texture->mWidth;
			image.height = texture->mHeight;
			image.pixels = reinterpret_cast<uint8_t*>(texture->pcData);
			image.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			image.rowPitch = image.width * 4;

			image.slicePitch = image.width * image.height * 4;

			Path::CreateFolders(Path::GetDirectoryName(path));

			SaveToWICFile(image, WIC_FLAGS_NONE, GetWICCodec(WIC_CODEC_PNG),
				String::ToWString(path).c_str());
		}
	}
	else
	{
		file = Path::GetDirectoryName(meshPath) +"/"+ file;
		String::Replace(&file, "\\", "/");
		if(!Path::ExistFile(file))
			return "";

		path = Path::GetDirectoryName(outPath) + "Textures/" + Path::GetFileName(file);

		Path::CreateFolders(Path::GetDirectoryName(path));

		CopyFileA(file.c_str(), path.c_str(), FALSE);
	}

	return path;
}

void AssimpConverter::ResetAnimations()
{
	for (auto i : animations)
	{
		for (auto j : i.keyframes)
		{
			vector<Key>().swap(j.keys);
		}
		vector<KeyFrame>().swap(i.keyframes);
	}
	vector<Animation>().swap(animations);
}

void AssimpConverter::AddAnimation(const aiScene* scene)
{
	for (UINT i = 0; i < scene->mNumAnimations; i++)
	{
		aiAnimation* anim = scene->mAnimations[i];

		Animation ani;

		ani.name = anim->mName.data;
		ani.tickPerSec = (float)anim->mTicksPerSecond;
		ani.duration = (float)anim->mDuration;

		UINT frameCount = (UINT)anim->mDuration + 1;//���� ���������� ������ �������� �𸣰ڴ� �෹�̼����� ���� ������ ���� floor�� frac���� ���� �ϱ⿡...Ȯ�� �ʿ�

		for (UINT t = 0; t < frameCount; t++)
		{
			KeyFrame keyframe;
			//vector<Key> keyframe;
			for (HierarchyNode i : hierarchyNodes)
			{
				Key key;
				keyframe.boneName = i.name;
				key.translate = i.translate;
				key.quaternion = i.quaternion;
				key.scale = i.scale;
				keyframe.keys.emplace_back(key);
			}
			for (UINT i = 0; i < anim->mNumChannels; i++)
			{
				const aiNodeAnim* nodeAnim = anim->mChannels[i];
				string nodeName(nodeAnim->mNodeName.data);


				if (hierarchyMap.count(nodeName) > 0)
				{
					UINT index = hierarchyMap[nodeName];
					CalcInterpolatedScaling(nodeAnim, t, keyframe.keys[index].scale);
					CalcInterpolatedQuaternion(nodeAnim, t, keyframe.keys[index].quaternion);
					CalcInterpolatedPosition(nodeAnim, t, keyframe.keys[index].translate);

					keyframe.keys[index].time = (float)t;
				}
				else {
					size_t idx = nodeName.find("_$AssimpFbx$_");
					if (string::npos != idx)
					{
						string subName = nodeName.substr(0, idx);
						//auto nodeit = find_if(hierarchyNodes.begin(), hierarchyNodes.end(), [subName](const HierarchyNode hNode)->bool { return hNode.name == subName; });
						//if (nodeit != hierarchyNodes.end())
						if (hierarchyMap.count(subName) > 0)
						{
							//UINT index = (UINT)distance(hierarchyNodes.begin(), nodeit);
							UINT index = hierarchyMap[subName];
							if (nodeName.find("$_Scaling") > idx && string::npos != nodeName.find("$_Scaling"))
								CalcInterpolatedScaling(nodeAnim, t, keyframe.keys[index].scale);

							if (nodeName.find("$_Rotation") > idx && string::npos != nodeName.find("$_Rotation"))
								CalcInterpolatedQuaternion(nodeAnim, t, keyframe.keys[index].quaternion);

							if (nodeName.find("$_Translation") > idx && string::npos != nodeName.find("$_Translation"))
								CalcInterpolatedPosition(nodeAnim, t, keyframe.keys[index].translate);
						}
					}
				}
			}
			ani.keyframes.emplace_back(keyframe);
		}
		
		animations.emplace_back(ani);
	}
}

void AssimpConverter::CalcInterpolatedPosition(const aiNodeAnim* nodeAnim, const UINT frame, XMFLOAT4& trans)
{
	if (nodeAnim->mNumPositionKeys == 0)
		return;
	if (nodeAnim->mNumPositionKeys == 1)
	{
		memcpy_s(&trans, sizeof(trans), &nodeAnim->mPositionKeys[0].mValue, sizeof(nodeAnim->mPositionKeys[0].mValue));
		return;
	}

	UINT index;
	for (index = 0; index < nodeAnim->mNumPositionKeys - 1; )
	{
		if (frame < nodeAnim->mPositionKeys[index + 1].mTime)
			break;
		index++;
	}
	if (index > nodeAnim->mNumPositionKeys - 1)
	{
		memcpy_s(&trans, sizeof(trans), &nodeAnim->mPositionKeys[nodeAnim->mNumPositionKeys - 1].mValue, sizeof(nodeAnim->mPositionKeys[nodeAnim->mNumPositionKeys - 1].mValue));
		return;
	}

	float Factor = ((float)frame - (float)nodeAnim->mPositionKeys[index].mTime) / (float)(nodeAnim->mPositionKeys[index + 1].mTime - nodeAnim->mPositionKeys[index].mTime);
	Factor = min(max(Factor, 0.0f), 1.0f);
	XMVECTOR Start, End;
	memcpy_s(&Start, sizeof(Start), &nodeAnim->mPositionKeys[index].mValue, sizeof(nodeAnim->mPositionKeys[index].mValue));
	memcpy_s(&End, sizeof(End), &nodeAnim->mPositionKeys[index + 1].mValue, sizeof(nodeAnim->mPositionKeys[index + 1].mValue));
	XMStoreFloat4(&trans, XMVectorLerp(Start, End, Factor));
}

void AssimpConverter::CalcInterpolatedQuaternion(const aiNodeAnim* nodeAnim, const UINT frame, XMFLOAT4& quat)
{
	if (nodeAnim->mNumRotationKeys == 0)
		return;
	if (nodeAnim->mNumRotationKeys == 1)
	{
		quat = { nodeAnim->mRotationKeys[0].mValue.x, nodeAnim->mRotationKeys[0].mValue.y, nodeAnim->mRotationKeys[0].mValue.z, nodeAnim->mRotationKeys[0].mValue.w };
		return;
	}

	UINT index;
	for (index = 0; index < nodeAnim->mNumRotationKeys - 1; )
	{
		if (frame < nodeAnim->mRotationKeys[index + 1].mTime)
			break;
		index++;
	}
	if (index >= nodeAnim->mNumRotationKeys - 1)
	{
		quat = { nodeAnim->mRotationKeys[nodeAnim->mNumRotationKeys - 1].mValue.x,
			nodeAnim->mRotationKeys[nodeAnim->mNumRotationKeys - 1].mValue.y,
			nodeAnim->mRotationKeys[nodeAnim->mNumRotationKeys - 1].mValue.z,
			nodeAnim->mRotationKeys[nodeAnim->mNumRotationKeys - 1].mValue.w };
		return;
	}

	float Factor = ((float)frame - (float)nodeAnim->mRotationKeys[index].mTime) / (float)(nodeAnim->mRotationKeys[index + 1].mTime - nodeAnim->mRotationKeys[index].mTime);
	Factor = min(max(Factor, 0.0f), 1.0f);
	XMVECTOR Start, End;
	Start = { nodeAnim->mRotationKeys[index].mValue.x,
		nodeAnim->mRotationKeys[index].mValue.y,
		nodeAnim->mRotationKeys[index].mValue.z,
		nodeAnim->mRotationKeys[index].mValue.w };
	End = { nodeAnim->mRotationKeys[index + 1].mValue.x,
		nodeAnim->mRotationKeys[index + 1].mValue.y,
		nodeAnim->mRotationKeys[index + 1].mValue.z,
		nodeAnim->mRotationKeys[index + 1].mValue.w };
	XMStoreFloat4(&quat, XMQuaternionSlerp(Start, End, Factor));
}

void AssimpConverter::CalcInterpolatedScaling(const aiNodeAnim* nodeAnim, const UINT frame, XMFLOAT4& scale)
{
	if (nodeAnim->mNumScalingKeys == 0)
		return;
	if (nodeAnim->mNumScalingKeys == 1)
	{
		memcpy_s(&scale, sizeof(scale), &nodeAnim->mScalingKeys[0].mValue, sizeof(nodeAnim->mScalingKeys[0].mValue));
		return;
	}

	UINT index;
	for (index = 0; index < nodeAnim->mNumScalingKeys - 1; )
	{
		if (frame < nodeAnim->mScalingKeys[index + 1].mTime)
			break;
		index++;
	}
	if (index >= nodeAnim->mNumScalingKeys - 1)
	{
		memcpy_s(&scale, sizeof(scale), &nodeAnim->mScalingKeys[nodeAnim->mNumScalingKeys - 1].mValue, sizeof(nodeAnim->mScalingKeys[nodeAnim->mNumScalingKeys - 1].mValue));
		return;
	}

	float Factor = ((float)frame - (float)nodeAnim->mScalingKeys[index].mTime) / (float)(nodeAnim->mScalingKeys[index + 1].mTime - nodeAnim->mScalingKeys[index].mTime);
	Factor = min(max(Factor, 0.0f), 1.0f);
	XMVECTOR Start, End;
	memcpy_s(&Start, sizeof(Start), &nodeAnim->mScalingKeys[index].mValue, sizeof(nodeAnim->mScalingKeys[index].mValue));
	memcpy_s(&End, sizeof(End), &nodeAnim->mScalingKeys[index + 1].mValue, sizeof(nodeAnim->mScalingKeys[index + 1].mValue));
	XMStoreFloat4(&scale, XMVectorLerp(Start, End, Factor));
}
