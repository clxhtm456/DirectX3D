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
	delete converter;
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
	FbxNodeAttribute* attribute = pNode->GetNodeAttribute();

	if (attribute != nullptr)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();

		bool b = false;
		b |= (nodeType == FbxNodeAttribute::eSkeleton);
		b |= (nodeType == FbxNodeAttribute::eMesh);
		b |= (nodeType == FbxNodeAttribute::eNull);
		b |= (nodeType == FbxNodeAttribute::eMarker);

		if (b)
		{
			FbxBoneData* bone = new FbxBoneData();
			bone->index = index;
			bone->parent = parent;
			bone->name = pNode->GetName();

			bone->local = FbxUtility::ToMatrix(pNode->EvaluateLocalTransform());
			bone->global = FbxUtility::ToMatrix(pNode->EvaluateGlobalTransform());

			bones.push_back(bone);

			if (nodeType == FbxNodeAttribute::eMesh)
			{
				converter->Triangulate(attribute, true, true);
				ReadMesh(pNode, index);
			}
		}
	}

	for (UINT i = 0; i < pNode->GetChildCount(); i++)
		ReadBone(pNode->GetChild(i), bones.size(), index);
}
//
void XmlExtractor::ReadSkin()
{
	int index = 0;
	for (FbxMeshData* data : meshes)
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
			FbxSurfaceMaterial* material = m_scene->GetMaterial(i);
			string materialName = material->GetName();

			vector<FbxVertex*> gather;
			for (FbxVertex* vertex : data->vertices)
			{
				if (vertex->materialName == materialName)
					gather.push_back(vertex);
			}
			if (gather.size() == 0)
				continue;

			/*FbxMeshPartData* meshPart = new FbxMeshPartData();
			meshPart->materialName = materialName;*/

			/*for (FbxVertex* temp : gather)
			{
				ModelVertexType vertex;
				vertex = temp->vertex;

				meshPart->vertices.push_back(vertex);
				meshPart->indices.push_back(meshPart->indices.size());
			}*/

			data->meshParts[index]->materialName = materialName;
		}
	}
	
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

	converter = new FbxGeometryConverter(m_sdkManager);

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
		w->Int(bone->index);
		w->String(bone->name);
		w->Int(bone->parent);

		w->Matrix(bone->local);
		w->Matrix(bone->global);

		delete bone;
	}

	w->UInt(meshes.size());
	for (FbxMeshData* data : meshes)
	{
		w->String(data->name);
		w->Int(data->parentBone);

		w->UInt(data->meshParts.size());
		for (FbxMeshPartData* part : data->meshParts)
		{
			w->String(part->materialName);

			w->UInt(part->vertices.size());
			w->BYTE(part->vertices.data(), sizeof(ModelVertexType) * part->vertices.size());

			w->UInt(part->indices.size());
			w->BYTE(part->indices.data(), sizeof(UINT) * part->indices.size());

			delete part;
		}
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
	if (node->GetMesh() == nullptr) return;

	FbxMeshData* meshData = new FbxMeshData();
	fbxsdk::FbxMesh* srcMesh = node->GetMesh();
	const int lVertexCount = srcMesh->GetControlPointsCount();
	const int lPolygonCount = srcMesh->GetPolygonCount();

	int vertexCounter = 0;

	meshData->name = node->GetName();
	meshData->parentBone = parentBone;

	meshData->mesh = srcMesh;

	vector<FbxMeshPartData*> meshPart;

	int count = m_scene->GetMaterialCount();
	for (int i = 0; i < m_scene->GetMaterialCount(); i++)
	{
		fbxsdk::FbxSurfaceMaterial* material = m_scene->GetMaterial(i);

		FbxVector4* lControlPoints = srcMesh->GetControlPoints();

		std::unordered_map<int, UINT> indexMapping;

		vector<FbxVertex*> vertices;
		
		FbxMeshPartData* meshPartData = new FbxMeshPartData();

		vector<Vector3> positions;
		for (UINT v = 0; v < lVertexCount; v++)
		{
			Vector3 position;

			position.x = static_cast<float>(lControlPoints[v].Buffer()[0]);
			position.y = static_cast<float>(lControlPoints[v].Buffer()[1]);
			position.z = static_cast<float>(lControlPoints[v].Buffer()[2]);

			positions.push_back(position);
		}

		for (UINT lPolygonIndex = 0; lPolygonIndex < lPolygonCount; lPolygonIndex++)
		{
			const int lVerticeCount = srcMesh->GetPolygonSize(lPolygonIndex);
			for (UINT lVerticeIndex = 0; lVerticeIndex < lVerticeCount; lVerticeIndex++)//3
			{
				int controlPointIndex = srcMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);

				UINT indexBuffer;

				FbxVertex* vertex = new FbxVertex();
				

				vertex->vertex.Position = positions[controlPointIndex];
				
				ReadNormal(srcMesh, controlPointIndex, vertexCounter, vertex->vertex.Normal);

				//ReadTangent(srcMesh, controlPointIndex, vertexCounter, vertex.Tangent);

				//ReadUv(srcMesh, controlPointIndex, srcMesh->GetTextureUVIndex(lPolygonIndex, lVerticeIndex), vertex->vertex.Uv);
				int uvIndex = srcMesh->GetTextureUVIndex(lPolygonIndex, lVerticeIndex);
				vertex->vertex.Uv = FbxUtility::GetUV(srcMesh, controlPointIndex, uvIndex);

				vertex->materialName = FbxUtility::GetMaterialName(srcMesh, lPolygonIndex, controlPointIndex);
				

				auto lookup = indexMapping.find(vertexCounter);
				if (lookup != indexMapping.end()) 
				{ 
					meshPartData->indices.push_back(lookup->second);
				}
				else 
				{ 
					unsigned int index = vertexCounter;
					indexMapping[vertexCounter] = index;
					meshPartData->indices.push_back(index);
					meshPartData->vertices.push_back(vertex->vertex);
				}

				vertexCounter++;
				vertices.push_back(vertex);
			}
			UINT index0 = vertices.size() - 3;
			UINT index1 = vertices.size() - 2;
			UINT index2 = vertices.size() - 1;

			ModelVertexType vertex0 = vertices[index0]->vertex;
			ModelVertexType vertex1 = vertices[index1]->vertex;
			ModelVertexType vertex2 = vertices[index2]->vertex;

			XMVECTOR p0 = XMVectorSet (vertex0.Position.x, vertex0.Position.y, vertex0.Position.z,0);
			XMVECTOR p1 = XMVectorSet (vertex1.Position.x, vertex1.Position.y, vertex1.Position.z,0);
			XMVECTOR p2 = XMVectorSet (vertex2.Position.x, vertex2.Position.y, vertex2.Position.z,0);

			Vector2 uv0 = vertex0.Uv;
			Vector2 uv1 = vertex1.Uv;
			Vector2 uv2 = vertex2.Uv;

			XMVECTOR e0 = p1 - p0;
			XMVECTOR e1 = p2 - p0;

			float u0 = uv1.x - uv0.x;
			float u1 = uv2.x - uv0.x;
			float v0 = uv1.y - uv0.y;
			float v1 = uv2.y - uv0.y;

			float d = 1.0f / (u0 * v1 - v0 * u1);

			XMVECTOR tangent;
			tangent = (v1 * e0 - v0 * e1) * d;

			XMStoreFloat3(&vertices[index0]->vertex.Tangent,(XMLoadFloat3(&vertices[index0]->vertex.Tangent) + tangent));
			XMStoreFloat3(&vertices[index1]->vertex.Tangent, (XMLoadFloat3(&vertices[index1]->vertex.Tangent) + tangent));
			XMStoreFloat3(&vertices[index2]->vertex.Tangent, (XMLoadFloat3(&vertices[index2]->vertex.Tangent) + tangent));
		}

		for (FbxVertex* vertex : vertices)
		{
			XMVECTOR t = XMVectorSet(vertex->vertex.Tangent.x, vertex->vertex.Tangent.y, vertex->vertex.Tangent.z,0);
			XMVECTOR n = XMVectorSet(vertex->vertex.Normal.x, vertex->vertex.Normal.y, vertex->vertex.Normal.z, 0);

			XMVECTOR temp = (t - n * XMVector2Dot(n, t));
			temp = XMVector3Normalize(temp);

			XMStoreFloat3(&vertex->vertex.Tangent, temp);
		}
		meshData->vertices = vertices;
		meshPart.push_back(meshPartData);
		meshData->meshParts = meshPart;
	}
	meshes.push_back(meshData);
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

void XmlExtractor::ReadPosition(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Vector3& outNormal)
{
}

void XmlExtractor::ReadNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Vector3& outNormal)
{
	if (inMesh->GetElementNormalCount() < 1)
	{
		return;
		//throw std::exception("Invalid Normal Number");
	}

	FbxGeometryElementNormal* vertexNormal = inMesh->GetElementNormal(0);
	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inCtrlPointIndex);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inVertexCounter);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void XmlExtractor::ReadTangent(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Vector3& outNormal)
{
	if (inMesh->GetElementTangentCount() < 1)
	{
		return;
		//throw std::exception("Invalid Tangent Number");
	}

	FbxGeometryElementTangent* vertexTangent = inMesh->GetElementTangent(0);
	switch (vertexTangent->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outNormal.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outNormal.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(inCtrlPointIndex);
			outNormal.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outNormal.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outNormal.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(inVertexCounter);
			outNormal.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void XmlExtractor::ReadUv(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Vector2& outNormal)
{
	if (inMesh->GetElementUVCount() < 1)
	{
		throw std::exception("Invalid Uv Number");
	}

	FbxGeometryElementUV* leUV = inMesh->GetElementUV(0);

	FbxVector2 l_uv;
	switch (leUV->GetMappingMode())
	{
	default:
		break;
	case FbxGeometryElement::eByControlPoint:
		switch (leUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			l_uv = leUV->GetDirectArray().GetAt(inCtrlPointIndex);
			outNormal.x = float(l_uv.mData[0]);
			outNormal.y = float(l_uv.mData[1]);

			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = leUV->GetIndexArray().GetAt(inCtrlPointIndex);
			l_uv = leUV->GetDirectArray().GetAt(id);
			outNormal.x = float(l_uv.mData[0]);
			outNormal.y = float(l_uv.mData[1]);
		}
		break;
		default:
			break; // other reference modes not shown here!
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		int lTextureUVIndex = inVertexCounter;
		switch (leUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			l_uv = leUV->GetDirectArray().GetAt(lTextureUVIndex);
			outNormal.x = float(l_uv.mData[0]);
			outNormal.y = float(l_uv.mData[1]);
		}
		break;
		default:
			break; // other reference modes not shown here!
		}
	}
	break;

	case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
	case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
	case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
		break;
	}
}

UINT XmlExtractor::GetBoneIndexByName(string name)
{
	for (UINT i = 0; i < bones.size(); i++)
	{
		if (bones[i]->name == name)
			return i;
	}

	return -1;
}
