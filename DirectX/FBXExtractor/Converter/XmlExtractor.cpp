#include "stdafx.h"
#include "XmlExtractor.h"
#include "FbxUtility.h"
#include "Model/FbxType.h"

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

void XmlExtractor::Reset()
{
	m_scene = nullptr;

	if (bones.size() > 0)
	{
		for (auto bone : bones)
		{
			delete bone;
		}
		bones.clear();
	}

	if (meshes.size() > 0)
	{
		for (auto mesh : meshes)
		{
			delete mesh;
		}
		meshes.clear();
	}
}

XmlExtractor* XmlExtractor::Create()
{
	XmlExtractor* extractor = new XmlExtractor();

	return extractor;
}

XmlExtractor::XmlExtractor():
m_sdkManager(FbxManager::Create()),
m_cameraArray(),
m_poseArray(),
m_supportVBO(true)
{
}

XmlExtractor::~XmlExtractor()
{
	m_scene->Destroy();
	m_sdkManager->Destroy();
	//m_importer->Destroy();
}

void XmlExtractor::InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	pManager = FbxManager::Create();
	if (!pManager)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}
	else
		FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);
	ios->WriteXMLFile("../../Test.xml");

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer());

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	pScene = FbxScene::Create(pManager, "My Scene");
	if (!pScene)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}
}

bool XmlExtractor::LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
{
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor, lSDKMinor, lSDKRevision;
	//int lFileFormat = -1;
	int lAnimStackCount;
	bool lStatus;
	char lPassword[1024];

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(pManager, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (!lImportStatus)
	{
		FbxString error = lImporter->GetStatus().GetErrorString();
		//FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
		//FBXSDK_printf("Error returned: %s\n\n", error.Buffer());

		if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
		{
			//FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			//FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	//FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		//FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.

		//FBXSDK_printf("Animation Stack Information\n");

		lAnimStackCount = lImporter->GetAnimStackCount();

		//FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
		//FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
		//FBXSDK_printf("\n");

		for (int i = 0; i < lAnimStackCount; i++)
		{
			FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			//FBXSDK_printf("    Animation Stack %d\n", i);
			//FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
			//FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the animation stack should be imported 
			// under a different name.
			//FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false if the animation stack should be not
			// be imported. 
			//FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
			//FBXSDK_printf("\n");
		}

		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
		IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED, false);
	}

	// Import the scene.
	lStatus = lImporter->Import(pScene);

	// Convert mesh, NURBS and patch into triangle mesh
	FbxGeometryConverter lGeomConverter(m_sdkManager);
	try {
		lGeomConverter.Triangulate(FbxCast<FbxScene>(pScene), /*replace*/true);
	}
	catch (std::runtime_error) {
		//FBXSDK_printf("Scene integrity verification failed.\n");
		return false;
	}
	if (lStatus == true)
	{

		// Check the scene integrity!
		FbxStatus status;
		FbxArray< FbxString*> details;
		FbxSceneCheckUtility sceneCheck(FbxCast<FbxScene>(pScene), &status, &details);
		lStatus = sceneCheck.Validate(FbxSceneCheckUtility::eCkeckData);
		bool lNotify = (!lStatus && details.GetCount() > 0) || (lImporter->GetStatus().GetCode() != FbxStatus::eSuccess);
		if (lNotify)
		{
			//FBXSDK_printf("\n");
			//FBXSDK_printf("********************************************************************************\n");
			if (details.GetCount())
			{
				//FBXSDK_printf("Scene integrity verification failed with the following errors:\n");
				for (int i = 0; i < details.GetCount(); i++)
					//FBXSDK_printf("   %s\n", details[i]->Buffer());

					FbxArrayDelete<FbxString*>(details);
			}

			if (lImporter->GetStatus().GetCode() != FbxStatus::eSuccess)
			{
				//FBXSDK_printf("\n");
				//FBXSDK_printf("WARNING:\n");
				//FBXSDK_printf("   The importer was able to read the file but with errors.\n");
				//FBXSDK_printf("   Loaded scene may be incomplete.\n\n");
				//FBXSDK_printf("   Last error message:'%s'\n", lImporter->GetStatus().GetErrorString());
			}
			//FBXSDK_printf("********************************************************************************\n");
			//FBXSDK_printf("\n");
		}
	}

	if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
	{
		//FBXSDK_printf("Please enter password: ");

		lPassword[0] = '\0';

		FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
			scanf("%s", lPassword);
		FBXSDK_CRT_SECURE_NO_WARNING_END

			FbxString lString(lPassword);

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(pScene);

		if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
		{
			//FBXSDK_printf("\nPassword is wrong, import aborted.\n");
		}
	}

	// Destroy the importer.
	m_importer = lImporter;

	return lStatus;
}

void XmlExtractor::ReadBone(FbxNode* pNode, int index, int parent)
{
	FbxNodeAttribute::EType lAttributeType;
	int i;

	if (pNode->GetNodeAttribute() == NULL)
	{
		//FBXSDK_printf("NULL Node Attribute\n\n");
	}
	else
	{
		lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

		bool b = false;
		b |= (lAttributeType == FbxNodeAttribute::eSkeleton);
		b |= (lAttributeType == FbxNodeAttribute::eMesh);
		b |= (lAttributeType == FbxNodeAttribute::eNull);
		b |= (lAttributeType == FbxNodeAttribute::eMarker);

		if (b)
		{
			FbxBoneData* bone = new FbxBoneData();
			bone->Index = index;
			bone->Parent = parent;
			bone->Name = pNode->GetName();

			Matrix transform = FbxUtility::ToMatrix(pNode->EvaluateGlobalTransform());
			bone->Transform = XMMatrixTranspose(transform);

			Matrix matParent;
			if (parent < 0)
				matParent = XMMatrixIdentity();
			else
				matParent = bones[parent]->Transform;
			bone->Transform = bone->Transform * matParent;

			bones.push_back(bone);

			if (lAttributeType == FbxNodeAttribute::eMesh)
			{
				FbxGeometryConverter lGeomConverter(m_sdkManager);
				try {
					lGeomConverter.Triangulate(pNode->GetNodeAttribute(), true, true);
				}
				catch (std::runtime_error) {
					FBXSDK_printf("Scene integrity verification failed.\n");
					return;
				}
				ReadMesh(pNode, index);
			}
		}
	}

	//DisplayUserProperties(pNode);
	//DisplayTarget(pNode);
	//DisplayPivotsAndLimits(pNode);
	//DisplayTransformPropagation(pNode);
	//DisplayGeometricTransform(pNode);
	for (UINT i = 0; i < pNode->GetChildCount(); i++)
		ReadBone(pNode->GetChild(i), bones.size(), index);
}
//
void XmlExtractor::ReadSkin()
{
	/*for (FbxMeshData* data : meshes)
	{
		FbxMesh* mesh = data->mesh;

		int deformerCount = mesh->GetDeformerCount();
		vector<FbxBoneWeights> boneWeights(mesh->GetControlPointsCount(), FbxBoneWeights());

		for (int i = 0; i < deformerCount; i++)
		{
			FbxDeformer* deformer = mesh->GetDeformer(i, FbxDeformer::eSkin);

			FbxSkin* skin = reinterpret_cast<FbxSkin*>(deformer);
			if (skin == nullptr)
				continue;

			for (int clusterIndex = 0; clusterIndex < skin->GetClusterCount(); clusterIndex++)
			{
				FbxCluster* cluster = skin->GetCluster(clusterIndex);

				string linkName = cluster->GetLink()->GetName();
				UINT boneIndex = GetBoneIndexByName(linkName);

				FbxAMatrix transform;
				FbxAMatrix linkTransform;

				cluster->GetTransformMatrix(transform);
				cluster->GetTransformLinkMatrix(linkTransform);

				bones[boneIndex]->local = FbxUtility::ToMatrix(transform);
				bones[boneIndex]->global = FbxUtility::ToMatrix(linkTransform);

				for (int indexCount = 0; indexCount < cluster->GetControlPointIndicesCount();
					indexCount++)
				{
					int temp = cluster->GetControlPointIndices()[indexCount];
					double* weights = cluster->GetControlPointWeights();

					boneWeights[temp].AddBoneWeight(boneIndex, (float)weights[indexCount]);
				}
			}
		}

		for (FbxBoneWeights& boneWeight : boneWeights)
			boneWeight.Normalize();

		for (FbxVertex* vertex : data->vertices)
		{
			int cpIndex = vertex->controlPoint;

			FbxBlendWeight weights;
			boneWeights[cpIndex].GetBlendWeiths(weights);
			vertex->vertex.BlendIndices = weights.indices;
			vertex->vertex.BlendWeights = weights.weights;
		}

		for (int i = 0; i < m_scene->GetMaterialCount(); i++)
		{
			fbxsdk::FbxSurfaceMaterial* material = m_scene->GetMaterial(i);
			string materialName = material->GetName();

			vector<FbxVertex*> gather;
			for (FbxVertex* vertex : data->vertices)
			{
				if (vertex->materialName == materialName)
					gather.push_back(vertex);
			}
			if (gather.size() == 0)
				continue;

			FbxMeshPartData* meshPart = new FbxMeshPartData();
			meshPart->materialName = materialName;

			for (FbxVertex* temp : gather)
			{
				ModelVertexType vertex;
				vertex = temp->vertex;

				meshPart->vertices.push_back(vertex);
				meshPart->indices.push_back(meshPart->indices.size());
			}

			data->meshParts.push_back(meshPart);
		}
	}*/
	////////////////////////////
	//for (UINT i = 0; i < scene->mNumMeshes; i++)
	//{
	//	aiMesh* aiMesh = scene->mMeshes[i];

	//	if (aiMesh->HasBones() == false) continue;

	//	asMesh* mesh = meshes[i];

	//	vector<asBoneWeights> boneWeighs;
	//	boneWeighs.assign(mesh->Vertices.size(), asBoneWeights());

	//	for (UINT b = 0; b < aiMesh->mNumBones; b++)
	//	{
	//		aiBone* aiBone = aiMesh->mBones[b];

	//		UINT boneIndex = 0;
	//		for (asBone* bone : bones)
	//		{
	//			if (bone->Name == (string)aiBone->mName.C_Str())
	//			{
	//				boneIndex = bone->Index;
	//				break;
	//			}
	//		}//for(bone)

	//		for (UINT w = 0; w < aiBone->mNumWeights; w++)
	//		{
	//			UINT index = aiBone->mWeights[w].mVertexId;
	//			float weight = aiBone->mWeights[w].mWeight;

	//			boneWeighs[index].AddWeights(boneIndex, weight);
	//		}//for(w)
	//	}//for(b)

	//	for (UINT w = 0; w < boneWeighs.size(); w++)
	//	{
	//		boneWeighs[w].Normalize();

	//		asBlendWeight blendWeights;
	//		boneWeighs[w].GetBlendWeighs(blendWeights);

	//		mesh->Vertices[w].BlendIndices = blendWeights.Indices;
	//		mesh->Vertices[w].BlendWeights = blendWeights.Weights;
	//	}
	//}
}

FbxAMatrix XmlExtractor::GetGeometry(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

FbxAMatrix XmlExtractor::GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition)
{
	FbxAMatrix lGlobalPosition;
	bool        lPositionFound = false;

	if (pPose)
	{
		int lNodeIndex = pPose->Find(pNode);

		if (lNodeIndex > -1)
		{
			// The bind pose is always a global matrix.
			// If we have a rest pose, we need to check if it is
			// stored in global or local space.
			if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
			{
				lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
			}
			else
			{
				// We have a local matrix, we need to convert it to
				// a global space matrix.
				FbxAMatrix lParentGlobalPosition;

				if (pParentGlobalPosition)
				{
					lParentGlobalPosition = *pParentGlobalPosition;
				}
				else
				{
					if (pNode->GetParent())
					{
						lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
					}
				}

				FbxAMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
				lGlobalPosition = lParentGlobalPosition * lLocalPosition;
			}

			lPositionFound = true;
		}
	}

	if (!lPositionFound)
	{
		// There is no pose entry for that node, get the current global position instead.

		// Ideally this would use parent global position and local position to compute the global position.
		// Unfortunately the equation 
		//    lGlobalPosition = pParentGlobalPosition * lLocalPosition
		// does not hold when inheritance type is other than "Parent" (RSrs).
		// To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
		lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
	}

	return lGlobalPosition;
}

FbxAMatrix XmlExtractor::GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
{
	FbxAMatrix lPoseMatrix;
    FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

    memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

    return lPoseMatrix;
}

void XmlExtractor::LoadModel(string filePath)
{
	string fileDir = "../../_Assets/Models/" + filePath + ".fbx";

	// Prepare the FBX SDK.
	InitializeSdkObjects(m_sdkManager, m_scene);
	// Load the scene.

	auto lResult = LoadScene(m_sdkManager, m_scene, fileDir.c_str());

	if (lResult == false)
	{
		FBXSDK_printf("\n\nAn error occurred while loading the scene...");
	}
	else
	{
		WriteMaterial( L"../../_Assets/Materials/" + String::ToWString(filePath) + L".mat", true);
		WriteMesh( L"../../_Assets/Meshes/" + String::ToWString(filePath) + L".mesh", true);
	}
	m_animStacks.clear();
}

void XmlExtractor::LoadAnimation(UINT index, string filePath)
{
	string fileDir = "../../_Assets/Models/" + filePath + ".fbx";

	// Prepare the FBX SDK.
	InitializeSdkObjects(m_sdkManager, m_scene);
	// Load the scene.

	auto lResult = LoadScene(m_sdkManager, m_scene, fileDir.c_str());

	if (lResult == false)
	{
		FBXSDK_printf("\n\nAn error occurred while loading the scene...");
	}
	else
	{
		FbxClip* clip = ReadAnimation(index);
		WriteClip(clip, L"../../_Assets/Clips/" + String::ToWString(filePath) + L".clip");
	}
	m_animStacks.clear();
}

void XmlExtractor::WriteMaterial(wstring savePath,bool bOverWrite)
{
	if (bOverWrite == false)
	{
		if (Path::ExistFile(savePath) == true)
			return;
	}

	int lMaterialCount = m_scene->GetMaterialCount();

	string folder = String::ToString(Path::GetDirectoryName(savePath));
	string file = String::ToString(Path::GetFileName(savePath));

	Path::CreateFolders(folder);

	Xml::XMLDocument* document = new Xml::XMLDocument();

	Xml::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Materials");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	for (int lCount = 0; lCount < lMaterialCount; lCount++)
	{
		Xml::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		Xml::XMLElement* element = NULL;
		
		fbxsdk::FbxSurfaceMaterial* lMaterial = m_scene->GetMaterial(lCount);
		FbxPropertyT<FbxDouble3> lKFbxDouble3;
		FbxColor theColor;

		element = document->NewElement("Name");
		element->SetText((char*)lMaterial->GetName());
		node->LinkEndChild(element);

		FbxProperty lProperty;
		
		//Diffuse Textures
		lProperty = lMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuse);
		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(lProperty).c_str());
		node->LinkEndChild(element);

		//Specular Textures
		lProperty = lMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sSpecular);
		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(lProperty).c_str());
		node->LinkEndChild(element);

		//Normal Textures
		lProperty = lMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sNormalMap);
		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(lProperty).c_str());
		node->LinkEndChild(element);

		if (lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId) || lMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{
			lKFbxDouble3 = ((FbxSurfaceLambert*)lMaterial)->Ambient;
			theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

			element = document->NewElement("Ambient");
			element->SetAttribute("R", theColor.mRed);
			element->SetAttribute("G", theColor.mGreen);
			element->SetAttribute("B", theColor.mBlue);
			element->SetAttribute("A", theColor.mAlpha);
			node->LinkEndChild(element);

			lKFbxDouble3 = ((FbxSurfaceLambert*)lMaterial)->Diffuse;
			theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
			element = document->NewElement("Diffuse");
			element->SetAttribute("R", theColor.mRed);
			element->SetAttribute("G", theColor.mGreen);
			element->SetAttribute("B", theColor.mBlue);
			element->SetAttribute("A", theColor.mAlpha);
			node->LinkEndChild(element);;

			lKFbxDouble3 = ((FbxSurfaceLambert*)lMaterial)->Emissive;
			theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);
			element = document->NewElement("Emissive");
			element->SetAttribute("R", theColor.mRed);
			element->SetAttribute("G", theColor.mGreen);
			element->SetAttribute("B", theColor.mBlue);
			element->SetAttribute("A", theColor.mAlpha);
			node->LinkEndChild(element);
		}
		else
		{
			element = document->NewElement("Ambient");
			element->SetAttribute("R", 1);
			element->SetAttribute("G", 1);
			element->SetAttribute("B", 1);
			element->SetAttribute("A", 1);
			node->LinkEndChild(element);

			element = document->NewElement("Diffuse");
			element->SetAttribute("R", 1);
			element->SetAttribute("G", 1);
			element->SetAttribute("B", 1);
			element->SetAttribute("A", 1);
			node->LinkEndChild(element);;

			element = document->NewElement("Emissive");
			element->SetAttribute("R", 0);
			element->SetAttribute("G", 0);
			element->SetAttribute("B", 0);
			element->SetAttribute("A", 0);
			node->LinkEndChild(element);
		}
		
	}

	document->SaveFile((folder + file).c_str());
	SafeDelete(document);
}

void XmlExtractor::WriteMesh(wstring savePath, bool bOverWrite)
{
	if (bOverWrite == false)
	{
		if (Path::ExistFile(savePath) == true)
			return;
	}

	Path::CreateFolders(Path::GetDirectoryName(savePath));

	FbxNode* lNode = m_scene->GetRootNode();

	if (lNode)
	{
		ReadBone(lNode, -1, -1);
		ReadSkin();
	}

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);

	w->UInt(bones.size());
	for (FbxBoneData* bone : bones)
	{
		w->Int(bone->Index);
		w->String(bone->Name);
		w->Int(bone->Parent);
		w->Matrix(bone->Transform);

		delete bone;
	}

	w->UInt(meshes.size());
	for (FbxMeshData* data : meshes)
	{
		w->String(data->Name);
		w->Int(data->BoneIndex);

		w->String(data->MaterialName);

		w->UInt(data->Vertices.size());
		w->BYTE(&data->Vertices[0], sizeof(ModelVertexType) * data->Vertices.size());

		w->UInt(data->Indices.size());
		w->BYTE(&data->Indices[0], sizeof(UINT) * data->Indices.size());
		delete data;
	}

	w->Close();
	SafeDelete(w);
}

void XmlExtractor::WriteClip(FbxClip* clip, wstring savePath)
{
	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);

	w->String(clip->name);
	w->Float(clip->duration);
	w->Float(clip->frameRate);
	w->Int(clip->frameCount);

	w->UInt(clip->keyFrames.size());
	for (FbxKeyFrame* frame : clip->keyFrames)
	{
		w->String(frame->boneName);

		w->UInt(frame->transforms.size());
		w->BYTE(frame->transforms.data(), sizeof(FbxKeyFrameData) * frame->transforms.size());

		delete frame;
	}

	w->Close();
	delete clip;
	delete w;
}

FbxClip* XmlExtractor::ReadAnimation(UINT index)
{
	UINT stackCount = (UINT)m_importer->GetAnimStackCount();

	FbxTime::EMode mode = m_scene->GetGlobalSettings().GetTimeMode();
	float frameRate = (float)FbxTime::GetFrameRate(mode);

	FbxClip* clip = new FbxClip();
	clip->frameRate = frameRate;

	FbxTakeInfo* takeInfo = m_importer->GetTakeInfo(index);
	clip->name = takeInfo->mName.Buffer();

	FbxTimeSpan span = takeInfo->mLocalTimeSpan;
	FbxTime tempDuration = span.GetDuration();

	int start = (int)span.GetStart().GetFrameCount();
	int end = (int)span.GetStop().GetFrameCount();

	float duration = (float)tempDuration.GetMilliSeconds();

	if (start < end)
		ReadKeyFrameData(clip, m_scene->GetRootNode(), start, end);

	clip->duration = duration;
	clip->frameCount = (end - start) + 1;

	return clip;
}

void XmlExtractor::ReadMesh(FbxNode* node, int parentBone)
{
	FbxMesh* mesh = node->GetMesh();

	for (int p = 0; p < mesh->GetPolygonCount(); p++)
	{
		for (int vi = 2; vi >= 0; vi--)
		{
			FbxVertex* vertex = new FbxVertex();

			int cpIndex = mesh->GetPolygonVertex(p, vi);
			vertex->controlPoint = cpIndex;

			FbxVector4 position = mesh->GetControlPointAt(cpIndex);
			Vector3 temp = FbxUtility::ToVector3(position);
			XMStoreFloat3(&vertex->vertex.Position, XMVector3TransformCoord(XMLoadFloat3(&temp), FbxUtility::Negative()));

			FbxVector4 normal;
			mesh->GetPolygonVertexNormal(p, vi, normal);
			normal.Normalize();
			temp = FbxUtility::ToVector3(normal);
			XMStoreFloat3(&vertex->vertex.Normal, XMVector3TransformCoord(XMLoadFloat3(&temp), FbxUtility::Negative()));

			vertex->materialName = FbxUtility::GetMaterialName(mesh, p, cpIndex);

			int uvIndex = mesh->GetTextureUVIndex(p, vi);
			vertex->vertex.Uv = FbxUtility::GetUV(mesh, cpIndex, uvIndex);

			vertices.push_back(vertex);
		}

		UINT index0 = vertices.size() - 3;
		UINT index1 = vertices.size() - 2;
		UINT index2 = vertices.size() - 1;

		ModelVertexType vertex0 = vertices[index0]->vertex;
		ModelVertexType vertex1 = vertices[index1]->vertex;
		ModelVertexType vertex2 = vertices[index2]->vertex;

		Vector3 p0 = vertex0.Position;
		Vector3 p1 = vertex1.Position;
		Vector3 p2 = vertex2.Position;

		Vector2 uv0 = vertex0.Uv;
		Vector2 uv1 = vertex1.Uv;
		Vector2 uv2 = vertex2.Uv;

		XMVECTOR e0 = XMLoadFloat3(&p1) - XMLoadFloat3(&p0);
		XMVECTOR e1 = XMLoadFloat3(&p2) - XMLoadFloat3(&p0);

		float u0 = uv1.x - uv0.x;
		float u1 = uv2.x - uv0.x;
		float v0 = uv1.y - uv0.y;
		float v1 = uv2.y - uv0.y;

		float d = 1.0f / (u0 * v1 - v0 * u1);

		XMVECTOR tangent;
		tangent = (v1 * e0 - v0 * e1) * d;

		XMStoreFloat3(&vertices[index0]->vertex.Tangent, XMLoadFloat3(&vertices[index0]->vertex.Tangent) + tangent);
		XMStoreFloat3(&vertices[index1]->vertex.Tangent, XMLoadFloat3(&vertices[index1]->vertex.Tangent) + tangent);
		XMStoreFloat3(&vertices[index2]->vertex.Tangent, XMLoadFloat3(&vertices[index2]->vertex.Tangent) + tangent);
	}

	for (FbxVertex* vertex : vertices)
	{
		XMVECTOR t = XMLoadFloat3(&vertex->vertex.Tangent);
		XMVECTOR n = XMLoadFloat3(&vertex->vertex.Normal);

		XMVECTOR temp = (t - n * XMVector3Dot(n, t));
		temp = XMVector3Normalize(temp);

		XMStoreFloat3(&vertex->vertex.Tangent, temp);
	}

	/*FbxMeshData* data = new FbxMeshData();
	data->name = node->GetName();
	data->parentBone = parentBone;
	data->vertices = vertices;
	data->mesh = mesh;
	meshes.push_back(data);*/
	/////////////////////////////
	if (node->GetMesh() == nullptr) return;

	FbxMeshData* meshData = new FbxMeshData();
	meshData->Name = node->GetName();
	meshData->BoneIndex = parentBone;

	FbxMesh* srcMesh = node->GetMesh();

	for (int i = 0; i < m_scene->GetMaterialCount(); i++)
	{
		fbxsdk::FbxSurfaceMaterial* material = m_scene->GetMaterial(i);
		meshData->MaterialName = material->GetName();

		UINT startVertex = meshData->Vertices.size();
		for (UINT v = 0; v < srcMesh->GetPolygonVertexCount(); v++)
		{
			for (int vi = 2; vi >= 0; vi--)
			{
				ModelVertexType vertex;

				FbxVertex* fvertex = new FbxVertex();
				int cpIndex = mesh->GetPolygonVertex(v, vi);
				fvertex->controlPoint = cpIndex;

				FbxVector4 position = mesh->GetControlPointAt(cpIndex);
				Vector3 temp = FbxUtility::ToVector3(position);
				XMStoreFloat3(&fvertex->vertex.Position, XMVector3TransformCoord(XMLoadFloat3(&temp), FbxUtility::Negative()));

				FbxVector4 normal;
				mesh->GetPolygonVertexNormal(v, vi, normal);
				normal.Normalize();
				temp = FbxUtility::ToVector3(normal);
				XMStoreFloat3(&fvertex->vertex.Normal, XMVector3TransformCoord(XMLoadFloat3(&temp), FbxUtility::Negative()));

				fvertex->materialName = FbxUtility::GetMaterialName(mesh, p, cpIndex);

				int uvIndex = mesh->GetTextureUVIndex(p, vi);
				fvertex->vertex.Uv = FbxUtility::GetUV(mesh, cpIndex, uvIndex);

				vertex.Position = 

				meshData->Vertices.push_back(fvertex->vertex);
				vertices.push_back(fvertex);
			}

			if (srcMesh->HasTextureCoords(0))
				memcpy(&vertex.Uv, &srcMesh->mTextureCoords[0][v], sizeof(Vector2));

			if (srcMesh->HasNormals())
				memcpy(&vertex.Normal, &srcMesh->mNormals[v], sizeof(Vector3));

			if (srcMesh->HasTangentsAndBitangents())
				memcpy(&vertex.Tangent, &srcMesh->mTangents[v], sizeof(Vector3));

			mesh->Vertices.push_back(vertex);

		}

		for (UINT f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; k++)
			{
				mesh->Indices.push_back(face.mIndices[k]);
				mesh->Indices.back() += startVertex;
			}
		}

		meshes.push_back(mesh);
	}
	
	/*srcMesh->GetPolygonVertexCount()
	UINT startVertex = mesh->Vertices.size();
	for (UINT v = 0; v < srcMesh->mNumVertices; v++)
	{
		Model::ModelVertex vertex;
		memcpy(&vertex.Position, &srcMesh->mVertices[v], sizeof(Vector3));

		if (srcMesh->HasTextureCoords(0))
			memcpy(&vertex.Uv, &srcMesh->mTextureCoords[0][v], sizeof(Vector2));

		if (srcMesh->HasNormals())
			memcpy(&vertex.Normal, &srcMesh->mNormals[v], sizeof(Vector3));

		if (srcMesh->HasTangentsAndBitangents())
			memcpy(&vertex.Tangent, &srcMesh->mTangents[v], sizeof(Vector3));

		mesh->Vertices.push_back(vertex);

	}

	for (UINT f = 0; f < srcMesh->mNumFaces; f++)
	{
		aiFace& face = srcMesh->mFaces[f];

		for (UINT k = 0; k < face.mNumIndices; k++)
		{
			mesh->Indices.push_back(face.mIndices[k]);
			mesh->Indices.back() += startVertex;
		}
	}

	meshes.push_back(mesh);*/
}

string XmlExtractor::WriteTexture(FbxProperty& pProperty)
{
	int lTextureCount = pProperty.GetSrcObjectCount<FbxTexture>();

	for (int j = 0; j < lTextureCount; ++j)
	{
		//Here we have to check if it's layeredtextures, or just textures:
		FbxLayeredTexture* lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
		if (lLayeredTexture)
		{
			int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
			for (int k = 0; k < lNbTextures; ++k)
			{
				FbxTexture* lTexture = lLayeredTexture->GetSrcObject<FbxTexture>(k);
				if (lTexture)
				{

					FbxLayeredTexture::EBlendMode lBlendMode;
					lLayeredTexture->GetTextureBlendMode(k, lBlendMode);
					FbxFileTexture* lFileTexture = FbxCast<FbxFileTexture>(lTexture);
					return (char*)lFileTexture->GetFileName();
				}

			}
		}
		else
		{
			//no layered texture simply get on the property
			FbxTexture* lTexture = pProperty.GetSrcObject<FbxTexture>(j);
			if (lTexture)
			{
				FbxFileTexture* lFileTexture = FbxCast<FbxFileTexture>(lTexture);
				return (char*)lFileTexture->GetFileName();
			}
		}
	}

	return "";
}


void XmlExtractor::ReadKeyFrameData(FbxClip* clip, FbxNode* node, int start, int end)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute != nullptr)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();

		if (nodeType == FbxNodeAttribute::eSkeleton)
		{
			FbxKeyFrame* keyFrame = new FbxKeyFrame();
			keyFrame->boneName = node->GetName();

			for (int i = start; i <= end; i++)
			{
				FbxTime animationTime;
				animationTime.SetFrame(i);

				FbxAMatrix matrix = node->EvaluateLocalTransform(animationTime);
				Matrix transform = FbxUtility::ToMatrix(matrix);

				FbxKeyFrameData data;
				data.time = (float)animationTime.GetMilliSeconds();

				XMVECTOR scale = XMLoadFloat3(&data.scale);
				XMVECTOR position = XMLoadFloat3(&data.position);

				XMMatrixDecompose(&scale, &data.rotation, &position, transform);
				XMStoreFloat3(&data.scale, scale);
				XMStoreFloat3(&data.position, position);
				keyFrame->transforms.push_back(data);
			}

			clip->keyFrames.push_back(keyFrame);
		}
	}

	for (int i = 0; i < node->GetChildCount(); i++)
		ReadKeyFrameData(clip, node->GetChild(i), start, end);
}
