#pragma once
#include "stdafx.h"
#include "FbxLoader.h"
#include <future>

namespace Utility
{
	class AssetLoader
	{
	public:
		static AssetLoader& GetLoader()
		{
			static AssetLoader l_loader;
			return l_loader;
		}

		__forceinline void LoadFbx(const std::string p_fileName)
		{
			m_fbxLoader->LoadSceneFromFile(p_fileName);
		};

		FbxLoader* m_fbxLoader;

		~AssetLoader();

	private:
		AssetLoader();


	};
}