#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdbool.h>
#include <assert.h>
#include <commctrl.h>

////////////////////////////////////////////////////////////////////////////
// Constants and defines
////////////////////////////////////////////////////////////////////////////

#define NOT !
#define wchar wchar_t

////////////////////////////////////////////////////////////////////////////
// Macros
////////////////////////////////////////////////////////////////////////////

#define getControl(_a) &((_a)->control)

////////////////////////////////////////////////////////////////////////////
// Form "class"
////////////////////////////////////////////////////////////////////////////

typedef struct structForm Form;
typedef void (*FormEventHandler) (Form* this, void* eventData);

typedef struct structFormEventHandlers
{
    FormEventHandler OnClick;
    FormEventHandler OnClose;
} FormEventHandlers;

typedef enum enumFormEvent 
{ 
    FormEvent_OnClick,
    FormEvent_OnClose 
} FormEvent;

typedef struct structForm
{
    HWND hWnd;
    int x;
    int y;
    int width;
    int height;
    wchar* title;
    FormEventHandlers eventHandlers;
} Form;

Form* createForm(int x, int y, int width, int height, wchar* title);

void showForm(Form* form, int showCommand);

void addFormEventHandler(
    Form* form, FormEvent event, FormEventHandler eventHandler
);

////////////////////////////////////////////////////////////////////////////
// Control "class"
////////////////////////////////////////////////////////////////////////////

typedef struct structControl Control;

typedef void (*ControlEventHandler) (Control* this, void* eventData);

typedef struct structControlEventHandlers
{
    ControlEventHandler OnClick;
} ControlEventHandlers;

typedef enum enumControlEvent
{
    ControlEvent_OnClick
} ControlEvent;

typedef struct structControl
{
    HWND hWnd;
    int x;
    int y;
    int width;
    int height;
    ControlEventHandlers eventHandlers;
} Control;

void addControlEventHandler(
    Control* control, ControlEvent event, ControlEventHandler eventHandler
);

////////////////////////////////////////////////////////////////////////////
// Button "class"
////////////////////////////////////////////////////////////////////////////

typedef struct structButton
{
    Control control;
    int id;
    wchar* text;
} Button;

Button* createButton(
    int x, int y, int width, int height, wchar* text, bool isDefault
);

////////////////////////////////////////////////////////////////////////////
// EventData structs
////////////////////////////////////////////////////////////////////////////

typedef struct structEventDataOnClick
{
    wchar* text;
} EventData_OnClick;

typedef struct structEventDataOnClose
{
    bool shouldClose;
} EventData_OnClose;