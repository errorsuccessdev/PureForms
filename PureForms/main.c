/*
 *	TODOS:
 *		1. Don't leak buttons at the end of the program
 *		2. Event handlers :)
 *		3. Probably don't want them resizing the form (for now)
 *		4. Allow to set default button
 * 
 */

#include "PureForms.h"

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
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		L"My Form"
	);
	assert(frmMain != NULL);

	Button* btnFirst = createButton(
		10,
		10,
		200,
		30,
		L"Click me!"
	);

	Button* btnSecond = createButton(
		10,
		50,
		200,
		30,
		L"No, click me instead!"
	);

	bool result = showForm(frmMain, showCommand);
	assert(result);
}