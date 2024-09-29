#include "PureForms.h"
#include <stdio.h>

void button_OnClick(Control* this, void* eventData);
void form_OnClose(Form* this, void* eventData);

Bitmap* global_ourBitmap = NULL;

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
		335,
		160,
		L"Worst color picker ever"
	);
	assert(frmMain != NULL);
	centerForm(frmMain);
	addFormEventHandler(
		frmMain, 
		FormEvent_OnClose, 
		form_OnClose
	);

	global_ourBitmap = createBitmapFromRGB(
		0,
		0,
		130,
		130,
		0,
		0,
		0
	);
	assert(global_ourBitmap != NULL);

	Button* btnRed = createButton(
		150,
		10,
		150,
		30,
		L"Red",
		true
	);
	createTooltip(
		getControl(btnRed), 
		L"Add red to the bitmap"
	);

	Button* btnGreen = createButton(
		150,
		45,
		150,
		30,
		L"Green", 
		false
	);
	createTooltip(
		getControl(btnGreen),
		L"Add green to the bitmap"
	);

	Button* btnBlue = createButton(
		150,
		80,
		150,
		30,
		L"Blue",
		false
	);
	createTooltip(
		getControl(btnBlue),
		L"Add blue to the bitmap"
	);

	addControlEventHandler(
		getControl(btnRed),
		ControlEvent_OnClick,
		button_OnClick
	);

	addControlEventHandler(
		getControl(btnGreen), 
		ControlEvent_OnClick,
		button_OnClick
	);

	addControlEventHandler(
		getControl(btnBlue),
		ControlEvent_OnClick,
		button_OnClick
	);

	showForm(
		frmMain, 
		showCommand
	);
}

void button_OnClick(Control* this, void* eventData)
{
	EventData_OnClick* data = (EventData_OnClick*) eventData;
	u8 red = global_ourBitmap->red;
	u8 green = global_ourBitmap->green;
	u8 blue = global_ourBitmap->blue;
	int increment = 16;
	if (wcscmp(L"Red", data->text) == 0)
	{
		red += increment;
	}
	if (wcscmp(L"Green", data->text) == 0)
	{
		green += increment;
	}
	if (wcscmp(L"Blue", data->text) == 0)
	{
		blue += increment;
	}
	setBitmapRGB(
		global_ourBitmap, 
		red, 
		green, 
		blue
	);
	char buffer[256]; 
	snprintf(
		buffer, 
		256, 
		"%d, %d, %d\n", 
		red,
		green, 
		blue
	);
	OutputDebugStringA(buffer);
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