#pragma once



class MapEditor
{
public:
	MapEditor(Scene* scene);
	~MapEditor();
public:
	void Update();//imgui 맵 로딩
	void Render();
public:
	void LoadMapDialog();
	void LoadMapFile(wstring fileDir);//맵파일 열기
	void CreateNewMapFile(wstring fileDir);//새로운 맵파일 생성

	void SaveMapDialog();
	void SaveMapFile();

	void CreateNewMap();

private:
	Terrain* terrain;
	vector<class Mesh*> meshes;

	Scene* m_scene;

};