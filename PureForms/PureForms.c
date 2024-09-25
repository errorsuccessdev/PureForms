#include "PureForms.h"

typedef struct structPrivateButtonList
{
	Button* button;
	struct structPrivateButtonList* next;
} private_ButtonList;

LRESULT private_windowProc(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
);
Form* private_getFormFromHWND(HWND hWnd);
int private_getButtonId(void);
void private_doCleanup(void);
void private_getGuiFont(void);
void private_addButtonToList(Button* button);
EventHandlers* private_initEventHandlers(void);

int global_nextButtonId = 100;
HFONT global_buttonFont = NULL;
Form* global_thisForm = NULL;
private_ButtonList* global_firstButton = NULL;
private_ButtonList* global_lastButton = NULL;

// #1
Form* createForm(
	int x, int y, int width, int height, wchar* title
)
{
	// Load common controls DLL 
	INITCOMMONCONTROLSEX icex = { 0 };
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&icex);
	HINSTANCE hInstance = GetModuleHandleW(NULL);
	WNDCLASS wndClass = { 0 };
	wndClass.lpfnWndProc = private_windowProc;
	wndClass.hInstance = hInstance;
	wndClass.lpszClassName = WINDOW_CLASS_NAME;
	RegisterClassW(&wndClass);

	Form* form = (Form*) HeapAlloc(
		GetProcessHeap(),
		HEAP_ZERO_MEMORY,
		sizeof(Form)
	);
	assert(form != NULL);

	// Create the window.
	HWND hWnd = CreateWindowExW(
		0,                          // Optional window styles.
		WINDOW_CLASS_NAME,          // Window class
		title,                // Window text
		WS_OVERLAPPEDWINDOW,        // Window style

		// Size and position
		x, y, width, height,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		form  // Additional application data
	);
	assert(hWnd != NULL);

	form->x = x;
	form->y = y;
	form->width = width;
	form->height = height;
	form->hWnd = hWnd;
	form->title = title;
	form->eventHandlers = private_initEventHandlers();
	global_thisForm = form;

	// This isn't used by the form, but we need for the buttons
	private_getGuiFont();

	return form;
}

// #3
Button* createButton(
	int x, int y, int width, int height, wchar* text
)
{
	int id = private_getButtonId();
	HWND hWndButton = CreateWindowW(
		L"BUTTON",  // Predefined class; Unicode assumed 
		text,      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
		x,         // x position 
		y,         // y position 
		width,        // Button width
		height,        // Button height
		global_thisForm->hWnd,       // Parent window
		(HMENU) (UINT_PTR) id,       // Button's ID
		GetModuleHandleW(NULL),
		NULL  // Pointer not needed.
	);
	assert(hWndButton != NULL);

	Button* button = (Button*) HeapAlloc(
		GetProcessHeap(),
		HEAP_ZERO_MEMORY,
		sizeof(Button)
	);
	assert(button != NULL);
	button->hWnd = hWndButton;
	button->id = id;
	button->x = x;
	button->y = y;
	button->width = width;
	button->height = height;
	button->text = text;
	button->eventHandlers = private_initEventHandlers();

	SendMessageW(
		button->hWnd,
		WM_SETFONT,
		(WPARAM) global_buttonFont,
		(LPARAM) MAKELONG(TRUE, 0)
	);

	private_addButtonToList(button);

	return button;
}

// #2
bool showForm(Form* form, int showCommand)
{
	//If the window was previously visible, the return value is nonzero.
	//If the window was previously hidden, the return value is zero.
	int result = ShowWindow(form->hWnd, showCommand);
	assert(result == 0);

	// Main message loop
	MSG msg = { 0 };
	while (GetMessageW(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return true;
}

Form* private_getFormFromHWND(HWND hWnd)
{
	LONG_PTR longPointer = GetWindowLongPtrW(
		hWnd,
		GWLP_USERDATA
	);
	return (Form*) longPointer;
}

LRESULT private_windowProc(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
)
{
	LRESULT result = 0;

	if (msg == WM_CREATE)
	{
		// Associate form with HWND
		CREATESTRUCT* createStruct = (CREATESTRUCT*) lParam;
		Form* form = (Form*) createStruct->lpCreateParams;
		SetWindowLongPtrW(
			hWnd,
			GWLP_USERDATA,
			(LONG_PTR) form
		);

		// Set up font we will use in the form
	/*	const long nFontSize = 12;
		HDC hdc = GetDC(hWnd);
		LOGFONT logFont = { 0 };
		logFont.lfHeight = -MulDiv(
			nFontSize,
			GetDpiForWindow(hWnd),
			72
		);
		logFont.lfWeight = FW_NORMAL;
		wcscpy(logFont.lfFaceName, L"Segoe UI Regular");
		font = CreateFontIndirectW(&logFont);
		ReleaseDC(hWnd, hdc);*/

		/*HWND button = createButton(L"Ok", 20, 40, 50, 20, hWnd, BTNID_OK);
		SendMessage(button, WM_SETFONT, (WPARAM) font, (LPARAM) MAKELONG(TRUE, 0));*/

	}
	else if (msg == WM_LBUTTONDOWN)
	{
		Form* form = private_getFormFromHWND(hWnd);
		if (form->eventHandlers->OnClick != NULL)
		{
			form->eventHandlers->OnClick(form);
		}
	}
	else if (msg == WM_COMMAND)
	{
		// Handle buttons
		int id = LOWORD(wParam);
		for (private_ButtonList* listItem = global_firstButton;
			listItem != NULL;
			listItem = listItem->next)
		{
			Button* button = listItem->button;
			EventHandler_OnClick onClick = button->eventHandlers->OnClick;
			if (id == button->id &&
				onClick != NULL)
			{
				onClick(button);
			}
		}
	}
	else if (msg == WM_DESTROY)
	{
		private_doCleanup();
		PostQuitMessage(0);
	}
	else
	{
		result = DefWindowProcW(
			hWnd,
			msg,
			wParam,
			lParam
		);
	}
	return result;
}

int private_getButtonId(void)
{
	int buttonId = global_nextButtonId;
	global_nextButtonId += 1;
	return buttonId;
}

void private_getGuiFont(void)
{
	// Set the font
	NONCLIENTMETRICS metrics = { 0 };
	metrics.cbSize = sizeof(metrics);
	SystemParametersInfoW(
		SPI_GETNONCLIENTMETRICS,
		metrics.cbSize,
		&metrics,
		0
	);
	int nFontSize = 11;
	metrics.lfCaptionFont.lfHeight = -MulDiv(
		nFontSize,
		GetDpiForWindow(global_thisForm->hWnd),
		72
	);
	global_buttonFont = CreateFontIndirectW(&metrics.lfCaptionFont);
}

void private_doCleanup(void)
{
	// Clean up form
	HeapFree(
		GetProcessHeap(),
		0,
		global_thisForm
	);

	// Clean up buttons and buttonList
	private_ButtonList* listItem = global_firstButton;
	private_ButtonList* nextItem;
	HANDLE processHeap = GetProcessHeap();
	while (listItem != NULL)
	{
		nextItem = listItem->next;
		HeapFree(processHeap, 0, listItem->button->eventHandlers);
		HeapFree(processHeap, 0, listItem->button);
		HeapFree(processHeap, 0, listItem);
		listItem = nextItem;
	}

	// Clean up fonts
	DeleteObject(global_buttonFont);
}

void private_addButtonToList(Button* button)
{
	private_ButtonList* newListEnd = (private_ButtonList*) HeapAlloc(
		GetProcessHeap(),
		HEAP_ZERO_MEMORY,
		sizeof(private_ButtonList)
	);
	assert(newListEnd);
	newListEnd->button = button;
	newListEnd->next = NULL;
	if (global_firstButton == NULL)
	{
		global_firstButton = newListEnd;
	}
	else
	{
		global_lastButton->next = newListEnd;
	}
	global_lastButton = newListEnd;
}

EventHandlers* private_initEventHandlers(void)
{
	EventHandlers* eventHandlers = (EventHandlers*) HeapAlloc(
		GetProcessHeap(),
		HEAP_ZERO_MEMORY,
		sizeof(EventHandlers)
	);
	assert(eventHandlers != NULL);
	return eventHandlers;
}