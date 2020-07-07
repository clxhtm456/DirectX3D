#pragma once


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
	Terrain* terrain;
	vector<class Mesh*> meshes;

	Scene* m_scene;

};