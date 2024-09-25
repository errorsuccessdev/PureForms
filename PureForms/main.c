/*
 *	TODOS:
 *		1. Probably don't want them resizing the form (for now)
 *		2. Allow to set default button
 * 
 * Tarriest_Python
: I just had a thought, if you do go for a function to add 
event handlers you can probably wrap the user provided function 
pointer with a private function and provide an API which uses 
the correct types as parameter
i.e. button_add_OnClick(Button *b, void (*)(Button *)) { // ... }

Tarriest_Python
: basically teh private function would just cast the 
void * -> Button * then pass it to the user provided function pointer
Tarriest_Python
: you store the user callback function on the button wiht the correct 
type, but actually call the private function with the button 
pointer and the function pointer as arguments when you get the event
Tarriest_Python
: so private_OnClick(button, button->OnClick)
 */

#include "PureForms.h"

void button_OnClick(void* this);
void form_OnClick(void* this);

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
	frmMain->eventHandlers->OnClick = form_OnClick;

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

	Button* btnThird = createButton(
		10,
		90,
		200,
		30,
		L"DO NOT click me!"
	);

	// Wrap in function, ideally? Maybe?
	btnFirst->eventHandlers->OnClick = button_OnClick;
	btnSecond->eventHandlers->OnClick = button_OnClick;
	btnThird->eventHandlers->OnClick = button_OnClick;

	bool result = showForm(frmMain, showCommand);
	assert(result);
}

void button_OnClick(void* this)
{
	Button* button = (Button*) this;
	OutputDebugStringW(button->text);
}

void form_OnClick(void* this)
{
	Form* form = (Form*) this;
	OutputDebugStringW(form->title);
}