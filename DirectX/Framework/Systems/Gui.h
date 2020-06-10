#pragma once

struct GuiText
{
	XMFLOAT2 Position;
	XMCOLOR Color;
	string Content;

	GuiText()
	{

	}
};

class Gui
{
public:
	static void Create();
	static void Delete();

	static Gui* Get();

	LRESULT MsgProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	void Resize();

	void Update();
	void Render();

	void AddWidget(class Widget* widget);

	void RenderText(GuiText& text);
	void RenderText(float x, float y, string content);
	void RenderText(float x, float y, float r, float g, float b, string content);
	void RenderText(XMFLOAT2 position, XMCOLOR color, string content);

private:
	Gui();
	~Gui();

private:
	void ApplyStyle();
	void DockingPannel();

private:
	static Gui* instance;
	vector<class Widget *> widgets;

	vector<GuiText> texts;
};