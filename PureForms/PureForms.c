#include "PureForms.h"

#define PRIVATE_WINDOW_CLASS_NAME	L"PureForms"
#define PRIVATE_SUBCLASS_ID			100

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
Tooltip private_createTooltip(HWND hWnd, wchar* tooltipText);
void private_centerForm(Form* form);
void private_registerSubclass(Control* control);
LRESULT CALLBACK private_subclassProc(
	HWND hWnd,
	unsigned int msg,
	WPARAM wParam,
	LPARAM lParam,
	UINT_PTR subclassId,
	DWORD_PTR tooltipPointer
);

int global_nextButtonId = 100;
HFONT global_buttonFont = NULL;
Form* global_thisForm = NULL;
private_ButtonList* global_firstButton = NULL;
private_ButtonList* global_lastButton = NULL;

Form* createForm(int x, int y, int width, int height, wchar* title)
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
	wndClass.lpszClassName = PRIVATE_WINDOW_CLASS_NAME;
	wndClass.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
	RegisterClassW(&wndClass);

	Form* form = (Form*) HeapAlloc(
		GetProcessHeap(),
		HEAP_ZERO_MEMORY,
		sizeof(Form)
	);
	assert(form != NULL);

	// Create the window.
	HWND hWnd = CreateWindowExW(
		0, // Optional window styles                  
		PRIVATE_WINDOW_CLASS_NAME, // Window class
		title, // Window title
		// Window style (no resizing allowed)
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		x, y, width, height, // Size and position
		NULL, // Parent window    
		NULL, // Menu
		hInstance, // Instance handle
		form // Additional application data
	);
	assert(hWnd != NULL);

	form->x = x;
	form->y = y;
	form->width = width;
	form->height = height;
	form->hWnd = hWnd;
	form->title = title;
	global_thisForm = form;

	// This isn't used by the form, but we need it for the buttons
	private_getGuiFont();

	return form;
}

Button* createButton(
	int x, int y, int width, int height, wchar* text, bool isDefault
)
{
	int id = private_getButtonId();
	long styles = WS_TABSTOP | WS_VISIBLE | WS_CHILD;
	styles |= (isDefault) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON;
	HWND hWndButton = CreateWindowW(
		L"BUTTON", // Predefined class; Unicode assumed 
		text, // Button text 
		styles, // Styles 
		x, // x position 
		y, // y position 
		width, // Button width
		height, // Button height
		global_thisForm->hWnd, // Parent window
		(HMENU) (UINT_PTR) id, // Button's ID
		GetModuleHandleW(NULL), // hInstance
		NULL  // Pointer not needed
	);
	assert(hWndButton != NULL);

	Button* button = (Button*) HeapAlloc(
		GetProcessHeap(),
		HEAP_ZERO_MEMORY,
		sizeof(Button)
	);
	assert(button != NULL);
	button->control.hWnd = hWndButton;
	button->id = id;
	button->control.x = x;
	button->control.y = y;
	button->control.width = width;
	button->control.height = height;
	button->text = text;

	SendMessageW(
		button->control.hWnd,
		WM_SETFONT,
		(WPARAM) global_buttonFont,
		(LPARAM) MAKELONG(TRUE, 0)
	);

	private_addButtonToList(button);

	return button;
}

void showForm(Form* form, int showCommand)
{
	//If the window was previously visible, the return value is nonzero
	//If the window was previously hidden, the return value is zero
	int result = ShowWindow(form->hWnd, showCommand);
	assert(result == 0);

	// Main message loop
	MSG msg = { 0 };
	while (GetMessageW(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

void addFormEventHandler(
	Form* form,
	FormEvent event,
	FormEventHandler eventHandler
)
{
	if (event == FormEvent_OnClick)
	{
		form->eventHandlers.OnClick = eventHandler;
	}
	else if (event == FormEvent_OnClose)
	{
		form->eventHandlers.OnClose = eventHandler;
	}
}

void addControlEventHandler(
	Control* control,
	ControlEvent event,
	ControlEventHandler eventHandler
)
{
	if (event == ControlEvent_OnClick)
	{
		control->eventHandlers.OnClick = eventHandler;
	}
	else if (event == ControlEvent_OnHover)
	{
		control->eventHandlers.OnHover = eventHandler;
		private_registerSubclass(control);
	}
}

void addTooltip(Control* control, wchar* text)
{
	control->tooltip = private_createTooltip(
		control->hWnd,
		text
	);
	private_registerSubclass(control);
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
	}
	else if (msg == WM_LBUTTONDOWN)
	{
		Form* form = private_getFormFromHWND(hWnd);
		if (form->eventHandlers.OnClick != NULL)
		{
			EventData_OnClick data = { form->title };
			form->eventHandlers.OnClick(form, &data);
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
			ControlEventHandler onClick =
				button->control.eventHandlers.OnClick;
			if (id == button->id &&
				onClick != NULL)
			{
				EventData_OnClick data = { button->text };
				onClick(&(button->control), &data);
			}
		}
	}
	else if (msg == WM_DESTROY)
	{
		private_doCleanup();
		PostQuitMessage(0);
	}
	else if (msg == WM_CLOSE)
	{
		Form* form = private_getFormFromHWND(hWnd);
		FormEventHandler onClose = form->eventHandlers.OnClose;
		if (onClose != NULL)
		{
			EventData_OnClose eventData = { false };
			onClose(form, &eventData);
			if (NOT eventData.shouldClose)
			{
				return 0;
			}
		}
		DestroyWindow(hWnd);
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

/// @brief centers a window on the desktop
/// @param hWnd a handle to the window to center
void private_centerForm(Form* form)
{
	HWND hWnd = form->hWnd;
	RECT rectDesktop, rectDialog, rectDesktopCopy;
	GetWindowRect(
		GetDesktopWindow(),
		&rectDesktop
	);
	GetWindowRect(
		hWnd,
		&rectDialog
	);
	CopyRect(
		&rectDesktopCopy,
		&rectDesktop
	);

	// Moves rect into top left corner
	// Desktop is already in the top left corner so we don't need to do this
	OffsetRect(
		&rectDialog,
		-rectDialog.left,
		-rectDialog.top
	);

	// Moves left and above the top left corner by the size of the dialog box
	OffsetRect(
		&rectDesktopCopy,
		-rectDialog.right,
		-rectDialog.bottom
	);

	SetWindowPos(
		hWnd,
		HWND_TOP,
		rectDesktop.left + (rectDesktopCopy.right / 2),
		rectDesktop.top + (rectDesktopCopy.bottom / 2),
		0,
		0,
		SWP_NOSIZE
	);
}

Tooltip private_createTooltip(HWND hWnd, wchar* tooltipText)
{
	HINSTANCE hInstance = (HINSTANCE) GetWindowLongPtrW(
		hWnd,
		GWLP_HINSTANCE
	);
	HWND hWndTooltip = CreateWindowExW(
		0,
		TOOLTIPS_CLASSW,
		NULL,
		WS_POPUP | TTS_ALWAYSTIP,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		hWnd,
		NULL,
		hInstance,
		NULL
	);
	assert(hWndTooltip);

	// Build the toolinfo structure
	TTTOOLINFOW toolInfo = { 0 };
	toolInfo.cbSize = sizeof(TTTOOLINFOW);
	toolInfo.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
	toolInfo.hwnd = hWnd;
	toolInfo.hinst = hInstance;
	toolInfo.lpszText = tooltipText;
	toolInfo.uId = (UINT_PTR) hWnd;
	GetClientRect(
		hWnd,
		&toolInfo.rect
	);

	// Associate the tooltip with the tool
	SendMessageW(hWndTooltip,
		TTM_ADDTOOL,
		0,
		(LPARAM) (LPTTTOOLINFOW) &toolInfo);

	// Build the Tooltip structure
	Tooltip tooltip = {
		.hWnd = hWndTooltip,
		.toolInfo = toolInfo
	};
	return tooltip;
}

void private_registerSubclass(Control* control)
{
	BOOL result = SetWindowSubclass(
		control->hWnd,
		(SUBCLASSPROC) private_subclassProc,
		PRIVATE_SUBCLASS_ID,
		(DWORD_PTR) control
	);
	assert(result);
}

/// @brief the window procedure for subclassed controls
/// @param hWnd a window handle to the control
/// @param msg the window message
/// @param wParam
/// @param lParam
/// @param subclassId required by subclass window proc template
/// @param controlPointer a pointer to the subclassed control
/// @return TRUE if message was fully processed by this procedure, 
///         FALSE or the return value of DefSubclassProc() if not
LRESULT CALLBACK private_subclassProc(
	HWND hWnd,
	unsigned int msg,
	WPARAM wParam,
	LPARAM lParam,
	UINT_PTR subclassId,
	DWORD_PTR controlPointer
)
{
	UNREFERENCED_PARAMETER(subclassId);
	BOOL result = TRUE;

	// Track if the mouse is currently within the control
	static bool mouseIsInsideControl = false;

	Control* control = (Control*) controlPointer;

	// Hide the tooltip when the mouse pointer leaves the window
	if (msg == WM_MOUSELEAVE)
	{
		SendMessageW(
			control->tooltip.hWnd,
			TTM_TRACKACTIVATE,
			(WPARAM) FALSE,
			(LPARAM) & (control->tooltip.toolInfo)
		);
		OutputDebugStringW(L"Mouse has left\n");
		mouseIsInsideControl = false;
	}
	else if (msg == WM_MOUSEMOVE)
	{
		// Request mouse hover notifications
		TRACKMOUSEEVENT tracker =
		{
			.cbSize = sizeof(TRACKMOUSEEVENT),
			.dwFlags = TME_HOVER,
			.dwHoverTime = HOVER_DEFAULT,
			.hwndTrack = hWnd
		};
		TrackMouseEvent(&tracker);
	}
	else if (msg == WM_MOUSEHOVER)
	{
		// Track the mouse position across calls
		static int priorMouseX, priorMouseY;
		int currentMouseX, currentMouseY;

		// If the mouse was not previously inside the checkbox
		if (NOT mouseIsInsideControl)
		{
			// Request notification for mouse leave
			TRACKMOUSEEVENT tracker =
			{
				.cbSize = sizeof(TRACKMOUSEEVENT),
				.hwndTrack = hWnd,
				.dwFlags = TME_LEAVE
			};
			TrackMouseEvent(&tracker);
			mouseIsInsideControl = true;

			// Call onHover event handler
			OutputDebugStringW(L"Mouse is hovering\n");
			ControlEventHandler onHover =
				control->eventHandlers.OnHover;
			if (onHover != NULL)
			{
				onHover(control, NULL);
			}

			// If we have a tooltip, show the tooltip
			if (control->tooltip.hWnd != NULL)
			{
				// Activate tooltip
				SendMessageW(
					control->tooltip.hWnd,
					TTM_TRACKACTIVATE,
					(WPARAM) TRUE,
					(LPARAM) &(control->tooltip.toolInfo)
				);
			}
		}

		// Check if mouse has moved and update 
		// the tooltip position if so
		if (control->tooltip.hWnd != 0)
		{
			currentMouseX = GET_X_LPARAM(lParam);
			currentMouseY = GET_Y_LPARAM(lParam);
			if ((currentMouseX != priorMouseX) ||
				(currentMouseY != priorMouseY))
			{
				priorMouseX = currentMouseX;
				priorMouseY = currentMouseY;
				POINT point = { currentMouseX, currentMouseY };
				ClientToScreen(
					hWnd,
					&point
				);

				// Offset checkbox so it is above the cursor, 
				// but still aligned with the left edge of the cursor
				SendMessageW(
					control->tooltip.hWnd,
					TTM_TRACKPOSITION,
					0,
					(LPARAM) MAKELONG(
						point.x,
						point.y - 20
					)
				);
			}
		}
	}
	else
	{
		result = DefSubclassProc(
			hWnd,
			msg,
			wParam,
			lParam
		);
	}
	return result;
}