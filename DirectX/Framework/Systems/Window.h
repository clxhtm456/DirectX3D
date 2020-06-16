#pragma once

class Window
{
public:
	static WPARAM Run(class IExecute* main);

private:
	static void Create();
	static void Destroy();

	static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

	static void MainRender();

private:
	static vector<class Node*> releaseList;
public:
	static void AddReleaseList(Node* b);
	static void ReleasePoolClear();

private:
	static class IExecute* mainExecute;
};