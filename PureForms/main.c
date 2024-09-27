/*
 *	TODOS:
 *		- Tooltips?
 */

#include "PureForms.h"

void button_OnClick(Control* this, void* eventData);
void form_OnClick(Form* this, void* eventData);
void form_OnClose(Form* this, void* eventData);

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR args,
	_In_ int showCommand
)
{
	Form* frmMain = createForm(
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		240,
		170,
		L"My Form"
	);
	assert(frmMain != NULL);
	addFormEventHandler(frmMain, FormEvent_OnClick, form_OnClick);
	addFormEventHandler(frmMain, FormEvent_OnClose, form_OnClose);

	Button* btnFirst = createButton(
		10,
		10,
		200,
		30,
		L"Click me!",
		true
	);

	Button* btnSecond = createButton(
		10,
		50,
		200,
		30,
		L"No, click me instead!", 
		false
	);

	Button* btnThird = createButton(
		10,
		90,
		200,
		30,
		L"DO NOT click me!",
		false
	);

	addControlEventHandler(
		getControl(btnFirst), 
		ControlEvent_OnClick,
		button_OnClick
	);
	addControlEventHandler(
		getControl(btnSecond), 
		ControlEvent_OnClick,
		button_OnClick
	);
	addControlEventHandler(
		getControl(btnThird), 
		ControlEvent_OnClick, 
		button_OnClick
	);

	showForm(frmMain, showCommand);
}

void button_OnClick(Control* this, void* eventData)
{
	EventData_OnClick* data = (EventData_OnClick*) eventData;
	OutputDebugStringW(data->text);
}

void form_OnClick(Form* this, void* eventData)
{
	EventData_OnClick* data = (EventData_OnClick*) eventData;
	OutputDebugStringW(data->text);
}

void form_OnClose(Form* this, void* eventData)
{
	EventData_OnClose* data = (EventData_OnClose*) eventData;
	int option = MessageBoxW(
		this->hWnd,
		L"Are you sure?",
		L"Closing",
		MB_YESNO | MB_ICONQUESTION
	);
	data->shouldClose = (option == IDYES) ? true : false;
}