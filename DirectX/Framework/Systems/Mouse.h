#pragma once

#define MAX_INPUT_MOUSE 8

class Mouse
{
public:
	void SetHandle(HWND handle)
	{
		this->handle = handle;
	}

	static Mouse* Get();

	static void Create();
	static void Delete();

	void Update();

	LRESULT InputProc(UINT message, WPARAM wParam, LPARAM lParam);

	Vector3 GetPosition() 
	{ 
		Vector3 temp;
		XMStoreFloat3(&temp, position);
		return temp;
	}

	bool Down(DWORD button)
	{
		return buttonMap[button] == BUTTON_INPUT_STATUS_DOWN;
	}

	bool Up(DWORD button)
	{
		return buttonMap[button] == BUTTON_INPUT_STATUS_UP;
	}

	bool Press(DWORD button)
	{
		return buttonMap[button] == BUTTON_INPUT_STATUS_PRESS;
	}

	XMFLOAT3 GetMoveValue()
	{
		Vector3 temp;
		XMStoreFloat3(&temp, wheelMoveValue);
		return temp;
	}

private:
	Mouse();
	~Mouse();

	static Mouse* instance;

	HWND handle;
	XMVECTOR position; //마우스 위치

	BYTE buttonStatus[MAX_INPUT_MOUSE];
	BYTE buttonOldStatus[MAX_INPUT_MOUSE];
	BYTE buttonMap[MAX_INPUT_MOUSE];

	XMVECTOR wheelStatus;
	XMVECTOR wheelOldStatus;
	XMVECTOR wheelMoveValue;

	DWORD timeDblClk;
	DWORD startDblClk[MAX_INPUT_MOUSE];
	int buttonCount[MAX_INPUT_MOUSE];

	enum
	{
		MOUSE_ROTATION_NONE = 0,
		MOUSE_ROTATION_LEFT,
		MOUSE_ROTATION_RIGHT
	};

	enum
	{
		BUTTON_INPUT_STATUS_NONE = 0,
		BUTTON_INPUT_STATUS_DOWN,
		BUTTON_INPUT_STATUS_UP,
		BUTTON_INPUT_STATUS_PRESS,
		BUTTON_INPUT_STATUS_DBLCLK
	};
};

