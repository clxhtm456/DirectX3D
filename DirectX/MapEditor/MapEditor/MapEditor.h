#pragma once

#define DEFAULT_HORIZONTAL	512
#define DEFAULT_VERTICAL	512

class MapEditor
{
public:
	MapEditor(Scene* scene);
	~MapEditor();
public:
	void Update();//imgui �� �ε�
	void Render();
public:
	void LoadMapDialog();
	void LoadMapFile(wstring fileDir);//������ ����
	void CreateNewMapFile(wstring fileDir);//���ο� ������ ����

	void SaveMapDialog();
	void SaveMapFile();

	void CreateNewMap();

private:
	class Terrain* terrain;
	vector<class Mesh*> meshes;

	Scene* m_scene;

};