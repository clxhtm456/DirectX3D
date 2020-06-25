#include "stdafx.h"
#include "AssetLoader.h"
#define STB_IMAGE_IMPLEMENTATION


Utility::AssetLoader::AssetLoader():
    m_fbxLoader(new FbxLoader)
{
}

Utility::AssetLoader::~AssetLoader()
{
    
}
